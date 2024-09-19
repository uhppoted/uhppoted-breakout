/*
 * Validation test suite for ASN.1 encoding.
 */

#include <stdbool.h>
#include <stdio.h>

#include <encoding/ASN.1/BER.h>

extern slice pack_integer(const field *f);

bool test_encode_integer(int64_t value, uint8_t *expected, int length, const char *message);

bool test_ASN1_encode_integer() {
    printf("-- ASN.1::encode integer\n");

    bool passed = true;

    // smallest positive number (0)
    {
        int64_t value = 0;
        uint8_t expected[] = {0x02, 0x01, 0x00};

        if (!test_encode_integer(value, expected, sizeof(expected), "smallest positive number")) {
            passed = false;
        }
    }

    // largest positive one-byte integer (127)
    {
        int64_t value = 127;
        uint8_t expected[] = {0x02, 0x01, 0x7f};

        if (!test_encode_integer(value, expected, sizeof(expected), "largest positive one-byte integer")) {
            passed = false;
        }
    }

    // smallest negative one-byte integer (-128)
    {
        int64_t value = -128;
        uint8_t expected[] = {0x02, 0x01, 0x80};

        if (!test_encode_integer(value, expected, sizeof(expected), "smallest negative one-byte integer")) {
            passed = false;
        }
    }

    // positive number needing a leading zero byte (128)
    {
        int64_t value = 128;
        uint8_t expected[] = {0x02, 0x02, 0x00, 0x80};

        if (!test_encode_integer(value, expected, sizeof(expected), "positive number needing a leading zero byte")) {
            passed = false;
        }
    }

    // negative number needing extra bytes (-129)
    {
        int64_t value = -129;
        uint8_t expected[] = {0x02, 0x02, 0xff, 0x7f};

        if (!test_encode_integer(value, expected, sizeof(expected), "negative number needing extra bytes")) {
            passed = false;
        }
    }

    // largest two-byte positive integer (32767)
    {
        int64_t value = 32767;
        uint8_t expected[] = {0x02, 0x02, 0x7f, 0xff};

        if (!test_encode_integer(value, expected, sizeof(expected), "largest two-byte positive integer")) {
            passed = false;
        }
    }

    // smallest two-byte negative integer (-32768)
    {
        int64_t value = -32768;
        uint8_t expected[] = {0x02, 0x02, 0x80, 0x00};

        if (!test_encode_integer(value, expected, sizeof(expected), "smallest two-byte negative integer")) {
            passed = false;
        }
    }

    // negative number requiring extra sign byte (-32769)
    {
        int64_t value = -32769;
        uint8_t expected[] = {0x02, 0x03, 0xff, 0x7f, 0xff};

        if (!test_encode_integer(value, expected, sizeof(expected), "negative number requiring extra sign byte")) {
            passed = false;
        }
    }

    // large positive number (65535)
    {
        int64_t value = 65535;
        uint8_t expected[] = {0x02, 0x03, 0x00, 0xff, 0xff};

        if (!test_encode_integer(value, expected, sizeof(expected), "large positive number")) {
            passed = false;
        }
    }

    // very large positive number (8388607)
    {
        int64_t value = 8388607;
        uint8_t expected[] = {0x02, 0x03, 0x7f, 0xff, 0xff};

        if (!test_encode_integer(value, expected, sizeof(expected), "very large positive number")) {
            passed = false;
        }
    }

    // negative zero
    {
        int64_t value = -0;
        uint8_t expected[] = {0x02, 0x01, 0x00};

        if (!test_encode_integer(value, expected, sizeof(expected), "negative zero")) {
            passed = false;
        }
    }

    // negative number requiring a larger length (-8388608)
    {
        int64_t value = -8388608;
        uint8_t expected[] = {0x02, 0x03, 0x80, 0x00, 0x00};

        if (!test_encode_integer(value, expected, sizeof(expected), "negative number requiring a larger length")) {
            passed = false;
        }
    }

    return passed;
}

bool test_encode_integer(int64_t value, uint8_t *expected, int length, const char *message) {
    field f = {
        .tag = FIELD_INTEGER,
        .integer = {
            .value = value,
        },
    };

    slice encoded = pack_integer(&f);
    bool ok = true;

    if (encoded.length != length) {
        ok = false;
    } else {
        for (int i = 0; i < length; i++) {
            if (encoded.bytes[i] != expected[i]) {
                ok = false;
                break;
            }
        }
    }

    printf("%-5s  %s\n", ok ? "ok" : "error", message);
    if (!ok) {
        char p[65];
        char q[65];
        int ix;

        ix = 0;
        for (int i = 0; i < length && ix < sizeof(p); i++) {
            ix += snprintf(&p[ix], sizeof(p) - ix, "%02x ", expected[i]);
        }

        ix = 0;
        for (int i = 0; i < encoded.length && ix < sizeof(q); i++) {
            ix += snprintf(&q[ix], sizeof(q) - ix, "%02x ", encoded.bytes[i]);
        }

        printf("       - expected: %s\n", p);
        printf("       - got:      %s\n", q);
    }

    slice_free(&encoded);

    return ok;
}
