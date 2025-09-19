/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : SSS LAB
Created          : 05-03-2021
Modified         : 09-16-2025
Language/ver     : C++ in Keil uVision
name			 : leejeayong
number		     : 22000561

Description      : Distributed to Students for LAB_GPIO
/----------------------------------------------------------------*/



#include "stm32f4xx.h"
#include "stm32f411xe.h"
#include "ecGPIO2.h"

void GPIO_init(PinName_t pinName, uint32_t mode){     
	GPIO_TypeDef * Port;
	unsigned int pin;
	ecPinmap(pinName, &Port, &pin);
	
	// mode  : Input(0), Output(1), AlterFunc(2), Analog(3)   
	if (Port == GPIOA)
		RCC_GPIOA_enable();
	if (Port == GPIOB)
		RCC_GPIOB_enable();
	if (Port == GPIOC)
		RCC_GPIOC_enable();
	

	
	//[TO-DO] YOUR CODE GOES HERE
	// Make it for GPIOB, GPIOD..GPIOH

	// You can also make a more general function of
	// void RCC_GPIO_enable(GPIO_TypeDef *Port); 
	

	GPIO_mode(pinName, mode);
}
void GPIO_write(PinName_t pinName, int Output){
	GPIO_TypeDef * Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	if(Output) Port->BSRR = 1UL << pin;  // Set bit
	else       Port->BSRR = 1UL << (pin+16); // Reset bit
}


// GPIO Mode          : Input(00), Output(01), AlterFunc(10), Analog(11)
void GPIO_mode(PinName_t pinName, uint32_t mode){
   GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
   Port->MODER &= ~(3UL<<(2*pin));     
   Port->MODER |= mode<<(2*pin);  
     
}


// GPIO Speed          : Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
void GPIO_ospeed(PinName_t pinName, int speed){
	GPIO_TypeDef * Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	Port->OSPEEDR &= ~(3UL<<(2*pin));     
	Port->OSPEEDR |= speed<<(2*pin);
}

// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
void GPIO_otype(PinName_t pinName, int type){
   	GPIO_TypeDef * Port;
    unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	Port->OTYPER &= ~(1UL<<pin);     
	Port->OTYPER |= type<<pin;
}

// GPIO Push-Pull    : No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
void GPIO_pupd(PinName_t pinName, int pupd){
   	GPIO_TypeDef * Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	Port->PUPDR &= ~(3UL<<(2*pin));     
	Port->PUPDR|= pupd <<(2*pin);
}

int GPIO_read(PinName_t pinName){
	GPIO_TypeDef * Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	return (Port->IDR & (1UL << pin));
	
}
// Initialize DOUT pins for 7 segment leds
void seven_seg_FND_init(void){	
    //pin name array
    PinName_t pinsFND[12]={PB_7, PB_6, PB_5, PB_4, PB_3, PB_2, PB_1, PB_0, PC_3, PC_4, PA_11, PA_10};

	
    //Iteratively initializing DOUT pins for pinsFND
    for(int i=0; i<12; i++){
        GPIO_init(pinsFND[i], OUTPUT); // Set as Output
        GPIO_write(pinsFND[i], LOW); // turn off all segments and FNDs
    }
}
void seven_seg_FND_display(uint8_t  num, uint8_t select){
	PinName_t pinsSEG[8] = {PB_7, PB_6, PB_5, PB_4, PB_3, PB_2, PB_1, PB_0};
	PinName_t pinsFNDselect[4] = {PA_10, PA_11,PC_4, PC_3};
  
    int segment[10][8] = {
		{0, 0, 1, 1, 1, 1, 1, 1}, // 0
		{0, 0, 0, 0, 0, 1, 1, 0}, // 1
		{0, 1, 0, 1, 1, 0, 1, 1}, // 2
		{0, 1, 0, 0, 1, 1, 1, 1}, // 3
		{0, 1, 1, 0, 0, 1, 1, 0}, // 4
		{0, 1, 1, 0, 1, 1, 0, 1}, // 5
		{0, 1, 1, 1, 1, 1, 0, 1}, // 6
		{0, 0, 0, 0, 0, 1, 1, 1}, // 7
		{0, 1, 1, 1, 1, 1, 1, 1}, // 8
		{0, 1, 1, 0, 1, 1, 1, 1}  // 9
	};

	GPIO_write(pinsFNDselect[select], HIGH);
	for(int i=0; i<8; i++){
		if(segment[num][i]==1){
			GPIO_write(pinsSEG[i], HIGH); // turn on segment
		}else{
			GPIO_write(pinsSEG[i], LOW); // turn off segment
		}
	}

	
}
