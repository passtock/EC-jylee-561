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
        seven_seg_FND_display(g_count, 0);

        // 2. delay for 1 second
        delay_ms(1000); 

        // 3. increment the count
        g_count++;
        
        // 4. 0~9 cycle
        if (g_count > 9) {
            g_count = 0;
        }

        // 5. if reset flag is set by EXTI ISR, reset count
        if (g_reset_flag == 1) {
            g_count = 0;      // reset count to 0
            g_reset_flag = 0; // reset the flag
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
