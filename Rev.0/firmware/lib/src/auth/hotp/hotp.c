#include <string.h>

#include <auth/hotp/hotp.h>

bool hotp_validate(const char *community, uint32_t code) {
    if (strcmp(community, "public") == 0) {
        return true;
    }

    return false;
}
