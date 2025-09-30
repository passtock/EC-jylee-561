#include "ecSTM32F4v2.h"

// 사용할 핀 정의 (PA4 사용)
#define BUTTON_PIN PA_4

// 전역 변수 선언
volatile int g_count = 0;        // 카운트 변수 (ISR과 main이 공유하므로 volatile)
volatile uint8_t g_button_flag = 0; // ISR이 main에 버튼 입력을 알리는 플래그

// 초기화 함수
void setup(void) {
    RCC_PLL_init();
    SysTick_init();
    seven_seg_FND_init(); // 7세그먼트 초기화 추가

    // 버튼 핀을 풀업(Pull-up) 입력으로 설정
    GPIO_init(BUTTON_PIN, INPUT);
    GPIO_pupd(BUTTON_PIN, 01);

    // EXTI 인터럽트 초기화 (Falling edge, 우선순위 0)
    EXTI_init(BUTTON_PIN, FALL, 0);
}

int main(void) {
    setup();

    while (1) {
        // 1. 7세그먼트에 현재 카운트 값을 계속 표시
        sevensegment_display_19(g_count);

        // 2. ISR이 버튼 입력을 감지했는지 플래그 확인
        if (g_button_flag == 1) {
            delay_ms(50); // 디바운싱(Debouncing)을 위한 50ms 지연

            // 3. 딜레이 후에도 버튼이 여전히 눌려있는지 확인 (실제 입력인지 확인)
            if (GPIO_read(BUTTON_PIN) == 0) {
                g_count = (g_count + 1) % 20; // 0~19 순환하도록 카운트 증가

                // 4. 버튼에서 손을 뗄 때까지 대기 (한 번만 카운트하기 위함)
                while (GPIO_read(BUTTON_PIN) == 0);
            }
            
            // 5. 플래그를 다시 0으로 초기화하여 다음 입력을 기다림
            g_button_flag = 0; 
        }
    }
}

// PA4는 EXTI4에 해당하므로, ISR 이름은 EXTI4_IRQHandler 입니다.
void EXTI4_IRQHandler(void) {
    // 1. 해당 핀의 인터럽트가 맞는지 확인
    if (is_pending_EXTI(BUTTON_PIN)) {
        // 2. main 함수에 버튼이 눌렸다고 알리기 위해 플래그만 1로 설정
        g_button_flag = 1;
        
        // 3. 인터럽트 처리 완료를 알리기 위해 Pending bit 클리어
        clear_pending_EXTI(BUTTON_PIN);
    }
}