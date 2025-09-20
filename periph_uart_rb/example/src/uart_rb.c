/**
 * @file main.c
 * @brief Example application for driving WS2812B LEDs on LPC microcontroller.
 *
 * This file sets up the system PLL, initializes GPIO, configures SysTick,
 * and demonstrates usage of the WS2812B driver to turn on a specific LED
 * with the color RED.
 *
 * Author: Aadithya (Phoeneix Automation)
 */

#include "chip.h"
#include <string.h>
#include "ws2812b.h"

/*------------------------------------------------------------------------------
 * Clock sources (fixed values from LPC datasheet)
 *----------------------------------------------------------------------------*/
const uint32_t OscRateIn   = 12000000;   /**< 12.0 MHz external oscillator */
const uint32_t RTCOscRateIn = 32768;     /**< 32.768 kHz RTC oscillator    */

/*------------------------------------------------------------------------------
 * System configuration
 *----------------------------------------------------------------------------*/
#define TICKRATE_HZ (1000U)   /**< SysTick interrupt rate = 1 ms tick */

volatile uint32_t tick_count = 0;   /**< Millisecond tick counter */

/*------------------------------------------------------------------------------
 * WS2812B LED strip buffer (96 LEDs)
 *----------------------------------------------------------------------------*/
RGB_t led_nums[96] = {0};   /**< Frame buffer for LED colors */
WS2812B ws2812b = {
   .leds     = led_nums,
   .num_leds = 96,
};

/*------------------------------------------------------------------------------
 * SysTick Handler
 *----------------------------------------------------------------------------*/
/**
 * @brief SysTick interrupt handler.
 *
 * This function is called every 1 ms.
 * Every 100 ms, the LED buffer is written to the WS2812B strip.
 */
void SysTick_Handler(void) {
    tick_count++;
    if (tick_count >= 100) {
         WS2812B_write(&ws2812b);
         tick_count = 0;
    }
}

/*------------------------------------------------------------------------------
 * PLL Setup Functions
 *----------------------------------------------------------------------------*/
static void pllfeed(void) {
    LPC_SYSCTL->PLL[0].PLLFEED = 0xAA;
    LPC_SYSCTL->PLL[0].PLLFEED = 0x55;
}

/**
 * @brief Configure system PLL for higher core clock.
 */
static void setpll(void) {
    uint16_t n = 2, m = 25, d = 3;

    LPC_SYSCTL->PLL[0].PLLCON = 0x01; pllfeed();
    LPC_SYSCTL->PLL[0].PLLCON = 0x00; pllfeed();
    LPC_SYSCTL->CLKSRCSEL     = 1;
    LPC_SYSCTL->PLL[0].PLLCFG = ((n-1)<<16)|(m-1); pllfeed();
    LPC_SYSCTL->PLL[0].PLLCON = 0x01; pllfeed();
    LPC_SYSCTL->CCLKSEL       = d-1;

    while ((LPC_SYSCTL->PLL[0].PLLSTAT & 0x04000000) == 0);

    LPC_SYSCTL->PLL[0].PLLCON = 0x03; pllfeed();
    SystemCoreClockUpdate();
}

/*------------------------------------------------------------------------------
 * Application Entry Point
 *----------------------------------------------------------------------------*/
int main(void) {
    /* Configure system PLL and core clock */
    setpll();

    /* Initialize IOCON and GPIO subsystems */
    Chip_IOCON_Init(LPC_IOCON);
    Chip_GPIO_Init(LPC_GPIO);
    Chip_GPIO_Init(LPC_GPIO3);

    /* Turn on the First three LEDs in Red color*/
    turnOnRed(&ws2812b, 1);
    turnOnRed(&ws2812b, 2);
    turnOnRed(&ws2812b, 3);

    /* Enable power to GPIO block */
    LPC_SYSCTL->PCONP |= (1 << 15);

    /* Configure P3.26 as digital output for WS2812B data line */
    Chip_IOCON_PinMuxSet(LPC_IOCON, 3, 26, IOCON_FUNC0 | IOCON_MODE_INACT);
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 3, 26);
    Chip_IOCON_PinMuxSet(LPC_IOCON, 3, 25, IOCON_FUNC0 | IOCON_MODE_INACT);
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 3, 25);

    /* Configure SysTick for 1 ms interrupts */
    if (SysTick_Config(SystemCoreClock / TICKRATE_HZ)) {
        while (1) {
            /* Error: SysTick config failed, trap CPU */
        }
    }

    /* Main application loop */
    while (1) {
        __WFI();   /* Wait For Interrupt - saves power */
    }

    return 0;
}
