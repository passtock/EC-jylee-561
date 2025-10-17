#include "ecPinNames.h"
#include "ecGPIO2.h"
#include "ecSysTick2.h"
#include "ecRCC2.h"
#include "ecPWM2.h"

// 서보모터 PWM 제어 (예: PA_0)
#define PWM_PIN 15

void setup(void);

int main(void) {
    setup();
    
    while (1) {
        
        for (float angle = 0; angle <= 10; angle += 0.1) {
            float pulse = angle / 3.0f * 2.0f + 0.5f;
            PWM_pulsewidth_ms(PWM_PIN, pulse); // 0.5ms ~ 2.5ms
            delay_ms(50);
        }
        
    }
}

void setup(void) {
    RCC_PLL_init();
    SysTick_init();

    GPIO_init(PWM_PIN, 2);        // PA_15를 AF 모드로
    PWM_init(PWM_PIN);            
    PWM_period_ms(PWM_PIN, 20);   // 20ms 주기 (서보모터 표준)
}
// ...existing code from EC/lab/LAB_PWM/LAB_PWM_RCmotor.c...