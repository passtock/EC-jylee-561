#include "ecRCC2.h"
#include "ecGPIO2.h"

#define LED_PIN      PB_12
#define BUTTON_PIN   PA_4

void setup(void) {
    RCC_HSI_init();
    GPIO_init(BUTTON_PIN, INPUT);
    GPIO_init(LED_PIN, OUTPUT);
}

int main(void) {
    setup();
    int buttonState = 0;
    int prevButtonState = GPIO_read(BUTTON_PIN);
    int ledState = 0;

    int debounceCounter = 0;
    const int DEBOUNCE_THRESHOLD = 500; 

    while(1){
        buttonState = GPIO_read(BUTTON_PIN);

        if (buttonState == 0 && prevButtonState == 1) {
            debounceCounter = 0; 
        }

        if (buttonState == 0) {
            debounceCounter++;
        }

        if (debounceCounter > DEBOUNCE_THRESHOLD) {
            ledState = !ledState;
            GPIO_write(LED_PIN, ledState);
            
            while(GPIO_read(BUTTON_PIN) == 0) {} 

            debounceCounter = 0;
        }

        prevButtonState = buttonState;
    }
}