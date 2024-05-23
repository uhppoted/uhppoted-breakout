#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include <I2C0.h>
#include <PCAL6408APW.h>
#include <log.h>

// RX8900SA I2C address
const uint8_t U5 = 0x64;

// RX8900SA registers
const uint8_t SECOND = 0x10;
const uint8_t MINUTE = 0x11;
const uint8_t HOUR = 0x12;
const uint8_t WEEKDAY = 0x13;
const uint8_t DAY = 0x14;
const uint8_t MONTH = 0x15;
const uint8_t YEAR = 0x16;
const uint8_t RAM = 0x07;
const uint8_t ALARM_MINUTE = 0x08;
const uint8_t ALARM_HOUR = 0x09;
const uint8_t ALARM_DAY = 0x0a;
const uint8_t TEMPERATURE = 0x17;
const uint8_t BACKUP = 0x18;
const uint8_t TIMER0 = 0x1b;
const uint8_t TIMER1 = 0x1c;
const uint8_t EXTENSION = 0x1d;
const uint8_t FLAG = 0x1e;
const uint8_t CONTROL = 0x1f;
const uint32_t tSTA = 3 * 1000; // 3s

// FLAG register bits
const uint8_t VDET = 0x01;
const uint8_t VLF = 0x02;
const uint8_t AF = 0x08;
const uint8_t TF = 0x10;
const uint8_t UF = 0x20;

// ALARM register bits
const uint8_t ALARM_OFF = 0x00;
const uint8_t ALARM_ON = 0x80;

// EXTENSION register bits
const uint8_t ALARM_DAILY = 0x40;
const uint8_t ALARM_WEEKDAY = 0x00;
const uint8_t UPDATE_SECOND = 0x00;
const uint8_t UPDATE_MINUTE = 0x20;
const uint8_t TIMER_ENABLED = 0x00;
const uint8_t TIMER_DISABLE = 0x10;
const uint8_t FREQ_DEFAULT = 0x00; // 32768
const uint8_t FREQ_1024 = 0x0c;
const uint8_t FREQ_1 = 0x08;
const uint8_t FREQ_32768 = 0x0c;
const uint8_t TICK_4096 = 0x00;
const uint8_t TICK_64 = 0x01;
const uint8_t TICK_SECOND = 0x02;
const uint8_t TICK_MINUTE = 0x03;

// FLAG register

// CONTROL register
const uint8_t COMPENSATE_05 = 0x00;
const uint8_t COMPENSATE_2 = 0x40;
const uint8_t COMPENSATE_10 = 0x80;
const uint8_t COMPENSATE_30 = 0xc0;
const uint8_t INTERRUPT_DISABLED = 0x00;
const uint8_t INTERRUPT_ENABLED = 0x20;
const uint8_t TIMER_INTERRUPT_DISABLED = 0x00;
const uint8_t TIMER_INTERRUPT_ENABLED = 0x10;
const uint8_t ALARM_INTERRUPT_DISABLED = 0x00;
const uint8_t ALARM_INTERRUPT_ENABLED = 0x08;
const uint8_t RESET = 0x01;
const uint8_t NO_RESET = 0x00;

// BACKUP register
const uint8_t BACKUP_DISABLED = 0x08;
const uint8_t BACKUP_ENABLED = 0x00;
const uint8_t BACKUP_SWITCH = 0x00;
const uint8_t BACKUP_DIODE = 0x04;
const uint8_t VDET_2MS = 0x00;
const uint8_t VDET_16MS = 0x01;
const uint8_t VDET_128MS = 0x02;
const uint8_t VDET_256MS = 0x03;

// weekdays
const uint8_t SUNDAY = 0x01;
const uint8_t MONDAY = 0x02;
const uint8_t TUESDAY = 0x04;
const uint8_t WEDNESDAY = 0x08;
const uint8_t THURSDAY = 0x10;
const uint8_t FRIDAY = 0x20;
const uint8_t SATURDAY = 0x40;

// months
const uint8_t JANUARY = 0x01;
const uint8_t FEBRUARY = 0x02;
const uint8_t MARCH = 0x03;
const uint8_t APRIL = 0x04;
const uint8_t MAY = 0x05;
const uint8_t JUNE = 0x06;
const uint8_t JULY = 0x07;
const uint8_t AUGUST = 0x08;
const uint8_t SEPTEMBER = 0x09;
const uint8_t OCTOBER = 0x10;
const uint8_t NOVEMBER = 0x011;
const uint8_t DECEMBER = 0x012;

// function prototypes
void RX8900SA_setup();

/*
 * Retrieves the RX8900SA FLAG register and initialises all registers if the VLF
 * bit is set.
 */
void RX8900SA_init() {
    infof("RX8900SA", "init");

    int err;

    sleep_us(1000);

    // ... reset RX8900SA
    uint8_t csel = COMPENSATE_2;
    uint8_t uie = INTERRUPT_DISABLED;
    uint8_t tie = TIMER_INTERRUPT_DISABLED;
    uint8_t aie = ALARM_INTERRUPT_DISABLED;
    uint8_t reset = RESET;

    if ((err = I2C0_write(U5, CONTROL, csel | uie | tie | aie | reset)) < 1) {
        warnf("RX8900SA", "CONTROL write error:%d", err);
    }

    // ... check VLF
    uint8_t flag;

    if ((err = I2C0_read(U5, FLAG, &flag)) < 1) {
        warnf("RX8900SA", "FLAG read error:%d", err);
    } else {
        debugf("RX8900SA", "FLAG:%02x", flag);
        debugf("RX8900SA", "     VDET:%d", (flag & VDET) == VDET, err);
        debugf("RX8900SA", "     VLF: %d", (flag & VLF) == VLF, err);
        debugf("RX8900SA", "     AF:  %d", (flag & AF) == AF, err);
        debugf("RX8900SA", "     TF:  %d", (flag & TF) == TF, err);
        debugf("RX8900SA", "     UF:  %d", (flag & UF) == UF, err);

        if ((flag & VLF) == VLF) {

            sleep_ms(tSTA); // FIXME use alarm timer
            RX8900SA_setup();
        }
    }

    sleep_us(1000);
}

/*
 * Sets all RX8900SA registers to known values:
 *
 */
void RX8900SA_setup() {
    debugf("RX8900SA", "setup");

    int err;

    // ... date/time
    uint8_t year = 0x24; // BCD
    uint8_t month = MAY;
    uint8_t day = 0x23;    // BCD
    uint8_t hour = 0x12;   // BCD
    uint8_t minute = 0x34; // BCD
    uint8_t second = 0x56; // BCD
    uint8_t weekday = THURSDAY;

    if ((err = I2C0_write(U5, SECOND, second)) < 1) {
        warnf("RX8900SA", "SECONDS write error:%d", err);
    }

    sleep_us(10);

    if ((err = I2C0_write(U5, MINUTE, minute)) < 1) {
        warnf("RX8900SA", "MINUTES write error:%d", err);
    }

    sleep_us(10);

    if ((err = I2C0_write(U5, HOUR, hour)) < 1) {
        warnf("RX8900SA", "HOURS write error:%d", err);
    }

    sleep_us(10);

    if ((err = I2C0_write(U5, DAY, day)) < 1) {
        warnf("RX8900SA", "DAY write error:%d", err);
    }

    sleep_us(10);

    if ((err = I2C0_write(U5, MONTH, month)) < 1) {
        warnf("RX8900SA", "MONTH write error:%d", err);
    }

    sleep_us(10);

    if ((err = I2C0_write(U5, YEAR, year)) < 1) {
        warnf("RX8900SA", "YEAR write error:%d", err);
    }

    sleep_us(10);

    if ((err = I2C0_write(U5, WEEKDAY, weekday)) < 1) {
        warnf("RX8900SA", "WEEKDAY write error:%d", err);
    }

    sleep_us(10);

    if ((err = I2C0_write(U5, RAM, 0x00)) < 1) {
        warnf("RX8900SA", "RAM write error:%d", err);
    }

    sleep_us(10);

    // ... alarm
    uint8_t alarm_enable = ALARM_OFF;
    uint8_t alarm_minute = 0x00;  // BCD
    uint8_t alarm_hour = 0x00;    // BCD
    uint8_t alarm_weekday = 0x00; // BCD

    if ((err = I2C0_write(U5, ALARM_MINUTE, alarm_enable | alarm_minute)) < 1) {
        warnf("RX8900SA", "ALARM MINUTE write error:%d", err);
    }

    sleep_us(10);

    if ((err = I2C0_write(U5, ALARM_HOUR, alarm_enable | alarm_hour)) < 1) {
        warnf("RX8900SA", "ALARM HOUR write error:%d", err);
    }

    sleep_us(10);

    if ((err = I2C0_write(U5, ALARM_DAY, alarm_enable | alarm_weekday)) < 1) {
        warnf("RX8900SA", "ALARM DAILY/WEEKDAY write error:%d", err);
    }

    sleep_us(10);

    // ... timers
    uint8_t timer0 = 0xff;
    uint8_t timer1 = 0x0f;

    if ((err = I2C0_write(U5, TIMER0, timer0)) < 1) {
        warnf("RX8900SA", "TIMER 0 write error:%d", err);
    }

    sleep_us(10);

    if ((err = I2C0_write(U5, TIMER1, timer1)) < 1) {
        warnf("RX8900SA", "TIMER 1 write error:%d", err);
    }

    sleep_us(10);

    // ... extension
    uint8_t alarm = ALARM_DAILY;
    uint8_t update = UPDATE_SECOND;
    uint8_t timer = TIMER_DISABLE;
    uint8_t freq = FREQ_1;
    uint8_t tick = TICK_SECOND;

    if ((err = I2C0_write(U5, EXTENSION, alarm | update | timer | freq | tick)) < 1) {
        warnf("RX8900SA", "EXTENSION write error:%d", err);
    }

    sleep_us(10);

    // ... flag
    uint8_t vdet = 0x00;
    uint8_t vlf = 0x00;
    uint8_t uf = 0x00;
    uint8_t tf = 0x00;
    uint8_t af = 0x00;

    if ((err = I2C0_write(U5, FLAG, af | tf | uf | vlf | vdet)) < 1) {
        warnf("RX8900SA", "FLAG write error:%d", err);
    }

    sleep_us(10);

    // ... control
    uint8_t csel = COMPENSATE_2;
    uint8_t uie = INTERRUPT_DISABLED;
    uint8_t tie = TIMER_INTERRUPT_DISABLED;
    uint8_t aie = ALARM_INTERRUPT_DISABLED;
    uint8_t reset = NO_RESET;

    if ((err = I2C0_write(U5, CONTROL, csel | uie | tie | aie | reset)) < 1) {
        warnf("RX8900SA", "CONTROL write error:%d", err);
    }

    sleep_us(10);

    // ... battery backup
    uint8_t vbat = BACKUP_ENABLED;
    uint8_t swoff = BACKUP_SWITCH;
    uint8_t bksmp = VDET_2MS;

    if ((err = I2C0_write(U5, BACKUP, vbat | swoff | bksmp)) < 1) {
        warnf("RX8900SA", "BACKUP write error:%d", err);
    }

    sleep_us(10);

    // ... all done
    infof("RX8900SA", "setup/done");
}
