/*
    BSN development kit sensor board - ADC
    by Alvaro Prieto
*/

#include "Msp430Adc12.h"

module ADCM {
	provides interface AdcConfigure<const msp430adc12_channel_config_t*> as Config0;
	provides interface AdcConfigure<const msp430adc12_channel_config_t*> as Config1;
	provides interface AdcConfigure<const msp430adc12_channel_config_t*> as Config2;
	provides interface AdcConfigure<const msp430adc12_channel_config_t*> as Config3;
	provides interface AdcConfigure<const msp430adc12_channel_config_t*> as Config4;
	provides interface AdcConfigure<const msp430adc12_channel_config_t*> as Config5;
	provides interface AdcConfigure<const msp430adc12_channel_config_t*> as Config6;
	provides interface AdcConfigure<const msp430adc12_channel_config_t*> as Config7;
}
implementation {
	msp430adc12_channel_config_t config0= {
		inch:INPUT_CHANNEL_A0,
		sref:REFERENCE_VREFplus_AVss,
		ref2_5v: REFVOLT_LEVEL_2_5,
		adc12ssel: SHT_SOURCE_SMCLK,
		adc12div: SHT_CLOCK_DIV_1,
		sht: SAMPLE_HOLD_512_CYCLES,
		sampcon_ssel: SAMPCON_SOURCE_SMCLK,
		sampcon_id:SAMPCON_CLOCK_DIV_1
	};
	
	msp430adc12_channel_config_t config1= {
		inch:INPUT_CHANNEL_A1,
		sref:REFERENCE_VREFplus_AVss,
		ref2_5v: REFVOLT_LEVEL_2_5,
		adc12ssel: SHT_SOURCE_SMCLK,
		adc12div: SHT_CLOCK_DIV_1,
		sht: SAMPLE_HOLD_4_CYCLES,
		sampcon_ssel: SAMPCON_SOURCE_SMCLK,
		sampcon_id:SAMPCON_CLOCK_DIV_1
	};

	msp430adc12_channel_config_t config2= {
		inch:INPUT_CHANNEL_A2,
		sref:REFERENCE_VREFplus_AVss,
		ref2_5v: REFVOLT_LEVEL_2_5,
		adc12ssel: SHT_SOURCE_SMCLK,
		adc12div: SHT_CLOCK_DIV_1,
		sht: SAMPLE_HOLD_4_CYCLES,
		sampcon_ssel: SAMPCON_SOURCE_SMCLK,
		sampcon_id:SAMPCON_CLOCK_DIV_1
	};

	msp430adc12_channel_config_t config3= {
		inch:INPUT_CHANNEL_A3,
		sref:REFERENCE_VREFplus_AVss,
		ref2_5v: REFVOLT_LEVEL_2_5,
		adc12ssel: SHT_SOURCE_SMCLK,
		adc12div: SHT_CLOCK_DIV_1,
		sht: SAMPLE_HOLD_4_CYCLES,
		sampcon_ssel: SAMPCON_SOURCE_SMCLK,
		sampcon_id:SAMPCON_CLOCK_DIV_1
	};
	
	msp430adc12_channel_config_t config4= {
		inch:INPUT_CHANNEL_A4,
		sref:REFERENCE_VREFplus_AVss,
		ref2_5v: REFVOLT_LEVEL_2_5,
		adc12ssel: SHT_SOURCE_SMCLK,
		adc12div: SHT_CLOCK_DIV_1,
		sht: SAMPLE_HOLD_4_CYCLES,
		sampcon_ssel: SAMPCON_SOURCE_SMCLK,
		sampcon_id:SAMPCON_CLOCK_DIV_1
	};
	
	msp430adc12_channel_config_t config5= {
		inch:INPUT_CHANNEL_A5,
		sref:REFERENCE_VREFplus_AVss,
		ref2_5v: REFVOLT_LEVEL_2_5,
		adc12ssel: SHT_SOURCE_SMCLK,
		adc12div: SHT_CLOCK_DIV_1,
		sht: SAMPLE_HOLD_4_CYCLES,
		sampcon_ssel: SAMPCON_SOURCE_SMCLK,
		sampcon_id:SAMPCON_CLOCK_DIV_1
	};

	msp430adc12_channel_config_t config6= {
		inch:INPUT_CHANNEL_A6,
		sref:REFERENCE_VREFplus_AVss,
		ref2_5v: REFVOLT_LEVEL_2_5,
		adc12ssel: SHT_SOURCE_SMCLK,
		adc12div: SHT_CLOCK_DIV_1,
		sht: SAMPLE_HOLD_4_CYCLES,
		sampcon_ssel: SAMPCON_SOURCE_SMCLK,
		sampcon_id:SAMPCON_CLOCK_DIV_1
	};

	msp430adc12_channel_config_t config7= {
		inch:INPUT_CHANNEL_A7,
		sref:REFERENCE_VREFplus_AVss,
		ref2_5v: REFVOLT_LEVEL_2_5,
		adc12ssel: SHT_SOURCE_SMCLK,
		adc12div: SHT_CLOCK_DIV_1,
		sht: SAMPLE_HOLD_4_CYCLES,
		sampcon_ssel: SAMPCON_SOURCE_SMCLK,
		sampcon_id:SAMPCON_CLOCK_DIV_1
	};
	
	async command const msp430adc12_channel_config_t *Config0.getConfiguration() {
	    return &config0;
	}
	
	async command const msp430adc12_channel_config_t *Config1.getConfiguration() {
	    return &config1;
	}
	
	async command const msp430adc12_channel_config_t *Config2.getConfiguration() {
	    return &config2;
	}
	
	async command const msp430adc12_channel_config_t *Config3.getConfiguration() {
	    return &config3;
	}
	
	async command const msp430adc12_channel_config_t *Config4.getConfiguration() {
	    return &config4;
	}
	
	async command const msp430adc12_channel_config_t *Config5.getConfiguration() {
	    return &config5;
	}
	
	async command const msp430adc12_channel_config_t *Config6.getConfiguration() {
	    return &config6;
	}
	
	async command const msp430adc12_channel_config_t *Config7.getConfiguration() {
	    return &config7;
	}

  }

