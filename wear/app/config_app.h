/*
 * config.h
 *
 * Created: 2014/8/6 18:35:03
 *  Author: Harifun
 */ 


#ifndef CONFIG_APP_H_
#define CONFIG_APP_H_

/*
	FFT config
	Fn = (n-1)Fs/FFT_N
*/
//#define LOG_OUT 	1		// use the log output function
//#define LIN_OUT8	1
//#define FFT_N		256		// set to 256 point ff

#define ENABLE_USARTD0
//#define ENABLE_USARTC0_REMAP

#define ENABLE_WDT
#define LIMIT_LOOP  //avoid loop too long time when problem happen.

/*
	WS2812 config
*/
#define DIN_CLR		ioport_set_pin_low(IOPORT_CREATE_PIN(PORTC, 6))
#define DIN_SET		ioport_set_pin_high(IOPORT_CREATE_PIN(PORTC, 6))
#define DIN_WS2812	IOPORT_CREATE_PIN(PORTC, 6)

/*
	LED defination
*/
#define LED_NUMBER	6
#define M_LED0		m_led_struct[0]
#define M_LED1		m_led_struct[1]
#define M_LED2		m_led_struct[2]
#define M_LED3		m_led_struct[3]
#define M_LED4		m_led_struct[4]
#define M_LED5		m_led_struct[5]

/**
 * \internal
 * \brief TWI master module
 */
#define TWI_MASTER			TWIC

/**
 * \internal
 * \brief TWI data transfer rate
 */
#define TWI_SPEED			400000

/**
 * \internal
 * \brief TWI master address
 */
#define TWI_MASTER_ADDR		0x50

/*
	charging pin
*/
#define CHARGING_PIN		IOPORT_CREATE_PIN(PORTR, 1)

/*
	playing pin
*/
#define PLAYING_PIN			IOPORT_CREATE_PIN(PORTD, 1)

/*
	led status piority
*/
#define STATUS_LED_OFF		0x00
#define STATUS_LED_CHARGE	0x01
#define STATUS_LED_FFT		0x02
#define STATUS_LED_UART		0x03

/*
	adc status config
*/
#define STATUS_NULL			0x00
#define STATUS_CHARGE		0x01
#define STATUS_FFT_L		0x02
#define STATUS_FFT_R		0x04
#define STATUS_LIGHT_READ	0x05
#define STATUS_BATT_READ 	0X06

//ADC Channel for charging
#define ADCCH_POS_CHARGE	ADCCH_POS_PIN4
//ADC Channel for L FFT
#define ADCCH_POS_FFT_L		ADCCH_POS_PIN1
#define ADCCH_NEG_FFT_L		ADCCH_NEG_PIN0
//ADC Channel for R FFT
#define ADCCH_POS_FFT_R		ADCCH_POS_PIN2
#define ADCCH_NEG_FFT_R		ADCCH_NEG_PIN3
//ADC Channel for Light sensor
#define ADCCH_POS_LIGHT		ADCCH_POS_PIN12



/*
	uart config
*/
#define START_FLAG			0x0F
#define STOP_FLAG			0xF0

#define UART_OK				0xFF
#define UART_FAIL			0x0C

#define CHECK_OK			0x00

#define WRITE_EEPROM		0x00
#define UART_LENGTH			0
#define UART_CMD			1
#define UART_EEPROM_ADDR	2
#define UART_EEPROM_DATA	3

#define M_USART				&USARTC0

#define BUBBLE_NUM			10

/*
	eeprom defines
*/
#define VISUALIZATION_STATE_BYTE 		0
#define EEPROM_VISUALIZATION_STATE		nvm_eeprom_read_byte(VISUALIZATION_STATE_BYTE)

/* This is the bytes where color saved values for the LED's */
/* Need to default the bytes at these locations to make headphones glow rainbow - Unboxing*/

/* LED1 */
#define EEPROM_LED1_R_BYTE				nvm_eeprom_read_byte(1)
#define EEPROM_LED1_G_BYTE				nvm_eeprom_read_byte(2)
#define EEPROM_LED1_B_BYTE				nvm_eeprom_read_byte(3)

/* LED2 */
#define EEPROM_LED2_R_BYTE				nvm_eeprom_read_byte(4)
#define EEPROM_LED2_G_BYTE				nvm_eeprom_read_byte(5)
#define EEPROM_LED2_B_BYTE				nvm_eeprom_read_byte(6)

/* LED2 */
#define EEPROM_LED3_R_BYTE				nvm_eeprom_read_byte(7)
#define EEPROM_LED3_G_BYTE				nvm_eeprom_read_byte(8)
#define EEPROM_LED3_B_BYTE				nvm_eeprom_read_byte(9)


// Command ID's for all messages
#define LED_COLOR_ID 0x00
#define VISUAL_ON_ID 0x01
#define VISUAL_OFF_ID 0x02
#define VBAT_ID 0x03
#define LIGHT_ID 0x04
#define NO_MESSAGE 0xFF

#endif /* CONFIG_H_ */