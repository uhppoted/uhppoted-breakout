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

    field oid = {
        .tag = FIELD_OID,
        .OID = {
            .OID = strdup(p.trap.OID),
        },
    };

    field id = {
        .tag = FIELD_INTEGER,
        .integer = {
            .value = p.trap.id,
        },
    };

    field category = {
        .tag = FIELD_INTEGER,
        .integer = {
            .value = p.trap.category,
        },
    };

    field event = {
        .tag = FIELD_INTEGER,
        .integer = {
            .value = p.trap.event,
        },
    };

    field timestamp = {
        .tag = FIELD_OCTET_STRING,
        .octets = {
            .length = strlen(p.trap.timestamp),
            .octets = strdup(p.trap.timestamp),
        },
    };

    field item_oid = {
        .tag = FIELD_OID,
        .OID = {
            .OID = strdup(p.trap.var.OID),
        },
    };

    // FIXME encode all var types
    field item_value = {
        .tag = FIELD_UNKNOWN,
    };

    switch (p.trap.var.value.tag) {
    case VALUE_BOOLEAN:
        item_value.tag = FIELD_BOOLEAN;
        item_value.boolean.value = p.trap.var.value.boolean;
        break;

    case VALUE_OCTET_STRING:
        item_value.tag = FIELD_OCTET_STRING;
        item_value.octets.length = p.trap.var.value.octets.length;
        item_value.octets.octets = strdup(p.trap.var.value.octets.bytes);
        break;

    case VALUE_UINT8:
        item_value.tag = FIELD_INTEGER;
        item_value.integer.value = p.trap.var.value.integer;
        break;
    }

    field var = {
        .tag = FIELD_SEQUENCE,
        .sequence = {
            .fields = vector_new(),
        },
    };

    var.sequence.fields = vector_add(var.sequence.fields, &item_oid);
    var.sequence.fields = vector_add(var.sequence.fields, &item_value);

    field vars = {
        .tag = FIELD_SEQUENCE,
        .sequence = {
            .fields = vector_new(),
        },
    };

    vars.sequence.fields = vector_add(vars.sequence.fields, &var);

    field pdu = {
        .tag = FIELD_PDU_TRAP,
        .sequence = {
            .fields = vector_new(),
        },
    };

    pdu.pdu.fields = vector_add(pdu.pdu.fields, &oid);
    pdu.pdu.fields = vector_add(pdu.pdu.fields, &id);
    pdu.pdu.fields = vector_add(pdu.pdu.fields, &category);
    pdu.pdu.fields = vector_add(pdu.pdu.fields, &event);
    pdu.pdu.fields = vector_add(pdu.pdu.fields, &timestamp);
    pdu.pdu.fields = vector_add(pdu.pdu.fields, &vars);

    field trap = {
        .tag = FIELD_SEQUENCE,
        .sequence = {
            .fields = vector_new(),
        },
    };

    trap.sequence.fields = vector_add(trap.sequence.fields, &version);
    trap.sequence.fields = vector_add(trap.sequence.fields, &community);
    trap.sequence.fields = vector_add(trap.sequence.fields, &pdu);

    slice encoded = BER_encode(trap);

    field_free(&trap);

    return encoded;
}
