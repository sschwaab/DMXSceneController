#ifndef TYPEDEF_H
#define TYPEDEF_H

#define DMX_PACKET_LEN 513

typedef struct{
  uint8_t start;
  uint8_t data[DMX_PACKET_LEN-1];
}tDmx_packet;

typedef enum {
  DMX_TRANSMIT_STOPPED,
  DMX_TRANSMIT_STOP_REQUESTED,
  DMX_TRANSMIT_TRANSMITTING
  
}tDmx_transmitter_status;

typedef struct{
  tDmx_packet buf;
  tDmx_transmitter_status transmitter_status;
}tDmx;

#endif
