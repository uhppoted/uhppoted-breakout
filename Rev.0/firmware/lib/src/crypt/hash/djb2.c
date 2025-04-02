#include <crypt/hash/djb2.h>

uint32_t djb2(const char *str) {
    uint32_t hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + (uint8_t)c; /* hash * 33 + c */

    return hash;
}