#include <stdlib.h>
#include <string.h>

#include <encoding/ASN.1/BER.h>
#include <encoding/ssmp/ssmp.h>

slice ssmp_encode_trap(packet p) {
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
            .octets = strdup(p.community),
        },
    };

    // field rqid = {
    //     .tag = FIELD_INTEGER,
    //     .integer = {
    //         .value = p.response.request_id,
    //     },
    // };

    // field error = {
    //     .tag = FIELD_INTEGER,
    //     .integer = {
    //         .value = p.response.error,
    //     },
    // };

    // field error_index = {
    //     .tag = FIELD_INTEGER,
    //     .integer = {
    //         .value = p.response.error_index,
    //     },
    // };

    // field oid = {
    //     .tag = FIELD_OID,
    //     .OID = {
    //         .OID = strdup(p.response.OID),
    //     },
    // };

    // field value = {
    //     .tag = FIELD_NULL,
    // };

    // switch (p.response.value.tag) {
    // case VALUE_BOOLEAN:
    //     value.tag = FIELD_BOOLEAN;
    //     value.boolean.value = p.response.value.boolean;
    //     break;

    // case VALUE_UINT8:
    //     value.tag = FIELD_INTEGER;
    //     value.integer.value = p.response.value.integer;
    //     break;

    // case VALUE_UINT16:
    //     value.tag = FIELD_INTEGER;
    //     value.integer.value = p.response.value.integer;
    //     break;

    // case VALUE_UINT32:
    //     value.tag = FIELD_INTEGER;
    //     value.integer.value = p.response.value.integer;
    //     break;

    // case VALUE_OCTET_STRING:
    //     value.tag = FIELD_OCTET_STRING;
    //     value.octets.length = p.response.value.octets.length;
    //     value.octets.octets = strdup(p.response.value.octets.bytes);
    //     break;
    // };

    // field item = {
    //     .tag = FIELD_SEQUENCE,
    //     .sequence = {
    //         .fields = vector_new(),
    //     },
    // };

    // item.sequence.fields = vector_add(item.sequence.fields, &oid);
    // item.sequence.fields = vector_add(item.sequence.fields, &value);

    // field list = {
    //     .tag = FIELD_SEQUENCE,
    //     .sequence = {
    //         .fields = vector_new(),
    //     },
    // };

    // list.sequence.fields = vector_add(list.sequence.fields, &item);

    // field pdu = {
    //     .tag = FIELD_PDU_RESPONSE,
    //     .sequence = {
    //         .fields = vector_new(),
    //     },
    // };

    // pdu.pdu.fields = vector_add(pdu.pdu.fields, &rqid);
    // pdu.pdu.fields = vector_add(pdu.pdu.fields, &error);
    // pdu.pdu.fields = vector_add(pdu.pdu.fields, &error_index);
    // pdu.pdu.fields = vector_add(pdu.pdu.fields, &list);

    field trap = {
        .tag = FIELD_SEQUENCE,
        .sequence = {
            .fields = vector_new(),
        },
    };

    trap.sequence.fields = vector_add(trap.sequence.fields, &version);
    trap.sequence.fields = vector_add(trap.sequence.fields, &community);
    // trap.sequence.fields = vector_add(response.sequence.fields, &pdu);

    slice encoded = BER_encode(trap);

    field_free(&trap);

    return encoded;
}
