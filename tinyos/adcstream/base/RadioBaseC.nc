#include <Timer.h>
#include "message_types.h"
#include "sync_common.h"

module RadioBaseC
{
    uses
    {
        interface Leds;
        interface Boot;
        
        // Radio
        interface Packet as RadioPacket;
        interface AMPacket;
        interface AMSend;
        interface SplitControl as AMControl;
        interface Receive as RadioReceive;
        
        // Serial
        interface Packet as SerialPacket;
        interface AMPacket as SerialAMPacket;
        interface AMSend as SerialAMSend;
        interface SplitControl as SerialAMControl;

        // Alarm
        interface Alarm<T32khz,uint16_t> as Alarm32;
        interface Msp430Timer as Timer32;
        interface Msp430TimerControl as Control32;
        interface Msp430Compare as Compare32;
    }
}
implementation {
    uint16_t counter = 0;
    bool radio_busy = FALSE;
    bool serial_busy = FALSE;
    message_t radio_packet;
    message_t serial_packet;
    
    // Timing globals
    uint16_t current_event;
    uint8_t sync_counter = 0;
    
    test_adc_stream_numbered_short_msg_t* p_serial_payload;
    test_adc_stream_numbered_short_msg_t* p_radio_payload;
    sync_msg_t* p_radio_send_payload;

    void send_serial_message( test_adc_stream_numbered_short_msg_t* );     

    event void Boot.booted() {        
        // Start Serial
        call SerialAMControl.start();
        
        // Start Radio
        call AMControl.start();
        
        // Timing configuration
        call Timer32.clear();
        // Set TIMERA to UP Mode, schedule first interrupt
        call Timer32.setMode( 0x01 );
        TBCCR0 = MAJORCT;
        
        atomic current_event = MINORCT;
        
        atomic call Compare32.setEvent(current_event);
        call Control32.enableEvents();
    }
    
    event void AMControl.startDone(error_t err) {
        if (err == SUCCESS) {
            // Initialize sync message packet
            p_radio_send_payload = (sync_msg_t*)call RadioPacket.getPayload(&radio_packet, sizeof(sync_msg_t));
            p_radio_send_payload->flags = 0xf0;
        }
        else {
            call AMControl.start();
        }
    }

    event void AMControl.stopDone(error_t err) {
    }
 
    event void AMSend.sendDone(message_t* msg, error_t error) {
        call Leds.led2Off();
        if (&radio_packet == msg) {
            atomic radio_busy = FALSE;
        }
    }
    
    event message_t* RadioReceive.receive(message_t* msg, void* payload, uint8_t len) 
    {
        call Leds.led0Toggle();
        if (len == sizeof(test_adc_stream_numbered_short_msg_t)) {
            p_radio_payload = (test_adc_stream_numbered_short_msg_t*)payload;
            send_serial_message( p_radio_payload );
        }
        return msg;
    }
    
    // SERIAL  
    void send_serial_message( test_adc_stream_numbered_short_msg_t* p_message ) 
    {
        p_serial_payload = (test_adc_stream_numbered_short_msg_t*)call SerialPacket.getPayload(&serial_packet, sizeof(test_adc_stream_numbered_short_msg_t));
        if (p_serial_payload == NULL) {
            return;
        }
        
        if (call SerialPacket.maxPayloadLength() < sizeof(test_adc_stream_numbered_short_msg_t))
        {
            return;
        }
        
        memcpy( p_serial_payload, p_message, sizeof(test_adc_stream_numbered_short_msg_t) );

        if (call SerialAMSend.send(AM_BROADCAST_ADDR, &serial_packet, sizeof(test_adc_stream_numbered_short_msg_t)) == SUCCESS) {
            serial_busy = TRUE;
            //call Leds.led0Toggle();
        }
    
    }
    
    event void SerialAMSend.sendDone(message_t* bufPtr, error_t error) {
        if ( &serial_packet == bufPtr) {
            call Leds.led0Toggle();
            serial_busy = FALSE;
        }
    }
    
    event void SerialAMControl.startDone(error_t err) {
        if (err == SUCCESS) {
            
        }
    }
    
    event void SerialAMControl.stopDone(error_t err) 
    {
    
    }
    
    async event void Alarm32.fired() {
    
        atomic current_event += MINORCT;
        atomic if( current_event >= MAJORCT ){
            atomic current_event = MINORCT;
        }
        atomic call Compare32.setEvent(current_event);
        call Control32.enableEvents();
        
    }
    
    async event void Timer32.overflow()
    { 

        // send sync message        
        if( SYNC_ROUNDS < sync_counter )
        {
            if (call AMSend.send(AM_BROADCAST_ADDR, &radio_packet, sizeof(sync_msg_t)) == SUCCESS) {
                atomic radio_busy = TRUE;
            }
            atomic sync_counter = 0;   
            call Leds.led2On();
        }

       atomic sync_counter++;
        
    }
    
    async event void Compare32.fired()
    {
        //call Leds.led0Toggle(); 
    }
    
}


