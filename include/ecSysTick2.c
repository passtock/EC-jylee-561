#include "ecSysTick2.h"



#define MCU_CLK_PLL 84000000
#define MCU_CLK_HSI 16000000

volatile uint32_t msTicks;
volatile uint32_t count = 0;

//EC_SYSTEM_CLK

void SysTick_init(void){	
	//  SysTick Control and Status Register
	SysTick->CTRL = 0;											// Disable SysTick IRQ and SysTick Counter

	// Select processor clock
	// 1 = processor clock;  0 = external clock
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

	// uint32_t MCU_CLK=EC_SYSTEM_CLK
	// SysTick Reload Value Register
	SysTick->LOAD = MCU_CLK_PLL / 1000 - 1;						// 1ms, for HSI PLL = 84MHz.

	// SysTick Current Value Register
	SysTick->VAL = 0;

	// Enables SysTick exception request
	// 1 = counting down to zero asserts the SysTick exception request
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	// Enable SysTick IRQ and SysTick Timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		
	NVIC_SetPriority(SysTick_IRQn, 16);		// Set Priority to 1
	NVIC_EnableIRQ(SysTick_IRQn);			// Enable interrupt in NVIC
}



void SysTick_Handler(void){
	SysTick_counter();	
}

// Removed duplicate definition of SysTick_counter



// msTicks는 전역(volatile uint32_t)으로 정의되어 있고,
// SysTick_Handler()에서 1ms마다 증가한다고 가정합니다.
// volatile uint32_t msTicks; 

void delay_ms (uint32_t mesc){ // 매개변수 이름을 mesc로 변경 (msTicks와 유사하게)
    uint32_t curTicks;

   
    curTicks = msTicks;

    while ((msTicks - curTicks) < mesc);
}


void SysTick_reset(void)
{
	// SysTick Current Value Register
	SysTick->VAL = 0;
}

uint32_t SysTick_val(void) {
	return SysTick->VAL;
}

void SysTick_counter() {
	msTicks++;
	if (msTicks % 1000 == 0) {
		count++;
	}
}
