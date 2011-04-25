/** @file main.c
*
* @brief TinyOS serial packet handling functions
*
* @author Alvaro Prieto
*/
//#define DEBUG_MESSAGES

#include "main.h"

static bsn_sensor_t sensors[ MAX_DEVICES ];
static int32_t serial_port_number;
static FILE* main_fp;
static uint32_t time_counter;
static uint8_t main_buffer[MAX_DEVICES][ADC_MAX_SAMPLES];

int main( int argc, char *argv[] )
{   
    
    uint8_t serial_buffer[BUFFER_SIZE];    
    uint8_t adc_sample_buffer[ADC_MAX_SAMPLES];        
    
    // Handle interrupt events to make sure files are closed before exiting
    (void) signal( SIGINT, sigint_handler );

    // Make sure input is correct
    if( argc < 2 )
    {
        printf("Usage: %s port baudrate\n", argv[0]);
        return 0;
    }
    
    initialize_sensors();        
    
    // Convert string serial number to integer
    serial_port_number = atoi( argv[1] );
    
    // Use RS232 library to open serial port    
    if ( OpenComport( serial_port_number, atoi(argv[2]) ) )
    {
        printf("Error opening serial port.\n");
        return 1;
    }
    
    // Run forever
    for(;;)
    {  
        // Read a buffer full of data (if available)
        if( PollComport( serial_port_number, serial_buffer, BUFFER_SIZE ) > 0 )
        {
            // After reading data, process it and see if there is a TOS packet
            if ( load_packet( serial_buffer ) == AM_ADC_STREAM_NUMBERED_SHORT_MSG )
            {                   
                // Copy samples to buffer
                adc_stream_short_msg_get_samples( adc_sample_buffer );
                
                // Process samples
                process_sensor_data( adc_stream_short_msg_get_source(),
                                adc_sample_buffer );
            }
            
        }
        
        // Don't take up all the processor time      
        usleep(10);
    }
    
    terminate_sensors();    

    return 0;
}

/*!
    @brief process and save sensor data to file
*/
void process_sensor_data( uint8_t device_id, uint8_t* buffer )
{
    uint8_t sample_counter;
    uint8_t device;
    static uint8_t lowest_device= 0xff;
    
    // Make sure device is allocated
    if ( device_id > MAX_DEVICES )
    {
        printf("Error: No space allocated for node %d.\nIncrease MAX_DEVICES to solve this problem.\n", device_id);
        
        return;
    }
    
    // Since the access point has id 0, decrease by 1 to occupy all locations in array
    device_id--;
    
    if( device_id < lowest_device )
    {
        lowest_device = device_id;
    }
    
    // Check if this packet is from new minor cycle
    if ( (device_id == lowest_device) || ( sensors[device_id].flags & MESSAGE_RECEIVED ) )
    {
        // Lowest device missed a packet, maybe disconnected, change to one higher
        if ( device_id != lowest_device )
        {
            printf("Missed packet from device #%d, updating cycle.\n", lowest_device );
            lowest_device++;
        }
        
        // Clear other device flags
        for ( device = 0; device < MAX_DEVICES; device++ )
        {
            // Only clear if the device has not received messages in this cycle
            if( !( sensors[device].flags & MESSAGE_RECEIVED ) )
            {
                // Set samples in main buffer to zero
                memset( main_buffer[device], 0x00, ADC_MAX_SAMPLES * sizeof( uint8_t ) );
                
                // Output zeroes to file
                for( sample_counter = 0; sample_counter < ADC_MAX_SAMPLES; sample_counter++ )
                {   
                    fprintf( sensors[device].fp, "%1.4f,%1.4f\n", 
                        ( ( time_counter * ADC_MAX_SAMPLES ) + sample_counter ) 
                        * SAMPLE_INTERVAL, 0.0f );                   
                }                
            }
            
            // Clear message received flags
            sensors[device].flags &= ~MESSAGE_RECEIVED;
        }                
        
        // Print to main buffer
        for( sample_counter = 0; sample_counter < ADC_MAX_SAMPLES; sample_counter++ )
        {
            // Print time
            fprintf( main_fp, "%1.4f", 
                        ( ( time_counter * ADC_MAX_SAMPLES ) + sample_counter ) 
                        * SAMPLE_INTERVAL); 
            for ( device = 0; device < MAX_DEVICES; device++ )
            {
                // Print voltage for each device
                fprintf( main_fp, ",%1.4f", 
                ( ( main_buffer[device][sample_counter] ) / 
                        ADC_RESOLUTION ) * REFERENCE_VOLTAGE );
            }
            fprintf( main_fp, "\n");
        }
        
        // Since new minor cycle has started, increase global time
        time_counter++;
        
         
#ifdef DEBUG_MESSAGES
    printf("\nNew Minor Cycle.\n");
#endif
    }
    
    // Make sure message received flag is set
    sensors[device_id].flags |= MESSAGE_RECEIVED;
      
    // Write data to file
    for( sample_counter = 0; sample_counter < ADC_MAX_SAMPLES; sample_counter++ )
    {    
        fprintf( sensors[device_id].fp, "%1.4f,%1.4f\n", 
                        ( ( time_counter * ADC_MAX_SAMPLES ) + sample_counter ) 
                        * SAMPLE_INTERVAL, ( ( buffer[sample_counter] ) / 
                        ADC_RESOLUTION ) * REFERENCE_VOLTAGE );
    }                        

    // Copy to main buffer
    memcpy( main_buffer[device_id], buffer, ADC_MAX_SAMPLES * sizeof( uint8_t ) );

#ifdef DEBUG_MESSAGES
    printf("Message from %d\n", (device_id + 1) );
#endif

}

/*!
    @brief Open a file for each sensor (along with a main file) and reset all flags
*/
void initialize_sensors ( )
{
    uint8_t device;
    char full_filename[255];
    char filename[10];
    
    // Open Main file
    main_fp = fopen( "./all.csv", "w" );
    fprintf( main_fp, "Time" );
    
    // Open individual file for each sensor
    for ( device = 0; device < MAX_DEVICES; device++ )
    {   
        strcpy( full_filename, FILE_PATH );    
        
        filename[0] = 0x31 + device;
        filename[1] = 0x00;
        strcat( filename, ".csv" );
        
        strcat( full_filename, filename );
        
        sensors[device].fp = fopen( full_filename, "w" );
        
        fprintf( sensors[device].fp, "Time, Value\n" );
        
        fprintf( main_fp, ",Sensor %d", (device+1) );
        
        // Clear flags
        sensors[device].flags = 0;
    }
    
    fprintf( main_fp, "\n" );
    
#ifdef DEBUG_MESSAGES
    printf("Files Created.\n");
#endif
    return;
}


/*!
    @brief Close all sensor files and serial port
*/
void terminate_sensors ()
{
    uint8_t device;
    
    fclose( main_fp );
    
    for ( device = 0; device < MAX_DEVICES; device++ )
    {
        fclose( sensors[device].fp );
    }
    
    CloseComport( serial_port_number );

#ifdef DEBUG_MESSAGES
    printf("Files Closed.\n");
#endif

    return;
}

/*!
    @brief Handle interrupt event (SIGINT) so program exits cleanly
*/
void sigint_handler( int32_t sig ) {
        terminate_sensors();
        printf("\nExiting...\n");
        exit(sig);
}

