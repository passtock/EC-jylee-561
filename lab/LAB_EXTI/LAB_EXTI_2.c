#include "ecSTM32F4v2.h"

volatile int g_count = 0;        // Current digit to display
volatile uint8_t g_reset_flag = 0; // Set by EXTI ISR when button pressed
#define BUTTON_PIN PA_4


void setup(void)
{
	RCC_PLL_init();
	SysTick_init();
	seven_seg_FND_init();
	// Button pin EXTI setup (assuming BUTTON_PIN is defined as PA4)
 	GPIO_init(BUTTON_PIN, INPUT);
    GPIO_pupd(BUTTON_PIN, 01);
	EXTI_init(BUTTON_PIN, FALL, 0);

}

int main(void) {
	setup();
	while (1) {
	     // 1. 현재 g_count 값을 7세그먼트에 표시
        seven_seg_FND_display(g_count, 0);

        // 2. 1초 대기
        delay_ms(1000); 

        // 3. g_count 값을 1 증가시킴
        g_count++;
        
        // 4. 숫자가 9를 넘어가면 다시 0으로 만들어 0~9를 반복
        // (만약 19까지 표시하고 싶다면 '9'를 '19'로 바꾸면 됩니다)
        if (g_count > 9) {
            g_count = 0;
        }

        // 5. 만약 버튼이 눌렸다면 (ISR이 플래그를 1로 만들었다면)
        if (g_reset_flag == 1) {
            g_count = 0;      // 카운트를 0으로 리셋
            g_reset_flag = 0; // 플래그를 다시 0으로 초기화
        }
}
}


// EXTI4 IRQ Handler for PA4 button
void EXTI4_IRQHandler(void) {
	if (is_pending_EXTI(BUTTON_PIN)) {
		g_reset_flag = 1; // Signal main to reset count
		clear_pending_EXTI(BUTTON_PIN);
	}
}
