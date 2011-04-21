/*
    BSN development kit sensor board - ADC
    by Alvaro Prieto
*/

generic configuration ADCC() {
    provides interface ReadNow<uint16_t> as ADC0;//AccelX;
    provides interface ReadNow<uint16_t> as ADC1;
    provides interface ReadNow<uint16_t> as ADC2;
    provides interface ReadNow<uint16_t> as ADC3;
    provides interface ReadNow<uint16_t> as ADC4;
    provides interface ReadNow<uint16_t> as ADC5;
    provides interface ReadNow<uint16_t> as ADC6;
    provides interface ReadNow<uint16_t> as ADC7;
    
    provides interface Resource as ADC0Resource;
}
implementation {
	components new AdcReadNowClientC() as A0,new AdcReadNowClientC() as A1, new AdcReadNowClientC() as A2, new AdcReadNowClientC() as A3,
	            new AdcReadNowClientC() as A4,new AdcReadNowClientC() as A5, new AdcReadNowClientC() as A6, new AdcReadNowClientC() as A7;
		
	components ADCM;
	
	ADC0 = A0;
	ADC0Resource = A0;
	A0.AdcConfigure -> ADCM.Config0;

	ADC1 = A1;
	A1.AdcConfigure -> ADCM.Config1;
	
	ADC2 = A2;
	A2.AdcConfigure -> ADCM.Config2;
	
	ADC3 = A3;
	A3.AdcConfigure -> ADCM.Config3;
	
	ADC4 = A4;
	A4.AdcConfigure -> ADCM.Config4;

	ADC5 = A5;
	A5.AdcConfigure -> ADCM.Config5;
	
	ADC6 = A6;
	A6.AdcConfigure -> ADCM.Config6;
	
	ADC7 = A7;
	A7.AdcConfigure -> ADCM.Config7;
}
