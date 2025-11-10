#include "ecSTM32F4v2.h"
// #include "ecADC.h"

//IR parameter//
uint32_t value1, value2;
PinName_t seqCHn[2] = {PB_0, PB_1};

void setup(void);

int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
		printf("value1 = %d \r\n",value1);
		printf("value2 = %d \r\n",value2);
		printf("\r\n");
		
		delay_ms(1000);
	}
}

// Initialiization 
void setup(void)
{	
	RCC_PLL_init();                 // System Clock = 84MHz
	UART2_init();			// UART2 Init
	SysTick_init();			// SysTick Init
	
	// ADC Init  Default: HW triggered by TIM3 counter @ 1msec
	JADC_init(PB_0);
	JADC_init(PB_1);

	// ADC channel sequence setting
	JADC_sequence(seqCHn, 2);
}


void ADC_IRQHandler(void){
	// if(is_ADC_OVR()) clear_ADC_OVR();	
	if(is_ADC_JEOC()){		// after finishing sequence ADC
		value1 = JADC_read(1);
		value2 = JADC_read(2);
		clear_ADC_JEOC();
	}
}
