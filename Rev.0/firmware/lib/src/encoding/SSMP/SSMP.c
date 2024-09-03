#include <stdlib.h>
#include <string.h>

#include <encoding/ASN.1/BER.h>
#include <encoding/SSMP/SSMP.h>

slice packet_encode_get_response(packet);

// NTS: expects 'p' to be free'd by caller
void packet_free(packet *const p) {
    free(p->community);

    if (p != NULL && p->tag == PACKET_GET) {
        free(p->get.OID);
    }
}

slice ssmp_encode(packet p) {
    if (p.tag == PACKET_GET_RESPONSE) {
        return packet_encode_get_response(p);
    }

    slice s = {
        .capacity = 0,
        .length = 0,
        .bytes = NULL,
    };

    return s;
}

slice packet_encode_get_response(packet p) {
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

    field rqid = {
        .tag = FIELD_INTEGER,
        .integer = {
            .value = p.get_response.request_id,
        },
    };

    field error = {
        .tag = FIELD_INTEGER,
        .integer = {
            .value = p.get_response.error,
        },
    };

    field error_index = {
        .tag = FIELD_INTEGER,
        .integer = {
            .value = p.get_response.error_index,
        },
    };

    field oid = {
        .tag = FIELD_OID,
        .OID = {
            .OID = p.get_response.OID,
        },
    };

    field value = {
        .tag = FIELD_INTEGER,
        .integer = {
            .value = p.get_response.value.integer,
        },
    };

    field item = {
        .tag = FIELD_SEQUENCE,
        .sequence = {
            .fields = vector_new(),
        },
    };

    item.sequence.fields = vector_add(item.sequence.fields, &oid);
    item.sequence.fields = vector_add(item.sequence.fields, &value);

    field list = {
        .tag = FIELD_SEQUENCE,
        .sequence = {
            .fields = vector_new(),
        },
    };

    list.sequence.fields = vector_add(list.sequence.fields, &item);

    field pdu = {
        .tag = FIELD_PDU_GET_RESPONSE,
        .sequence = {
            .fields = vector_new(),
        },
    };

    pdu.pdu.fields = vector_add(pdu.pdu.fields, &rqid);
    pdu.pdu.fields = vector_add(pdu.pdu.fields, &error);
    pdu.pdu.fields = vector_add(pdu.pdu.fields, &error_index);
    pdu.pdu.fields = vector_add(pdu.pdu.fields, &list);

    field response = {
        .tag = FIELD_SEQUENCE,
        .sequence = {
            .fields = vector_new(),
        },
    };

    response.sequence.fields = vector_add(response.sequence.fields, &version);
    response.sequence.fields = vector_add(response.sequence.fields, &community);
    response.sequence.fields = vector_add(response.sequence.fields, &pdu);

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
