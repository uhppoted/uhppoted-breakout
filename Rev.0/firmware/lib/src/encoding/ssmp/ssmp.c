#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/ASN.1/BER.h>
#include <encoding/ssmp/ssmp.h>

extern packet *ssmp_decode_get(int64_t version, char *community, vector *pdu);
extern packet *ssmp_decode_set(int64_t version, char *community, vector *pdu);
extern slice ssmp_encode_response(packet);
extern slice ssmp_encode_trap(packet);

slice ssmp_encode(packet p) {
    if (p.tag == PACKET_RESPONSE) {
        return ssmp_encode_response(p);
    }

    if (p.tag == PACKET_TRAP) {
        return ssmp_encode_trap(p);
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
        if (fields->size > 0) {

            if (fields->fields[0] != NULL) {
                if (fields->fields[0]->tag == FIELD_SEQUENCE) {
                    if (fields->fields[0]->sequence.fields != NULL) {
                        vector *message = fields->fields[0]->sequence.fields;

                        // ... SSMP GET request ?
                        if (message->size > 2 && message->fields[2] != NULL && message->fields[2]->tag == FIELD_PDU_GET) {
                            int64_t version = 0;
                            char *community = NULL;
                            vector *pdu = NULL;

                            // ... version
                            if (message->fields[0] != NULL && message->fields[0]->tag == FIELD_INTEGER) {
                                version = message->fields[0]->integer.value;
                            }

                            // ... community
                            if (message->fields[1] != NULL && message->fields[1]->tag == FIELD_OCTET_STRING) {
                                community = strndup(message->fields[1]->octets.octets, message->fields[1]->octets.length);
                            } else {
                                community = strndup("", 0);
                            }

                            if (message->fields[2]->pdu.fields != NULL) {
                                pdu = message->fields[2]->pdu.fields;
                            }

                            return ssmp_decode_get(version, community, pdu);
                        }

                        // ... SSMP SET request ?
                        if (message->size > 2 && message->fields[2] != NULL && message->fields[2]->tag == FIELD_PDU_SET) {
                            int64_t version = 0;
                            char *community = NULL;
                            vector *pdu = NULL;

                            // ... version
                            if (message->fields[0] != NULL && message->fields[0]->tag == FIELD_INTEGER) {
                                version = message->fields[0]->integer.value;
                            }

                            // ... community
                            if (message->fields[1] != NULL && message->fields[1]->tag == FIELD_OCTET_STRING) {
                                community = strndup(message->fields[1]->octets.octets, message->fields[1]->octets.length);
                            } else {
                                community = strndup("", 0);
                            }

                            if (message->fields[2]->pdu.fields != NULL) {
                                pdu = message->fields[2]->pdu.fields;
                            }

                            return ssmp_decode_set(version, community, pdu);
                        }
                    }
                }
            }
        }
    }

    return NULL;
}
