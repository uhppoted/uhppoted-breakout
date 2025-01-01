#include <stddef.h>
#include <stdlib.h>

#include <trace.h>
#include <types/packet.h>

void packet_free(packet *p) {
    uint32_t trace = trace_in(TRACE_PACKET_FREE);
    if (p != NULL) {
        free(p->community);

        switch (p->tag) {
        case PACKET_GET: {
            uint32_t trace1 = trace_in(TRACE_PACKET_FREE_OID);
            free(p->get.OID);
            trace_out(TRACE_PACKET_FREE_OID, trace1);
        } break;

        case PACKET_GET_RESPONSE: {
            uint32_t trace2 = trace_in(TRACE_PACKET_FREE_RESPONSE);
            free(p->get_response.OID);
            trace_out(TRACE_PACKET_FREE_RESPONSE, trace2);
        } break;
        }

        if (p->dynamic) {
            uint32_t trace3 = trace_in(TRACE_PACKET_FREE_PACKET);
            free(p);
            trace_out(TRACE_PACKET_FREE_PACKET, trace3);
        }
    }

    trace_out(TRACE_PACKET_FREE, trace);
}
