#include <stdlib.h>
#include <string.h>

#include <encoding/ASN.1/BER.h>
#include <encoding/ssmp/ssmp.h>
#include <trace.h>

extern slice ssmp_encode_get_response(packet);
extern packet *ssmp_decode_get(const vector *fields);

slice ssmp_encode(packet p) {
    if (p.tag == PACKET_GET_RESPONSE) {
        return ssmp_encode_get_response(p);
    }

    slice s = {
        .capacity = 0,
        .length = 0,
        .bytes = NULL,
    };

    return s;
}

struct packet *ssmp_decode(const vector *fields) {
    uint32_t trace = trace_in(TRACE_SSMP_DECODE);
    if (fields != NULL) {
        if (fields->size > 0) {

            if (fields->fields[0] != NULL) {
                if (fields->fields[0]->tag == FIELD_SEQUENCE) {
                    if (fields->fields[0]->sequence.fields != NULL) {
                        vector *message = fields->fields[0]->sequence.fields;

                        // ... version
                        if (message->size > 0 && message->fields[0] != NULL) {
                        }

                        // ... community
                        if (message->size > 1 && message->fields[1] != NULL) {
                        }

                        // ... PDU
                        if (message->size > 2 && message->fields[2] != NULL) {
                            // ... SSMP GET request ?
                            if (message->fields[2]->tag == FIELD_PDU_GET) {
                                packet *p = ssmp_decode_get(fields);
                                trace_out(TRACE_SSMP_DECODE, trace);
                                return p;
                            }
                        }
                    }
                }
            }
        }
    }

    trace_out(TRACE_SSMP_DECODE, trace);
    return NULL;
}
