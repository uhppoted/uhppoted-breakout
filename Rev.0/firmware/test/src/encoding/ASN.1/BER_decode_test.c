/*
 * Validation test suite for ASN.1 decoding.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <encoding/ASN.1/BER.h>

extern field *unpack_integer(const uint8_t *byte, int N, int *ix);

bool test_decode_integer(uint8_t *bytes, int length, int64_t expected, const char *message);

bool test_ASN1_decode_integer() {
    printf("-- ASN.1::decode integer\n");

    bool passed = true;

    // smallest positive number (0)
    {
        int64_t expected = 0;
        uint8_t bytes[] = {0x02, 0x01, 0x00};

        if (!test_decode_integer(bytes, sizeof(bytes), expected, "smallest positive number")) {
            passed = false;
        }
    }

    // largest positive one-byte integer (127)
    {
        int64_t expected = 127;
        uint8_t bytes[] = {0x02, 0x01, 0x7f};

        if (!test_decode_integer(bytes, sizeof(bytes), expected, "largest positive one-byte integer")) {
            passed = false;
        }
    }

    // smallest negative one-byte integer (-128)
    {
        int64_t expected = -128;
        uint8_t bytes[] = {0x02, 0x01, 0x80};

        if (!test_decode_integer(bytes, sizeof(bytes), expected, "smallest negative one-byte integer")) {
            passed = false;
        }
    }

    // positive number needing a leading zero byte (128)
    {
        int64_t expected = 128;
        uint8_t bytes[] = {0x02, 0x02, 0x00, 0x80};

        if (!test_decode_integer(bytes, sizeof(bytes), expected, "positive number needing a leading zero byte")) {
            passed = false;
        }
    }

    // negative number needing extra bytes (-129)
    {
        int64_t expected = -129;
        uint8_t bytes[] = {0x02, 0x02, 0xff, 0x7f};

        if (!test_decode_integer(bytes, sizeof(bytes), expected, "negative number needing extra bytes")) {
            passed = false;
        }
    }

    // largest two-byte positive integer (32767)
    {
        int64_t expected = 32767;
        uint8_t bytes[] = {0x02, 0x02, 0x7f, 0xff};

        if (!test_decode_integer(bytes, sizeof(bytes), expected, "largest two-byte positive integer")) {
            passed = false;
        }
    }

    // smallest two-byte negative integer (-32768)
    {
        int64_t expected = -32768;
        uint8_t bytes[] = {0x02, 0x02, 0x80, 0x00};

        if (!test_decode_integer(bytes, sizeof(bytes), expected, "smallest two-byte negative integer")) {
            passed = false;
        }
    }

    // negative number requiring extra sign byte (-32769)
    {
        int64_t expected = -32769;
        uint8_t bytes[] = {0x02, 0x03, 0xff, 0x7f, 0xff};

        if (!test_decode_integer(bytes, sizeof(bytes), expected, "negative number requiring extra sign byte")) {
            passed = false;
        }
    }

    // large positive number (65535)
    {
        int64_t expected = 65535;
        uint8_t bytes[] = {0x02, 0x03, 0x00, 0xff, 0xff};

        if (!test_decode_integer(bytes, sizeof(bytes), expected, "large positive number")) {
            passed = false;
        }
    }

    // very large positive number (8388607)
    {
        int64_t expected = 8388607;
        uint8_t bytes[] = {0x02, 0x03, 0x7f, 0xff, 0xff};

        if (!test_decode_integer(bytes, sizeof(bytes), expected, "very large positive number")) {
            passed = false;
        }
    }

    // negative zero
    {
        int64_t expected = -0;
        uint8_t bytes[] = {0x02, 0x01, 0x00};

        if (!test_decode_integer(bytes, sizeof(bytes), expected, "negative zero")) {
            passed = false;
        }
    }

    // negative number requiring a larger length (-8388608)
    {
        int64_t expected = -8388608;
        uint8_t bytes[] = {0x02, 0x03, 0x80, 0x00, 0x00};

        if (!test_decode_integer(bytes, sizeof(bytes), expected, "negative number requiring a larger length")) {
            passed = false;
        }
    }

    return passed;
}

bool test_decode_integer(uint8_t *bytes, int length, int64_t expected, const char *message) {
    bool ok = true;
    int ix = 1;

    field *f = unpack_integer(bytes, length, &ix);

    if (f == NULL) {
        ok = false;
    } else if (f->tag != FIELD_INTEGER) {
        ok = false;
    } else if (f->integer.value != expected) {
        ok = false;
    } else if (ix != length) {
        ok = false;
    }

    printf("%-5s  %s\n", ok ? "ok" : "error", message);
    if (!ok) {
        printf("       - expected: %lld\n", expected);
        if (f == NULL) {
            printf("       - got:      field:NULL\n");
        } else if (f->tag != FIELD_INTEGER) {
            printf("       - got:      tag:%d\n", f->tag);
            ok = false;
        } else if (f->integer.value != expected) {
            printf("       - got:      %lld\n", f->integer.value);
        } else if (ix != length) {
            printf("       - got:      ix:%d\n", ix);
        }
    }

    field_free(f);

    return ok;
}
