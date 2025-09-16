#include "ecRCC2.h"
#include "ecGPIO2.h"

#define LED_PIN      PB_12
#define LED_PIN2     PB_13
#define LED_PIN3     PB_14
#define LED_PIN4     PB_15
#define BUTTON_PIN   PA_4

void setup(void) {
    RCC_HSI_init();
    GPIO_init(BUTTON_PIN, INPUT);
    GPIO_init(LED_PIN, OUTPUT);
    GPIO_init(LED_PIN2, OUTPUT);
    GPIO_init(LED_PIN3, OUTPUT);
    GPIO_init(LED_PIN4, OUTPUT);

    GPIO_write(LED_PIN, LOW);
    GPIO_write(LED_PIN2, LOW);
    GPIO_write(LED_PIN3, LOW);
    GPIO_write(LED_PIN4, LOW);
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
            ledState++;
            if (ledState > 4) {
                ledState = 0;
            }

            GPIO_write(LED_PIN, LOW);
            GPIO_write(LED_PIN2, LOW);
            GPIO_write(LED_PIN3, LOW);
            GPIO_write(LED_PIN4, LOW);

            if (ledState == 1) {
                GPIO_write(LED_PIN, HIGH);
            } else if (ledState == 2) {
                GPIO_write(LED_PIN2, HIGH);
            } else if (ledState == 3) {
                GPIO_write(LED_PIN3, HIGH);
            } else if (ledState == 4) {
                GPIO_write(LED_PIN4, HIGH);
            }

            while(GPIO_read(BUTTON_PIN) == 0) {} 

            debounceCounter = 0;
        }

        prevButtonState = buttonState;
    }
}