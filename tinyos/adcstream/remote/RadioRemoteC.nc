#include <Timer.h>
#include "message_types.h"
#include "sync_common.h"

#define NODE_ADDRESS TOS_NODE_ID

module RadioRemoteC
{
    uses
    {
        
        interface Leds;
        interface Boot;
              
        interface Packet as RadioPacket;
        interface AMPacket;
        interface AMSend;
        interface SplitControl as AMControl;
        interface Receive as RadioReceive;
        
        // Alarm        
        interface Alarm<T32khz,uint16_t> as Alarm32;
        interface Msp430Timer as Timer32;
        interface Msp430TimerControl as Control32;
        interface Msp430Compare as Compare32;
        
        // ADC
        //interface StdControl as SensorControl;
	    
	    interface ReadNow<uint16_t> as A0Read;
	    interface ReadNow<uint16_t> as A1Read;
	    interface ReadNow<uint16_t> as A2Read;
	    interface ReadNow<uint16_t> as A3Read;
	    interface ReadNow<uint16_t> as A4Read;
	    interface ReadNow<uint16_t> as A5Read;
	    interface ReadNow<uint16_t> as A6Read;
	    interface ReadNow<uint16_t> as A7Read;
	    
	    interface Resource as A0Resource;
    
    }
}
implementation {
    bool radio_busy = FALSE;
    message_t radio_packet;

    // ADC
    test_adc_stream_numbered_short_msg_t* p_radio_payload;
    
    uint8_t sample_buffer[ADC_MAX_SAMPLES * 2];
    uint16_t buffer_index;
    uint16_t packet_number = 0;
    uint8_t current_buffer; 
    uint16_t next_message;
    uint8_t message_ready;
    uint8_t send_message;
    uint8_t message_overflow;
    
    // Timing
    uint16_t current_event;
    
    task void sendMessage(  ) 
    {
        
        atomic if( message_ready == 0 )
        {
            //call Leds.led0Toggle();
            //post sendMessage( );
            return;
        }
        
        call Leds.led0Toggle();
        
        atomic message_ready = 0;
        
        //call Leds.led1Toggle();
        p_radio_payload = (test_adc_stream_numbered_short_msg_t*)call RadioPacket.getPayload(&radio_packet, sizeof(test_adc_stream_numbered_short_msg_t));
        if (p_radio_payload == NULL) {
            return;
        }
        
        
        atomic memcpy( p_radio_payload->samples, &sample_buffer[ current_buffer * ADC_MAX_SAMPLES ], ( sizeof(sample_buffer) / 2 ) );
        //call Leds.led0Toggle();
        p_radio_payload->source = NODE_ADDRESS;
        p_radio_payload->total_samples = current_buffer;
        p_radio_payload->packet_number = call Timer32.get();
        if (call RadioPacket.maxPayloadLength() < sizeof(test_adc_stream_numbered_short_msg_t))
        {
            return;
        }

        if (call AMSend.send(AM_BROADCAST_ADDR, &radio_packet, sizeof(test_adc_stream_numbered_short_msg_t)) == SUCCESS) {
            radio_busy = TRUE;
            call Leds.led1On();
        }
        
        
    
    }
    

    event void Boot.booted() {
        atomic buffer_index = 0;
        atomic message_ready = 0;
        atomic send_message = 0;
        atomic message_overflow = 0;
        //call SensorControl.start();
        
        call Leds.led0Off();
        
        call AMControl.start();        
    }
    
    event void AMControl.startDone(error_t err) {
        if (err == SUCCESS) {        
            call A0Resource.request();            
        }
        else {
            call AMControl.start();
        }
    }
    
    event void A0Resource.granted()
    {
        // Initialize timer
        call Timer32.clear();
        // Set TIMERA to UP Mode, schedule first interrupt
        call Timer32.setMode( 0x01 );
        atomic TBCCR0 = MAJORCT;
        call Timer32.clear();
        call A0Read.read();
        
        atomic current_event = MINORCT;
        
        atomic next_message = ( (REST_TIME/2) + ( 2 * MCT_TICKS ) * NODE_ADDRESS );
        
        atomic call Compare32.setEvent(current_event);
        call Control32.enableEvents();
    }

    event void AMControl.stopDone(error_t err) {
    }

    
    event void AMSend.sendDone(message_t* msg, error_t error) {
        
        if (&radio_packet == msg) {
            call Leds.led1Off();
            atomic radio_busy = FALSE;
        }
    }
    
    event message_t* RadioReceive.receive(message_t* msg, void* payload, uint8_t len) 
    {
        // Reset timer
        call Timer32.clear();
        call A0Read.read();
        
        atomic current_event = MINORCT;
        
        atomic call Compare32.setEvent(current_event);
        call Control32.enableEvents();
        
        // Initiate measurement                       
        atomic current_event = MINORCT;
        atomic next_message = ( (REST_TIME/2) + ( 2 * MCT_TICKS ) * NODE_ADDRESS );
        
        atomic buffer_index = 0;
        atomic message_overflow = 0;                
        
        //call Leds.led0Toggle();
        //call Leds.led2Toggle();
        return msg;
    }
    
    
    // Timing Functions
    async event void Alarm32.fired() {
        
        // Initiate measurement
        call A0Read.read();
        
        
        if( 0 == send_message )
        {
            current_event += MINORCT;
            if( current_event >= MAJORCT ){
                current_event = MINORCT;
            }
            
            // hoping they're not equal for now
            if( (current_event > next_message) && !message_overflow )
            {
                // schedule next event as a message sending one
                call Compare32.setEvent(next_message);
                
                next_message += POLLING_PERIOD;
                if( (next_message < call Compare32.getEvent()) || (next_message > MAJORCT) )
                {
                    message_overflow = 1;
                    next_message = ( (REST_TIME/2) + ( 2 * MCT_TICKS ) * NODE_ADDRESS );
                    //call Leds.led0Toggle();
                }
                send_message = 1;
                
            }
            else
            {
                call Compare32.setEvent(current_event);
                //call Leds.led0Toggle();
            }
            
        }
        else
        {
            post sendMessage( );
            send_message = 0;
            call Leds.led2Toggle();
            // Schedule next sampling event
            call Compare32.setEvent(current_event);
        }
        
        call Control32.enableEvents();
                
    }
    
    async event void Timer32.overflow()
    { 
        // Initiate measurement
        call A0Read.read();                            
        //call Leds.led0Toggle();
        
        atomic buffer_index = 0;
        atomic message_overflow = 0;
        atomic current_event = MINORCT;
        atomic next_message = ( (REST_TIME/2) + ( 2 * MCT_TICKS ) * NODE_ADDRESS );
    }
    
    async event void Compare32.fired()
    {
        //call Leds.led0Toggle(); 
    }
    
    
    // ADC Functions
   
    async event void A0Read.readDone(error_t result, uint16_t this_data) 
    {
        atomic this_data >>= 4;
        atomic sample_buffer[buffer_index] = this_data ;
        
        atomic buffer_index++;
        //call Leds.led0Toggle();
        
         if ( (ADC_MAX_SAMPLES) == buffer_index )
        {      
            atomic current_buffer = 0;
            atomic message_ready = 1;
            
        }
        else if( (2*ADC_MAX_SAMPLES) == buffer_index )
        {
            atomic buffer_index = 0;
            atomic current_buffer = 1;
            atomic message_ready = 1;           
        }
        
        
                      
    }
    
    async event void A1Read.readDone(error_t result, uint16_t this_data) 
    {
        //atomic  channels[1]=this_data;
    }

    async event void A2Read.readDone(error_t result, uint16_t this_data) 
    {
        //atomic  channels[2]=this_data;
    }
    
    async event void A3Read.readDone(error_t result, uint16_t this_data) 
    {
        //atomic  channels[3]=this_data;
    }

    async event void A4Read.readDone(error_t result, uint16_t this_data) 
    {
        //atomic  channels[4]=this_data;
    }
    
    async event void A5Read.readDone(error_t result, uint16_t this_data) 
    {
        //atomic  channels[5]=this_data;
    }
    
    async event void A6Read.readDone(error_t result, uint16_t this_data) 
    {
        //atomic  channels[6]=this_data;
    }
    
    async event void A7Read.readDone(error_t result, uint16_t this_data) 
    {
        //atomic  channels[7]=this_data;
    }
    
}


