/**
******************************************************************************
* @author  SSSLAB
* @Mod     2023-10-31 by YKKIM
* @brief   Embedded Controller: LAB - Timer Input Capture
*          - with Ultrasonic Distance Sensor (HC-SR04)
* @reviser 2025-11-07 by Gemini
* @brief   500ms마다 측정, PWM 대신 GPIO 트리거, 측정값 반복 출력X, 측정 실패 구분
******************************************************************************
*/

#include "stm32f411xe.h"
#include "math.h"
#include "ecSTM32F4v2.h"

// --- 공유 변수 ---
volatile uint32_t ovf_cnt = 0;
volatile uint32_t timeInterval = 0;
volatile uint8_t is_rising_captured = 0;
volatile uint32_t measurement_start_time = 0;
volatile uint8_t new_data_available = 0; // 새로운 측정값 도착시만 출력

extern volatile uint32_t msTicks;

// --- 상수 정의 ---
#define TRIG_PIN   PA_6   // HC-SR04 TRIG 연결(환경에 맞게 설정)
#define ECHO_PIN   PB_6   // HC-SR04 ECHO 연결(환경에 맞게 설정)
#define TIMEOUT_MS 60     // 60ms 이상 응답 없으면 측정 실패
#define MEASURE_INTERVAL_MS 500 // 500ms마다 측정

uint32_t last_measure_time = 0;

// ★ 트리거 함수: PWM 대신 GPIO 제어 ★
void trigger_ultrasonic(void){
    GPIO_write(TRIG_PIN, 1);      // HIGH
    delay_us(10);                 // 10us 펄스
    GPIO_write(TRIG_PIN, 0);      // LOW
}

void setup(void);

int main(void){
    setup();

    while(1){
        // 500ms마다 초음파 트리거
        if(msTicks - last_measure_time >= MEASURE_INTERVAL_MS) {
            trigger_ultrasonic();      // 트리거 신호 보내기
            last_measure_time = msTicks;
            // 새로운 측정 준비: 변수 리셋
            is_rising_captured = 0;
            timeInterval = 0;
            new_data_available = 0;
        }

        // ECHO 타임아웃 발생 시 에러 출력
        if(is_rising_captured == 1 && (msTicks - measurement_start_time > TIMEOUT_MS)) {
            is_rising_captured = 0;
            printf("Timeout! Measurement failed.\r\n");
            timeInterval = 0; // 실패값
        }

        // 새로운 측정값이 들어왔을 때만 거리 출력
        if(new_data_available == 1 && timeInterval != 0) {
            uint32_t distance_mm = (timeInterval * 10 * 17) / 100;
            printf("Interval: %lu ticks, Distance: %lu mm\r\n", timeInterval, distance_mm);
            new_data_available = 0;
        }

        // 측정 실패/실패값일 땐 오류 표기(필요시)
        if(new_data_available == 0 && timeInterval == 0){
            printf("Distance: Invalid (timeout or error)\r\n");
        }

        delay_ms(5); // 실시간 응답 확보
    }
}

// --- 인터럽트 핸들러(에지 캡처&오버플로우) ---
void TIM4_IRQHandler(void){
    // 상승 에지(ECHO HIGH): 측정 시작
    if(is_CCIF(TIM4, 1)){
        clear_CCIF(TIM4, 1);
        uint32_t time1 = ICAP_capture(TIM4, 1);
        is_rising_captured = 1;
        ovf_cnt = 0;
        measurement_start_time = msTicks;
    }

    // 하강 에지(ECHO LOW): 측정 종료, 값 갱신
    if(is_CCIF(TIM4, 2)){
        clear_CCIF(TIM4, 2);
        if(is_rising_captured == 1) {
            uint32_t time2 = ICAP_capture(TIM4, 2);
            uint32_t time1_val = TIM4->CCR1;
            timeInterval = (ovf_cnt * 65536) + time2 - time1_val;
            is_rising_captured = 0;
            new_data_available = 1; // 새 측정값 도착
        }
    }

    // 오버플로우
    if(is_UIF(TIM4)){
        clear_UIF(TIM4);
        if(is_rising_captured == 1){
            ovf_cnt++;
        }
    }
}

// --- 시스템 셋업(초기화, 핀설정 등) ---
void setup(){
    RCC_PLL_init();
    SysTick_init();
    UART2_init();

    // ★ GPIO 모드로 TRIG OUTPUT 설정 및 초기값 LOW ★
    GPIO_init(TRIG_PIN, EC_OP);
    GPIO_write(TRIG_PIN, 0);

    // ECHO INPUT(Alternate/AF) 설정 및 풀다운 유지
    GPIO_init(ECHO_PIN, EC_AF);
    GPIO_pupd(ECHO_PIN, EC_PD);

    // 타이머 입력캡처/인터럽트 셋업
    ICAP_init(ECHO_PIN);
    ICAP_counter_us(ECHO_PIN, 10);
    ICAP_setup(ECHO_PIN, 1, IC_RISE);
    ICAP_setup(ECHO_PIN, 2, IC_FALL);

    clear_UIF(TIM4);
    clear_CCIF(TIM4, 1);
    clear_CCIF(TIM4, 2);

    NVIC_EnableIRQ(TIM4_IRQn);
    NVIC_SetPriority(TIM4_IRQn, 2);

    delay_ms(100);
}
