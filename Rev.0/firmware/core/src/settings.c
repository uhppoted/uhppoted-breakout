#include <log.h>
#include <settings.h>

#define LOGTAG "DOORS"

settings SETTINGS = {
    .doors = {
        .interlock = 0,

        .door1 = {
            .mode = 3,
            .delay = 5,
        },
        .door2 = {
            .mode = 3,
            .delay = 5,
        },
        .door3 = {
            .mode = 3,
            .delay = 5,
        },
        .door4 = {
            .mode = 3,
            .delay = 5,
        },
    },
};

void settings_save() {
    infof(LOGTAG, "settings saved");
}

void settings_restore() {
    infof(LOGTAG, "settings restored");
}