#include "stm32f411xe.h"
#include "math.h"

// #include "ecSTM32F411.h"
#include "ecPinNames.h"
#include "ecGPIO2.h"
#include "ecSysTick2.h"
#include "ecRCC2.h"
#include "ecTIM2.h"
#include "ecPWM2.h"   // ecPWM2.h

// Pin Definitions
#define BUTTON_PIN 45
#define PWM_PIN 0
#define in1 PB_10

// Function Prototypes
void setup(void);
void motor_stop(void);

// Global variables to manage motor state without blocking
int motor_mode = 0; // 0: STOP, 1: RUN
static int run_state = 0; // Internal state for RUN mode (0: speed 1, 1: speed 2)
static uint32_t last_speed_change_time = 0; // Timestamp for the last speed change

int main(void) {
	// Initialization
	setup();
    
	int button_prev_state = 1;

	while(1) {
			// --- 1. Handle button (non-blocking) ---
		int button_curr_state = GPIO_read(BUTTON_PIN);
        
	// Detect button press (falling edge)
		if (button_curr_state == 0 && button_prev_state == 1) {
			motor_mode = 1 - motor_mode; // toggle mode (RUN <-> STOP)
            
			// reset run state and timer when mode changes
			run_state = 0; 
			last_speed_change_time = millis();
            
			delay_ms(50); // debouncing
		}
		button_prev_state = button_curr_state;

	// --- 2. Motor logic (based on current state) ---
		if (motor_mode == 1) { // when in RUN mode
			uint32_t current_time = millis();

			// Non-blocking logic: change speed every 2 seconds
			if (current_time - last_speed_change_time >= 2000) {
				run_state = 1 - run_state; // toggle 0 <-> 1
				last_speed_change_time = current_time; // update reference time
			}

			// Set PWM duty according to run_state
			if (run_state == 0) {
				PWM_duty(PWM_PIN, 0.3f); // 25% duty
			} else {
				PWM_duty(PWM_PIN, 0.8f); // 75% duty
			}

		} else { // STOP 
			motor_stop();
		}
	}
}

// Initialization function
void setup(void) {
	RCC_PLL_init();
	SysTick_init(); // millis() 
    
	// GPIO and PWM initialization
	GPIO_init(BUTTON_PIN, INPUT);
	GPIO_pupd(BUTTON_PIN, 1);
	GPIO_init(in1, OUTPUT);
	GPIO_write(in1, 0);
    
	PWM_init(PWM_PIN);
	PWM_period(PWM_PIN, 1);
}

// Motor stop function
void motor_stop(void){
	PWM_duty(PWM_PIN, 0.0f);
}