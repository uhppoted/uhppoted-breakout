#include <stdlib.h>
#include <string.h>

#include <encoding/ASN.1/BER.h>
#include <encoding/ssmp/ssmp.h>

packet *ssmp_decode_get(const vector *fields) {
    // vector message = *fields->fields[0]->sequence.fields;
    //
    // // ... SSMP GET request ?
    int64_t version = 0;
    char *community = NULL;
    int64_t request_id = 0;
    int64_t error = 0;
    int64_t error_index = 0;
    char *OID = NULL;

    // if (message.fields[0]->tag == FIELD_INTEGER) {
    //     version = message.fields[0]->integer.value;
    // }
    //
    // if (message.fields[1]->tag == FIELD_OCTET_STRING) {
    //     community = strndup(message.fields[1]->octets.octets, message.fields[1]->octets.length);
    // }
    //
    // if (message.fields[2]->sequence.fields != NULL) {
    //     vector pdu = *message.fields[2]->sequence.fields;
    //
    //     if (pdu.size > 0 && pdu.fields[0]->tag == FIELD_INTEGER) {
    //         request_id = pdu.fields[0]->integer.value;
    //     }
    //
    //     if (pdu.size > 1 && pdu.fields[1]->tag == FIELD_INTEGER) {
    //         error = pdu.fields[1]->integer.value;
    //     }
    //
    //     if (pdu.size > 2 && pdu.fields[2]->tag == FIELD_INTEGER) {
    //         error_index = pdu.fields[2]->integer.value;
    //     }
    //
    //     if (pdu.size > 3 && pdu.fields[3]->tag == FIELD_SEQUENCE && pdu.fields[3]->sequence.fields != NULL) {
    //         vector content = *pdu.fields[3]->sequence.fields;
    //
    //         if (content.size > 0 && content.fields[0]->tag == FIELD_SEQUENCE && content.fields[0]->sequence.fields != NULL) {
    //             vector rq = *content.fields[0]->sequence.fields;
    //
    //             if (rq.size > 0 && rq.fields[0]->tag == FIELD_OID) {
    //                 OID = strdup(rq.fields[0]->OID.OID);
    //             }
    //
    //             if (rq.size > 1 && rq.fields[1]->tag == FIELD_NULL) {
    //             }
    //         }
    //     }
    // }

    packet *p = (packet *)calloc(1, sizeof(packet));

    p->dynamic = true;
    p->tag = PACKET_GET;
    p->version = version;
    p->community = community;
    p->get.request_id = request_id;
    p->get.error = error;
    p->get.error_index = error_index;
    p->get.OID = OID;

    return p;
}
