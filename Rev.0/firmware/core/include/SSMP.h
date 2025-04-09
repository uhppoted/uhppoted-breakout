#pragma once

struct circular_buffer;

extern void SSMP_init();
extern void SSMP_start();
extern void SSMP_reset();
extern void SSMP_rx(struct circular_buffer *);

extern const int64_t SSMP_ERROR_NONE;
extern const int64_t SSMP_ERROR_NO_SUCH_OBJECT;
extern const int64_t SSMP_ERROR_BAD_VALUE;
extern const int64_t SSMP_ERROR_READONLY;
extern const int64_t SSMP_ERROR_NO_ACCESS;
extern const int64_t SSMP_ERROR_WRONG_TYPE;
extern const int64_t SSMP_ERROR_COMMIT_FAILED;
extern const int64_t SSMP_ERROR_AUTHORIZATION;
extern const int64_t SSMP_ERROR_NOT_WRITABLE;
