#include "stm32f411xe.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"
#include "ecEXTI2.h"
#include "ecSysTick2.h"
#include "ecStepper2.h"

#define BUTTON_PIN   PC_13   // External Interrupt Pin

void setup(void);
	
int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	
	Stepper_step(2048, 1, FULL);  // 2048 steps = 360 degrees (1 revolution) in FULL mode
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
        
    }
}

// Initialiization 
void setup(void){
	
	RCC_PLL_init();                                 // System Clock = 84MHz
	SysTick_init();                                 // Systick init
	
	EXTI_init(BUTTON_PIN, FALL,0);           // External Interrupt Setting
	GPIO_init(BUTTON_PIN, 0);           // GPIOC pin13 initialization

	Stepper_init(PB_10,PB_4,PB_5,PB_3); // Stepper GPIO pin initialization
	Stepper_setSpeed(2);                          	//  set stepper motor speed
}

void EXTI15_10_IRQHandler(void) {  
	if (is_pending_EXTI(BUTTON_PIN)) {
		Stepper_stop();
		clear_pending_EXTI(BUTTON_PIN); // cleared by writing '1'
	}
}
