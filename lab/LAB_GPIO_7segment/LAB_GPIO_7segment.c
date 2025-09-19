#include "stm32f4xx.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"


#define BUTTON_PIN   PA_4


void setup(void){
   GPIO_init(BUTTON_PIN, INPUT);
   GPIO_pupd(BUTTON_PIN, 1); // Pull-up

    // Intialize System Clock
    RCC_HSI_init();
    
    // Intialize DOUT Pins      
    // and Others
    // [YOUR CODE GOES HERE]    
    seven_seg_FND_init(); 
};

int main(void) {
    setup();
    int buttonState = 0;
    int prevButtonState = GPIO_read(BUTTON_PIN);
    

    int debounceCounter = 0;
    const int DEBOUNCE_THRESHOLD = 500; 
    uint8_t numDisplay=8;
    uint8_t selectFND=0;
   while(1){
        buttonState = GPIO_read(BUTTON_PIN);
        seven_seg_FND_display(numDisplay,selectFND);

        if (buttonState == 0 && prevButtonState == 1) {
            debounceCounter = 0; 
        }

        if (buttonState == 0) {
            debounceCounter++;
        }

        if (debounceCounter > DEBOUNCE_THRESHOLD) {
            numDisplay = (numDisplay + 1) % 10; // Cycle through 0-9
            while(GPIO_read(BUTTON_PIN) == 0) {} 
            debounceCounter = 0;
        }

        prevButtonState = buttonState;
    }
   
}


