#include <stdlib.h>
#include <string.h>

#include <encoding/ASN.1/BER.h>
#include <encoding/ssmp/ssmp.h>

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
    if (fields != NULL) {
        // if (fields->size > 0 && fields->fields[0] != NULL && fields->fields[0]->tag == FIELD_SEQUENCE && fields->fields[0]->sequence.fields != NULL) {
        //     vector message = *fields->fields[0]->sequence.fields;
        //
        //     // ... SSMP GET request ?
        //     if (message.size > 2 && message.fields[2]->tag == FIELD_PDU_GET) {
        //         return ssmp_decode_get(fields);
        //     }
        // }
    }

    return NULL;
}
