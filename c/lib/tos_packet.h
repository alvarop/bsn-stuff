/** @file tos_packet.h
*
* @brief TinyOS serial packet handling functions
*
* @author Alvaro Prieto
*/
#ifndef _TOS_PACKET_H
#define _TOS_PACKET_H

#include <stdio.h>
#include <stdint.h>

#define BUFFER_SIZE 512
#define SYNC_BYTE 0x7E
#define ESCAPE_BYTE 0x7D

#define ADC_CHANNELS 8
#define ADC_MAX_SAMPLES 50

// NOTE: does not include start char
typedef struct {
  uint8_t packet_type;
  uint8_t unknown; // could be sequence number?
  uint16_t destination;
  uint16_t source;
  uint8_t length;
  uint8_t group;
  uint8_t am_type;
} __attribute__((__packed__)) tos_packet_header_t;

uint16_t compute_crc( uint8_t*, uint8_t );
uint8_t check_crc( uint8_t*, uint8_t );
uint16_t find_and_escape_packet( uint8_t*, uint8_t* );

inline uint16_t endian_swap16(uint16_t );
inline uint32_t endian_swap32(uint32_t );

#endif /* _TOS_PACKET_H */\

