#pragma once

#include <types/packet.h>

slice ssmp_encode(packet);
packet *ssmp_decode(const vector *fields);
