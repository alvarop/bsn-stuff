#define TOSH_DATA_LENGTH 70
#include <Timer.h>
#include "message_types.h"

configuration RadioBaseAppC
{

}

implementation {
    components MainC;
    components LedsC;
    components RadioBaseC as App;

    // Radio components
    components ActiveMessageC;
    components new AMSenderC(AM_SYNC_MSG);
    components new AMReceiverC(AM_ADC_STREAM_NUMBERED_SHORT_MSG);

    // Serial components
    components SerialActiveMessageC;
    components new SerialAMSenderC(AM_ADC_STREAM_NUMBERED_SHORT_MSG);

    // Alarm for timing and synchronization
    components new TestAlarmC() as Alarm32;

    App.Boot    -> MainC;
    App.Leds    -> LedsC;
    
    // Timing Connections
    App.Alarm32  -> Alarm32;
    App.Timer32 -> Alarm32;
    App.Control32 -> Alarm32;
    App.Compare32 -> Alarm32;
    
    // Radio Connections
    App.RadioPacket     -> AMSenderC;
    App.AMPacket        -> AMSenderC;
    App.AMSend          -> AMSenderC;
    App.AMControl       -> ActiveMessageC;
    App.RadioReceive    -> AMReceiverC;
    
    // Serial Connections
    App.SerialPacket     -> SerialAMSenderC;
    App.SerialAMPacket   -> SerialAMSenderC;
    App.SerialAMSend     -> SerialAMSenderC;
    App.SerialAMControl  -> SerialActiveMessageC;
    
}

