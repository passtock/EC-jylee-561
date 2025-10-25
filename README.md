# Documentation

[See Class Github](https://github.com/ykkimhgu/EC-student/blob/main/docs/EC_HAL_Documentation.md) for the example documentation.

## Embedded Controller HAL Library

**Written by:** leejeayongs  
**Program:** C/C++  
**IDE/Compiler:** PlatformIO  
**OS:** Win10  
**MCU:** STM32F411RE, Nucleo-64

---

## Table of Contents

- [Pin Names (ecPinNames.h)](#pin-names-ecpinnamesh)
  - [PinName_t Enumeration](#pinname_t-enumeration)
  - [ecPinmap()](#ecpinmap)
  - [Bitwise Macros](#bitwise-macros)
- [RCC (ecRCC2.h)](#rcc-ecrcc2h)
  - [RCC_HSI_init()](#rcc_hsi_init)
  - [RCC_PLL_init()](#rcc_pll_init)
  - [RCC_GPIOx_enable()](#rcc_gpiox_enable)
- [GPIO Digital In/Out (ecGPIO2.h)](#gpio-digital-inout-ecgpio2h)
  - [GPIO_init()](#gpio_init)
  - [GPIO_mode()](#gpio_mode)
  - [GPIO_write()](#gpio_write)
  - [GPIO_read()](#gpio_read)
  - [GPIO_ospeed()](#gpio_ospeed)
  - [GPIO_otype()](#gpio_otype)
  - [GPIO_pupd()](#gpio_pupd)
  - [seven_seg_FND_init()](#seven_seg_fnd_init)
  - [seven_seg_FND_display()](#seven_seg_fnd_display)
  - [sevensegment_display_19()](#sevensegment_display_19)
- [EXTI (ecEXTI2.h)](#exti-ecexti2h)
  - [EXTI_init()](#exti_init)
  - [EXTI_enable()](#exti_enable)
  - [EXTI_disable()](#exti_disable)
  - [is_pending_EXTI()](#is_pending_exti)
  - [clear_pending_EXTI()](#clear_pending_exti)
- [SysTick (ecSysTick2.h)](#systick-ecsystick2h)
  - [SysTick_init()](#systick_init)
  - [SysTick_Handler()](#systick_handler)
  - [delay_ms()](#delay_ms)
  - [delay_us()](#delay_us)
  - [SysTick_reset()](#systick_reset)
  - [SysTick_val()](#systick_val)
  - [SysTick_counter()](#systick_counter)
  - [millis()](#millis)
- [Timer (ecTIM2.h)](#timer-ectim2h)
  - [TIM_init()](#tim_init)
  - [TIM_period_ms / TIM_period_us()](#tim_period)
  - [TIM_UI_init / TIM_UI_enable / TIM_UI_disable()](#tim_ui)
- [PWM (ecPWM2.h)](#pwm-ecpwm2h)
  - [PWM_init()](#pwm_init)
  - [PWM_period_ms / PWM_period_us()](#pwm_period)
  - [PWM_pulsewidth_ms / PWM_pulsewidth()](#pwm_pulsewidth)
  - [PWM_duty()](#pwm_duty)
- [Stepper Motor (ecStepper2.h)](#stepper-motor-ecstepper2h)
  - [Stepper_init()](#stepper_init)
  - [Stepper_setSpeed()](#stepper_setspeed)
  - [Stepper_step()](#stepper_step)
  - [Stepper_stop()](#stepper_stop)

---

## Pin Names (ecPinNames.h)

### PinName_t Enumeration

The `PinName_t` enum provides symbolic names for all STM32F411 GPIO pins, including Arduino connector naming conventions.

**Example Pin Names:**
- Port-based: `PA_0` to `PA_15`, `PB_0` to `PB_15`, `PC_0` to `PC_15`, etc.
- Arduino naming: `A0` (=PA_0), `A1` (=PA_1), `D0` (=PA_3), `D13` (=PA_5), etc.
- Not connected: `NC` (0xFFFFFFFF)

**Pin Encoding:**
Each `PinName_t` value encodes the port and pin number:
- Upper nibble (bits 7-4): Port number (0=GPIOA, 1=GPIOB, 2=GPIOC, ...)
- Lower nibble (bits 3-0): Pin number (0-15)

---

### ecPinmap()

Decodes a `PinName_t` value into a GPIO port pointer and pin number.

```c
void ecPinmap(PinName_t pinName, GPIO_TypeDef **GPIOx, unsigned int *pin);
```

**Parameters**

- **pinName:** Pin identifier (e.g., `PA_5`, `D13`)
- **GPIOx:** Pointer to receive the GPIO port address (e.g., GPIOA)
- **pin:** Pointer to receive the pin number (0-15)

**Example code**

```c
GPIO_TypeDef *port;
unsigned int pin;
ecPinmap(PA_5, &port, &pin);  // port = GPIOA, pin = 5
```

---

### Bitwise Macros

Utility macros for bit manipulation:

```c
BIT_SET(REG, BIT)           // Set bit in register
BIT_CLEAR(REG, BIT)         // Clear bit in register
BIT_READ(REG, BIT)          // Read bit from register
BITS_SET(REG, BIT, NUM)     // Set multiple bits
BITS_CLEAR(REG, BIT, NUM)   // Clear multiple bits
```

**Example code**

```c
BIT_SET(GPIOA->ODR, 5);     // Set PA5 high
BIT_CLEAR(GPIOA->ODR, 5);   // Set PA5 low
```

---

## RCC (ecRCC2.h)

### RCC_HSI_init()

Initializes the system clock using the internal High-Speed Internal (HSI) oscillator (16 MHz).

```c
void RCC_HSI_init(void);
```

**Example code**

```c
RCC_HSI_init();  // System clock = 16 MHz
```

---

### RCC_PLL_init()

Initializes the system clock using the Phase-Locked Loop (PLL) to achieve 84 MHz from HSI.

```c
void RCC_PLL_init(void);
```

**Configuration:**
- HSI (16 MHz) â†’ PLL (Ã—84/16) â†’ SYSCLK = 84 MHz
- AHB prescaler: /1 (HCLK = 84 MHz)
- APB1 prescaler: /2 (PCLK1 = 42 MHz, Timer clock = 84 MHz)
- APB2 prescaler: /1 (PCLK2 = 84 MHz)

**Example code**

```c
RCC_PLL_init();  // System clock = 84 MHz
```

---

### RCC_GPIOx_enable()

Enables the clock for a specific GPIO port.

```c
void RCC_GPIOA_enable(void);
void RCC_GPIOB_enable(void);
void RCC_GPIOC_enable(void);
void RCC_GPIOD_enable(void);
void RCC_GPIOE_enable(void);
```

**Example code**

```c
RCC_GPIOA_enable();  // Enable GPIOA clock
```

---

## GPIO Digital In/Out (ecGPIO2.h)

### GPIO_init()

Initializes a GPIO pin with the specified mode and enables the GPIO port clock.

```c
void GPIO_init(PinName_t pinName, uint32_t mode);
```

**Parameters**

- **pinName:** Pin identifier (e.g., `PA_5`, `PC_13`)
- **mode:** INPUT(0), OUTPUT(1), AF(2), ANALOG(3)

**Example code**

```c
GPIO_init(PA_5, OUTPUT);   // Initialize PA5 as output
GPIO_init(PC_13, INPUT);   // Initialize PC13 as input
```

---

### GPIO_mode()

Configures the mode of a GPIO pin.

```c
void GPIO_mode(PinName_t pinName, uint32_t mode);
```

**Parameters**

- **pinName:** Pin identifier
- **mode:** INPUT(0), OUTPUT(1), AF(2), ANALOG(3)

**Example code**

```c
GPIO_mode(PA_5, OUTPUT);
```

---

### GPIO_write()

Writes a digital value (HIGH/LOW) to a GPIO pin using atomic BSRR register access.

```c
void GPIO_write(PinName_t pinName, int Output);
```

**Parameters**

- **pinName:** Pin identifier
- **Output:** LOW(0) or HIGH(1)

**Example code**

```c
GPIO_write(PA_5, HIGH);   // Set PA5 high
GPIO_write(PA_5, LOW);    // Set PA5 low
```

---

### GPIO_read()

Reads the digital value from a GPIO pin's Input Data Register (IDR).

```c
int GPIO_read(PinName_t pinName);
```

**Parameters**

- **pinName:** Pin identifier

**Returns**

- 0 (LOW) or 1 (HIGH)

**Example code**

```c
int state = GPIO_read(PC_13);  // Read button state
```

---

### GPIO_ospeed()

Configures the output speed of a GPIO pin.

```c
void GPIO_ospeed(PinName_t pinName, int speed);
```

**Parameters**

- **pinName:** Pin identifier
- **speed:** 0 (Low), 1 (Medium), 2 (Fast), 3 (High)

**Example code**

```c
GPIO_ospeed(PA_5, 2);  // Fast speed
```

---

### GPIO_otype()

Configures the output type of a GPIO pin.

```c
void GPIO_otype(PinName_t pinName, int type);
```

**Parameters**

- **pinName:** Pin identifier
- **type:** 0 (Push-Pull), 1 (Open-Drain)

**Example code**

```c
GPIO_otype(PA_5, 0);  // Push-pull output
```

---

### GPIO_pupd()

Configures the pull-up/pull-down resistor mode.

```c
void GPIO_pupd(PinName_t pinName, int pupd);
```

**Parameters**

- **pinName:** Pin identifier
- **pupd:** 0 (No pull-up/pull-down), 1 (Pull-up), 2 (Pull-down)

**Example code**

```c
GPIO_pupd(PC_13, 1);  // Enable pull-up on PC13
```

---

### seven_seg_FND_init()

Initializes GPIO pins for 7-segment display control.

```c
void seven_seg_FND_init(void);
```

**Description**

Configures segment pins (a-g) and digit select pins as outputs and turns off all segments.

**Example code**

```c
seven_seg_FND_init();
```

---

### seven_seg_FND_display()

Displays a single digit (0-9) on a selected 7-segment display.

```c
void seven_seg_FND_display(uint8_t num, uint8_t select);
```

**Parameters**

- **num:** Digit to display (0-9)
- **select:** Digit position (0-3)

**Example code**

```c
seven_seg_FND_display(5, 2);  // Display '5' on digit 2
```

---

### sevensegment_display_19()

Displays numbers 0-19 on a 2-digit 7-segment display using multiplexing.

```c
void sevensegment_display_19(uint8_t num);
```

**Parameters**

- **num:** Number to display (0-19)

**Example code**

```c
sevensegment_display_19(13);  // Display '13'
```

---

## EXTI (ecEXTI2.h)

### EXTI_init()

Initializes external interrupt for a GPIO pin.

```c
void EXTI_init(PinName_t pinName, int trig_type, int priority);
```

**Parameters**

- **pinName:** Pin identifier (e.g., `PC_13`)
- **trig_type:** FALL(0), RISE(1), BOTH(2)
- **priority:** Interrupt priority (0-15, lower = higher priority)

**Description**

- Enables SYSCFG clock
- Connects EXTI line to GPIO pin
- Configures trigger edge (rising/falling/both)
- Enables interrupt mask
- Configures NVIC with specified priority

**Example code**

```c
EXTI_init(PC_13, FALL, 0);  // Falling edge interrupt on PC13, priority 0
```

**Implementation Notes:**

The function automatically handles EXTI line mapping:
- Pins 0-4: Individual IRQs (EXTI0_IRQn to EXTI4_IRQn)
- Pins 5-9: Shared IRQ (EXTI9_5_IRQn)
- Pins 10-15: Shared IRQ (EXTI15_10_IRQn)

---

### EXTI_enable()

Enables external interrupt for a pin by unmasking the EXTI line.

```c
void EXTI_enable(PinName_t pinName);
```

**Parameters**

- **pinName:** Pin identifier

**Example code**

```c
EXTI_enable(PC_13);
```

---

### EXTI_disable()

Disables external interrupt for a pin by masking the EXTI line.

```c
void EXTI_disable(PinName_t pinName);
```

**Parameters**

- **pinName:** Pin identifier

**Example code**

```c
EXTI_disable(PC_13);
```

---

### is_pending_EXTI()

Checks if an EXTI interrupt is pending for a specific pin.

```c
uint32_t is_pending_EXTI(PinName_t pinName);
```

**Parameters**

- **pinName:** Pin identifier

**Returns**

- Non-zero if interrupt is pending, 0 otherwise

**Example code**

```c
if (is_pending_EXTI(PC_13)) {
    // Handle interrupt
}
```

---

### clear_pending_EXTI()

Clears the pending EXTI interrupt flag for a specific pin.

```c
void clear_pending_EXTI(PinName_t pinName);
```

**Parameters**

- **pinName:** Pin identifier

**Description**

Must be called in the EXTI ISR to clear the pending bit (Write-1-Clear semantics).

**Example code**

```c
void EXTI15_10_IRQHandler(void) {
    if (is_pending_EXTI(PC_13)) {
        // Handle button press
        clear_pending_EXTI(PC_13);
    }
}
```

---

## SysTick (ecSysTick2.h)

### SysTick_init()

Initializes the SysTick timer for 1ms interrupts at 84 MHz system clock.

```c
void SysTick_init(void);
```

**Configuration:**
- Clock source: Processor clock (84 MHz)
- Reload value: 84000 - 1 (for 1ms period)
- Interrupt: Enabled
- Priority: 16

**Example code**

```c
SysTick_init();
```

---

### SysTick_Handler()

SysTick interrupt handler called every 1ms.

```c
void SysTick_Handler(void);
```

**Description**

Automatically increments the global `msTicks` counter.

---

### delay_ms()

Blocking delay for specified milliseconds.

```c
void delay_ms(uint32_t msec);
```

**Parameters**

- **msec:** Delay time in milliseconds

**Example code**

```c
delay_ms(500);  // Wait 500ms
```

---

### delay_us()

Blocking delay for specified microseconds with improved accuracy.

```c
void delay_us(uint32_t usec);
```

**Parameters**

- **usec:** Delay time in microseconds

**Description**

- For delays â‰¥ 1ms: Uses `delay_ms()` for the ms portion
- For sub-ms delays: Uses SysTick VAL register for precise timing
- Handles SysTick counter wrap-around correctly

**Example code**

```c
delay_us(100);    // Wait 100Î¼s
delay_us(7324);   // Wait 7.324ms
```

---

### SysTick_reset()

Resets the SysTick current value register to zero.

```c
void SysTick_reset(void);
```

---

### SysTick_val()

Returns the current SysTick counter value.

```c
uint32_t SysTick_val(void);
```

**Returns**

- Current SysTick VAL register value (counts down from LOAD to 0)

---

### SysTick_counter()

Increments the millisecond tick counter (called by SysTick_Handler).

```c
void SysTick_counter(void);
```

---

### millis()

Returns the number of milliseconds since SysTick initialization.

```c
uint32_t millis(void);
```

**Returns**

- Millisecond count (wraps around every ~49.7 days)

**Example code**

```c
uint32_t start = millis();
// ... do something ...
uint32_t elapsed = millis() - start;
```

---

## Timer (ecTIM2.h)

### PWM_init

```c
void PWM_init(PinName_t pinName);
```

Initializes PWM on the specified pin. The function configures the GPIO alternate function mapping, initializes the associated TIMx channel, and sets a default period and duty (default: 1 ms period, 50% duty) suitable for common use. Use `PWM_pinmap()` to inspect which timer/channel is used for a given pin.

**Parameters**

- **pinName:** Pin identifier (PinName_t) â€” use values from `ecPinNames.h` (e.g., `PA_0`, `PB_6`).

**Example code**

```c
PWM_init(PA_0); // initialize PWM on PA0 (TIM2_CH1)
```

---

### PWM_period_ms / PWM_period_us

```c
void PWM_period_ms(PinName_t pinName, uint32_t msec);
void PWM_period_us(PinName_t pinName, uint32_t usec);
```

Set the PWM update period for the pin's timer. Valid ranges are shown in the header comments (ms: 1..2000, us: 1..1000). Internally this sets the timer PSC/ARR to achieve the requested period and keeps the currently configured duty/pulse width.

**Parameters**

- **pinName:** Pin identifier (PinName_t)
- **msec/usec:** Desired period in milliseconds or microseconds

**Example code**

```c
PWM_period_ms(PA_0, 20); // 20 ms period (50 Hz)
PWM_period_ms(PA_0, 1);  // 1 ms period (1 kHz)
```

---

### PWM_pulsewidth_ms / PWM_pulsewidth

```c
void PWM_pulsewidth_ms(PinName_t pinName, uint32_t pulse_width_ms);
void PWM_pulsewidth(PinName_t pinName,  uint32_t msec);
```

Set the high pulse width (in milliseconds) for the PWM output. This is useful when you want to specify an absolute pulse length (e.g., servo control where 1.0â€“2.0 ms pulses are common) rather than a duty fraction.

**Parameters**

- **pinName:** Pin identifier (PinName_t)
- **pulse_width_ms:** High-level pulse width in milliseconds

**Example code**

```c
PWM_pulsewidth_ms(PA_0, 1); // output high for 1.0 ms each period
```

---

### PWM_duty

```c
void PWM_duty(PinName_t pinName, float duty);
```

Set the PWM duty ratio (0.0 â€” 1.0) for the configured PWM pin. The duty is applied relative to the current PWM period configured via `PWM_period_ms`/`_us`.

**Parameters**

- **pinName:** Pin identifier (PinName_t)
- **duty:** Duty ratio between `0.0` (0%) and `1.0` (100%) as `float`

**Example code**

```c
PWM_duty(PA_0, 0.25f); // 25% duty
```

---

## Timer (ecTIM2.h)

### TIM_init

```c
void TIM_init(TIM_TypeDef* TIMx);
```

Initialize a hardware Timer (TIM2, TIM3, ...). The function enables the timer clock and sets a default counter clock and period. After `TIM_init()` you can set the period with `TIM_period_ms()` / `TIM_period_us()` or configure update interrupts with `TIM_UI_init()`.

**Parameters**

- **TIMx:** Pointer to timer (e.g., `TIM2`, `TIM3`)

**Example code**

```c
TIM_init(TIM2);
TIM_period_ms(TIM2, 1); // 1 ms timer period
```

---

### TIM_period_ms / TIM_period_us

```c
void TIM_period_ms(TIM_TypeDef* TIMx, uint32_t msec);
void TIM_period_us(TIM_TypeDef* TIMx, uint32_t usec);
```

Set the timer update event period. The implementation sets PSC/ARR to achieve the requested update period using the configured timer counter clock (see header comments for expected default clocks).

**Parameters**

- **TIMx:** Pointer to timer (e.g., `TIM2`)
- **msec/usec:** Desired update period in ms or us

**Example code**

```c
TIM_period_ms(TIM3, 500); // TIM3 update event every 500 ms
```

---

### TIM_UI_init / TIM_UI_enable / TIM_UI_disable

```c
void TIM_UI_init(TIM_TypeDef* TIMx, uint32_t msec);
void TIM_UI_enable(TIM_TypeDef* TIMx);
void TIM_UI_disable(TIM_TypeDef* TIMx);
uint32_t is_UIF(TIM_TypeDef *TIMx);
void clear_UIF(TIM_TypeDef *TIMx);
```

Configure Timer Update Interrupt (UI) for the specified timer. `TIM_UI_init()` sets the update period (ms) and prepares NVIC for the corresponding TIMx IRQ. After initialization call `TIM_UI_enable()` to allow interrupts.

**Parameters**

- **TIMx:** Pointer to timer (e.g., `TIM3`)
- **msec:** Update period in milliseconds

**Example code**

```c
TIM_UI_init(TIM3, 500); // setup TIM3 update every 500 ms
TIM_UI_enable(TIM3);
// in ISR
if (is_UIF(TIM3)) {
  clear_UIF(TIM3);
  // handle event
}
```
 
  
 - - -  
  
 # #   S t e p p e r   M o t o r   ( e c S t e p p e r 2 . h )  
  
 # # #   S t e p p e r _ i n i t ( )  
  
 I n i t i a l i z e s   G P I O   p i n s   f o r   s t e p p e r   m o t o r   c o n t r o l   ( 2 8 B Y J - 4 8 ) .  
  
 ` ` ` c  
 v o i d   S t e p p e r _ i n i t ( P i n N a m e _ t   p i n N a m e 1 ,   P i n N a m e _ t   p i n N a m e 2 ,   P i n N a m e _ t   p i n N a m e 3 ,   P i n N a m e _ t   p i n N a m e 4 ) ;  
 ` ` `  
  
 * * P a r a m e t e r s * *  
  
 -   * * p i n N a m e 1 - 4 : * *   F o u r   G P I O   p i n s   c o n n e c t e d   t o   m o t o r   d r i v e r   ( I N 1 - I N 4 )  
  
 * * D e s c r i p t i o n * *  
  
 C o n f i g u r e s   a l l   f o u r   p i n s   a s   o u t p u t s   w i t h :  
 -   N o   p u l l - u p / p u l l - d o w n  
 -   P u s h - p u l l   o u t p u t   t y p e  
 -   F a s t   o u t p u t   s p e e d  
  
 * * E x a m p l e   c o d e * *  
  
 ` ` ` c  
 S t e p p e r _ i n i t ( P B _ 1 0 ,   P B _ 4 ,   P B _ 5 ,   P B _ 3 ) ;     / /   I n i t i a l i z e   s t e p p e r   m o t o r   o n   P B   p i n s  
 ` ` `  
  
 - - -  
  
 # # #   S t e p p e r _ s e t S p e e d ( )  
  
 S e t s   t h e   r o t a t i o n   s p e e d   o f   t h e   s t e p p e r   m o t o r   i n   R P M .  
  
 ` ` ` c  
 v o i d   S t e p p e r _ s e t S p e e d ( l o n g   w h a t S p e e d ) ;  
 ` ` `  
  
 * * P a r a m e t e r s * *  
  
 -   * * w h a t S p e e d : * *   D e s i r e d   s p e e d   i n   R P M   ( r e v o l u t i o n s   p e r   m i n u t e )  
  
 * * D e s c r i p t i o n * *  
  
 C a l c u l a t e s   t h e   d e l a y   b e t w e e n   s t e p s   u s i n g   t h e   f o r m u l a :  
 -   * * F U L L   m o d e : * *   s t e p _ d e l a y   =   6 0 , 0 0 0 , 0 0 0   /   ( R P M   žÖ  2 0 4 8 )  
 -   * * H A L F   m o d e : * *   s t e p _ d e l a y   =   6 0 , 0 0 0 , 0 0 0   /   ( R P M   žÖ  4 0 9 6 )  
  
 F o r   2 8 B Y J - 4 8   m o t o r :  
 -   I n t e r n a l   m o t o r :   3 2   s t e p s / r e v   ( F U L L ) ,   6 4   s t e p s / r e v   ( H A L F )  
 -   G e a r   r a t i o :   6 4 : 1  
 -   O u t p u t   s h a f t :   2 0 4 8   s t e p s / r e v   ( F U L L ) ,   4 0 9 6   s t e p s / r e v   ( H A L F )  
  
 * * E x a m p l e   c o d e * *  
  
 ` ` ` c  
 S t e p p e r _ s e t S p e e d ( 2 ) ;       / /   S e t   s p e e d   t o   2   R P M  
 S t e p p e r _ s e t S p e e d ( 1 0 ) ;     / /   S e t   s p e e d   t o   1 0   R P M  
 ` ` `  
  
 - - -  
  
 # # #   S t e p p e r _ s t e p ( )  
  
 R o t a t e s   t h e   s t e p p e r   m o t o r   f o r   a   s p e c i f i e d   n u m b e r   o f   s t e p s .  
  
 ` ` ` c  
 v o i d   S t e p p e r _ s t e p ( u i n t 3 2 _ t   s t e p s ,   u i n t 3 2 _ t   d i r e c t i o n ,   u i n t 3 2 _ t   m o d e ) ;  
 ` ` `  
  
 * * P a r a m e t e r s * *  
  
 -   * * s t e p s : * *   N u m b e r   o f   s t e p s   t o   r o t a t e  
 -   * * d i r e c t i o n : * *   0   ( C C W   -   c o u n t e r - c l o c k w i s e )   o r   1   ( C W   -   c l o c k w i s e )  
 -   * * m o d e : * *   F U L L ( 1 )   o r   H A L F ( 0 )  
  
 * * D e s c r i p t i o n * *  
  
 * * F U L L   S t e p p i n g   M o d e : * *  
 -   4 - s t a t e   F S M :   1 0 1 0   ? ? 0 1 1 0   ? ? 0 1 0 1   ? ? 1 0 0 1   ? ? ( r e p e a t )  
 -   2 0 4 8   s t e p s   =   3 6 0 øÉ  ( o n e   f u l l   r e v o l u t i o n )  
 -   H i g h e r   t o r q u e ,   l o w e r   r e s o l u t i o n  
  
 * * H A L F   S t e p p i n g   M o d e : * *  
 -   8 - s t a t e   F S M   w i t h   i n t e r m e d i a t e   s t a t e s  
 -   4 0 9 6   s t e p s   =   3 6 0 øÉ  ( o n e   f u l l   r e v o l u t i o n )  
 -   L o w e r   t o r q u e ,   h i g h e r   r e s o l u t i o n   ( d o u b l e   t h e   s t e p s )  
  
 * * E x a m p l e   c o d e * *  
  
 ` ` ` c  
 / /   F U L L   m o d e   e x a m p l e s  
 S t e p p e r _ s t e p ( 2 0 4 8 ,   1 ,   F U L L ) ;     / /   R o t a t e   3 6 0 øÉ  c l o c k w i s e   i n   F U L L   m o d e  
 S t e p p e r _ s t e p ( 1 0 2 4 ,   0 ,   F U L L ) ;     / /   R o t a t e   1 8 0 øÉ  c o u n t e r - c l o c k w i s e  
  
 / /   H A L F   m o d e   e x a m p l e s  
 S t e p p e r _ s t e p ( 4 0 9 6 ,   1 ,   H A L F ) ;     / /   R o t a t e   3 6 0 øÉ  c l o c k w i s e   i n   H A L F   m o d e  
 S t e p p e r _ s t e p ( 2 0 4 8 ,   1 ,   H A L F ) ;     / /   R o t a t e   1 8 0 øÉ  c l o c k w i s e   i n   H A L F   m o d e  
 ` ` `  
  
 - - -  
  
 # # #   S t e p p e r _ s t o p ( )  
  
 I m m e d i a t e l y   s t o p s   t h e   s t e p p e r   m o t o r   a n d   d e - e n e r g i z e s   a l l   c o i l s .  
  
 ` ` ` c  
 v o i d   S t e p p e r _ s t o p ( v o i d ) ;  
 ` ` `  
  
 * * D e s c r i p t i o n * *  
  
 -   S e t s   ` _ s t e p _ n u m `   t o   0   t o   t e r m i n a t e   a n y   o n g o i n g   r o t a t i o n  
 -   S e t s   a l l   f o u r   m o t o r   p i n s   t o   L O W   t o   d e - e n e r g i z e   c o i l s  
 -   T y p i c a l l y   c a l l e d   f r o m   a n   i n t e r r u p t   ( e . g . ,   e m e r g e n c y   s t o p   b u t t o n )  
  
 * * E x a m p l e   c o d e * *  
  
 ` ` ` c  
 v o i d   E X T I 1 5 _ 1 0 _ I R Q H a n d l e r ( v o i d )   {  
         i f   ( i s _ p e n d i n g _ E X T I ( P C _ 1 3 ) )   {  
                 S t e p p e r _ s t o p ( ) ;     / /   E m e r g e n c y   s t o p   o n   b u t t o n   p r e s s  
                 c l e a r _ p e n d i n g _ E X T I ( P C _ 1 3 ) ;  
         }  
 }  
 ` ` `  
  
 - - -  
  
 # #   C o m p l e t e   S t e p p e r   M o t o r   E x a m p l e  
  
 ` ` ` c  
 # i n c l u d e   " e c S T M 3 2 F 4 v 2 . h "  
 # i n c l u d e   " e c S t e p p e r 2 . h "  
  
 # d e f i n e   B U T T O N _ P I N   P C _ 1 3  
  
 v o i d   s e t u p ( v o i d )   {  
         R C C _ P L L _ i n i t ( ) ;                                                                 / /   S y s t e m   c l o c k   =   8 4   M H z  
         S y s T i c k _ i n i t ( ) ;                                                                 / /   I n i t i a l i z e   S y s T i c k  
          
         E X T I _ i n i t ( B U T T O N _ P I N ,   F A L L ,   0 ) ;                                 / /   B u t t o n   i n t e r r u p t  
         G P I O _ i n i t ( B U T T O N _ P I N ,   I N P U T ) ;                                     / /   B u t t o n   a s   i n p u t  
          
         S t e p p e r _ i n i t ( P B _ 1 0 ,   P B _ 4 ,   P B _ 5 ,   P B _ 3 ) ;                 / /   I n i t i a l i z e   s t e p p e r  
         S t e p p e r _ s e t S p e e d ( 2 ) ;                                                       / /   2   R P M  
 }  
  
 i n t   m a i n ( v o i d )   {  
         s e t u p ( ) ;  
          
         / /   R o t a t e   3 6 0   d e g r e e s   c l o c k w i s e   i n   F U L L   m o d e  
         S t e p p e r _ s t e p ( 2 0 4 8 ,   1 ,   F U L L ) ;  
          
         w h i l e ( 1 )   {  
                 / /   M a i n   l o o p  
         }  
 }  
  
 v o i d   E X T I 1 5 _ 1 0 _ I R Q H a n d l e r ( v o i d )   {  
         i f   ( i s _ p e n d i n g _ E X T I ( B U T T O N _ P I N ) )   {  
                 S t e p p e r _ s t o p ( ) ;                                                         / /   E m e r g e n c y   s t o p  
                 c l e a r _ p e n d i n g _ E X T I ( B U T T O N _ P I N ) ;  
         }  
 }  
 ` ` `  
 