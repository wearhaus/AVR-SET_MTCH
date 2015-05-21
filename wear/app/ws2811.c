/*
 * ws2811.c
 *
 * Created: 2014/7/19 15:54:12
 *  Author: Harifun
 */ 

#include "asf.h"
#include <ws2811.h>
#include <util/delay.h>

#define DIN_CLR	ioport_set_pin_low(IOPORT_CREATE_PIN(PORTR, 0))
#define DIN_SET	ioport_set_pin_high(IOPORT_CREATE_PIN(PORTR, 0))

inline void set_dat(void);
inline void clr_dat(void);
inline void reset_dat(void);
inline void send_bit(uint8_t data);
inline void send_char(uint8_t data);
inline void reset_dat(void);

void init_ws2811(void)
{
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTR, 0), IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
}
//2000ns high, 500ns low
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
	asm("nop");asm("nop");	//62.5ns
}
//500ns high, 2000ns low
inline void clr_dat(void)
{
	DIN_SET;				//62.5ns
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
	asm("nop");asm("nop");	//62.5ns
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

void set_value_ws2811(uint8_t r, uint8_t g, uint8_t b)
{
	send_char(r);
	send_char(g);
	send_char(b);

	reset_dat();
}

