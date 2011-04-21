/** @file tos_message.h
*
* @brief TinyOS message sending/receiving functions
*
* @author Alvaro Prieto
*/
#ifndef _TOS_MESSAGE_H
#define _TOS_MESSAGE_H

#include "tos_packet.h"

// AM type codes
enum {
    AM_SERIAL_MSG = 0xA1,
    AM_ADC_STREAM_NUMBERED_MSG = 0xA0,
    AM_ADC_STREAM_NUMBERED_SHORT_MSG = 0xAA,
    AM_ADC_SERIAL_MSG = 0xAD,
    AM_ADC_STREAM_MSG = 0xAF,
    AM_SYNC_MSG = 0x66,
    AM_SYNC_OFFSET_MSG = 0x60,
    AM_SYNC_DEBUG_MSG = 0x60
};

// List of message types
typedef struct {
  uint32_t counter2;
  uint16_t counter1;
} __attribute__((__packed__)) test_serial_msg_t ;


typedef struct {
    uint16_t channels[ADC_CHANNELS];
} __attribute__((__packed__)) test_adc_serial_msg_t;



typedef struct {
    uint8_t source;
    uint8_t total_samples;    
    uint16_t samples[ADC_MAX_SAMPLES];
} __attribute__((__packed__)) test_adc_stream_msg_t;

typedef struct test_adc_stream_numbered_short_msg {
    uint8_t source;
    uint8_t total_samples;  
    uint8_t samples[ADC_MAX_SAMPLES];
    uint16_t packet_number;
} __attribute__((__packed__)) test_adc_stream_numbered_short_msg_t;

typedef struct {
    uint8_t source;
    uint8_t total_samples;   
    uint16_t samples[ADC_MAX_SAMPLES];
    uint16_t packet_number; 
} __attribute__((__packed__)) test_adc_stream_numbered_msg_t;

typedef struct sync_debug_msg {
    uint16_t local_timer;
    uint16_t remote_timer;
    int16_t remote_offset;
    uint8_t remote_id;
} __attribute__((__packed__)) sync_debug_msg_t;



uint8_t load_packet( uint8_t* );

uint16_t serial_msg_get_counter1( );
uint32_t serial_msg_get_counter2( );
uint8_t serial_msg_build( uint16_t, uint8_t* );

void serial_msg_set_counter1( uint16_t );
void serial_msg_set_counter2( uint32_t );

uint16_t serial_adc_msg_get_channel( uint8_t );

uint8_t adc_stream_msg_get_source();
uint8_t adc_stream_msg_get_total_samples();
void adc_stream_msg_get_samples( uint16_t* );

uint16_t adc_stream_msg_get_packet_number();

uint8_t adc_stream_short_msg_get_total_samples();
uint16_t adc_stream_short_msg_get_packet_number();
uint8_t adc_stream_short_msg_get_source();
void adc_stream_short_msg_get_samples( uint8_t* );

void sync_debug_msg_get_all( uint16_t*, uint16_t*, int16_t*, uint8_t* );

#endif
