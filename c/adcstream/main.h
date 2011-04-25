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
#include "../lib/tos_packet.h"
#include "../lib/tos_message.h"

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
#define SYNCHRONIZED        (1 << 0) /* 0x01 */
#define MESSAGE_RECEIVED    (1 << 1) /* 0x02 */

typedef struct
{
    FILE*       fp;
    uint16_t    total_packets;
    uint16_t    last_packet;
    uint8_t     id;
    uint8_t     flags;

} bsn_sensor_t;

// Function Prototypes
void process_sensor_data( uint8_t, uint8_t* );
void initialize_sensors ();
void terminate_sensors ();
void sigint_handler( int32_t sig );

#endif
