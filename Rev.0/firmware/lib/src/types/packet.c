#include <stddef.h>
#include <stdlib.h>

#include <types/packet.h>

void packet_free(packet *p) {
    if (p != NULL) {
        free(p->community);

        switch (p->tag) {
        case PACKET_GET:
            free(p->get.OID);
            break;

        case PACKET_SET:
            free(p->get.OID);
            break;

        case PACKET_RESPONSE:
            free(p->response.OID);
            break;
        }

        if (p->dynamic) {
            free(p);
        }
    }
}
