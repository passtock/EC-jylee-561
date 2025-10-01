#include "ecSTM32F4v2.h"

#define BUTTON_PIN PA_4

volatile int g_count = 0;        // count should be volatile
volatile uint8_t g_button_flag = 0; // flag that ISR informs main of button input

// 초기화 함수
void setup(void) {
    RCC_PLL_init();
    SysTick_init();
    seven_seg_FND_init(); // 7 seg init

    GPIO_init(BUTTON_PIN, INPUT);
    GPIO_ospeed(BUTTON_PIN, 01); // medium speed
    GPIO_otype(BUTTON_PIN, 0); // Push-Pull
    GPIO_pupd(BUTTON_PIN, 0); // no pull-up, pull-down

    // exti init falling edge trigger, priority 0
    EXTI_init(BUTTON_PIN, FALL, 0);
}

int main(void) {
    setup();

    while (1) {
        // 1. display the current count on the 7-segment display
        sevensegment_display_19(g_count);

        // 2. check if the button was pressed (flag set by ISR)
        if (g_button_flag == 1) {
            delay_ms(50); // 50ms delay for debouncing

            // 3. check if the button is still pressed
            if (GPIO_read(BUTTON_PIN) == 0) {
                g_count = (g_count + 1) % 20; // 0~19 cycle

                // 4. wait until the button is released
                while (GPIO_read(BUTTON_PIN) == 0);
            }
            
            // 5. clear the button flag
            g_button_flag = 0; 
        }
    }
}

// pa4 is extu4, so isr name is EXTI4_IRQHandler
void EXTI4_IRQHandler(void) {
    // 1. check if the interrupt is from the correct pin
    if (is_pending_EXTI(BUTTON_PIN)) {
        // 2. set the button flag to indicate a button press
        g_button_flag = 1;
        
        // 3. clear the pending bit for the EXTI line
        clear_pending_EXTI(BUTTON_PIN);
    }
}