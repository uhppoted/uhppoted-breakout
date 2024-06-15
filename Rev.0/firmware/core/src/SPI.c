#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    uint baudrate = spi_init(spi0, 2000000);
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    spi_set_slave(spi0, true);

    spi0_hw->imsc = SPI_SSPIMSC_RXIM_BITS | SPI_SSPIMSC_RTIM_BITS | SPI_SSPIMSC_RORIM_BITS;
    irq_set_exclusive_handler(SPI0_IRQ, SPI_on_interrupt);
    irq_set_enabled(SPI0_IRQ, true);

    debugf("SPI", "initialised %u", baudrate);
}

void SPI_on_interrupt() {
    uint8_t bytes[16];
    int ix = 0;

    while (spi_is_readable(spi0) && ix < sizeof(buffer)) {
        int N;
        uint8_t byte;

        if ((N = spi_read_blocking(spi0, 0x12, &byte, 1)) == 1) {
            bytes[ix++] = byte;
        }
    }

    if (ix > 0) {
        buffer *b = (buffer *)calloc(1, sizeof(buffer));
        b->N = ix;
        b->data = (uint8_t *)calloc(ix, sizeof(uint8_t));

        memmove(b->data, bytes, ix);

        uint32_t msg = MSG_SPI | ((uint32_t)b & 0x0fffffff); // SRAM_BASE is 0x20000000
        if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
            set_error(ERR_QUEUE_FULL, "SPI", "rx: queue full");
        }
    }
}

void SPI_rx(int N, const uint8_t *data) {
    char s[N + 1];

    snprintf(s, N + 1, "%s", data);
    debugf("SPI", "RX: %s", s);
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