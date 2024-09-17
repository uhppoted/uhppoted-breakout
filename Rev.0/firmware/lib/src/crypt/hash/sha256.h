#pragma once

#include <stdint.h>

#define HASH_LENGTH 32
#define BLOCK_LENGTH 64

typedef struct sha256 {
    union {
  uint8_t b[HASH_LENGTH];
  uint32_t w[HASH_LENGTH/4];        
    } hash;

union _buffer {
  uint8_t b[BLOCK_LENGTH];
  uint32_t w[BLOCK_LENGTH/4];
};

    uint32_t byteCount;
    uint8_t bufferOffset;

} sha256;

// class Sha256Class : public Print
// {
//   public:
//     void init(void);
//     void initHmac(const uint8_t* secret, int secretLength);
//     uint8_t* result(void);
//     uint8_t* resultHmac(void);
//     virtual void write(uint8_t);
//     using Print::write;
//   private:
//     void pad();
//     void addUncounted(uint8_t data);
//     void hashBlock();
//     uint32_t ror32(uint32_t number, uint8_t bits);
//     _buffer buffer;
//     uint8_t bufferOffset;
//     _state state;
//     uint32_t byteCount;
//     uint8_t keyBuffer[BLOCK_LENGTH];
//     uint8_t innerHash[HASH_LENGTH];
// };
// extern Sha256Class Sha256;

void sha256_init(sha256 *ctx);
void sha256_update(sha256 *ctx, uint8_t *block, uint32_t length);
