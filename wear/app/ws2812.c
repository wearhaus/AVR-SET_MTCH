/*
 * ws2812.c
 *
 * Created: 2014/7/20 14:22:16
 *  Author: Harifun
 */ 

#include "asf.h"
#include <ws2812.h>
#include <util/delay.h>
#include <config_app.h>

/*
	* \param pin The pin to configure
	IOPORT_CREATE_PIN(PORTC, 5) as example
*/
void init_ws2812(port_pin_t pin)
{
	ioport_configure_pin(pin, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
}

//700ns high, 600ns low
inline void set_dat(void)
{
	DIN_SET;				//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	DIN_CLR;				//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");				//31.25ns
}

//350ns high, 800ns low
inline void clr_dat(void)
{
	DIN_SET;				//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");				//31.25ns
	DIN_CLR;				//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");asm("nop");	//62.5ns
	asm("nop");				//31.25ns
}

inline void reset_dat(void)
{
	DIN_CLR;
	_delay_ms(1);
}

inline void send_bit(uint8_t data)
{
	if (data)
	{
		set_dat();
	}
	else
	{
		clr_dat();
	}
}

inline void send_char(uint8_t data)
{
	uint8_t i;
	for (i=0;i<8;i++)
	{
		send_bit(data&0x80);
		data = data << 1;
	}
}

void set_value_ws2812(uint8_t r, uint8_t g, uint8_t b)
{
	send_char(r);
	send_char(g);
	send_char(b);

	reset_dat();
}

void set_led_ws2812(led_struct * m_led_struct)
{
	send_char(m_led_struct->g);
	send_char(m_led_struct->r);
	send_char(m_led_struct->b);
}

/*
This is a brief function to light a rgb led string.
led_struct-----single led rgb parameter
num------------the number of led you want to light
*/
inline void set_flash_ws2812(led_struct * m_led_struct, uint8_t num)
{
	uint8_t i=0;
	
	for (i=0;i<num;i++)
	{
		set_led_ws2812(&m_led_struct[i]);
	}
	
	reset_dat();
}

