# WS2812B LED Driver – LPC Series

This project provides a minimal **bit-banged WS2812B driver** for LPC microcontrollers (e.g., LPC176x).  
It allows precise control of individually addressable RGB LEDs using inline GPIO toggling.

## Features
- Pure C implementation, no external libraries.
- Configurable number of LEDs via `WS2812B` struct.
- Supports writing LED data in GRB order.
- Helper function `turnOnRed()` to quickly enable red on a selected LED.
- Dual GPIO output support for driving parallel LED lines.

## Usage
1. Initialize LED buffer:

```c
RGB_t leds[96] = {0};
WS2812B ws2812b = { .leds = leds, .num_leds = 96 };

2. Set LED state
turnOnRed(&ws2812b, 1);    // LED #1 → Red
WS2812B_write(&ws2812b);   // Update strip

3. Optional periodic refresh with SysTick.
