/*
 * Line Tracing program (extended)
 * - Manual mode: UART keyboard control (i/k/j/l) and PA5 LED indicates MANUAL
 * - 'a' switches to AUTO mode: ADC IR-based line tracing
 * - Line-tracing only (ADC IR automatic)
 */

#include "ecSTM32F4v2.h"
#include "ecADC2.h"
#include "ecPWM2.h"
#include "ecICAP2.h"

// IR sensor pins
#define IR_RIGHT_PIN PB_0
#define IR_LEFT_PIN  PB_1

// Motor pins
#define LEFT_MOTOR_PIN  PA_0
#define RIGHT_MOTOR_PIN PA_1



// Bell servo pin

// Ultrasonic removed for line-tracing-only build

// Ultrasonic pins
#define TRIG_PIN   PA_6
#define ECHO_PIN   PB_6

// stop indicator LED
#define STOP_LED PA_5

// Sensor thresholds
#define WHITE_THRESHOLD 400U
#define BLACK_THRESHOLD 3500U

volatile uint32_t ir_right_value = 0;
volatile uint32_t ir_left_value = 0;
PinName_t seqCHn[2] = {IR_RIGHT_PIN, IR_LEFT_PIN};

extern volatile uint32_t msTicks;

// Ultrasonic capture globals
volatile uint32_t us_ovf_cnt = 0;
volatile uint32_t us_time1 = 0;
volatile uint32_t us_time2 = 0;
volatile uint32_t us_timeInterval = 0; // ticks (10us)
volatile uint8_t us_new_data = 0;

// Ultrasonic removed

// bell/servo removed: line-tracing only

static inline void set_speed(float left, float right)
{
    if(left < 0.f) left = 0.f;
    if(left > 1.f) left = 1.f;
    if(right < 0.f) right = 0.f;
    if(right > 1.f) right = 1.f;
    PWM_duty(LEFT_MOTOR_PIN, left);
    PWM_duty(RIGHT_MOTOR_PIN, right);
}

// bell_press removed (servo not used for line tracing)

void setup(void)
{
    RCC_PLL_init();
    SysTick_init();
    UART2_init();

    // ADC init and sequence
    ADC_init(IR_RIGHT_PIN);
    ADC_init(IR_LEFT_PIN);
    ADC_sequence(seqCHn, 2);
    ADC->CCR = (ADC->CCR & ~(3UL << 16)) | (3UL << 16); // ADCPRE = /8

    // Motors (1kHz)
    PWM_init(LEFT_MOTOR_PIN);
    PWM_init(RIGHT_MOTOR_PIN);
    PWM_period_us(LEFT_MOTOR_PIN, 100);
    PWM_period_us(RIGHT_MOTOR_PIN, 100);
    set_speed(0.f, 0.f);
    // stop indicator LED
    GPIO_init(STOP_LED, OUTPUT);
    GPIO_write(STOP_LED, 0);

    // Ultrasonic TRIG (GPIO) + ECHO (ICAP)
    GPIO_init(TRIG_PIN, OUTPUT);
    GPIO_write(TRIG_PIN, 0);

    GPIO_init(ECHO_PIN, EC_AF);
    GPIO_pupd(ECHO_PIN, EC_PD);

    ICAP_init(ECHO_PIN);
    ICAP_counter_us(ECHO_PIN, 10);    // 10 us tick
    ICAP_setup(ECHO_PIN, 1, IC_RISE);
    ICAP_setup(ECHO_PIN, 2, IC_FALL);

    // clear and enable TIM4 IRQ for input capture
    clear_UIF(TIM4);
    clear_CCIF(TIM4, 1);
    clear_CCIF(TIM4, 2);
    NVIC_EnableIRQ(TIM4_IRQn);
    NVIC_SetPriority(TIM4_IRQn, 2);
}

// ADC IRQ handler
void ADC_IRQHandler(void)
{
    static int idx = 0;
    if(is_ADC_EOC()){
        uint32_t v = ADC_read();
        if(idx == 0) ir_right_value = v; else ir_left_value = v;
        idx = (idx + 1) & 1;
    }
}

// TIM4 IRQ for ultrasonic input-capture
void TIM4_IRQHandler(void)
{
    // overflow
    if(is_UIF(TIM4)){
        clear_UIF(TIM4);
        if(1) us_ovf_cnt++;
    }

    // rising edge (IC1)
    if(is_CCIF(TIM4, 1)){
        clear_CCIF(TIM4, 1);
        us_time1 = ICAP_capture(TIM4, 1);
        us_ovf_cnt = 0;
    }

    // falling edge (IC2)
    if(is_CCIF(TIM4, 2)){
        clear_CCIF(TIM4, 2);
        us_time2 = ICAP_capture(TIM4, 2);
        us_timeInterval = (us_ovf_cnt * 65536UL) + us_time2 - us_time1;
        us_new_data = 1;
    }
}

int main(void)
{
    setup();

    const float SPEED_FORWARD = 1.0f;
    const float SPEED_TURN    = 0.00f;

    // modes removed; continuous automatic line tracing with ultrasonic stop

    uint32_t last_trig_ms = 0;
    const uint32_t TRIG_PERIOD_MS = 60; // trigger every 60 ms
    uint8_t stopped = 0;

    while(1){
            // print current IR sensor RAW values
            printf("IR_RAW: R=%lu  L=%lu\r\n", (unsigned long)ir_right_value, (unsigned long)ir_left_value);

            // Trigger ultrasonic periodically
            if ((msTicks - last_trig_ms) >= TRIG_PERIOD_MS) {
                last_trig_ms = msTicks;
                // send single 10us pulse
                GPIO_write(TRIG_PIN, 1);
                for (volatile int i = 0; i < 84; i++); // approx 10us (calibrate if needed)
                GPIO_write(TRIG_PIN, 0);
            }

            // if stopped by obstacle, skip line-tracing control
            if (stopped == 0) {
                // Continuous line-tracing logic (no modes)
                int ir1_white = (ir_right_value <= WHITE_THRESHOLD);
                int ir2_white = (ir_left_value <= WHITE_THRESHOLD);
                int ir1_black = (ir_right_value >= BLACK_THRESHOLD);
                int ir2_black = (ir_left_value >= BLACK_THRESHOLD);

                if(ir1_white && ir2_white){
                    // both white -> straight
                    set_speed(SPEED_TURN, SPEED_TURN);
                } else if(ir1_black && !ir2_black){
                    // right sensor sees black -> go right (right motor stronger)
                    set_speed(SPEED_TURN, SPEED_FORWARD);
                } else if(ir2_black && !ir1_black){
                    // left sensor sees black -> go left (left motor stronger)
                    set_speed(SPEED_FORWARD, SPEED_TURN);
                } else if(ir1_black && ir2_black){
                    // both black -> treat as on line -> forward
                    set_speed(SPEED_TURN, SPEED_TURN);
                } else{
                    // gray area -> go straight
                    set_speed(SPEED_TURN, SPEED_TURN);
                }
            }

            // handle ultrasonic measurement result
            if (us_new_data) {
                us_new_data = 0;
                uint32_t time_us = us_timeInterval * 10UL; // 10us tick
                uint32_t distance_cm = (uint32_t)(((uint64_t)time_us * 343ULL) / 20000ULL);

                if (distance_cm <= 7) {
                    // stop motors
                    set_speed(1.f, 1.f);
                    GPIO_write(STOP_LED, 1);
                    stopped = 1;
                    printf("ULTRA STOP: %lu cm\r\n", (unsigned long)distance_cm);
                } else {
                    if (stopped) {
                        // resume
                        GPIO_write(STOP_LED, 0);
                        stopped = 0;
                        printf("ULTRA RESUME: %lu cm\r\n", (unsigned long)distance_cm);
                    } else {
                        // print distance only
                        printf("ULTRA: %lu cm\r\n", (unsigned long)distance_cm);
                    }
                }
            }

            delay_ms(10);
    }

    return 0;
}

// TIM4 IRQ removed (no ultrasonic input-capture in this build)
