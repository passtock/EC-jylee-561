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
#define BUTTON_PIN 45
#define PWM_PIN 0
#define in1 PB_10

void setup(void);
void motortoggle(){
	static int state = 0;
	if (state == 0) {
		PWM_duty(PWM_PIN, 0.25f);
		delay_ms(2000);
		PWM_duty(PWM_PIN, 0.75f);
		delay_ms(2000);
		state = 1;
	} else {
		PWM_duty(PWM_PIN, 0.5f); // 50% duty cycle
		state = 0;
	}
}
int main(void) {
	// Initialization --------------------------------------------------
	setup();	
	
	// Infinite Loop ---------------------------------------------------
	while(1){
		int buttonstate = GPIO_read(BUTTON_PIN);
		if (buttonstate == 0) {  // 버튼이 눌렸을 때
			motortoggle();
			delay_ms(500);  // 디바운싱을 위한 지연
}
}
}

// Initialiization 
void setup(void) {	
	RCC_PLL_init();
	SysTick_init();
		
	GPIO_init(0, 2);
	GPIO_init(26, 1);
	GPIO_init(20, 1);
	GPIO_write(in1, 0);
	PWM_init(PWM_PIN);	
	PWM_period(PWM_PIN, 1);   // 20 msec PWM period
}
// ...existing code from EC/lab/LAB_PWM/LAB_PWM_DCmotor.c...