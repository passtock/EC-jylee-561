/**
  ******************************************************************************
  * @author  SSSLAB
	* @Mod		 2024-8-23 by YKKIM  	
  * @brief   Embedded Controller:  Tutorial Digital Out
  *					 - Turn on LED LD2
  * 
  ******************************************************************************
*/
// GPIO Mode			 : Input(00), Output(01), AlterFunc(10), Analog(11, reset)
// GPIO Speed			 : Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
// GPIO Output Type: Output push-pULl (0, reset), Output open drain (1)
// GPIO Push-PULl	 : No pULl-up, pULl-down (00), PULl-up (01), PULl-down (10), Reserved (11)
#include "stm32f4xx.h"
#include "ecRCC2.h"

#define LED_PIN  5   //LD2

int main(void) {	
		/* Part 1. RCC GPIOA Register Setting */
		RCC_HSI_init();
		RCC_GPIOA_enable();
		
		/* Part 2. GPIO Register Setting */			
		// GPIO Mode Register
		GPIOA->MODER &= ~(3UL<<(2*LED_PIN));											// Clear '00' for Pin 5
		GPIOA->MODER |= 1UL<<(2*LED_PIN); 											// Set '01' for Pin 5
		
		// GPIO Output Type Register  
		GPIOA->OTYPER &= ~(3UL<<(2*LED_PIN));									// Clear '00'   
		
			
		// GPIO PULl-Up/PULl-Down Register 
		GPIOA->PUPDR  &= ~(3<<(LED_PIN *2));					// 00: none
		
		// GPIO Output Speed Register 
		GPIOA->OSPEEDR &= ~(3UL<<(2*LED_PIN));
		GPIOA->OSPEEDR |=  2UL<<(2*LED_PIN); 										//10:Fast Speed
	
		// Dead loop & program hangs here
		while (1) {
			//	 GPIOA->ODR = 1UL << LED_PIN; 	// Set LED_PIN = H, others=L
			GPIOA->ODR = 1 << LED_PIN;	 		// Change only LED_PIN = H  
		}
}