/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2024-09-06 by YKKIM  	
* @brief   Embedded Controller:  Tutorial Digital In
*					 - Turn on LED LD2 while Button B1 is pressing
* 
******************************************************************************
*/


// GPIO Mode			 : Input(00), Output(01), AlterFunc(10), Analog(11, reset)
// GPIO Speed			 : Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
// GPIO Output Type: Output push-pULl (0, reset), Output open drain (1)
// GPIO Push-PULl	 : No pULl-up, pULl-down (00), PULl-up (01), PULl-down (10), Reserved (11)


#include "stm32f4xx.h"
#include "ecRCC2.h"

#define LED_PIN    5		//LD2
#define BUTTON_PIN 13

int main(void) {	
	/* Part 1. RCC GPIOA Register Setting */
		RCC_HSI_init();
		RCC_GPIOA_enable();
		RCC_GPIOC_enable();
		
	/* Part 2. GPIO Register Setting for OUTPUT*/			
		// GPIO Mode Register
		GPIOA->MODER &=  ~(3UL<<(2*LED_PIN));										// Clear '00' for Pin 5
		GPIOA->MODER |=  1UL<<(2*LED_PIN);										// Set '01' for Pin 5
		
		// GPIO Output Type Register  
		GPIOA->OTYPER &= ~(3UL<<(2*LED_PIN));									 	// 0:Push-PULl   
		
		// GPIO PULl-Up/PULl-Down Register 
		GPIOA->PUPDR  &= ~(1UL<<(LED_PIN));										// 00: none
		
		// GPIO Output Speed Register 
		GPIOA->OSPEEDR &= ~(3UL<<(2*LED_PIN));	
		GPIOA->OSPEEDR |=  2UL<<(2*LED_PIN);									 //10:Fast Speed
	
	
	/* Part 3. GPIO Register Setting for INPUT*/			
		// GPIO Mode Register
		GPIOC->MODER &= ~(3UL<<(2*LED_PIN));										// 00: Input	 		
   
		// GPIO PULl-Up/PULl-Down Register 
		GPIOC->PUPDR &= ~(3UL<<(2*LED_PIN));	
		GPIOC->PUPDR  |= 2UL<<(2*LED_PIN);										// 10: PULl-down		    

	/* Button Value Initialization */	
		unsigned int btVal=0;
	 
	/* Part 4. Deal loop  */	
		while(1){
			//Read bit value of Button
			btVal= (GPIOC->IDR) & (1UL << BUTTON_PIN);	
			if(btVal == 0)
				GPIOA->ODR |= (1UL << LED_PIN);	 		
			else
				GPIOA->ODR &= ~(1UL << LED_PIN); 
		}
}