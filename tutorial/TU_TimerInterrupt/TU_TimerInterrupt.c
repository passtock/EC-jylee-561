/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2024-8-23 by YKKIM  	
* @brief   Embedded Controller:  Tutorial _____
*					 - ____________________
* 
******************************************************************************
*/
#include "stm32f411xe.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"

void LED_toggle(PinName_t pinName);

PinName_t LED_pin = 5;

void setup(void);
	
int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	TIM_TypeDef* timerx;
	timerx = TIM2;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	timerx->PSC = 8399;				// Prescaler: 84MHz / (8399 + 1) = 10kHz
	timerx->ARR = 9999;				// Auto-Reload: 10000 counts @ 10kHz = 1s
	timerx->DIER |=  1UL << 0;                   	// Enable Interrupt
	timerx->CR1 |=  1UL << 0;                    	// Enable counter
	
	NVIC_SetPriority(TIM2_IRQn, 2);               	// TIM2_IRQHandler Set priority as 2
	NVIC_EnableIRQ(TIM2_IRQn);			// TIM2_IRQHandler Enable
	
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){}
}

// Initialiization 
void setup(void)
{	
	RCC_PLL_init();                       // System Clock = 84MHz
	GPIO_init(LED_pin, OUTPUT);    // calls RCC_GPIOA_enable()	
}

void TIM2_IRQHandler(void){
	if((TIM2->SR & TIM_SR_UIF) == TIM_SR_UIF){ // update interrupt flag
		// Toggle LED
		LED_toggle(LED_pin);
		TIM2->SR &= ~TIM_SR_UIF;                 // clear by writing 0
	}
}

void LED_toggle(PinName_t pinName) {
	GPIO_TypeDef* port;
	unsigned int pin;
	ecPinmap(pinName, &port, &pin);
	port->ODR ^= (1UL << pin); // Toggle LED by XORing the ODR bit
}
