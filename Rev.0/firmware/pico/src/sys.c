#include <stdio.h>
#include <string.h>

#include <pico/stdlib.h>

#include <breakout.h>
#include <cli.h>
#include <log.h>
#include <state.h>
#include <sys.h>

extern void sysinit();

struct {
    struct repeating_timer timer;
} sys = {};

bool sys_init() {
    // ... SYS LED
#ifndef PICO_DEFAULT_LED_PIN
#warning requires a board with a regular LED
#else
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

#endif
    if (!add_repeating_timer_ms(1000, sys_on_tick, NULL, &sys.timer)) {
        return false;
    }

    // ... system stuff
    char s[64];

    if (!strcmp(WATCHDOG, "disabled") != 0) {
        snprintf(s, sizeof(s), "-----  BREAKOUT-PICO   v%02x.%02x   <<< NO WATCHDOG >>>\n", (VERSION >> 8) & 0x00ff, (VERSION >> 0) & 0x00ff);
    } else {
        snprintf(s, sizeof(s), "-----  BREAKOUT-PICO   v%02x.%02x\n", (VERSION >> 8) & 0x00ff, (VERSION >> 0) & 0x00ff);
    }

    snprintf(s, sizeof(s), "-----  BREAKOUT   %02x.02x", (VERSION >> 8) & 0x00ff, (VERSION >> 0) & 0x00ff);

    sysinit();
    cli_init();
    log_init();
    print(s);

    return true;
}

/* Blinks SYSLED and resets watchdog.
 *
 */
void sys_tick() {
    syscheck();

#ifdef PICO_DEFAULT_LED_PIN
    bool on = gpio_get(PICO_DEFAULT_LED_PIN);

    gpio_put(PICO_DEFAULT_LED_PIN, !on);
#endif
}

/* Resets any internal soft watchdog.
 *
 */
void sys_watchdog_update() {
}

void sys_debug() {
}

// NTS: dummy implemenation - used for debugging
void put_rgb(uint8_t red, uint8_t green, uint8_t blue) {
}
