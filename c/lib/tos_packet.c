/** @file tos_packet.c
*
* @brief TinyOS serial packet handling functions
*
* @author Alvaro Prieto
*/
#include "tos_packet.h"

static uint16_t calc_crc_byte( uint16_t, uint16_t );

/*!
  @brief helper function used to calculate CRC byte
*/
static uint16_t calc_crc_byte( uint16_t crc, uint16_t b )
{
  uint16_t i;
  crc ^= (uint16_t)b << 8;
  
  for( i=0; i < 8; i++ )
  {
    if( ( crc & 0x8000) == 0x8000 )
    {
      crc = (crc << 1) ^ 0x1021;
    }
    else
    {
      crc = crc << 1;
    }
  }
  
  return crc & 0xffff;
}   

/*!
  @brief Calculate the CRC of a packet
*/
uint16_t compute_crc( uint8_t* packet, uint8_t count )
{
  uint16_t crc = 0;
  uint16_t index = 1;
  uint8_t local_count;
  local_count = count - 4; // Don't count the CRC and end byte
  
  while( local_count > 0 )
  {   
    crc = calc_crc_byte( crc, packet[index++]);
    local_count--;
  }
  
  return crc;   
}

/*!
  @brief Check if a packet is valid by checking CRC
*/
uint8_t check_crc( uint8_t* packet, uint8_t count )
{
  // Return 1 if crc matches, 0 otherwise
  // CRC starts 3 bytes from the end
  return (compute_crc( packet, count ) == 
          (uint16_t)*((uint16_t*)(packet+count-3)) );
}

/*!
  @brief Find a packet in the buffer
*/
uint16_t find_and_escape_packet( uint8_t* old_buffer, uint8_t* new_buffer )
{
  // replace escaped characters
  uint16_t buffer_index = 0;
  uint16_t packet_size = 0;
  uint8_t start=0;

  // find first SYNC_BYTE
  while( (!start) & (buffer_index < BUFFER_SIZE) )
  {
    if( old_buffer[buffer_index] == SYNC_BYTE )
    {
      start = 1;
      new_buffer[packet_size++] = old_buffer[buffer_index];
    }
    buffer_index++;
  }

  
  // process until next SYNC_BYTE is found
  while ( (start) & (buffer_index < BUFFER_SIZE) )
  {
    // Handle escape characters
    if( old_buffer[buffer_index] == ESCAPE_BYTE )
    {
      buffer_index++;
      old_buffer[buffer_index] ^= 0x20;
    }
    // Handle end character
    else if( old_buffer[buffer_index] == SYNC_BYTE )
    {
      start = 0;
    }
    new_buffer[packet_size++] = old_buffer[buffer_index];
    buffer_index++;
  }

  return packet_size;  
}

/*!
  @brief swap bytes to correct for endianness
*/
inline uint16_t endian_swap16(uint16_t x)
{
  return (x>>8) | 
    (x<<8);
}

/*!
  @brief swap bytes to correct for endianness
*/
inline uint32_t endian_swap32(uint32_t x)
{
  return (x>>24) | 
    ((x<<8) & 0x00FF0000) |
    ((x>>8) & 0x0000FF00) |
    (x<<24);
}
