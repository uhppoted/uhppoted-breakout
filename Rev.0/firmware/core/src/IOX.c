#include <pico/stdlib.h>

#include <I2C0.h>
#include <I2C1.h>
#include <IOX.h>
#include <U2.h>
#include <U3.h>
#include <breakout.h>
#include <log.h>

void IOX_init() {
    infof("IOX", "init");

    gpio_init(IOX_RESET);
    gpio_set_dir(IOX_RESET, GPIO_OUT);

    gpio_put(IOX_RESET, 0);
    sleep_us(5);
    gpio_put(IOX_RESET, 1);
    sleep_us(10);

    U2_init();
    U3_init();

    irq_set_enabled(IO_IRQ_BANK0, true);
}