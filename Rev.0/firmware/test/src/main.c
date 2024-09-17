#include <stdio.h>
#include <stdlib.h>

#include <pico/binary_info.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

#include <hardware/watchdog.h>

#define _VERSION "v0.0"
#define _I2C0SDA 12
#define _I2C0SCL 13
#define _I2C1SDA 26
#define _I2C1SCL 27

const char *VERSION = _VERSION;
const uint32_t WATCHDOG_TIMEOUT = 5000; // ms

extern bool sys_init();
extern bool sys_blink();

int main() {
    bi_decl(bi_program_description("uhppoted-breakout::test"));
    bi_decl(bi_program_version_string(_VERSION));
    bi_decl(bi_2pins_with_func(_I2C0SDA, _I2C0SCL, GPIO_FUNC_I2C));
    bi_decl(bi_2pins_with_func(_I2C1SDA, _I2C1SCL, GPIO_FUNC_I2C));

    stdio_init_all();
    // watchdog_enable(WATCHDOG_TIMEOUT, true);

    sleep_ms(5000);

    // ... initialise system
    if (!sys_init()) {
        printf("ERROR INITIALISING TEST SYSTEM\n");
        return -1;
    }

    // ... run tests
    while (true) {
        sleep_ms(1000);
        sys_blink();
    }
}
