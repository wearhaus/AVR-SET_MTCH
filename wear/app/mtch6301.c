/*
 * MTCH6301.c
 *
 * Created: 2014/7/12 18:55:10
 *  Author: Harifun
 */ 

#include "asf.h"
#include <mtch6301.h>
#include <util/delay.h>
#include <config_app.h>
#include <wear.h>

volatile uint8_t cmd_status = 0;
volatile uint8_t mapio_mtch6301 = 0;
touch_package_t touch_pkg;
touch_package_t * const m_touch_pkg = &touch_pkg;
uint8_t response_slaver[9]={0};
volatile uint8_t cmd_res = 0x01;	

volatile uint8_t latest_gesture = 0x00;

void cmd_read_register(uint8_t index, uint8_t offset);

void init_i2c(void)
{
	// TWI master options
	twi_options_t m_options = {
		.speed = TWI_SPEED,
		.chip  = TWI_MASTER_ADDR,
		.speed_reg = TWI_BAUD(sysclk_get_cpu_hz(), TWI_SPEED)
	};
	
	sysclk_enable_peripheral_clock(&TWI_MASTER);
	twi_master_init(&TWI_MASTER,&m_options);
	twi_master_enable(&TWI_MASTER);
	
	cpu_irq_enable();
}

static inline void wait_result(void)
{
	while(cmd_res == 0x01);
	
	cmd_res = 0x01;
}	
	
void init_mtch6301(void)
{
	/*
	iniatize the io interrupt pin pc5
	pc7 作为复位管脚，初始化为输出高电平
	*/
//	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 7), IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
		
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 5), IOPORT_DIR_INPUT | IOPORT_RISING | IOPORT_PULL_UP);
	PORTCFG.MPCMASK = 0x20;
	PORTC.INTMASK = 0x20;
	PORTC.INTCTRL = PMIC_LOLVLEN_bm;
	pmic_enable_level(PMIC_LOLVLEN_bm);
	cpu_irq_enable();

	/*
	reset the mtch6301
	*/
/*	ioport_set_pin_low(IOPORT_CREATE_PIN(PORTC, 7));
	_delay_ms(10);
	ioport_set_pin_high(IOPORT_CREATE_PIN(PORTC, 7));
	while(ioport_pin_is_high(IOPORT_CREATE_PIN(PORTC, 2)));
	
	//disable touch
	cmd_mtch6301(0x01);
	wait_result();
	//enable gesture
	cmd_write_register(0xF0,0x09,0x81);
	wait_result();
	//disable touch package 
	cmd_write_register(0xF0,0x07,0x01);
	wait_result();
	//read device id
	cmd_mtch6301(0x83);
	wait_result();
	//enable touch
	cmd_mtch6301(0x00);
	wait_result();*/
}

static inline uint8_t decode_mtch6301(uint8_t m_gesture)
{
	//twinkle(255, 255, 255);
	uint8_t cmd;
	
	switch(m_gesture)
	{
		case SINGLE_TAP:
			cmd = 0x01;
			latest_gesture = PLAY;
			////twinkle(255, 0, 0);
			break;
		case SINGLE_TAP_HOLD:
			cmd = 0x02;
			latest_gesture = CONNDISC;
			//twinkle(255, 255, 0);
			break;
		case DOUBLE_TAP:
			cmd = 0x03;
			latest_gesture = INQUIRE;
			break;
			//twinkle(0, 255, 0);
		case UP_SWIPE:
			cmd = 0x04;
			latest_gesture = VOLUP;
			//twinkle(0, 255, 255);
			break;
		case UP_SWIPE_HOLD:
			cmd = 0x04;
			latest_gesture = VOLUP;
			//twinkle(0, 255, 255);
			break;
		case RIGHT_SWIPE:
			cmd = 0x05;
			latest_gesture = NEXT;
			//twinkle(0, 0, 255);
			break;
		case RIGHT_SWIPE_HOLD:
			cmd = 0x05;
			latest_gesture = NEXT;
			//twinkle(0, 0, 255);
			break;
		case DOWN_SWIPE:
			cmd = 0x06;
			latest_gesture = VOLDOWN;
			//twinkle(255, 0, 255);
			break;
		case DOWN_SWIPE_HOLD:
			cmd = 0x06;
			latest_gesture = VOLDOWN;
			//twinkle(255, 0, 255);
			break;
		case LEFT_SWIPE:
			cmd = 0x07;
			latest_gesture = PREV;
			//twinkle(255, 255, 255);
			break;
		case LEFT_SWIPE_HOLD:
			cmd = 0x07;
			latest_gesture = PREV;
			//twinkle(255, 255, 255);
			break;
		default:
			cmd = 0x00;
			latest_gesture = NONE;
			break;
	}
	
	return cmd;
}

void init_mtchio(void)
{
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTA, 5), IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTA, 6), IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTA, 7), IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 6), IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTR, 0), IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 0), IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 1), IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
}

void init_ledhack(void)
{
	//ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 0), IOPORT_DIR_INPUT | IOPORT_RISING | IOPORT_PULL_DOWN);
	//ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 1), IOPORT_DIR_INPUT | IOPORT_RISING | IOPORT_PULL_DOWN);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 5), IOPORT_DIR_INPUT | IOPORT_RISING | IOPORT_PULL_DOWN);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 7), IOPORT_DIR_INPUT | IOPORT_RISING | IOPORT_PULL_DOWN);
}

void mapio(uint8_t cmd)
{
	if (cmd == 0x01)
	{
		latest_gesture = 0x01;
	} 
	else
	{
		//ioport_set_pin_low(IOPORT_CREATE_PIN(PORTA, 5));
	}/*
	if (cmd == 0x02)
	{
		ioport_set_pin_high(IOPORT_CREATE_PIN(PORTA, 5));
	}
	else
	{
		ioport_set_pin_low(IOPORT_CREATE_PIN(PORTA, 5));
	}
	if (cmd == 0x03)
	{
		ioport_set_pin_high(IOPORT_CREATE_PIN(PORTA, 5));
	}
	else
	{
		ioport_set_pin_low(IOPORT_CREATE_PIN(PORTA, 5));
	}*/
	if (cmd == 0x05)
	{
		latest_gesture = 0x05;
	}
	else
	{
		//ioport_set_pin_low(IOPORT_CREATE_PIN(PORTA, 6));
	}
	if (cmd == 0x07)
	{
		latest_gesture = 0x07;
	}
	else
	{
		//ioport_set_pin_low(IOPORT_CREATE_PIN(PORTA, 7));
	}
	if (cmd == 0x04)
	{
		latest_gesture = 0x04;
	}
	else
	{
		//ioport_set_pin_low(IOPORT_CREATE_PIN(PORTD, 6));
	}
	if (cmd == 0x06)
	{
		latest_gesture = 0x06;
	}
	else
	{
		//ioport_set_pin_low(IOPORT_CREATE_PIN(PORTR, 0));
	}
}

static inline void mtch6301_recv_data(void)
{
	uint8_t recv_pattern[RECV_SIZE_MTCH6301] = {0};	
	
	twi_package_t package = {
		.addr_length	= 0,
		.chip			= ADDR_MTCH6301,
		.buffer			= (void *)recv_pattern,
		.length			= RECV_SIZE_MTCH6301,
		.no_wait		= false
	};
	
	//twinkle(255, 0, 0);
	
	twi_master_read(&TWI_MASTER, &package);
	m_touch_pkg->tch	= (recv_pattern[1] & 0x04) >> 2;
	
	if (m_touch_pkg->tch == 0)
	{/*
		m_touch_pkg->id		= (recv_pattern[1] & 0x78) >> 3;		
		m_touch_pkg->pen	= (recv_pattern[1] & 0x01) >> 0;
		m_touch_pkg->x		= ((recv_pattern[3] & 0x1F) << 7) | ((recv_pattern[2] & 0x7F) >> 0);
		m_touch_pkg->y		= ((recv_pattern[5] & 0x1F) << 7) | ((recv_pattern[4] & 0x7F) >> 0);	
		twinkle(0, 255, 0);	*/
	} 
	else
	{
		m_touch_pkg->id		= (recv_pattern[1] & 0x78) >> 3;
		m_touch_pkg->gesture= (recv_pattern[2] & 0x7F) >> 0;
		
		mapio_mtch6301 = decode_mtch6301(m_touch_pkg->gesture);
		//usart_putchar(M_USART, mapio_mtch6301);
		//mapio(mapio_mtch6301);
		//_delay_ms(200);
		//mapio(0);
	}

	return NULL;
}

void cmd_write_register(uint8_t index, uint8_t offset, uint8_t value)
{
	uint8_t cmd_slaver[6]={0x55,0x04,0x15,index,offset,value};
	
	twi_package_t package = {
		.addr_length	= 0,
		.chip			= ADDR_MTCH6301,
		.buffer			= (void *)cmd_slaver,
		.length			= 6,
		.no_wait		= false
	};
	
	twi_master_write(&TWI_MASTER, &package);
	cmd_status = 1;
}

uint8_t read_register(uint8_t index, uint8_t offset)
{
	cmd_read_register(index, offset);
	wait_result();
	
	return REG_READ;
}

void cmd_read_register(uint8_t index, uint8_t offset)
{
	uint8_t cmd_slaver[5]={0x55,0x03,0x16,index,offset};
	
	twi_package_t package = {
		.addr_length	= 0,
		.chip			= ADDR_MTCH6301,
		.buffer			= (void *)cmd_slaver,
		.length			= 5,
		.no_wait		= false
	};
	
	twi_master_write(&TWI_MASTER, &package);
	cmd_status = 1;
}

void cmd_mtch6301(uint8_t cmd)
{
	uint8_t cmd_slaver[3]={0x55,0x01,cmd};
	
	twi_package_t package = {
		.addr_length	= 0,
		.chip			= ADDR_MTCH6301,
		.buffer			= (void *)cmd_slaver,
		.length			= 3,
		.no_wait		= false
	};
	
	twi_master_write(&TWI_MASTER, &package);
	cmd_status = 1;
}

static inline uint8_t response_cmd()
{
	uint8_t i;
	
	twi_package_t package = {
		.addr_length	= 0,
		.chip			= ADDR_MTCH6301,
		.buffer			= (void *)response_slaver,
		.length			= 9,
		.no_wait		= false
	};
	
	twi_master_read(&TWI_MASTER, &package);
	cmd_status = 0;
	
	for (i=0;i<9;i++)
	{
		//usart_putchar(&USARTD0, response_slaver[i]);
	}
	
	return response_slaver[3];
}


ISR(PORTC_INT_vect)
{	
	PORTC.INTFLAGS = 0x20;
//	usart_putchar(&USARTD0, 0xF0);
	//twinkle(255, 255, 255);
	
	switch(cmd_status)
	{
		case 0:			
			mtch6301_recv_data();
			break;
		case 1:
			cmd_res = response_cmd();
			break;
		default:
			return;
	}
}