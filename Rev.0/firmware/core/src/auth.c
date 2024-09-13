#include <stdio.h>
#include <string.h>

#include <auth.h>

const char *PUBLIC[] = {
    "0.1.3.6.1.4.1.65536.1.1",
};

bool authorised(const char *community, const char *oid) {
    if (strcmp(community, "public") == 0) {
        int N = sizeof(PUBLIC) / sizeof(const char *);

        for (int i = 0; i < N; i++) {
            if (strcmp(oid, PUBLIC[i]) == 0) {
                return true;
            }
        }
    }

    return false;
}