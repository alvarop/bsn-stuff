/** @file main.h
*
* @brief Main definitions and data types
*
* @author Alvaro Prieto
*/
#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
//#include <unistd.h>

#include "../lib/rs232.h"

// File path
#define FILE_PATH "./"

/**
 * Packet header structure. The packet length byte is omitted, since the receive
 * function strips it away. Also, the cc2500_tx_packet function inserts it 
 * automatically.
 */
typedef struct
{
  uint8_t length;
  uint8_t destination;
  uint8_t source;
  
  // The first four bits of the packet type are used as flags, while the last
  // four are the actual packet type
  uint8_t type;
} packet_header_t;

#define MAX_DEVICES (6)

#define BROADCAST_ADDRESS (0x00)

#define RADIO_BUFFER_SIZE (64)

#define TYPE_MASK (0x0F)
#define FLAG_MASK (0xF0)

#define PACKET_START (0x1)
#define PACKET_POLL (0x2)
#define PACKET_SYNC (0x3)

// The ACK flag is used to acknowledge a packet
#define FLAG_ACK ( 0x10 )

// The BURST flag is used to identify a multi-packet transmission
#define FLAG_BURST ( 0x20 )



// Function Prototypes
void process_packet( uint8_t* buffer );
void sigint_handler( int32_t sig );

#endif
