/*
 * eeprom_init.h
 *
 * Created: 2014/8/8 14:14:01
 *  Author: Harifun
 */ 


#ifndef EEPROM_INIT_H_
#define EEPROM_INIT_H_

#define CHARGE1P4_R			255
#define CHARGE1P4_G			0
#define CHARGE1P4_B			0

#define CHARGE2P4_R			0
#define CHARGE2P4_G			0
#define CHARGE2P4_B			255

#define CHARGE3P4_R			0
#define CHARGE3P4_G			255
#define CHARGE3P4_B			0

#define CHARGE9P10_R		0
#define CHARGE9P10_G		255
#define CHARGE9P10_B		0

#define LOW_TH_FRE_FFT_L		2
#define LOW_TH_RESERVED			0
#define LOW_FFT_L_R				255
#define LOW_FFT_L_G				255
#define LOW_FFT_L_B				255
#define LOW_FFT_L_PATTERN		0

#define MED_TH_FRE_FFT_L		10
#define MED_TH_RESERVED			0
#define MED_FFT_L_R				255
#define MED_FFT_L_G				255
#define MED_FFT_L_B				255
#define MED_FFT_L_PATTERN		0

#define HIGH_TH_FRE_FFT_L		20
#define HIGH_TH_RESERVED		0
#define HIGH_FFT_L_R			255
#define HIGH_FFT_L_G			255
#define HIGH_FFT_L_B			255
#define HIGH_FFT_L_PATTERN		0

#define LOW_TH_FRE_FFT_R		2
#define LOW_TH_RESERVED			0
#define LOW_FFT_R_R				255
#define LOW_FFT_R_G				255
#define LOW_FFT_R_B				255
#define LOW_FFT_R_PATTERN		0

#define MED_TH_FRE_FFT_R		10
#define MED_TH_RESERVED			0
#define MED_FFT_R_R				255
#define MED_FFT_R_G				255
#define MED_FFT_R_B				255
#define MED_FFT_R_PATTERN		0

#define HIGH_TH_FRE_FFT_R		20
#define HIGH_TH_RESERVED		0
#define HIGH_FFT_R_R			255
#define HIGH_FFT_R_G			255
#define HIGH_FFT_R_B			255
#define HIGH_FFT_R_PATTERN		0

#define MTCH_CONFIG				0

#define HOLD_TIME				100
#define VISUALIZATION_STATE_INIT	1

volatile const uint8_t data_init[10] __attribute__ ((section (".eeprom"))) ={
	VISUALIZATION_STATE_INIT,
	
	255,
	0,
	0,
	0,
	255,
	0,
	0,
	0,
	255
};



#endif /* EEPROM_INIT_H_ */