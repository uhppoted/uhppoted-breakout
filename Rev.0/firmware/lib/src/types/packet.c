#include <stddef.h>
#include <stdlib.h>

#include <types/packet.h>

void free_packet(packet *p) {
    if (p == NULL) {
        return;
    }

    free(p->community);

    if (p->tag == PACKET_GET) {
        free(p->get.OID);
    }

    if (p->tag == PACKET_GET_RESPONSE) {
        free(p->get_response.OID);
    }

    if (p->dynamic) {
        free(p);
    }
}
