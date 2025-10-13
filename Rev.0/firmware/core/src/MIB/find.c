#include <string.h>

#include <crypt/hash/djb2.h>

#include <MIB.h>

bool MIB_has(const char *OID) {
    uint32_t hash = djb2(OID);

    int N = sizeof(OIDs) / sizeof(MIBItem);
    for (int i = 0; i < N; i++) {
        MIBItem item = OIDs[i];

        if ((hash == item.hash) && (strcmp(OID, item.OID) == 0) && (item.get != NULL || item.set != NULL)) {
            return true;
        }
    }

    return false;
}
