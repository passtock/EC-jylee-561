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
// manual control direction pin (shared dir pin)
#define DIR_PIN PA_4

// Sensor thresholds
#define WHITE_THRESHOLD 400U
#define BLACK_THRESHOLD 3500U

volatile uint32_t ir_right_value = 0;
volatile uint32_t ir_left_value = 0;
PinName_t seqCHn[2] = {IR_RIGHT_PIN, IR_LEFT_PIN};

// UART1 (BT) RX buffer
#define UART1_RX_BUF_SZ 64
static volatile uint8_t uart1_rx_buf[UART1_RX_BUF_SZ];
static volatile uint32_t uart1_rx_head = 0;
static volatile uint32_t uart1_rx_tail = 0;

// Manual control state
typedef enum {MODE_AUTO = 0, MODE_MANUAL} run_mode_t;
static volatile run_mode_t run_mode = MODE_MANUAL;
static volatile uint8_t stopped_by_user = 0;
static volatile char dir_flag = 'F'; // 'F' forward, 'B' back
static volatile int8_t steer_level = 0; // -3..+3 steering (negative = left, positive = right)
static volatile uint8_t vel_stage = 0; // 0..3 velocity

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

    // UART1 for Bluetooth (PA9 TX, PA10 RX) — user will wire RX to PA10
    UART1_init();
    UART1_baud(BAUD_9600);

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
    // if starting in MANUAL, indicate with LED on
    if(run_mode == MODE_MANUAL) GPIO_write(STOP_LED, 1); else GPIO_write(STOP_LED, 0);

    // direction pin (shared dir control)
    GPIO_init(DIR_PIN, OUTPUT);
    GPIO_write(DIR_PIN, 1); // default forward (mapping: forward -> 1)

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

    // enable USART1 IRQ for BT RX
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, 3);
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

// USART1 IRQ: push received bytes into ring buffer
void USART1_IRQHandler(void){
    if(is_USART1_RXNE()){
        uint8_t b = USART1_read();
        uint32_t next = (uart1_rx_head + 1) % UART1_RX_BUF_SZ;
        if(next != uart1_rx_tail){
            uart1_rx_buf[uart1_rx_head] = b;
            uart1_rx_head = next;
        }
    }
}

// fetch a byte from uart1 rx buffer, return -1 if empty
static int uart1_getchar_nb(void){
    if(uart1_rx_tail == uart1_rx_head) return -1;
    uint8_t b = uart1_rx_buf[uart1_rx_tail];
    uart1_rx_tail = (uart1_rx_tail + 1) % UART1_RX_BUF_SZ;
    return b;
}

// apply manual steering/velocity to motors
static void apply_manual_controls(void){
    const float vel_map[4] = {0.0f, 0.33f, 0.66f, 1.0f};
    float v = vel_map[vel_stage & 0x3];
    float left = v;
    float right = v;

    // steer_level: -3..+3
    int s = steer_level;
    if(s == 0){
        left = v; right = v;
    } else if(s > 0){
        // steer right: left stays full, right reduced
        float factor = (float)s / 3.0f; // 0..1
        left = v;
        right = v * (1.0f - factor);
    } else {
        // steer left: right stays full, left reduced
        float factor = (float)(-s) / 3.0f; // 0..1
        right = v;
        left = v * (1.0f - factor);
    }

    // set DIR pin according to dir_flag mapping: forward -> 1, backward -> 0
    if(dir_flag == 'B') GPIO_write(DIR_PIN, 0); else GPIO_write(DIR_PIN, 1);

    if(stopped_by_user){
        set_speed(0.f, 0.f);
    } else {
        set_speed(left, right);
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
            // process any incoming BT commands (non-blocking)
            // Also poll USART1 RX in case IRQ isn't firing / wiring differences
            while(is_USART1_RXNE()){
                uint8_t pb = USART1_read();
                uint32_t nextp = (uart1_rx_head + 1) % UART1_RX_BUF_SZ;
                if(nextp != uart1_rx_tail){
                    uart1_rx_buf[uart1_rx_head] = pb;
                    uart1_rx_head = nextp;
                }
                // echo polled byte for visibility
                printf("BT_POLL_RX: %c\r\n", pb);
            }

            
            int ch;
            while((ch = uart1_getchar_nb()) >= 0){
                // echo received byte to PC for visibility
                uint8_t rb = (uint8_t)ch;
                printf("BT_RX: %c\r\n", rb);

                // handle arrow key escape sequences (ESC '[' 'A'/'B'/'C'/'D')
                uint8_t lc = rb;
                if(rb == 0x1B){ // ESC
                    int n1 = uart1_getchar_nb();
                    int n2 = uart1_getchar_nb();
                    if(n1 == '[' && n2 >= 0){
                        if(n2 == 'C') lc = 'r'; // RIGHT
                        else if(n2 == 'D') lc = 'l'; // LEFT
                        else if(n2 == 'A') lc = 'u'; // UP
                        else if(n2 == 'B') lc = 'd'; // DOWN
                        else lc = 0;
                    } else {
                        // fallback: treat first read as next char
                        if(n1 >= 0) lc = (uint8_t)n1; else lc = 0;
                    }
                }
                // make parsing case-insensitive
                if(lc >= 'A' && lc <= 'Z') lc = lc + ('a' - 'A');

                // simple parser: single-letter commands + tXX (steer) + vXX (vel)
                if(lc == 'm'){
                    run_mode = MODE_MANUAL;
                    stopped_by_user = 0;
                    GPIO_write(STOP_LED, 1); // indicate manual
                    // apply current dir flag to DIR pin immediately
                    GPIO_write(DIR_PIN, (dir_flag == 'B') ? 0 : 1);
                    printf("MODE -> MANUAL\r\n");
                } else if(lc == 'a'){
                    run_mode = MODE_AUTO;
                    stopped_by_user = 0;
                    // clear ultrasonic stopped state so AUTO can resume
                    stopped = 0;
                    GPIO_write(STOP_LED, 0);
                    // apply current dir flag to DIR pin immediately
                    GPIO_write(DIR_PIN, (dir_flag == 'B') ? 0 : 1);
                    printf("MODE -> AUTO\r\n");
                } else if(lc == 's'){
                    // stop in any mode
                    stopped_by_user = 1;
                    set_speed(0.f, 0.f);
                    printf("CMD: STOP\r\n");
                } else if(lc == 'f'){
                    dir_flag = 'F';
                    stopped_by_user = 0;
                    // set DIR pin for forward
                    GPIO_write(DIR_PIN, 1);
                } else if(lc == 'b'){
                    dir_flag = 'B';
                    stopped_by_user = 0;
                    // set DIR pin for backward
                    GPIO_write(DIR_PIN, 0);
                } else if(lc == 'l'){
                    // steer left: decrease level (more negative), limit -3
                    if(steer_level > -3) steer_level--;
                } else if(lc == 'r'){
                    // steer right: increase level (more positive), limit +3
                    if(steer_level < 3) steer_level++;
                } else if(lc == 'u'){
                    // vel up
                    if(vel_stage < 3) vel_stage++;
                } else if(lc == 'd'){
                    // vel down
                    if(vel_stage > 0) vel_stage--;
                } else if(lc == 't' || lc == 'v'){
                    // steering/value set: support optional '-' for steer
                    int sign = 1;
                    int d1 = uart1_getchar_nb();
                    if(d1 == '-'){
                        sign = -1;
                        d1 = uart1_getchar_nb();
                    }
                    if(d1 >= '0' && d1 <= '9'){
                        int val = (d1 - '0');
                        if(lc == 't'){
                            // allow 0..3 with optional sign
                            if(val >= 0 && val <= 3){
                                steer_level = (int8_t)(sign * val);
                            }
                        } else {
                            // vel: single digit 0..3
                            if(val >= 0 && val <= 3) vel_stage = (uint8_t)val;
                        }
                    }
                }
            }

            // periodic status broadcast on UART1 (every 1s) - send regardless of mode
            static uint32_t last_status_ms = 0;
            if((msTicks - last_status_ms) >= 1000){
                last_status_ms = msTicks;
                char status[64];
                // MOD: A/M DIR: F/B STR: 00 VEL: 00
                snprintf(status, sizeof(status), "MOD: %c DIR: %c STR: %+02d VEL: %02u\r\n",
                    (run_mode==MODE_AUTO)?'A':'M', dir_flag, (int)steer_level, (unsigned)vel_stage);
                USART1_write((uint8_t*)status, strlen(status));
            }
            // If manual mode, skip IR/ULTRA printing and sensor triggering; just apply manual controls
            if(run_mode == MODE_MANUAL){
                apply_manual_controls();
                delay_ms(10);
                continue;
            }

            // print current IR sensor RAW values (AUTO mode only)
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

            // handle ultrasonic measurement result (AUTO mode only)
            if (us_new_data && run_mode == MODE_AUTO) {
                us_new_data = 0;
                uint32_t time_us = us_timeInterval * 10UL; // 10us tick
                uint32_t distance_cm = (uint32_t)(((uint64_t)time_us * 343ULL) / 20000ULL);

                if (distance_cm <= 7) {
                    // stop motors (do not reverse)
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

            // AUTO mode will continue with line-tracing and ultrasonic handling

            delay_ms(10);
    }

    return 0;
}

// TIM4 IRQ removed (no ultrasonic input-capture in this build)
