/*
 * Smart Robot
 * Priority 1: Red Object ('E') -> FORCE STOP
 * Priority 2: 10s Back ('B') -> Green Object
 * Priority 3: 10s Line Trace
 * Priority 4: Face Tracking ('F','L','R') OR Search ('X' with delay)
 */

#include "ecSTM32F4v2.h"
#include "ecPWM2.h"
#include "ecADC2.h"

// 핀 정의
#define L_PWM_PIN  PA_0
#define L_DIR_PIN  PA_4
#define R_PWM_PIN  PA_1
#define R_DIR_PIN  PC_1 
PinName_t seqCHn[2] = {PB_0, PB_1};
#define STOP_LED   PA_5

// 변수
volatile uint32_t value1 = 0;
volatile uint32_t value2 = 0;
volatile uint8_t adc_ready = 0; 
int flag = 0;

volatile char python_cmd = 'S'; 
int trace_timer = 0;            // 라인트레이싱 (10초)
int back_timer = 0;             // 강제 후진 (10초)

// 수색 모드용 타이머
int search_timer = 0;           

// 속도 설정
#define FACE_SPD_MAX    1.0f   
#define FACE_SPD_TURN   0.60f
#define BACK_SPD        0.60f
#define SEARCH_SPD      0.70f

#define TRACE_SPD       0.60f   
#define TURN_SPD        0.60f
#define BLACK_THRESHOLD 3000U 

// 모터 제어 함수
void motor_control(float speed_L, float speed_R) {
    // 왼쪽
    if (speed_L >= 0) { 
        GPIO_write(L_DIR_PIN, 1);
        PWM_duty(L_PWM_PIN, 1.0f - speed_L);
    } else { 
        GPIO_write(L_DIR_PIN, 0);
        // speed_L이 음수이므로 -를 붙여 양수로 변환해 입력
        PWM_duty(L_PWM_PIN, -speed_L); 
    }
    // 오른쪽
    if (speed_R >= 0) { 
        GPIO_write(R_DIR_PIN, 1);
        PWM_duty(R_PWM_PIN, 1.0f - speed_R);
    } else { 
        GPIO_write(R_DIR_PIN, 0);
        PWM_duty(R_PWM_PIN, -speed_R); 
    }
}

void setup(void) {
    RCC_PLL_init(); SysTick_init();
    
    PWM_init(L_PWM_PIN); PWM_init(R_PWM_PIN);
    PWM_period_us(L_PWM_PIN, 1000); PWM_period_us(R_PWM_PIN, 1000); 
    GPIO_init(L_DIR_PIN, OUTPUT); GPIO_init(R_DIR_PIN, OUTPUT);
    motor_control(0, 0); 
    GPIO_init(STOP_LED, OUTPUT);

    UART2_init(); UART2_baud(BAUD_9600);
    UART1_init(); UART1_baud(BAUD_9600); 
    
    ADC_init(PB_0); ADC_init(PB_1);
    ADC_sequence(seqCHn, 2);
    ADC->CCR = (ADC->CCR & ~(3UL << 16)) | (3UL << 16); 
    GPIO_init(PB_0, ANALOG); GPIO_pupd(PB_0, EC_NONE);
    GPIO_init(PB_1, ANALOG); GPIO_pupd(PB_1, EC_NONE);

    NVIC_EnableIRQ(ADC_IRQn);      NVIC_SetPriority(ADC_IRQn, 2);
    NVIC_EnableIRQ(USART1_IRQn);   NVIC_SetPriority(USART1_IRQn, 3);
    ADC_start(); 
    
    printf("System Ready\r\n");
}

void ADC_IRQHandler(void){
    if(is_ADC_OVR()) clear_ADC_OVR();
    if(is_ADC_EOC()){       
        if (flag==0) { value1 = ADC_read(); }
        else if (flag==1) { value2 = ADC_read(); adc_ready = 1; }
        flag = !flag;
    }
}

// [핵심 수정 부분]
void USART1_IRQHandler(void){
    if(is_USART1_RXNE()){
        char received = (char)USART1_read();
        
        // 'B'(초록색) 수신 시
        if (received == 'B') {
            back_timer = 10000;   // 10초 후진 설정
            python_cmd = 'S';     // [중요] 기존에 'E'(정지)가 남아있다면 해제해줍니다.
        }
        
        // 후진 중이 아닐 때만 명령 업데이트
        if (back_timer == 0) {
            python_cmd = received;
        }
    }
}

int main(void) {
    setup();
    
    while(1){
        if(adc_ready){
            adc_ready = 0; 
            uint32_t v1 = value1; 
            uint32_t v2 = value2; 

            int is_Black_R = (v1 >= BLACK_THRESHOLD); 
            int is_Black_L = (v2 >= BLACK_THRESHOLD);
            
            // ----------------------------------------------------
            // 1. [최우선] 빨간색(E) -> 정지
            // ----------------------------------------------------
            if (python_cmd == 'E') {
                motor_control(0.0f, 0.0f); 
                GPIO_write(STOP_LED, 1);
                back_timer = 0; 
                trace_timer = 0; 
                search_timer = 0; 
            }
            // ----------------------------------------------------
            // 2. 10초 강제 후진 중 (초록색 감지 시)
            // ----------------------------------------------------
            else if (back_timer > 0) {
                back_timer--;
                motor_control(-BACK_SPD, -BACK_SPD); // 후진
                GPIO_write(STOP_LED, 1);
            }
            // ----------------------------------------------------
            // 3. 라인트레이싱 모드
            // ----------------------------------------------------
            else if (trace_timer > 0) {
                trace_timer--; 
                GPIO_write(STOP_LED, 1);
                if (!is_Black_L && !is_Black_R) motor_control(TRACE_SPD, TRACE_SPD);
                else if (is_Black_L && !is_Black_R) motor_control(-TURN_SPD, TURN_SPD);
                else if (!is_Black_L && is_Black_R) motor_control(TURN_SPD, -TURN_SPD);
                else motor_control(0.0f, 0.0f);
            }
            // ----------------------------------------------------
            // 4. 평상시 모드
            // ----------------------------------------------------
            else {
                GPIO_write(STOP_LED, 0); 

                // 4-1. 검은 선 발견 -> 라인트레이싱 10초 시작
                if (is_Black_L || is_Black_R) {
                    trace_timer = 10000; 
                }
                // 4-2. 비전 명령 수행
                else {
                    // [수색 모드] 'X' 명령일 때
                    if (python_cmd == 'X') {
                        if (search_timer <= 0) search_timer = 6000; 

                        search_timer--; 

                        if (search_timer > 1000) {
                            motor_control(0.0f, 0.0f); // 5초 정지
                        }
                        else {
                            motor_control(-SEARCH_SPD, SEARCH_SPD); // 1초 회전
                        }
                    } 
                    // 'X'가 아닐 때
                    else {
                        search_timer = 0; 
                        
                        switch(python_cmd) {
                            case 'F': motor_control(FACE_SPD_MAX, FACE_SPD_MAX); break;
                            case 'L': motor_control(FACE_SPD_TURN, -FACE_SPD_TURN); break;
                            case 'R': motor_control(-FACE_SPD_TURN, FACE_SPD_TURN); break;
                            case 'S': default: motor_control(0.0f, 0.0f); break;
                        }
                    }
                }
            }
        }
    }
}