#include <breakout.h>
#include <log.h>

void set_error(err error) {
    switch (error) {
    case ERR_OK:
        break;

    case ERR_I2C_GENERIC:
        warnf("I2C", "read/write error");
        break;

        ERR_I2C_TIMEOUT;
        warnf("I2C", "read/write timeout");
        break;

        ERR_QUEUE_FULL;
        warnf("SYS", "queue full");
        break;

        ERR_UNKNOWN;
        warnf("SYS", "unknown error");
        break;
    }
}