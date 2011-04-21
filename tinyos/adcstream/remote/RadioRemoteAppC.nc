
#define TOSH_DATA_LENGTH 70
#include <Timer.h>
#include "message_types.h"

configuration RadioRemoteAppC {
}
implementation {
    components MainC;
    components LedsC;
    components RadioRemoteC as App;
    
    components ActiveMessageC;
    components new AMSenderC(AM_ADC_STREAM_NUMBERED_SHORT_MSG);
    components new AMReceiverC(AM_SYNC_MSG);
    
    components new TestAlarmC() as Alarm32;
    
    // ADC
    components new ADCC(); 

    App.Boot -> MainC;
    App.Leds -> LedsC;
    
    // Radio Connections
    App.RadioPacket     -> AMSenderC;
    App.AMPacket        -> AMSenderC;
    App.AMSend          -> AMSenderC;
    App.AMControl       -> ActiveMessageC;
    App.RadioReceive    -> AMReceiverC;
    
    // Timing Connections
    App.Alarm32     -> Alarm32;
    App.Timer32     -> Alarm32;
    App.Control32   -> Alarm32;
    App.Compare32   -> Alarm32;
    
    // ADC Connections
    //App.SensorControl -> ADCC.StdControl;
    App.A0Read        -> ADCC.ADC0;
    App.A1Read        -> ADCC.ADC1;
    App.A2Read        -> ADCC.ADC2;
    App.A3Read        -> ADCC.ADC3;
    App.A4Read        -> ADCC.ADC4;
    App.A5Read        -> ADCC.ADC5;
    App.A6Read        -> ADCC.ADC6;
    App.A7Read        -> ADCC.ADC7;
    App.A0Resource    -> ADCC.ADC0Resource;
}

