/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2023-10-18 by YKKIM  	
* @brief   Embedded Controller:  EC_HAL_for_student_exercise 
* 
******************************************************************************
*/

#include "stm32f4xx.h"
#include "ecPWM2.h"
#include "math.h"

/* PWM Configuration using PinName_t Structure */

/* PWM initialization */
// Default: 84MHz PLL, 1MHz CK_CNT, 50% duty ratio, 1msec period
void PWM_init(PinName_t pinName){

// 0. Match TIMx from  Port and Pin 	
	GPIO_TypeDef *port;
	unsigned int pin;
	ecPinmap(pinName, &port, &pin);
	TIM_TypeDef *TIMx;
	int chN;
	PWM_pinmap(pinName, &TIMx, &chN);
	
	
// 1. Initialize GPIO port and pin as AF	
	GPIO_init(pinName, 2);  // AF=2
	GPIO_otype(pinName, 0); 	
	GPIO_pupd(pinName, 1); 					
	
	
// 2. Configure GPIO AFR by Pin num.	
	//  AFR[0] for pin: 0~7,     AFR[1] for pin 8~15
	//  AFR=1 for TIM1,TIM2	AFR=2 for TIM3 etc		
	if(pin < 8) {
    port->AFR[0] &= ~(0xFUL << (4*pin));
    if(TIMx == TIM1 || TIMx == TIM2) port->AFR[0] |= (1UL << (4*pin));
    else if(TIMx == TIM3 || TIMx == TIM4 || TIMx == TIM5) port->AFR[0] |= (2UL << (4*pin));
    else if(TIMx == TIM9 || TIMx == TIM10 || TIMx == TIM11) port->AFR[0] |= (3UL << (4*pin));
}
else {
    port->AFR[1] &= ~(0xFUL << (4*(pin-8)));
    if(TIMx == TIM1 || TIMx == TIM2) port->AFR[1] |= (1UL << (4*(pin-8)));
    else if(TIMx == TIM3 || TIMx == TIM4 || TIMx == TIM5) port->AFR[1] |= (2UL << (4*(pin-8)));
    else if(TIMx == TIM9 || TIMx == TIM10 || TIMx == TIM11) port->AFR[1] |= (3UL << (4*(pin-8)));
}

// 3. Initialize Timer 
	TIM_init(TIMx);	// with default msec=1msec value.		
	TIMx->CR1 &= ~TIM_CR1_CEN;
	
// 3-2. Direction of Counter
	//YOUR CODE GOES HERE
	TIMx->CR1 &= ~TIM_CR1_DIR;                          // Counting direction: 0 = up-counting, 1 = down-counting
			
	
// 4. Configure Timer Output mode as PWM
	uint32_t ccVal = TIMx->ARR/2;  // default value  CC=ARR/2
	if(chN == 1){
		TIMx->CCMR1 &= ~TIM_CCMR1_OC1M;                     // Clear ouput compare mode bits for channel 1
		TIMx->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // OC1M = 110 for PWM Mode 1 output on ch1. #define TIM_CCMR1_OC1M_1          (0x2UL << TIM_CCMR1_OC1M_Pos)
		TIMx->CCMR1	|= TIM_CCMR1_OC1PE;                     // Output 1 preload enable (make CCR1 value changable)
		TIMx->CCR1  = ccVal; 																// Output Compare Register for channel 1 (default duty ratio = 50%)		
		TIMx->CCER &= ~TIM_CCER_CC1P;                       // select output polarity: active high	
		TIMx->CCER  |= TIM_CCER_CC1E;												// Enable output for ch1
	}
	else if(chN == 2){
		TIMx->CCMR1 &= ~TIM_CCMR1_OC2M;                     // Clear ouput compare mode bits for channel 2
		TIMx->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;  // OC1M = 110 for PWM Mode 1 output on ch2
		TIMx->CCMR1	|= TIM_CCMR1_OC2PE;                     					// Output 1 preload enable (make CCR2 value changable)	
		TIMx->CCR1  = ccVal; 															// Output Compare Register for channel 2 (default duty ratio = 50%)		
		TIMx->CCER &= ~TIM_CCER_CC2P;                      				// select output polarity: active high	
		TIMx->CCER  |= TIM_CCER_CC2E;															// Enable output for ch2
	}
	else if(chN == 3){
		TIMx->CCMR2 &= ~TIM_CCMR2_OC3M;                     // Clear ouput compare mode bits for channel 3
		TIMx->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;  // OC1M = 110 for PWM Mode 1 output on ch3
		TIMx->CCMR2	|= TIM_CCMR2_OC3PE;                    					// Output 1 preload enable (make CCR3 value changable)	
		TIMx->CCR1  = ccVal; 												// Output Compare Register for channel 3 (default duty ratio = 50%)		
		TIMx->CCER &= ~TIM_CCER_CC3P;                       				// select output polarity: active high	
		TIMx->CCER  |= TIM_CCER_CC3E;										// Enable output for ch3	
	}
	else if(chN == 4){
		TIMx->CCMR2 &= ~TIM_CCMR2_OC4M;                     // Clear ouput compare mode bits for channel 4
		TIMx->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;  // OC1M = 110 for PWM Mode 1 output on ch4
		TIMx->CCMR2	|= TIM_CCMR2_OC4PE;                    					// Output 1 preload enable (make CCR4 value changable)
		TIMx->CCR1  = ccVal; 												// Output Compare Register for channel 4 (default duty ratio = 50%)
		TIMx->CCER &= ~TIM_CCER_CC4P;                       				// select output polarity: active high
		TIMx->CCER  |= TIM_CCER_CC4E;										// Enable output for ch4
	}	
	
	
// 5. Enable Timer Counter
	// For TIM1 ONLY
	if(TIMx == TIM1) TIMx->BDTR |= TIM_BDTR_MOE;					// Main output enable (MOE): 0 = Disable, 1 = Enable	
	// Enable timers
	TIMx->CR1  |= TIM_CR1_CEN;  													// Enable counter
	
}

// ...existing code from EC/include/ecPWM2.c...