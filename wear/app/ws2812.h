/*
 * ws2812.h
 *
 * Created: 2014/7/20 14:22:25
 *  Author: Harifun
 */ 


#ifndef WS2812_H_
#define WS2812_H_

typedef struct
{
	//! r
	uint8_t r;
	//! g
	uint8_t g;
	//! b
	uint8_t b;
} led_struct;

/*
	* \param pin The pin to configure
	IOPORT_CREATE_PIN(PORTC, 5) as example
*/
void init_ws2812(port_pin_t pin);
void set_value_ws2812(uint8_t r, uint8_t g, uint8_t b);
void set_flash_ws2812(led_struct * m_led_struct, uint8_t num);

#endif /* WS2812_H_ */