#include <hardware/spi.h>
#include <pico/stdlib.h>

#include <SPI.h>
#include <breakout.h>
#include <log.h>
#include <state.h>

void SPI_on_interrupt();

void SPI_init() {
    gpio_set_function(SPI_TX, GPIO_FUNC_SPI);
    gpio_set_function(SPI_RX, GPIO_FUNC_SPI);
    gpio_set_function(SPI_CLK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_CSn, GPIO_FUNC_SPI);

    uint baudrate = spi_init(spi0, 5000000);
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    spi_set_slave(spi0, true);

    // spi0_hw->imsc = 1 << 2;s
    // spi0_hw->imsc = SSPRXINTR;
    // spi0_hw->imsc = SPI_SSPIMSC_RTIM_BITS | SPI_SSPIMSC_RORIM_BITS | SPI_SSPIMSC_RXIM_BITS;

    irq_set_exclusive_handler(SPI0_IRQ, SPI_on_interrupt);
    irq_set_enabled(SPI0_IRQ, true);

    debugf("SPI", "initialised %u", baudrate);
}

void SPI_on_interrupt() {
    uint32_t msg = MSG_SPI;
    uint8_t buffer[16];

    while (spi_is_readable(spi0)) {
        int N = spi_read_blocking(spi0, 0x12, buffer, 16);
        // debugf("SPI", ">>>> read: %d", N);

        // for (int i = 0; i < N; i++) {
        //     debugf("SPI", ">>>> read: %d %02x %c", i, buffer[i], buffer[i]);
        // }
    }

    if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
        set_error(ERR_QUEUE_FULL, "SPI", "rx: queue full");
    }
}

void SPI_debug() {
    uint baudrate = spi_get_baudrate(spi0);
    bool writeable = spi_is_writable(spi0);
    bool readable = spi_is_readable(spi0);
    uint8_t buffer[16];

    debugf("SPI", ">>>> baudrate: %u", baudrate);
    debugf("SPI", ">>>> writeable:%d", writeable);
    debugf("SPI", ">>>> readable: %d", readable);

    while (spi_is_readable(spi0)) {
        int N = spi_read_blocking(spi0, 0x12, buffer, 1);
        debugf("SPI", ">>>> read: %d", N);

        for (int i = 0; i < N; i++) {
            debugf("SPI", ">>>> read: %d %02x %c", i, buffer[i], buffer[i]);
        }
    }

    debugf("SPI", ">>>> 'k, done");
}