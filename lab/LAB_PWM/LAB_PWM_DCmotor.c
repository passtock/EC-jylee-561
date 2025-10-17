#include "stm32f411xe.h"
#include "math.h"

// #include "ecSTM32F411.h"
#include "ecPinNames.h"
#include "ecGPIO2.h"
#include "ecSysTick2.h"
#include "ecRCC2.h"
#include "ecTIM2.h"
#include "ecPWM2.h"   // ecPWM2.h


// Definition Button Pin & PWM Port, Pin
#define BUTTON_PIN 13
#define PWM_PIN 0
void setup(void);


int main(void) {
	// Initialization --------------------------------------------------
	setup();	
	
	// Infinite Loop ---------------------------------------------------
	while(1){
		for (int i=0; i<5; i++) {						
			PWM_duty(PWM_PIN, (float)0.2*i);
            PWM_pulsewidth_ms(PWM_PIN, 20.0*0.2*i);			
			delay_ms(1000);
		}		
	}
}


// Initialiization 
void setup(void) {	
	RCC_PLL_init();
	SysTick_init();
		
	// PWM of 20 msec:  TIM2_CH1 (PA_5 AFmode)
	GPIO_init(5, 2);
	PWM_init(PWM_PIN);	
	PWM_period(PWM_PIN, 20);   // 20 msec PWM period
}
// ...existing code from EC/lab/LAB_PWM/LAB_PWM_DCmotor.c...