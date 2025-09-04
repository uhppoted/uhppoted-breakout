#include <string.h>

#include <hardware/flash.h>
#include <hardware/sync.h>

#include <log.h>
#include <settings.h>

#define LOGTAG "DOORS"

void flash_read();
void flash_write();
int flash_get_current_page();
uint32_t flash_get_version(int page);
uint32_t crc32(const char *, size_t);

settings SETTINGS = {
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

void settings_save() {
    flash_write();
    infof(LOGTAG, "settings saved");
}

void settings_restore() {
    flash_read();
    infof(LOGTAG, "settings restored");
}

// *** FLASH MEMORY ***

const uint32_t OFFSETS[2] = {
    PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE,
    PICO_FLASH_SIZE_BYTES - 2 * FLASH_SECTOR_SIZE,
};

const int PAGES = sizeof(OFFSETS) / sizeof(uint32_t);
const uint32_t MAX_VERSION = 16384;
const uint32_t MAGIC_WORD = 0x53455453u;
const uint32_t DATA_OFFSET = 128;
const uint32_t DATA_PREAMBLE = 128;

typedef struct header {
    uint32_t magic;
    uint32_t version;
    uint32_t crc;
} header;

/* Reads the settings from the onboard flash.
 *
 */
void flash_read() {
    uint32_t page = flash_get_current_page();

    debugf(LOGTAG, ">>>>>>>> FLASH READ  page:%d", page);

    if (page != -1 && page < PAGES) {
        //     uint32_t addr = XIP_BASE + OFFSETS[page];
        //     uint32_t size = *(((uint32_t *)addr) + 2);
        //     uint32_t *p = (uint32_t *)(addr + CARDS_OFFSET);
        //     uint32_t *q = (uint32_t *)(addr + PASSCODES_OFFSET);
        //     int ix = 0;

        //     uint32_t *r = (uint32_t *)(addr);

        //     // ... get passcodes
        //     for (int i = 0; i < max_passcodes; i++) {
        //         passcodes[i] = *q++;
        //     }

        //     // ... get cards
        //     for (uint32_t i = 0; i < size && ix < *N; i++) {
        //         uint32_t card = *(p + 0);
        //         uint32_t start = *(p + 1);
        //         uint32_t end = *(p + 2);
        //         bool allowed = *(p + 3) == ACL_ALLOWED;
        //         char *PIN = (char *)(p + 4);
        //         char *name = (char *)(p + 6);

        //         p += 16;

        //         cards[ix].card_number = card;
        //         cards[ix].start = bin2date(start);
        //         cards[ix].end = bin2date(end);
        //         cards[ix].allowed = allowed;
        //         snprintf(cards[i].PIN, sizeof(cards[ix].PIN), PIN);
        //         snprintf(cards[ix].name, sizeof(cards[ix].name), name);

        //         ix++;
        //     }

        //     *N = ix;

        return;
    }

    warnf(LOGTAG, "*** NO VALID FLASH SETTINGS, USING DEFAULTS");
}

/* Writes the settings to the onboard flash.
 *
 */

uint32_t buffer[FLASH_SECTOR_SIZE / sizeof(uint32_t)];

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
    uint32_t N = 16 * 4; // 9 uint32 values plus some padding

    debugf(LOGTAG, ">>>>>>>> FLASH WRITE page:%d  version:%d  buffer:%d  N:%d", page, version, sizeof(buffer), N);

    memset(buffer, 0, sizeof(buffer));

    // ... pack buffer
    // uint32_t *p = &buffer[32];

    // *p++ = SETTINGS.doors.interlock;
    // *p++ = SETTINGS.doors.door1.mode;
    // *p++ = SETTINGS.doors.door1.delay;
    // *p++ = SETTINGS.doors.door2.mode;
    // *p++ = SETTINGS.doors.door2.delay;
    // *p++ = SETTINGS.doors.door3.mode;
    // *p++ = SETTINGS.doors.door3.delay;
    // *p++ = SETTINGS.doors.door4.mode;
    // *p++ = SETTINGS.doors.door4.delay;

    // // ... set header
    // header.magic = MAGIC_WORD;
    // header.version = version;
    // header.crc = crc32((char *)(&buffer[32]), DATA_PREAMBLE + N);

    // // ... copy header to buffer
    // buffer[0] = header.magic;
    // buffer[1] = header.version;
    // buffer[2] = header.crc;

    // // ... write to flash
    // uint32_t interrupts = save_and_disable_interrupts();

    // flash_range_erase(offset, FLASH_SECTOR_SIZE);
    // flash_range_program(offset, (uint8_t *)buffer, FLASH_SECTOR_SIZE);
    // restore_interrupts(interrupts);
}

int flash_get_current_page() {
    uint32_t version1 = *((uint32_t *)(XIP_BASE + OFFSETS[0]) + 1) % MAX_VERSION;
    uint32_t version2 = *((uint32_t *)(XIP_BASE + OFFSETS[1]) + 1) % MAX_VERSION;
    int index = (version2 == 0 ? MAX_VERSION : version2) > (version1 == 0 ? MAX_VERSION : version1) ? 1 : 0;

    for (int ix = 0; ix < PAGES; ix++) {
        uint32_t addr = XIP_BASE + OFFSETS[index % PAGES];
        uint32_t *p = (uint32_t *)addr;
        struct header header;
        uint32_t N = 16 * 4; // 9 uint32 values plus some padding

        header.magic = *p++;
        header.version = *p++;
        header.crc = *p++;

        if (header.magic == MAGIC_WORD && header.version < MAX_VERSION) {
            uint32_t crc = crc32((char *)(addr + DATA_OFFSET), DATA_PREAMBLE + N);

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
