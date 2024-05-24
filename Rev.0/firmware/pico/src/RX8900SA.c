#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include <I2C0.h>
#include <PCAL6408APW.h>
#include <log.h>

// RX8900SA registers
const uint8_t TIME = 0x10;
const uint8_t DATE = 0x14;

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
int RX8900SA_reset(uint8_t addr);
int RX8900SA_setup(uint8_t addr);
uint8_t bcd(uint8_t N);

/*
 * Retrieves the RX8900SA FLAG register and initialises all registers if the VLF
 * bit is set.
 */
void RX8900SA_init(uint8_t addr) {
    infof("RX8900SA", "%02x  init", addr);

    int err;

    sleep_us(1000);
    if ((err = RX8900SA_reset(addr)) != 0) {
        warnf("RX8900SA", "%02x  reset errror (%d)", addr, err);
    }

    // ... check VLF
    uint8_t flag;

    if ((err = I2C0_read(addr, FLAG, &flag)) < 1) {
        warnf("RX8900SA", "%02x  FLAG read error:%d", addr, err);
    } else {
        debugf("RX8900SA", "%02x  FLAG:%02x", addr, flag);
        debugf("RX8900SA", "%02x       VDET:%d", addr, (flag & VDET) == VDET, err);
        debugf("RX8900SA", "%02x       VLF: %d", addr, (flag & VLF) == VLF, err);
        debugf("RX8900SA", "%02x       AF:  %d", addr, (flag & AF) == AF, err);
        debugf("RX8900SA", "%02x       TF:  %d", addr, (flag & TF) == TF, err);
        debugf("RX8900SA", "     UF:  %d", addr, (flag & UF) == UF, err);

        if ((flag & VLF) == VLF) {

            sleep_ms(tSTA); // FIXME use alarm timer
            RX8900SA_setup(addr);
        }
    }

    sleep_us(1000);
}

/*
 * Resets the RX8900SA.
 */
int RX8900SA_reset(uint8_t addr) {
    infof("RX8900SA", "%02x  reset", addr);

    int err;

    // ... reset RX8900SA
    uint8_t csel = COMPENSATE_2;
    uint8_t uie = INTERRUPT_DISABLED;
    uint8_t tie = TIMER_INTERRUPT_DISABLED;
    uint8_t aie = ALARM_INTERRUPT_DISABLED;
    uint8_t reset = RESET;

    if ((err = I2C0_write(addr, CONTROL, csel | uie | tie | aie | reset)) != 1) {
        warnf("RX8900SA", "%02x  CONTROL write error:%d", addr, err);
        return -1;
    }

    sleep_us(1000);

    return 0;
}

/*
 * Sets all RX8900SA registers to known values:
 *
 */
int RX8900SA_setup(uint8_t addr) {
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

    if ((err = I2C0_write(addr, SECOND, second)) < 1) {
        warnf("RX8900SA", "%02x  SECONDS write error:%d", addr, err);
    }

    sleep_us(10);

    if ((err = I2C0_write(addr, MINUTE, minute)) < 1) {
        warnf("RX8900SA", "%02x  MINUTES write error:%d", addr, err);
    }

    sleep_us(10);

    if ((err = I2C0_write(addr, HOUR, hour)) < 1) {
        warnf("RX8900SA", "%02x  HOURS write error:%d", addr, err);
    }

    sleep_us(10);

    if ((err = I2C0_write(addr, DAY, day)) < 1) {
        warnf("RX8900SA", "%02x  DAY write error:%d", addr, err);
    }

    sleep_us(10);

    if ((err = I2C0_write(addr, MONTH, month)) < 1) {
        warnf("RX8900SA", "%02x  MONTH write error:%d", addr, err);
    }

    sleep_us(10);

    if ((err = I2C0_write(addr, YEAR, year)) < 1) {
        warnf("RX8900SA", "%02x  YEAR write error:%d", addr, err);
    }

    sleep_us(10);

    if ((err = I2C0_write(addr, WEEKDAY, weekday)) < 1) {
        warnf("RX8900SA", "%02x  WEEKDAY write error:%d", addr, err);
    }

    sleep_us(10);

    if ((err = I2C0_write(addr, RAM, 0x00)) < 1) {
        warnf("RX8900SA", "%02x  RAM write error:%d", addr, err);
    }

    sleep_us(10);

    // ... alarm
    uint8_t alarm_enable = ALARM_OFF;
    uint8_t alarm_minute = 0x00;  // BCD
    uint8_t alarm_hour = 0x00;    // BCD
    uint8_t alarm_weekday = 0x00; // BCD

    if ((err = I2C0_write(addr, ALARM_MINUTE, alarm_enable | alarm_minute)) < 1) {
        warnf("RX8900SA", "%02x  ALARM MINUTE write error:%d", addr, err);
    }

    sleep_us(10);

    if ((err = I2C0_write(addr, ALARM_HOUR, alarm_enable | alarm_hour)) < 1) {
        warnf("RX8900SA", "%02x  ALARM HOUR write error:%d", addr, err);
    }

    sleep_us(10);

    if ((err = I2C0_write(addr, ALARM_DAY, alarm_enable | alarm_weekday)) < 1) {
        warnf("RX8900SA", "%02x  ALARM DAILY/WEEKDAY write error:%d", addr, err);
    }

    sleep_us(10);

    // ... timers
    uint8_t timer0 = 0xff;
    uint8_t timer1 = 0x0f;

    if ((err = I2C0_write(addr, TIMER0, timer0)) < 1) {
        warnf("RX8900SA", "%02x  TIMER 0 write error:%d", addr, err);
    }

    sleep_us(10);

    if ((err = I2C0_write(addr, TIMER1, timer1)) < 1) {
        warnf("RX8900SA", "%02x  TIMER 1 write error:%d", addr, err);
    }

    sleep_us(10);

    // ... extension
    uint8_t alarm = ALARM_DAILY;
    uint8_t update = UPDATE_SECOND;
    uint8_t timer = TIMER_DISABLE;
    uint8_t freq = FREQ_1;
    uint8_t tick = TICK_SECOND;

    if ((err = I2C0_write(addr, EXTENSION, alarm | update | timer | freq | tick)) < 1) {
        warnf("RX8900SA", "%02x  EXTENSION write error:%d", addr, err);
    }

    sleep_us(10);

    // ... flag
    uint8_t vdet = 0x00;
    uint8_t vlf = 0x00;
    uint8_t uf = 0x00;
    uint8_t tf = 0x00;
    uint8_t af = 0x00;

    if ((err = I2C0_write(addr, FLAG, af | tf | uf | vlf | vdet)) < 1) {
        warnf("RX8900SA", "%02x  FLAG write error:%d", addr, err);
    }

    sleep_us(10);

    // ... control
    uint8_t csel = COMPENSATE_2;
    uint8_t uie = INTERRUPT_DISABLED;
    uint8_t tie = TIMER_INTERRUPT_DISABLED;
    uint8_t aie = ALARM_INTERRUPT_DISABLED;
    uint8_t reset = NO_RESET;

    if ((err = I2C0_write(addr, CONTROL, csel | uie | tie | aie | reset)) < 1) {
        warnf("RX8900SA", "%02x  CONTROL write error:%d", addr, err);
    }

    sleep_us(10);

    // ... battery backup
    uint8_t vbat = BACKUP_ENABLED;
    uint8_t swoff = BACKUP_SWITCH;
    uint8_t bksmp = VDET_2MS;

    if ((err = I2C0_write(addr, BACKUP, vbat | swoff | bksmp)) < 1) {
        warnf("RX8900SA", "%02x  BACKUP write error:%d", addr, err);
    }

    sleep_us(10);

    // ... all done
    infof("RX8900SA", "%02x  setup/done", addr);

    return 0;
}

void RX8900SA_get_date(uint8_t addr, char yymmmdd[11]) {
    uint8_t date[3];
    int err;

    if ((err = I2C0_read_all(addr, DATE, date, 3)) != 3) {
        warnf("RX8900SA", "%02x  DATE read error:%d", addr, err);

        snprintf(yymmmdd, 11, "2000-01-01");
    } else {
        uint8_t year = date[2];
        uint8_t month = date[1];
        uint8_t day = date[0];

        snprintf(yymmmdd, 11, "20%02x-%02x-%02x", year, month, day);
    }
}

void RX8900SA_set_date(uint8_t addr, uint16_t year, uint8_t month, uint8_t day) {
    int err;
    uint8_t yy = bcd(year % 100);
    uint8_t mm = bcd(month);
    uint8_t dd = bcd(day);
    uint8_t date[] = {dd, mm, yy};

    if ((err = I2C0_write_all(addr, DATE, date, 3)) != 3) {
        warnf("RX8900SA", "%02x  DATE write error:%d", addr, err);
    }
}

void RX8900SA_get_time(uint8_t addr, char HHmmss[9]) {
    uint8_t time[3];
    int err;

    if ((err = I2C0_read_all(addr, TIME, time, 3)) != 3) {
        warnf("RX8900SA", "%02x  TIME read error:%d", addr, err);

        snprintf(HHmmss, 9, "00:00:00");
    } else {
        uint8_t hour = time[2];
        uint8_t minute = time[1];
        uint8_t second = time[0];

        snprintf(HHmmss, 9, "%02x:%02x:%02x", hour, minute, second);
    }
}

void RX8900SA_set_time(uint8_t addr, uint8_t hour, uint8_t minute, uint8_t second) {
    int err;
    uint8_t hh = bcd(hour);
    uint8_t mm = bcd(minute);
    uint8_t ss = bcd(second);
    uint8_t time[] = {ss, mm, hh};

    if ((err = I2C0_write_all(addr, TIME, time, 3)) != 3) {
        warnf("RX8900SA", "%02x  TIME write error:%d", addr, err);
    }
}

void RX8900SA_debug(uint8_t addr) {
    int err;
    uint8_t flags;
    uint8_t byte = 0x00;

    if ((err = I2C0_read(addr, FLAG, &flags)) != 1) {
        warnf("RX8900SA", "%02x FLAG read error:%d", addr, err);
    }

    sleep_us(10);
    debugf("RX8900SA", "debug  FLAGS:%02x", flags);

    if ((err = I2C0_write(addr, FLAG, byte)) != 0) {
        warnf("RX8900SA", "%02x FLAG write error:%d", addr, err);
    }

    sleep_us(10);

    if ((err = I2C0_read(addr, FLAG, &flags)) != 1) {
        warnf("RX8900SA", "%02x FLAG read error:%d", addr, err);
    }

    sleep_us(10);
    debugf("RX8900SA", "debug  FLAGS:%02x", flags);
}

uint8_t bcd(uint8_t N) {
    uint8_t v = N % 100;

    return ((v / 10) << 4) | (v % 10);
}