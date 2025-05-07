#include <stddef.h>
#include <stdlib.h>

#include <types/packet.h>

void packet_free(packet *p) {
    if (p != NULL) {

        switch (p->tag) {
        case PACKET_GET:
            free(p->community);
            free(p->get.OID);
            break;

        case PACKET_SET:
            free(p->community);
            free(p->set.OID);
            if (p->set.value.tag == VALUE_OCTET_STRING) {
                slice_free(&p->set.value.octets);
            }
            break;

        case PACKET_RESPONSE:
            free(p->community);
            free(p->response.OID);
            if (p->response.value.tag == VALUE_OCTET_STRING) {
                slice_free(&p->response.value.octets);
            }
            break;

        case PACKET_TRAP:
            free(p->community);
            free(p->trap.OID);
            free(p->trap.timestamp);
            free(p->trap.var.OID);
            if (p->trap.var.value.tag == VALUE_OCTET_STRING) {
                slice_free(&p->trap.var.value.octets);
            }
            break;
        }

        if (p->dynamic) {
            free(p);
        }
    }
}
