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
	uint8_t segs[10]={
		// 01234567
		0b11111100, // 0
		0b01100000, // 1
		0b11011010, // 2
		0b11110010, // 3
		0b01100110, // 4
		0b10110110, // 5
		0b10111110, // 6
		0b11100000, // 7
		0b11111110, // 8
		0b11110110  // 9
	};
	for(int j =0; j<8; j++){
		GPIO_write(pinsSEG[j], (segs[num] >> j) & 0x01); // turn on/off segment
	}
	GPIO_write(pinsFNDselect[select], HIGH);
}
volatile uint8_t multiplex_state = 0;



void sevensegment_display_19(uint8_t num){
    // 7세그먼트 세그먼트 핀 (a-g, dp)
    PinName_t pinsSEG[8] = {PB_7, PB_6, PB_5, PB_4, PB_3, PB_2, PB_1, PB_0};
    // 7세그먼트 선택 핀: pinsFNDselect[0] = LSD (일의 자리, PA_10), pinsFNDselect[1] = MSD (십의 자리, PA_11)
    PinName_t pinsFNDselect[4] = {PA_10, PA_11, PC_4, PC_3}; 
    // num이 0~9일 때는 LSD(PA_10)만 사용, num이 10~19일 때는 LSD(PA_10)와 MSD(PA_11) 모두 사용
    // 가정: MSD = pinsFNDselect[1] (PA_11), LSD = pinsFNDselect[0] (PA_10)
    // 세그먼트 디코딩 테이블 (PB_7 to PB_0 순서에 맞게)
    // 0bDPGFEDCBA
    const uint8_t segs[10]={
        0b11111100, // 0
        0b01100000, // 1
        0b11011010, // 2
        0b11110010, // 3
        0b01100110, // 4
        0b10110110, // 5
        0b10111110, // 6
        0b11100000, // 7
        0b11111110, // 8
        0b11110110  // 9
    };
    
    // 1. 디스플레이를 모두 끕니다 (잔상 방지)
    // 두 자릿수만 사용
    GPIO_write(pinsFNDselect[0], LOW); // LSD 끄기
    GPIO_write(pinsFNDselect[1], LOW); // MSD 끄기

    // 2. 숫자를 LSD와 MSD로 분리합니다.
    int digit_lsd = num % 10; // 일의 자리
    int digit_msd = num / 10; // 십의 자리 (0 또는 1)

    // 3. 멀티플렉싱 상태에 따라 출력합니다.
    
    if (multiplex_state == 0) {
        // --- 일의 자리 (LSD) 출력 ---
        
        // 세그먼트 데이터 출력 (LSD)
        uint8_t segment_data = segs[digit_lsd];
        for(int j = 0; j < 8; j++){
            GPIO_write(pinsSEG[j], (segment_data >> j) & 0x01); // 세그먼트 핀 설정
        }

        // LSD (PA_10) 활성화
        GPIO_write(pinsFNDselect[0], HIGH);
        
        // 다음 상태는 MSD
        multiplex_state = 1;
        
    } else {
        // --- 십의 자리 (MSD) 출력 ---
        
        if (num >= 10) {
            // 10 이상일 때만 MSD(십의 자리)를 표시합니다 (항상 1)
            
            // 세그먼트 데이터 출력 (MSD: 항상 1)
            uint8_t segment_data = segs[digit_msd]; // digit_msd는 항상 1
            for(int j = 0; j < 8; j++){
                GPIO_write(pinsSEG[j], (segment_data >> j) & 0x01); // 세그먼트 핀 설정
            }
            
            // MSD (PA_11) 활성화
            GPIO_write(pinsFNDselect[1], HIGH);
            
        } else {
            // 0~9일 때는 십의 자리를 비활성화 상태로 유지하거나 0을 표시합니다.
            // 여기서는 이미 두 디스플레이가 LOW 상태이므로 추가적인 활성화는 하지 않습니다.
            // 대신, 세그먼트 핀을 모두 꺼서 잔상을 방지할 수 있습니다.
            for(int j = 0; j < 8; j++){
                GPIO_write(pinsSEG[j], LOW); // 세그먼트 핀 모두 끄기
            }
        }
        multiplex_state = 0;
    }
    
    
}