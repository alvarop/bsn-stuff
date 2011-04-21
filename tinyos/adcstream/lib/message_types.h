#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

#define ADC_MAX_SAMPLES 50

typedef nx_struct sync_msg {
    nx_uint8_t flags;
} sync_msg_t;

typedef nx_struct test_adc_stream_numbered_msg {
    nx_uint8_t source;
    nx_uint8_t total_samples;  
    nx_uint16_t samples[ADC_MAX_SAMPLES];
    nx_uint16_t packet_number; 
} test_adc_stream_numbered_msg_t;

typedef nx_struct test_adc_stream_numbered_short_msg {
    nx_uint8_t source;
    nx_uint8_t total_samples;  
    nx_uint8_t samples[ADC_MAX_SAMPLES];
    nx_uint16_t packet_number;
} test_adc_stream_numbered_short_msg_t;

enum {
    AM_SYNC_MSG = 0x66,
    AM_ADC_STREAM_NUMBERED_MSG = 0xA0,
    AM_ADC_STREAM_NUMBERED_SHORT_MSG = 0xAA  
};

#endif
