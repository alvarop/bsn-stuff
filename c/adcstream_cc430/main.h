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

// Maximum number of devices (Files will be created for each)
#define MAX_DEVICES 3

// Time between samples in seconds
#define SAMPLE_INTERVAL 0.00332641602f

// Number of ADC states (2^12 in this case)
//#define ADC_RESOLUTION 4096.0f
#define ADC_RESOLUTION 256.0f // testing 8-bit resolution

// ADC Reference Voltage
#define REFERENCE_VOLTAGE 2.50f

// File path
#define FILE_PATH "./"

// flag definitions
#define SYNCHRONIZED    (1 << 0) /* 0x01 */
#define MESSAGE_RECEIVED  (1 << 1) /* 0x02 */

//#define ADC_MAX_SAMPLES (50)

//#define BUFFER_SIZE (256)

typedef struct
{
  uint8_t length;
  uint8_t source;
  uint8_t type;
  uint8_t flags;
} packet_header_t;

typedef struct
{
  uint8_t samples[ADC_MAX_SAMPLES];
} packet_data_t;

typedef struct
{
  uint8_t rssi;
  uint8_t lqi_crcok;
} packet_footer_t;

// Function Prototypes
void process_packet( uint8_t* buffer );
void initialize_sensors ();
void terminate_sensors ();
void sigint_handler( int32_t sig );

#endif
