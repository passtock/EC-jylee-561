#include "ecGPIO2.h"
//#include "ecSysTick2.h"
#include "ecEXTI2.h"


void EXTI_init(PinName_t pinName, int trig_type,int priority){

	GPIO_TypeDef *port;
	unsigned int pin;
	ecPinmap(pinName,&port,&pin);
	// SYSCFG peripheral clock enable	
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;		
	
	// Connect External Line to the GPIO
	int EXTICR_port;
	if		(port == GPIOA) EXTICR_port = 0;
	else if	(port == GPIOB) EXTICR_port = 1;
	else if	(port == GPIOC) EXTICR_port = 2;
	else if	(port == GPIOD) EXTICR_port = 3;
	else 										EXTICR_port = 4;
	
	SYSCFG->EXTICR[pin >> 2] &= ~(0xF << (4 * (pin & 0x3)));			// clear 4 bits
	SYSCFG->EXTICR[pin >> 2] |= (EXTICR_port << (4 * (pin & 0x3)));	// set 4 bits
	
	// Configure Trigger edge
	if (trig_type == FALL) EXTI->FTSR |= 1 << pin;   // Falling trigger enable 
	else if	(trig_type == RISE) EXTI->RTSR |= 1 << pin;   // Rising trigger enable 
	else if	(trig_type == BOTH) {			// Both falling/rising trigger enable
		EXTI->RTSR |= 1 << pin; 
		EXTI->FTSR |= 1 << pin;
	} 
	
	// Configure Interrupt Mask (Interrupt enabled)
	EXTI->IMR  |= 1 << pin;     // not masked
	
	
	// NVIC(IRQ) Setting
	int EXTI_IRQn = 0;
	
	if (pin < 5) 	EXTI_IRQn = EXTI0_IRQn + pin;
	else if	(pin < 10) 	EXTI_IRQn = EXTI9_5_IRQn;
	else 			EXTI_IRQn = EXTI15_10_IRQn;
								
	NVIC_SetPriority(EXTI_IRQn, priority);	// EXTI priority
	NVIC_EnableIRQ(EXTI_IRQn); 	// EXTI IRQ enable
}


void EXTI_enable(PinName_t pinName) {
	//Todo Port, Pin Configuration 
	EXTI->IMR |= 1 << pinName;     // not masked (i.e., Interrupt enabled)
}
void EXTI_disable(PinName_t pinName) {
	//Todo Port, Pin Configuration 
	EXTI->IMR |= 1 << pinName;     // masked (i.e., Interrupt disabled)
}

uint32_t is_pending_EXTI(PinName_t pinName) {
	//Todo Port, Pin Configuration 
	uint32_t EXTI_PRx = EXTI->PR;     	// check  EXTI pending 	
	return ((EXTI->PR & (1 << pinName)) == (1UL << pinName));
}


void clear_pending_EXTI(PinName_t pinName) {
	//Todo Port, Pin Configuration 
	EXTI->PR |= 0 << pinName;     // clear EXTI pending 
}
