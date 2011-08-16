/** @file main.c
*
* @brief TinyOS serial packet handling functions
*
* @author Alvaro Prieto
*/
#include "tos_packet.h"
#include "main.h"

static int32_t serial_port_number;
static uint32_t time_counter = 0;
static FILE* main_fp;

int main( int argc, char *argv[] )
{   
  uint8_t serial_buffer[BUFFER_SIZE]; 
  uint8_t packet_buffer[BUFFER_SIZE];
  uint8_t final_buffer[BUFFER_SIZE];  
  //uint8_t adc_sample_buffer[ADC_MAX_SAMPLES];    
  uint16_t buffer_offset = 0;
  uint8_t new_packet = 0;
  uint16_t buffer_index;
  uint16_t bytes_read = 0;  
  
  // Handle interrupt events to make sure files are closed before exiting
  (void) signal( SIGINT, sigint_handler );

  // Make sure input is correct
  if( argc < 2 )
  {
    printf("Usage: %s port baudrate\n", argv[0]);
    return 0;
  }  
  
  printf("CC2500 Sniffer\r\n");
  
  // Convert string serial number to integer
  serial_port_number = atoi( argv[1] );
  
  // Use RS232 library to open serial port  
  if ( OpenComport( serial_port_number, atoi(argv[2]) ) )
  {
    
    printf("Error opening serial port.\n");
    return 1;
  }
  // Flush the port
  while( PollComport( serial_port_number, serial_buffer, BUFFER_SIZE ) );
  
  memset( serial_buffer, 0x00, sizeof(serial_buffer) );
  
  // Run forever
  for(;;)
  {  
    // Read a buffer full of data (if available)
    bytes_read = PollComport( serial_port_number, serial_buffer, BUFFER_SIZE );
    if( bytes_read > 0 )
    {

      if( bytes_read >= BUFFER_SIZE )
      {
        printf("Uh oh!\n");
      }
      else
      {
        //printf("Read:%3d Offset:%3d\n", bytes_read,buffer_offset);
        /*printf("sb: ");
        for( buffer_index = 0; buffer_index < bytes_read; buffer_index++ )
        {
          printf("%02x", serial_buffer[buffer_index] );          
        }
        printf("\n");*/
        
        memcpy( packet_buffer+buffer_offset, serial_buffer, bytes_read );                        
        
        /*printf("pb: ");          
        for( buffer_index = 0; buffer_index < (bytes_read+buffer_offset); 
                                                            buffer_index++ )
        {
          printf("%02x", packet_buffer[buffer_index] );          
        }*/
        
        if ( packet_in_buffer( packet_buffer ) )
        {          
                    
          //printf(" OK\n");
          
          buffer_offset = 0;
          
          new_packet = 1;
          bytes_read = find_and_escape_packet( packet_buffer, final_buffer );
          
          
        } 
        else
        {   
          //memcpy( packet_buffer+buffer_offset, serial_buffer, bytes_read );         

          //printf("\n");
          
          buffer_offset += bytes_read;
          
          if( buffer_offset > BUFFER_SIZE )
          {
            printf("damn...\n");
            memset( packet_buffer, 0x00, sizeof(packet_buffer) );
            memset( serial_buffer, 0x00, sizeof(serial_buffer) );
            buffer_offset = 0;
          }
        }
        
        if( new_packet == 1 )
        {
          /*printf("fb: ");          
          for( buffer_index = 0; buffer_index < bytes_read; buffer_index++ )
          {
            printf("%02x", final_buffer[buffer_index] );          
          }
          printf("\n");*/
          
          process_packet( final_buffer );
          
          new_packet = 0;
          memset( packet_buffer, 0x00, sizeof(packet_buffer) );
        }      

      }
    }
    memset( serial_buffer, 0x00, sizeof(serial_buffer) );
    // Don't take up all the processor time    
    usleep(20000);

  }
  


  return 0;
}

void process_packet( uint8_t* buffer )
{
  uint8_t packet_length;
  uint8_t byte_index;
  packet_header_t* packet;
  packet = (packet_header_t*) buffer;
  
  packet_length = buffer[0];
  
  // Comment out if you want to see sync messages
  if ( PACKET_SYNC == ( packet->type & TYPE_MASK ) )
  {
    return;
  }
  
  // Print packet in hex
  printf("0x");
  for( byte_index = 0; byte_index <= packet_length; byte_index++ )
  {
    printf("%02X", buffer[byte_index]);
  }    

  switch( ( packet->type & TYPE_MASK ) )
  {
    case PACKET_START:
    {
      printf("START");
      break;
    }
    
    case PACKET_POLL:
    {
      printf("POLL");
      break;
    }
    
    default:
    {
      break;
    }
  }
  
  if ( packet->type & FLAG_ACK )
  {
    printf(" ACK");
  }

  printf( " from %d to %d", packet->source, packet->destination );  

  time_counter++;
  return;
}

/*!
  @brief Handle interrupt event (SIGINT) so program exits cleanly
*/
void sigint_handler( int32_t sig ) {
    CloseComport( serial_port_number );
    printf("\nExiting...\n");
    exit(sig);
}

