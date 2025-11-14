#include "ecSTM32F4v2.h"
#include "ecADC2.h"
#include "ecPWM2.h"

//IR parameter//
uint32_t value1, value2;
volatile uint8_t adc_ready = 0; // set when a full sequence (both channels) completed
int flag = 0;
PinName_t seqCHn[2] = {PB_0, PB_1};

// Sensor thresholds
#define WHITE_THRESHOLD 400U
#define BLACK_THRESHOLD 3500U
// Motor PWM pins
#define MOTOR_LEFT_PIN  PA_0
#define MOTOR_RIGHT_PIN PA_1

// Set motor speeds: left and right in range 0.0 .. 1.0
static inline void set_speed(float left, float right)
{
	if(left < 0.f) left = 0.f; if(left > 1.f) left = 1.f;
	if(right < 0.f) right = 0.f; if(right > 1.f) right = 1.f;
	PWM_duty(MOTOR_LEFT_PIN, left);
	PWM_duty(MOTOR_RIGHT_PIN, right);
}

void setup(void);

int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
		if(adc_ready){
			uint32_t v1 = value1;
			uint32_t v2 = value2;
			adc_ready = 0;

			printf("IR1 = %lu\r\n", (unsigned long)v1);
			printf("IR2 = %lu\r\n", (unsigned long)v2);

			const char *s1 = (v1 >= BLACK_THRESHOLD) ? "BLACK" : ((v1 <= WHITE_THRESHOLD) ? "WHITE" : "GRAY");
			const char *s2 = (v2 >= BLACK_THRESHOLD) ? "BLACK" : ((v2 <= WHITE_THRESHOLD) ? "WHITE" : "GRAY");
			printf("IR1=%s  IR2=%s\r\n", s1, s2);

			// Mapping: both WHITE -> STRAIGHT
			// IR1 BLACK -> GO RIGHT; IR2 BLACK -> GO LEFT; BOTH BLACK unexpected
			int ir1_white = (v1 <= WHITE_THRESHOLD);
			int ir2_white = (v2 <= WHITE_THRESHOLD);
			int ir1_black = (v1 >= BLACK_THRESHOLD);
			int ir2_black = (v2 >= BLACK_THRESHOLD);

			if(ir1_white && ir2_white){
				printf("GO STRAIGHT\r\n");
				// both white -> go straight
				set_speed(0.65f, 0.65f);
			} else if(ir1_black && !ir2_black){
				printf("GO RIGHT\r\n");
				// IR1 (right sensor) black -> turn right (slow right motor)
				set_speed(0.65f, 0.30f);
			} else if(ir2_black && !ir1_black){
				printf("GO LEFT\r\n");
				// IR2 (left sensor) black -> turn left (slow left motor)
				set_speed(0.30f, 0.65f);
			} else if(ir1_black && ir2_black){
				printf("ERR: BOTH BLACK (unexpected)\r\n");
				// stop on unexpected both-black
				set_speed(0.f, 0.f);
			} else {
				printf("SEARCH\r\n");
				set_speed(0.45f, 0.45f);
			}

			printf("\r\n");
		}

		delay_ms(50);
	}
}

// Initialiization 
void setup(void)
{	
	RCC_PLL_init();                         // System Clock = 84MHz
	UART2_init();				// UART2 Init
	SysTick_init();				// SysTick Init
	
	// ADC Init  Default: HW triggered by TIM3 counter @ 1msec
	ADC_init(PB_0);
	ADC_init(PB_1);

	// ADC channel sequence setting
	ADC_sequence(seqCHn, 2);
	
	/* Set ADC clock prescaler to /8 (ADCPRE = 11b at bits [17:16]) */
	ADC->CCR = (ADC->CCR & ~(3UL << 16)) | (3UL << 16);
	
	// Ensure pins are in analog/no pull state
	GPIO_init(PB_0, 3);
	GPIO_pupd(PB_0, 0);
	GPIO_init(PB_1, 3);
	GPIO_pupd(PB_1, 0);

	// Motor PWM initialization (1kHz)
	PWM_init(MOTOR_LEFT_PIN);
	PWM_init(MOTOR_RIGHT_PIN);
	PWM_period_us(MOTOR_LEFT_PIN, 1000);
	PWM_period_us(MOTOR_RIGHT_PIN, 1000);
	// stop motors initially
	PWM_duty(MOTOR_LEFT_PIN, 0.f);
	PWM_duty(MOTOR_RIGHT_PIN, 0.f);
}


void ADC_IRQHandler(void){
	if(is_ADC_OVR())
		clear_ADC_OVR();
	
	if(is_ADC_EOC()){		// after finishing sequence
		if (flag==0) {
			value1 = ADC_read();
		} else if (flag==1) {
			value2 = ADC_read();
			// second channel done -> mark ready for main loop
			adc_ready = 1;
		}

		flag = !flag;		// flag toggle
	}
}