#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

struct Packet
{
  uint8_t id;
  char payload[31];
};

extern Packet packet;

#endif /* PACKET_H */