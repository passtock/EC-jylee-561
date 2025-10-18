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

- [GPIO Digital In/Out](#gpio-digital-inout)
  - [Header File](#header-file)
  - [GPIO_init()](#gpio_init)
  - [GPIO_mode()](#gpio_mode)
  - [GPIO_write()](#gpio_write)
  - [GPIO_read()](#gpio_read)
  - [GPIO_ospeed()](#gpio_ospeed)
  - [GPIO_otype()](#gpio_otype)
  - [GPIO_pupdr()](#gpio_pupdr)
- [RCC](#rcc)
  - [seven_seg_FND_init](#seven_seg_fnd_init)
  - [seven_seg_FND_display](#seven_seg_fnd_display)
- [ecSysTick2.h](#ecsystick2h)
  - [SysTick_init](#systick_init)
  - [SysTick_Handler](#systick_handler)
  - [delay_ms](#delay_ms)
  - [SysTick_reset](#systick_reset)
  - [SysTick_val](#systick_val)
  - [SysTick_counter](#systick_counter)
- [Additional Functions](#additional-functions)
  - [GPIO_pupd()](#gpio_pupd)
  - [sevensegment_display_19](#sevensegment_display_19)
- [PWM (ecPWM2.h)](#pwm-ecpwm2h)
  - [PWM_init](#pwm_init)
  - [PWM_period_ms / PWM_period_us](#pwm_period)
  - [PWM_pulsewidth_ms / PWM_pulsewidth](#pwm_pulsewidth)
  - [PWM_duty](#pwm_duty)
- [Timer (ecTIM2.h)](#timer-ectim2h)
  - [TIM_init](#tim_init)
  - [TIM_period_ms / TIM_period_us](#tim_period)
  - [TIM_UI_init / TIM_UI_enable / TIM_UI_disable](#tim_ui)

---

## GPIO Digital In/Out

### Header File

```c
#include "ecGPIO.h"
#include "stm32f411xe.h"
#include "ecRCC.h"

#ifndef __ECGPIO_H
#define __ECGPIO_H

// MODER
#define INPUT        0x00
#define OUTPUT       0x01
#define AF           0x02
#define ANALOG       0x03

// IDR & ODR
#define HIGH         1
#define LOW          0

// OSPEED
#define LOW_SPEED    0x00
#define MID_SPEED    0x01
#define FAST_SPEED   0x02
#define HIGH_SPEED   0x03

// OTYPER
#define PUSH_PULL    0   // Push-pull
#define OPEN_DRAIN   1   // Open-Drain

// PUDR
#define NO_PUPD      0x00    // No pull-up, pull-down
#define PULL_UP      0x01    // Pull-up
#define PULL_DOWN    0x02    // Pull-down  
#define RESERVED     0x03    // Reserved

// PIN
#define LED_PIN      5
#define BUTTON_PIN   13

#ifdef __cplusplus
 extern "C" {
#endif

void GPIO_init(GPIO_TypeDef *Port, int pin, int mode);
void GPIO_mode(GPIO_TypeDef* Port, int pin, int mode);
void GPIO_write(GPIO_TypeDef *Port, int pin, int output);
int  GPIO_read(GPIO_TypeDef *Port, int pin);
void GPIO_ospeed(GPIO_TypeDef* Port, int pin, int speed);
void GPIO_otype(GPIO_TypeDef* Port, int pin, int type);
void GPIO_pupdr(GPIO_TypeDef* Port, int pin, int pupd);
void seven_seg_FND_init(void); 
void seven_seg_FND_display(uint8_t  num, uint8_t select);

#ifdef __cplusplus
}
#endif

#endif
```

---

### GPIO_init()

Initializes GPIO pins with default setting and enables GPIO Clock.  
Mode: In/Out/AF/Analog

```c
void GPIO_init(GPIO_TypeDef *Port, int pin, int mode);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
- **pin:** pin number (int) 0~15
- **mode:** INPUT(0), OUTPUT(1), AF(2), ANALOG(3)

**Example code**

```c
GPIO_init(GPIOA, 5, OUTPUT);
GPIO_init(GPIOC, 13, INPUT); // GPIO_init(GPIOC, 13, 0);
```

---

### GPIO_mode()

Configures GPIO pin modes: In/Out/AF/Analog

```c
void GPIO_mode(GPIO_TypeDef *Port, int pin, int mode);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
- **pin:** pin number (int) 0~15
- **mode:** INPUT(0), OUTPUT(1), AF(2), ANALOG(3)

**Example code**

```c
GPIO_mode(GPIOA, 5, OUTPUT);
```

---

### GPIO_write()

Write the data to GPIO pin: High, Low

```c
void GPIO_write(GPIO_TypeDef *Port, int pin, int output);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
- **pin:** pin number (int) 0~15
- **output:** LOW(0), HIGH(1)

**Example code**

```c
GPIO_write(GPIOA, 5, HIGH);  // 1: High
```

---

### GPIO_read()

Read the data from GPIO pin

```c
int GPIO_read(GPIO_TypeDef *Port, int pin);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
- **pin:** pin number (int) 0~15

**Example code**

```c
GPIO_read(GPIOC, 13);
```

---

### GPIO_ospeed()

Configures output speed of GPIO pin: Low, Mid, Fast, High

```c
void GPIO_ospeed(GPIO_TypeDef* Port, int pin, int speed);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
- **pin:** pin number (int) 0~15
- **speed:** LOW_SPEED(0), MID_SPEED(1), FAST_SPEED(2), HIGH_SPEED(3)

**Example code**

```c
GPIO_ospeed(GPIOA, 5, FAST_SPEED);  // 2: FAST_SPEED
```

---

### GPIO_otype()

Configures output type of GPIO pin: Push-Pull / Open-Drain

```c
void GPIO_otype(GPIO_TypeDef* Port, int pin, int type);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
- **pin:** pin number (int) 0~15
- **type:** PUSH_PULL(0), OPEN_DRAIN(1)

**Example code**

```c
GPIO_otype(GPIOA, 5, PUSH_PULL);  // 0: Push-Pull
```

---

### GPIO_pupdr()

Configures Pull-up/Pull-down mode of GPIO pin: No Pull-up, Pull-down/ Pull-up/ Pull-down/ Reserved

```c
void GPIO_pupdr(GPIO_TypeDef* Port, int pin, int pupd);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
- **pin:** pin number (int) 0~15
- **pupd:** NO_PUPD(0), PULL_UP(1), PULL_DOWN(2), RESERVED(3)

**Example code**

```c
GPIO_pupdr(GPIOA, 5, NO_PUPD);  // 0: No Pull-up, Pull-down
```

---

## RCC

### seven_seg_FND_init

```c
void seven_seg_FND_init(void);
```

**Description**

Initializes the DOUT pins for 7-segment LEDs.  
Sets all segment and digit select pins as outputs and turns off all segments and digits.

**Parameters**

None

**Example code**

```c
seven_seg_FND_init();
```

---

### seven_seg_FND_display

```c
void seven_seg_FND_display(uint8_t num, uint8_t select);
```

**Description**

Displays a decimal digit (`num`) on a selected 7-segment display (`select`).  
Sets the appropriate segment pins to show the digit and activates the selected digit.

**Parameters**

- **num:** The digit to display (0~9)
- **select:** The digit position to activate (0~3)

**Example code**

```c
seven_seg_FND_display(5, 2); // Display '5' on the third digit
```

---

## ecSysTick2.h

### SysTick_init

```c
void SysTick_init(void);
```

**Description**

Initializes the SysTick timer to generate an interrupt every 1ms using the processor clock (default: 84MHz PLL).  
Configures SysTick registers, enables the interrupt, and sets its priority.

**Example code**

```c
SysTick_init();
```

---

### SysTick_Handler

```c
void SysTick_Handler(void);
```

**Description**

Interrupt handler for the SysTick timer.  
Calls `SysTick_counter()` to increment the millisecond tick counter.

---

### delay_ms

```c
void delay_ms(uint32_t mesc);
```

**Description**

Delays execution for a specified number of milliseconds using the SysTick timer.

**Parameters**

- **mesc:** Number of milliseconds to delay

**Example code**

```c
delay_ms(500); // Delay for 500 ms
```

---

### SysTick_reset

```c
void SysTick_reset(void);
```

**Description**

Resets the SysTick current value register to zero.

**Example code**

```c
SysTick_reset();
```

---

### SysTick_val

```c
uint32_t SysTick_val(void);
```

**Description**

Returns the current value of the SysTick timer.

**Example code**

```c
uint32_t val = SysTick_val();
```

---

### SysTick_counter

```c
void SysTick_counter(void);
```

**Description**

Increments the global millisecond tick counter (`msTicks`).  
Also increments a secondary counter (`count`) every 1000 ms (1 second).

---

## Additional Functions

### GPIO_pupd()

Configures the pull-up/pull-down resistor mode for a GPIO pin.

```c
void GPIO_pupd(PinName_t pinName, int pupd);
```

**Parameters**

- **pinName:** Pin identifier (PinName_t)
- **pupd:** NO_PUPD(0), PULL_UP(1), PULL_DOWN(2), RESERVED(3)

**Example code**

```c
GPIO_pupd(PA_10, NO_PUPD); // No pull-up, pull-down
```

---

### sevensegment_display_19

Displays numbers from 0 to 19 on a 2-digit 7-segment display using multiplexing.

```c
void sevensegment_display_19(uint8_t num);
```

**Description**

Handles multiplexed display of numbers 0–19 on two 7-segment digits.  
Alternates between least significant digit (LSD) and most significant digit (MSD) using a global `multiplex_state` variable.

**Parameters**

- **num:** Number to display (0–19)

**Example code**

```c
sevensegment_display_19(13); // Display '13' on the 7-segment display
```

---

## PWM (ecPWM2.h)

### PWM_init

```c
void PWM_init(PinName_t pinName);
```

Initializes PWM on the specified pin. The function configures the GPIO alternate function mapping, initializes the associated TIMx channel, and sets a default period and duty (default: 1 ms period, 50% duty) suitable for common use. Use `PWM_pinmap()` to inspect which timer/channel is used for a given pin.

**Parameters**

- **pinName:** Pin identifier (PinName_t) — use values from `ecPinNames.h` (e.g., `PA_0`, `PB_6`).

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

Set the high pulse width (in milliseconds) for the PWM output. This is useful when you want to specify an absolute pulse length (e.g., servo control where 1.0–2.0 ms pulses are common) rather than a duty fraction.

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

Set the PWM duty ratio (0.0 — 1.0) for the configured PWM pin. The duty is applied relative to the current PWM period configured via `PWM_period_ms`/`_us`.

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
