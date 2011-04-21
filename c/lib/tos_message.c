/** @file tos_message.c
*
* @brief TinyOS message sending/receiving functions
*
* @author Alvaro Prieto
*/
#include <string.h>
#include "tos_message.h"

static tos_packet_header_t in_packet;
static tos_packet_header_t out_packet;
static uint8_t p_in_buffer[BUFFER_SIZE];
static uint8_t p_tmp_buffer[BUFFER_SIZE];
static uint8_t p_data_buffer[BUFFER_SIZE];

/*!
    @brief Checks if packet is in buffer, loads it to p_in_buffer, then returns the packet type
*/
uint8_t load_packet( uint8_t* buffer )
{
    uint8_t packet_size;
     

    packet_size = find_and_escape_packet( buffer, p_in_buffer );

#ifdef PACKET_DEBUG
     {
        uint8_t tmp_count;
        printf("\nESC: ");   
        for( tmp_count = 0; tmp_count< packet_size; tmp_count++)
        {
            printf("%02x ", p_in_buffer[tmp_count]);
        }
        printf("\n");
    }
#endif
    
    if ( (packet_size > 0) && check_crc( p_in_buffer, packet_size) )
    {
 
        // Move packet to serial_buffer to align memory and get rid of start char
        memmove( p_in_buffer, ( p_in_buffer + 1 ), (packet_size - 2) );
        memcpy( (void *)(&in_packet), (void*)p_in_buffer, sizeof( in_packet ) );
  
        // move data to start of serial_buffer
        memmove( p_in_buffer, (p_in_buffer + sizeof(tos_packet_header_t)), in_packet.length );
        
        return in_packet.am_type;
    }
    else
    {
        printf("Bad packet - ");
        printf("CRC Match = %d\n", check_crc( p_in_buffer, packet_size));
        
        return 0;
    }
}

/*!
    @brief Set-up outgoing packet's address and am_type
*/
uint8_t serial_msg_build( uint16_t destination, uint8_t* packet_buffer )
{
    uint8_t total_size = 0;
    uint8_t buffer_index = 0;
    uint16_t tmpcrc;
    
    out_packet.source = 0xcc;
    out_packet.destination = destination; // broadcast
    out_packet.packet_type = 0xbb; // ??
    out_packet.group = 0xdd;
    out_packet.length = sizeof( test_serial_msg_t );
    out_packet.am_type = AM_SERIAL_MSG;
    
    // Add start character
    p_tmp_buffer[0] = SYNC_BYTE; 
    
    // Copy the packet header
    memcpy( (void*)( p_tmp_buffer + 1), (void*)( &out_packet ), sizeof( tos_packet_header_t ) );
    
    // Copy data
    memcpy( (void*)( p_tmp_buffer + 1 + sizeof( tos_packet_header_t ) ), (void*)( &p_data_buffer), sizeof( test_serial_msg_t ) );
    
    // calculate CRC
    tmpcrc = compute_crc( p_tmp_buffer, sizeof( tos_packet_header_t ) + sizeof( test_serial_msg_t ) + 4 );
    
    // Add CRC
    memcpy( (void*)( p_tmp_buffer + 1 + sizeof( tos_packet_header_t ) + sizeof( test_serial_msg_t ) ), (void*)(&tmpcrc), sizeof(uint16_t) );
    
    // add end byte
    memset( (void*)( p_tmp_buffer + 1 + sizeof( tos_packet_header_t ) + sizeof( test_serial_msg_t ) + sizeof(uint16_t) ), SYNC_BYTE, 1 ); 
    //total_size = sizeof( tos_packet_header_t ) + sizeof( test_serial_msg_t ) + 2 + 2; // 2 CRC bytes + header and footer
    
    // escape characters and copy to output buffer
    
    packet_buffer[total_size++]=p_tmp_buffer[buffer_index++];
    
    while( buffer_index < ( 1 +sizeof( tos_packet_header_t ) + sizeof( test_serial_msg_t ) + sizeof(uint16_t) + 1 - 1) )
    {
        if( ( SYNC_BYTE == p_tmp_buffer[buffer_index] ) || ( ESCAPE_BYTE == p_tmp_buffer[buffer_index] ) )
        {
            packet_buffer[total_size++] = ESCAPE_BYTE;
            packet_buffer[total_size++] = p_tmp_buffer[buffer_index++] ^ 0x20;
        }
        else
        {
            packet_buffer[total_size++] = p_tmp_buffer[buffer_index++];
        }
    }
    
    packet_buffer[total_size++] = p_tmp_buffer[buffer_index++];
    
    return total_size;
}

uint16_t serial_msg_get_counter1()
{
    // Make sure the message in buffer is of correct type, otherwise return 0
    if( AM_SERIAL_MSG == in_packet.am_type )
    {
        test_serial_msg_t* p_packet_data = (test_serial_msg_t*)p_in_buffer;
        return endian_swap16( p_packet_data->counter1 );
    }
    return 0;
}


uint32_t serial_msg_get_counter2()
{
    // Make sure the message in buffer is of correct type, otherwise return 0
    if( AM_SERIAL_MSG == in_packet.am_type )
    {
        test_serial_msg_t* p_packet_data = (test_serial_msg_t*)p_in_buffer;
        return endian_swap32( p_packet_data->counter2 );
    }
    return 0;
}

void serial_msg_set_counter1(uint16_t counter1)
{
    test_serial_msg_t* data_pointer;
    
    data_pointer = (test_serial_msg_t*)p_data_buffer;
    
    data_pointer->counter1 = counter1;

    return;
}

void serial_msg_set_counter2(uint32_t counter2 )
{
    test_serial_msg_t* data_pointer;
    
    data_pointer = (test_serial_msg_t*)p_data_buffer;
    
    data_pointer->counter2 = counter2;
    
    return;
}

uint16_t serial_adc_msg_get_channel( uint8_t channel )
{
    // Make sure the message in buffer is of correct type, otherwise return 0
    if( (AM_ADC_SERIAL_MSG == in_packet.am_type) && ( channel < ADC_CHANNELS )  )
    {
        test_adc_serial_msg_t* p_packet_data = (test_adc_serial_msg_t*)p_in_buffer;
        
        return endian_swap16( p_packet_data->channels[channel] );
    }
    return 0;
}

uint8_t adc_stream_msg_get_source()
{
    test_adc_stream_msg_t* p_packet_data = (test_adc_stream_msg_t*)p_in_buffer;
    
    return p_packet_data->source;
    
}

uint8_t adc_stream_msg_get_total_samples()
{
    test_adc_stream_msg_t* p_packet_data = (test_adc_stream_msg_t*)p_in_buffer;
    
    return p_packet_data->total_samples;
    
}

uint16_t adc_stream_msg_get_packet_number()
{
    test_adc_stream_numbered_msg_t* p_packet_data = (test_adc_stream_numbered_msg_t*)p_in_buffer;
    return endian_swap16( p_packet_data->packet_number );
}


uint8_t adc_stream_short_msg_get_total_samples()
{
   test_adc_stream_numbered_short_msg_t* p_packet_data = (test_adc_stream_numbered_short_msg_t*)p_in_buffer;
    
    return p_packet_data->total_samples;
    
}

uint16_t adc_stream_short_msg_get_packet_number()
{
    test_adc_stream_numbered_short_msg_t* p_packet_data = (test_adc_stream_numbered_short_msg_t*)p_in_buffer;
    return endian_swap16( p_packet_data->packet_number );
}

uint8_t adc_stream_short_msg_get_source()
{
    test_adc_stream_numbered_short_msg_t* p_packet_data = (test_adc_stream_numbered_short_msg_t*)p_in_buffer;
    
    return p_packet_data->source;
    
}

void adc_stream_short_msg_get_samples( uint8_t* buffer )
{
    uint8_t sample_counter;
    test_adc_stream_numbered_short_msg_t* p_packet_data = (test_adc_stream_numbered_short_msg_t*)p_in_buffer;
    
    for( sample_counter = 0; sample_counter < ADC_MAX_SAMPLES; sample_counter++ )
    {
        buffer[sample_counter] = ( p_packet_data->samples[sample_counter] );
    }
    return;
}

void adc_stream_msg_get_samples( uint16_t* buffer )
{
    uint8_t sample_counter;
    test_adc_stream_msg_t* p_packet_data = (test_adc_stream_msg_t*)p_in_buffer;
    
    for( sample_counter = 0; sample_counter < ADC_MAX_SAMPLES; sample_counter++ )
    {
        buffer[sample_counter] = ( p_packet_data->samples[sample_counter] );
    }
    return;
}


void sync_debug_msg_get_all( uint16_t* local_timer, uint16_t* remote_timer, int16_t* remote_offset, uint8_t* remote_id )
{
    sync_debug_msg_t* p_packet_data = (sync_debug_msg_t*)p_in_buffer;
    *local_timer = endian_swap16( p_packet_data->local_timer );
    *remote_timer = endian_swap16( p_packet_data->remote_timer );
    *remote_offset = endian_swap16( p_packet_data->remote_offset );
    *remote_id = p_packet_data->remote_id;
    return;
}

