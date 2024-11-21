#include <log.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/ASN.1/BER.h>

void field_free(field *f) {
    if (f != NULL) {
        switch (f->tag) {
        case FIELD_OCTET_STRING:
            free(f->octets.octets);
            break;

        case FIELD_OID:
            free(f->OID.OID);
            break;

        case FIELD_SEQUENCE:
            vector_free(f->sequence.fields);
            free(f->sequence.fields);
            break;

        case FIELD_PDU_GET:
            vector_free(f->pdu.fields);
            free(f->pdu.fields);
            break;
        }

        if (f->dynamic) {
            free(f);
        }
    }
}
