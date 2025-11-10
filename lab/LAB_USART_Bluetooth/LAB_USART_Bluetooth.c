#include "ecSTM32F4v2.h"
// this should include #include "ecUART2.h" 
//#include "ecUART2.h" 

static volatile uint8_t PC_Data = 0;
static volatile uint8_t BT_Data = 0;
uint8_t PC_string[]="Hi_BT\r\n";
#define LEDPIN PA_5
#define dir PA_1
#define motor1 PA_0
#define motor2 PA_2
#define dir2 PA_3

void setup(void){
	RCC_PLL_init();
	SysTick_init();
	
    GPIO_init(LEDPIN, OUTPUT);    // LED 핀 초기화
    GPIO_write(LEDPIN, 0);        // LED OFF
    GPIO_init(dir, OUTPUT);       // 모터 방향 제어 핀 초기화
    GPIO_write(dir, 0);     
      // 
	// USART1: BT serial init 
	UART1_init();
	UART1_baud(BAUD_9600);
    UART2_init();

    PWM_init(motor1);
    PWM_period(motor1, 10); // 10ms period
    PWM_duty(motor1, 0.0f); // 초기 듀티 사이클 0%
    

}

int main(void){	
	setup();
	printf("MCU Initialized!!\r\n");	
	while(1){
		// USART Receive: Use Interrupt only
		// USART Transmit:  Interrupt or Polling
		USART1_write(PC_string, 7);
		delay_ms(2000);        
	}
}
void USART1_IRQHandler(void){
    if(USART_SR_RXNE & USART1->SR){  // RXNE 플래그 확인
        BT_Data = USART1_read();
        if(BT_Data == 'w'){   // 숫자 문자 '1' 수신
            PWM_duty(motor1, 0.75f); // 듀티 사이클 50%

        }
        else if(BT_Data == 'a'){  //
            PWM_duty(motor1, 0.75f); // 듀티 사이클 50%
        }
        else if(BT_Data == 'd'){  //
            PWM_duty(motor1, 0.15f); // 듀티 사이클 25%
        }
        else if(BT_Data == 's'){  // 숫자 문자 '0' 수신
            PWM_duty(motor1, 0.0f); // 듀티 사이클 0%
        }
        // 추가적으로 다른 숫자 처리 가능
    }
}
