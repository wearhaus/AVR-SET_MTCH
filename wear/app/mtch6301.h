/*
 * ch6301.h
 *
 * Created: 2014/7/12 18:55:27
 *  Author: Harifun
 */ 


#ifndef CH6301_H_
#define CH6301_H_

#define SINGLE_TAP			0x10
#define SINGLE_TAP_HOLD		0x11
#define DOUBLE_TAP			0x20
#define UP_SWIPE			0x31
#define UP_SWIPE_HOLD		0x32
#define RIGHT_SWIPE			0x41
#define RIGHT_SWIPE_HOLD	0x42
#define DOWN_SWIPE			0x51
#define DOWN_SWIPE_HOLD		0x52
#define LEFT_SWIPE			0x61
#define LEFT_SWIPE_HOLD		0x62

#define PLAY				0xa0
#define CONNDISC			0xa1
#define INQUIRE				0xa2
#define VOLUP				0xa3
#define NEXT				0xa4
#define VOLDOWN				0xa5
#define PREV				0xa6
#define NONE				0x00

#define ADDR_MTCH6301			0x25
#define REG_READ				response_slaver[5]
#define RECV_SIZE_MTCH6301		6
extern volatile uint8_t cmd_status;

typedef struct
{
	//! Touch ID
	uint8_t id;
	//! TCH
	uint8_t tch;
	//! PEN
	uint8_t pen;
	//! X Coordinate
	uint16_t x;
	//! Y Coordinate
	uint16_t y;
	//! Gesture
	uint8_t gesture;
} touch_package_t;

//void mtch6301_recv_data(void);
void init_mtch6301(void);
void read_device_id(void);
void cmd_mtch6301(uint8_t cmd);
bool cmd_write_register(uint8_t index, uint8_t offset, uint8_t value);
uint8_t read_register(uint8_t index, uint8_t offset);
void init_i2c(void);
void init_mtchio(void);
bool write_mtch_settings(void);

extern volatile uint8_t latest_gesture;

#endif /* CH6301_H_ */