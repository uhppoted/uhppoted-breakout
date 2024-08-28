#include <stdlib.h>

#include <encoding/SSMP/SSMP.h>

void packet_free(packet *p) {
    if (p != NULL && p->tag == PACKET_GET) {
        free(p->get.community);
        free(p->get.OID);
    }

    free(p);
}

packet *ssmp_get(int64_t version, char *community, int64_t request_id, int64_t error, int64_t error_index, char *OID) {
    packet *p = (packet *)malloc(sizeof(packet));

    p->tag = PACKET_GET;
    p->get.version = version;
    p->get.community = community;
    p->get.request_id = request_id;
    p->get.error = error;
    p->get.error_index = error_index;
    p->get.OID = NULL;

    return p;
}

// message BER_encodex(const struct field f) {
//     struct message m = {
//         .data = NULL,
//         .length = 0,
//     };
//
//     if (p.tag == PACKET_GET_RESPONSE) {
//         int N = 16;
//         uint8_t *buffer = (uint8_t *)calloc(N, sizeof(uint8_t));
//         int ix = 0;
//         int len;
//
//         switch (p.get_response.value.tag) {
//         default:
//             len = pack_null(&buffer[ix], N - ix);
//             ix += len;
//         }
//
//         m.data = reverse(buffer, ix);
//         m.length = ix;
//     }
//
//     return m;
// }
