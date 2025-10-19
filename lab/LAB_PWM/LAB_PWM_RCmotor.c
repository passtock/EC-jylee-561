#include "ecPinNames.h"
#include "ecGPIO2.h"
#include "ecSysTick2.h"
#include "ecRCC2.h"
#include "ecPWM2.h"

// 서보모터 PWM 핀 정의 (예: PA_0)
#define PWM_PIN 15

void setup(void);

int main(void) {
    setup();
    
    while (1) {
        
        for (float angle = 0.5; angle <= 2.5; angle += ((2.5f- 0.5f) / 18.0f)) {
            
            PWM_pulsewidth_ms(PWM_PIN, angle); // 0.5ms ~ 2.5ms
            delay_ms(500);
        }
        for(float angle = 2.5; angle >= 0.5; angle -= ((2.5f- 0.5f) / 18.0f)) {
            
            PWM_pulsewidth_ms(PWM_PIN, angle); // 2.5ms ~ 0.5ms
            delay_ms(500);
     }
    }
}
void setup(void) {
    RCC_PLL_init();
    SysTick_init();

    GPIO_init(PWM_PIN, 2);        // PA_15를 AF 모드로
    GPIO_ospeed(PWM_PIN, 3);      // 고속
    GPIO_otype(PWM_PIN, 0);    // 푸시풀
    GPIO_pupd(PWM_PIN, 1);      // 풀업
    
    PWM_init(PWM_PIN);            
    PWM_period_ms(PWM_PIN, 20);   // 20ms 주기 (서보모터 표준)
}