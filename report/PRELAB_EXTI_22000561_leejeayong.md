# PreLAB: External Interrupt

Name:leejeayong

ID: 22000561

## I. Introduction

In this tutorial, we will learn how to use External Interrupt. We will create functions that capture the falling edge trigger by pushing a button using an external interrupt.

The objectives of this tutorial are how to

* Configure External input (EXTI) interrupt with NVIC
* Create your own functions for configuration of interrupts

### Hardware

* NUCLEO -F411RE

### Software

* VS code, CMSIS, EC\_HAL

### Documentation

* [STM32 Reference Manual](https://ykkim.gitbook.io/ec/stm32-m4-programming/hardware/nucleo-f411re#manual-documentation)

## II.Basics of External Interrupt (EXTI)

### A. Register List

List of external interrupt (EXTI) registers used in this tutorial \[Reference Manual ch7, ch10.2]

![Register List](https://raw.githubusercontent.com/LeeJunjae1/EC_22000573/main/img/exti.png)

### B. Register Setting

**(Digital Input Setting)**

* Enable GPIO peripheral clock **RCC->AHB1ENR**
* Configure DigitalIn pin

**(EXTI Setting)**

* Enable SYSCFG peripheral clock. **RCC->APB2ENR**
* Connect the corresponding external line to GPIO **SYSCFG->EXTICR**
* Configure the trigger edge. **EXTI->FTSR/RTSR**
* Configure Interrupt mask **EXTI->IMR**
* Enable EXTI. **EXTI->IMR**

**(NVIC Setting)**

* Configure the priority of EXTI interrupt request. **NVIC\_SetPriority(**)
* Enable EXTI interrupt request. **NVIC\_EnableIRQ(**)

**(EXTI Use)**

* Create user codes in handler **EXTIx\_IRQHandler()**
* Clear pending bit after interrupt call

##

## III. Tutorial

### A. Register Configuration

Fill in the blanks below

1. **Pin Initialization & Set LED and Push-button**

* LED Pin : Port B Pin 12 / Output / Push-Pull / No Pull-Up & No Pull-Down
* Push-Button:  Port A Pin 4 / Input / No Pull-Up & No Pull-Down

```
// Use your library  
GPIO_init(pin12, 1)
GPIO_otype(pin12, 0)
GPIO_pupd(pin12, 0)

GPIO_init(pin4, 0)
GPIO_pupd(pin4, 0)


```

2. **Enable Peripheral Clock:** SYSCFGEN

* **RCC\_APB2ENR:** Enable SYSCFG

![Image](https://github.com/user-attachments/assets/0a3c512f-3746-4d00-a980-ca4ce915d1a4)

3. **EXTI Initialization & Connect Push-button to EXTI line**

* **SYSCFG\_EXTICR2:** Connect PA\_4(push-button) to EXTI4 line

![Image](https://github.com/user-attachments/assets/a5485597-5b6c-4f85-811f-df5843dc96ab))

* **EXTI\_FTSR:** Enable Falling Trigger

![Image](https://github.com/user-attachments/assets/ab9afc70-734e-470e-8fdf-0db1584f3c6e))

* **EXTI\_IMR:** Interrupt NOT masked (Enable)

![Image](https://github.com/user-attachments/assets/0d6d4fa9-ec8c-47e7-b202-82007c395a75)

### B. Programming

This is an example code for toggling LED on/off with the button input trigger (EXTI)&#x20;

Fill in the empty spaces in the code.

#### Procedure

* Name the project as `TU_EXTI` by creating a new folder as `tutorial\TU_EXTI`

* Download the template code
  * `TU_EXTI_student.c` [Click here to download](https://github.com/ykkimhgu/EC-student/tree/main/tutorial/tutorial-student)

* Fill in the empty spaces in the code.

* Run the program and check your result.

* Your tutorial report must be submitted to LMS

{% hint style="info" %}
DO NOT use `ecEXTI2_student.h`  for this tutorial.
{% endhint %}

> You MUST write your name on the source file inside the comment section

```c
/**
******************************************************************************
* @author  SSSLAB
* @Mod       2025-9-25 by YKKIM
* @brief   Embedded Controller:  Tutorial ___
*leejeayong
******************************************************************************
*/
//#include "ecSTM32F4v2.h"
#include "ecRCC2.h"
#include "ecGPIO2.h"
  
#define LED_PIN   PB_12         //EVAL board JKIT
#define BUTTON_PIN PA_4         //EVAL board JKIT
  
void LED_toggle(PinName_t pinName);
void EXTI_init_tutorial(PinName_t pinName);
  
// Initialiization
void setup(void)
{
    RCC_PLL_init();                         // System Clock = 84MHz
    // Initialize GPIOB_12 for Output
    GPIO_init(LED_PIN, OUTPUT);    // LED for EVAL board    
    // Initialize GPIOA_4 for Input Button
    GPIO_init(BUTTON_PIN, INPUT);  // OUTPUT for EVAL borad
    EXTI_init_tutorial(PA_4);
}
// MAIN  ----------------------------------------
int main(void) {
    setup();
    while (1);
}
// EXTI Initialiization ------------------------------------------------------  
void EXTI_init_tutorial(PinName_t pinName)
{
    GPIO_TypeDef *Port;
    unsigned int pin;
    ecPinmap(pinName, &Port, &pin);
    // SYSCFG peripheral clock enable
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    // Connect External Line to the GPIO
    // Button: PA_4 -> EXTICR2(EXTI4)
    SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI4;
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PA;
    // Falling trigger enable (Button: pull-up)
    EXTI->FTSR |= (1UL << 4);
    // Unmask (Enable) EXT interrupt
    EXTI->IMR |= (1UL << 4);
    // Interrupt IRQn, Priority
    NVIC_SetPriority(EXTI4_IRQn, 0);        // Set EXTI priority as 0  
    NVIC_EnableIRQ(EXTI4_IRQn);             // Enable EXTI
}
void EXTI4_IRQHandler(void) {
    if ((EXTI->PR & EXTI_PR_PR4) == EXTI_PR_PR4) {
        LED_toggle(LED_PIN);
        EXTI->PR |= EXTI_PR_PR4; // cleared by writing '1'
    }
}

void LED_toggle(PinName_t pinName){
    GPIO_TypeDef *Port;
    unsigned int pin;
    ecPinmap(pinName,&Port,&pin);
    // YOUR CODE GOES HERE
    GPIO_write(pinName, !GPIO_read(pinName));
}
```

## results
![Image](https://github.com/user-attachments/assets/31b842b4-397c-442a-9a9d-172f5ca9316a)


![Image](https://github.com/user-attachments/assets/88e9e393-6eee-42d2-8af5-773418512910)


![Image](https://github.com/user-attachments/assets/ece0f969-c216-4b0c-b42f-2bf2cfbc649a)