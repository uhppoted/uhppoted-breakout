#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/ASN.1/BER.h>
#include <encoding/ssmp/ssmp.h>

packet *ssmp_decode_set(int64_t version, char *community, vector *pdu) {
    int64_t request_id = 0;
    int64_t error = 0;
    int64_t error_index = 0;
    char *OID = NULL;
    value value = {
        .tag = FIELD_NULL,
    };

    if (pdu != NULL) {
        if (pdu->size > 0 && pdu->fields[0]->tag == FIELD_INTEGER) {
            request_id = pdu->fields[0]->integer.value;
        }

        if (pdu->size > 1 && pdu->fields[1]->tag == FIELD_INTEGER) {
            error = pdu->fields[1]->integer.value;
        }

        if (pdu->size > 2 && pdu->fields[2]->tag == FIELD_INTEGER) {
            error_index = pdu->fields[2]->integer.value;
        }

        if (pdu->size > 3 && pdu->fields[3]->tag == FIELD_SEQUENCE && pdu->fields[3]->sequence.fields != NULL) {
            vector *content = pdu->fields[3]->sequence.fields;

            if (content->size > 0 && content->fields[0]->tag == FIELD_SEQUENCE && content->fields[0]->sequence.fields != NULL) {
                vector *rq = content->fields[0]->sequence.fields;

                if (rq->size > 0 && rq->fields[0]->tag == FIELD_OID) {
                    OID = strdup(rq->fields[0]->OID.OID);
                }

                if (rq->size > 1 && rq->fields[1]->tag == FIELD_NULL) {
                    value.tag = VALUE_NULL;
                } else if (rq->size > 1 && rq->fields[1]->tag == FIELD_INTEGER) {
                    value.tag = VALUE_UINT32;
                    value.integer = rq->fields[1]->integer.value;
                } else if (rq->size > 1 && rq->fields[1]->tag == FIELD_OCTET_STRING) {
                    int N = 64;
                    while (N < rq->fields[1]->octets.length && N < 2048) {
                        N += 64;
                    }

                    if (rq->fields[1]->octets.length <= N) {
                        value.tag = VALUE_OCTET_STRING;
                        value.octets.capacity = N;
                        value.octets.length = rq->fields[1]->octets.length;
                        value.octets.bytes = (char *)calloc(value.octets.capacity, sizeof(uint8_t));

                        memmove(value.octets.bytes, rq->fields[1]->octets.octets, value.octets.length);
                    }
                } else if (rq->size > 1 && rq->fields[1]->tag == FIELD_NULL) {
                    value.tag = VALUE_NULL;
                } else {
                    value.tag = VALUE_NULL;
                }
            }
        }
    }

    packet *p = (packet *)calloc(1, sizeof(packet));

    p->dynamic = true;
    p->tag = PACKET_SET;
    p->version = version;
    p->community = community;
    p->set.request_id = request_id;
    p->set.error = error;
    p->set.error_index = error_index;
    p->set.OID = OID;
    p->set.value = value;

    return p;
}
