#pragma once

#include <I2C.h>
#include <stdint.h>

extern int RX8900SA_init(I2C dev);
extern int RX8900SA_reset(I2C dev);
extern int RX8900SA_setup(I2C dev);
extern int RX8900SA_get_date(I2C dev, uint16_t *year, uint8_t *month, uint8_t *day);
extern int RX8900SA_set_date(I2C dev, uint16_t year, uint8_t month, uint8_t day);
extern int RX8900SA_get_time(I2C dev, uint8_t *hour, uint8_t *minute, uint8_t *second);
extern int RX8900SA_set_time(I2C dev, uint8_t hour, uint8_t minute, uint8_t second);
extern int RX8900SA_get_datetime(I2C dev, uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second);
extern int RX8900SA_set_datetime(I2C dev, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t weekday);
extern int RX8900SA_get_dow(I2C dev, uint8_t *weekday);
extern int RX8900SA_set_dow(I2C dev, uint8_t weekday);

extern const uint8_t SUNDAY;
extern const uint8_t MONDAY;
extern const uint8_t TUESDAY;
extern const uint8_t WEDNESDAY;
extern const uint8_t THURSDAY;
extern const uint8_t FRIDAY;
extern const uint8_t SATURDAY;

extern const uint32_t RX8900SA_tSTA;
