#include <stdio.h>
#include <string.h>

#include <auth/hotp/hotp.h>
#include <crypt/hash/sha256.h>

const uint8_t IPAD = 0x36;
const uint8_t OPAD = 0x5c;

void hmac(const uint8_t *key, size_t keylen, const uint8_t *in, size_t inlen, uint8_t hash[32]);

bool hotp_validate(const char *community, uint32_t code) {
    if (strcmp(community, "public") == 0) {
        return true;
    }

    return false;
}

uint32_t hotp_generate(const uint8_t *secret, int length, uint64_t counter) {
    int digits = 6;
    uint8_t buffer[8];
    int ix = 7;

    uint64_t v = counter;

    for (int i = 0; i < 8; i++) {
        buffer[ix--] = (uint8_t)(v & 0x00ff);
        v >>= 8;
    }

    printf(">> HOTP:counter %llu\n", counter);
    printf(">> HOTP:buffer  %02x %02x %02x %02x %02x %02x %02x %02x\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);

    uint8_t hash[32];
    hmac(secret, length, buffer, sizeof(buffer), hash);

    printf(">> HOTP:hash");
    for (int i = 0; i < 32; i++) {
        printf(" %02x", hash[i]);
    }
    printf("\n");

    return 0;
}

void hmac(const uint8_t *key, size_t keylen, const uint8_t *in, size_t inlen, uint8_t hash[32]) {
    // sha256 ctx;

    // sha256_init(&ctx);
    // sha256_update(&ctx, "abc", 3);
    // sha256_finalize(&ctx);

    struct sha256 inner;
    struct sha256 outer;
    //   char optkeybuf[32];
    char block[64];
    //   char innerhash[32];

    //   /* Reduce the key's size, so that it becomes <= 64 bytes large.  */
    //
    //   if (keylen > 64)
    //     {
    //       struct sha256_ctx keyhash;
    //
    //       sha256_init_ctx (&keyhash);
    //       sha256_process_bytes (key, keylen, &keyhash);
    //       sha256_finish_ctx (&keyhash, optkeybuf);
    //
    //       key = optkeybuf;
    //       keylen = 32;
    //     }

    // ... compute inner hash from 'key' and 'in'
    sha256_init(&inner);

    memset(block, IPAD, sizeof(block));
    for (size_t i = 0; i < keylen; i++) {
        block[i] ^= key[i];
    }

    sha256_update(&inner, block, 64);
    sha256_update(&inner, in, inlen);
    sha256_finalize(&inner);

    // ... compute result from 'key' and inner hash
    sha256_init(&outer);

    memset(block, OPAD, sizeof(block));
    for (size_t i = 0; i < keylen; i++) {
        block[i] ^= key[i];
    }

    sha256_update(&outer, block, 64);
    sha256_update(&outer, inner.hash.b, 32);
    sha256_finalize(&outer);

    // ... return hash
    memmove(hash, outer.hash.b, 32);
}

// func GenerateCodeCustom(secret string, counter uint64, opts ValidateOpts) (passcode string, err error) {
//  mac := hmac.New(opts.Algorithm.Hash, secretBytes)
//  binary.BigEndian.PutUint64(buf, counter)
//  if debug {
//      fmt.Printf("counter=%v\n", counter)
//      fmt.Printf("buf=%v\n", buf)
//  }
//
//  mac.Write(buf)
//  sum := mac.Sum(nil)
//
//  // "Dynamic truncation" in RFC 4226
//  // http://tools.ietf.org/html/rfc4226#section-5.4
//  offset := sum[len(sum)-1] & 0xf
//  value := int64(((int(sum[offset]) & 0x7f) << 24) |
//      ((int(sum[offset+1] & 0xff)) << 16) |
//      ((int(sum[offset+2] & 0xff)) << 8) |
//      (int(sum[offset+3]) & 0xff))
//
//  l := opts.Digits.Length()
//  mod := int32(value % int64(math.Pow10(l)))
//
//  if debug {
//      fmt.Printf("offset=%v\n", offset)
//      fmt.Printf("value=%v\n", value)
//      fmt.Printf("mod'ed=%v\n", mod)
//  }
//
//  return opts.Digits.Format(mod), nil
// }

// func ValidateCustom(passcode string, counter uint64, secret string, opts ValidateOpts) (bool, error) {
//  passcode = strings.TrimSpace(passcode)
//
//  if len(passcode) != opts.Digits.Length() {
//      return false, otp.ErrValidateInputInvalidLength
//  }
//
//  otpstr, err := GenerateCodeCustom(secret, counter, opts)
//  if err != nil {
//      return false, err
//  }
//
//  if subtle.ConstantTimeCompare([]byte(otpstr), []byte(passcode)) == 1 {
//      return true, nil
//  }
//
//  return false, nil
// }
