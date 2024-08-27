#include <stdlib.h>

#include <encoding/SSMP/SSMP.h>

void packet_free(packet *p) {
    if (p != NULL && p->tag == PACKET_GET) {
        free(p->get.community);
        free(p->get.OID);
    }

    free(p);
}

packet *ssmp_get(int64_t version, char *community, int64_t request_id, int64_t error, int64_t error_index, char *OID) {
    packet *p = (packet *)malloc(sizeof(packet));

    p->tag = PACKET_GET;
    p->get.version = version;
    p->get.community = community;
    p->get.request_id = request_id;
    p->get.error = error;
    p->get.error_index = error_index;
    p->get.OID = NULL;

    return p;
}
