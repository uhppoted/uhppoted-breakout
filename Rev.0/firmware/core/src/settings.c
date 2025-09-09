#include <string.h>

#include <hardware/flash.h>
#include <hardware/sync.h>

#include <breakout.h>
#include <log.h>
#include <settings.h>

#define LOGTAG "SETTINGS"

void flash_read();
void flash_write();
int flash_get_current_page();
uint32_t flash_get_version(int page);
uint32_t crc32(const char *, size_t);

settings SETTINGS = {
    .version = 0,

    .doors = {
        .interlock = NO_INTERLOCK,

        .door1 = {
            .mode = CONTROLLED,
            .delay = 5,
        },
        .door2 = {
            .mode = CONTROLLED,
            .delay = 5,
        },
        .door3 = {
            .mode = CONTROLLED,
            .delay = 5,
        },
        .door4 = {
            .mode = CONTROLLED,
            .delay = 5,
        },
    },
};

setting settings_get(SETTING tag) {
    switch (tag) {
    case INTERLOCK:
        return (setting){
            .tag = INTERLOCK,
            .value.uint8 = SETTINGS.doors.interlock,
        };
        break;

    case DOOR1_MODE:
        return (setting){
            .tag = DOOR1_MODE,
            .value.uint8 = SETTINGS.doors.door1.mode,
        };
        break;

    case DOOR2_MODE:
        return (setting){
            .tag = DOOR2_MODE,
            .value.uint8 = SETTINGS.doors.door2.mode,
        };
        break;

    case DOOR3_MODE:
        return (setting){
            .tag = DOOR3_MODE,
            .value.uint8 = SETTINGS.doors.door3.mode,
        };
        break;

    case DOOR4_MODE:
        return (setting){
            .tag = DOOR4_MODE,
            .value.uint8 = SETTINGS.doors.door4.mode,
        };
        break;

    case DOOR1_DELAY:
        return (setting){
            .tag = DOOR1_DELAY,
            .value.uint8 = SETTINGS.doors.door1.delay,
        };
        break;

    case DOOR2_DELAY:
        return (setting){
            .tag = DOOR2_DELAY,
            .value.uint8 = SETTINGS.doors.door2.delay,
        };
        break;

    case DOOR3_DELAY:
        return (setting){
            .tag = DOOR3_DELAY,
            .value.uint8 = SETTINGS.doors.door3.delay,
        };
        break;

    case DOOR4_DELAY:
        return (setting){
            .tag = DOOR4_DELAY,
            .value.uint8 = SETTINGS.doors.door4.delay,
        };
        break;
    }

    return (setting){
        .tag = UNKNOWN,
    };
}

void settings_set(setting v) {
    switch (v.tag) {
    case INTERLOCK:
        SETTINGS.doors.interlock = v.value.uint8;
        break;

    case DOOR1_MODE:
        SETTINGS.doors.door1.mode = v.value.uint8;
        break;

    case DOOR2_MODE:
        SETTINGS.doors.door2.mode = v.value.uint8;
        break;

    case DOOR3_MODE:
        SETTINGS.doors.door3.mode = v.value.uint8;
        break;

    case DOOR4_MODE:
        SETTINGS.doors.door4.mode = v.value.uint8;
        break;

    case DOOR1_DELAY:
        SETTINGS.doors.door1.delay = v.value.uint8;
        break;

    case DOOR2_DELAY:
        SETTINGS.doors.door2.delay = v.value.uint8;
        break;

    case DOOR3_DELAY:
        SETTINGS.doors.door3.delay = v.value.uint8;
        break;

    case DOOR4_DELAY:
        SETTINGS.doors.door4.delay = v.value.uint8;
        break;
    }

    push((message){
        .message = MSG_SAVE,
        .tag = MESSAGE_NONE,
    });
}

void settings_save() {
    flash_write();
    infof(LOGTAG, "settings saved");
}

void settings_restore() {
    flash_read();
    infof(LOGTAG, "settings restored (v%u)", SETTINGS.version);
}

// *** FLASH MEMORY ***

typedef struct header {
    uint32_t magic;
    uint32_t version;
    uint32_t crc;
} header;

struct {
    uint32_t buffer[FLASH_SECTOR_SIZE / sizeof(uint32_t)];
} flash_buffer = {};

const uint32_t OFFSETS[2] = {
    PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE,
    PICO_FLASH_SIZE_BYTES - 2 * FLASH_SECTOR_SIZE,
};

const int PAGES = sizeof(OFFSETS) / sizeof(uint32_t);
const uint32_t MAX_VERSION = 16384;
const uint32_t MAGIC_WORD = 0x53455453u;
const uint32_t DATA_OFFSET = 32;  // 3 header words + some padding
const size_t DATA_BYTES = 16 * 4; // 9 data words + some padding

/* Reads the settings from the onboard flash.
 *
 */
void flash_read() {
    uint32_t page = flash_get_current_page();

    if (page != -1 && page < PAGES) {
        uint32_t *addr = (uint32_t *)(XIP_BASE + OFFSETS[page]);
        uint32_t *version = addr + 1;
        uint32_t *p = addr + DATA_OFFSET;

        SETTINGS.version = *version;
        SETTINGS.doors.interlock = *p++;
        SETTINGS.doors.door1.mode = *p++;
        SETTINGS.doors.door1.delay = *p++;
        SETTINGS.doors.door2.mode = *p++;
        SETTINGS.doors.door2.delay = *p++;
        SETTINGS.doors.door3.mode = *p++;
        SETTINGS.doors.door3.delay = *p++;
        SETTINGS.doors.door4.mode = *p++;
        SETTINGS.doors.door4.delay = *p++;

        return;
    }

    warnf(LOGTAG, "*** NO VALID FLASH SETTINGS, USING DEFAULTS");
}

/* Writes the settings to the onboard flash.
 *
 */
void flash_write() {
    uint32_t page = flash_get_current_page();
    uint32_t version = flash_get_version(page);

    if (page != -1 && page < PAGES) {
        page = (page + 1) % PAGES;
        version = (version + 1) % MAX_VERSION;
    } else {
        page = 0;
        version = 1;
    }

    uint32_t offset = OFFSETS[page];
    struct header header;
    uint32_t *buffer = flash_buffer.buffer;

    memset(buffer, 0, FLASH_SECTOR_SIZE);

    // ... pack buffer
    uint32_t *p = buffer + DATA_OFFSET;

    *p++ = SETTINGS.doors.interlock;
    *p++ = SETTINGS.doors.door1.mode;
    *p++ = SETTINGS.doors.door1.delay;
    *p++ = SETTINGS.doors.door2.mode;
    *p++ = SETTINGS.doors.door2.delay;
    *p++ = SETTINGS.doors.door3.mode;
    *p++ = SETTINGS.doors.door3.delay;
    *p++ = SETTINGS.doors.door4.mode;
    *p++ = SETTINGS.doors.door4.delay;

    // ... set header
    header.magic = MAGIC_WORD;
    header.version = version;
    header.crc = crc32((char *)(buffer + DATA_OFFSET), DATA_BYTES);

    // ... copy header to buffer
    buffer[0] = header.magic;
    buffer[1] = header.version;
    buffer[2] = header.crc;

    // ... write to flash
    uint32_t interrupts = save_and_disable_interrupts();

    flash_range_erase(offset, FLASH_SECTOR_SIZE);
    flash_range_program(offset, (uint8_t *)buffer, FLASH_SECTOR_SIZE);
    restore_interrupts(interrupts);
}

int flash_get_current_page() {
    uint32_t version1 = *((uint32_t *)(XIP_BASE + OFFSETS[0]) + 1) % MAX_VERSION;
    uint32_t version2 = *((uint32_t *)(XIP_BASE + OFFSETS[1]) + 1) % MAX_VERSION;
    int index = (version2 == 0 ? MAX_VERSION : version2) > (version1 == 0 ? MAX_VERSION : version1) ? 1 : 0;

    for (int ix = 0; ix < PAGES; ix++) {
        uint32_t *addr = (uint32_t *)(XIP_BASE + OFFSETS[index % PAGES]);
        uint32_t *p = addr;
        struct header header;

        header.magic = *p++;
        header.version = *p++;
        header.crc = *p++;

        if (header.magic == MAGIC_WORD && header.version < MAX_VERSION) {
            uint32_t crc = crc32((char *)(addr + DATA_OFFSET), DATA_BYTES);
            if (header.crc == crc) {
                return index % PAGES;
            }
        }

        index++;
    }

    return -1;
}

uint32_t flash_get_version(int page) {
    if (page != -1 && page < PAGES) {
        uint32_t addr = XIP_BASE + OFFSETS[page];
        uint32_t version = *(((uint32_t *)addr) + 1);

        return version;
    }

    return 0;
}

// Ref. https://lxp32.github.io/docs/a-simple-example-crc32-calculation
uint32_t crc32(const char *s, size_t N) {
    uint32_t crc = 0xffffffff;

    for (size_t i = 0; i < N; i++) {
        char ch = s[i];
        for (size_t j = 0; j < 8; j++) {
            crc = ((ch ^ crc) & 1) ? (crc >> 1) ^ 0xEDB88320 : crc >> 1;
            ch >>= 1;
        }
    }

    return ~crc;
}
