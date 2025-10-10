/**
******************************************************************************
* @author  SSSLAB
* @Mod     2024-8-23 by Jin Kwak
* @brief   Embedded Controller: Tutorial - PWM on TIM2_CH1 (PA5)
*          - PWM 1kHz with Duty Ratios of 0%, 50%, 100%
*
******************************************************************************
*/

#include "stm32f411xe.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"
#include "ecSysTick2.h"

#define LED_PIN  PA_5  // On-board LED pin (PA5)
void setup(void);

int main(void) {
    // Initialization --------------------------------------------------------
    setup();

    // GPIO PA5 (LED_PIN) Alternate Function (AF) Mode for TIM2_CH1
    GPIO_init(LED_PIN, 2);      // Initialize PA5 for Alternate Function
    GPIO_ospeed(LED_PIN, 2); // Set PA5 to High speed
    GPIOA->AFR[0] |= (1UL << 20);      // Set AF1 (TIM2) for PA5

    // TIMER TIM2 Initialization for PWM ------------------------------------
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable TIM2 clock

    // Timer configuration
    // System Clock (SYSCLK) = 84MHz, APB1 Prescaler = 2, so APB1 Timer Clock = 84MHz
    TIM2->PSC = 840 - 1;                // Timer Clock = 84MHz / 840 = 100kHz
    TIM2->ARR = 1000 - 1;               // PWM Frequency = 100kHz / 1000 = 1kHz

    // PWM Mode 1 configuration for Channel 1
    TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;     // Clear OC1M bits
    TIM2->CCMR1 |= 6UL << 4; // Set PWM mode 1 (110)
    TIM2->CCMR1 |= TIM_CCMR1_OC1PE;     // Enable OC1 preload

    // Channel 1 output configuration
    TIM2->CCER |= TIM_CCER_CC1E;        // Enable CH1 output
    TIM2->CCER &= ~TIM_CCER_CC1P;       // Set active high polarity

    TIM2->CR1 |= TIM_CR1_CEN;           // Enable TIM2 counter

    while(1){
        // Duty Ratio: 0%
		TIM2->CCR1 = 0;
        delay_ms(1000);
		TIM2->CCR1 = 500;
        delay_ms(1000);
        TIM2->CCR1 = 1000;
        delay_ms(1000);


    }
}

// Initialization
void setup(void)
{
    RCC_PLL_init();       // System Clock = 84MHz
    SysTick_init();       // for delay_ms()
}