#include <stdlib.h>
#include <string.h>

#include <encoding/BER/BER.h>
#include <encoding/SSMP/SSMP.h>

slice ssmp_encode_get_response(packet);

// NTS: expects 'p' to be free'd by caller
void packet_free(packet *const p) {
    free(p->community);

    if (p != NULL && p->tag == PACKET_GET) {
        free(p->get.OID);
    }
}

slice packet_encode(packet p) {
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

slice ssmp_encode(packet p) {
    field version = {
        .tag = FIELD_INTEGER,
        .integer = {
            .value = p.version,
        },
    };

    field community = {
        .tag = FIELD_OCTET_STRING,
        .octets = {
            .length = strlen(p.community),
            .octets = p.community,
        },
    };

    field response = {
        .tag = FIELD_SEQUENCE,
        .sequence = {
            .fields = vector_new(),
        },
    };

    response.sequence.fields = vector_add(response.sequence.fields, &version);
    response.sequence.fields = vector_add(response.sequence.fields, &community);

    return BER_encode(response);
}

packet *ssmp_get(int64_t version, char *community, int64_t request_id, int64_t error, int64_t error_index, char *OID) {
    packet *p = (packet *)malloc(sizeof(packet));

    p->tag = PACKET_GET;
    p->version = version;
    p->community = community;
    p->get.request_id = request_id;
    p->get.error = error;
    p->get.error_index = error_index;
    p->get.OID = NULL;

    return p;
}
