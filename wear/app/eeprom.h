/*
 * eeprom.h
 *
 * Created: 2014/8/6 20:37:57
 *  Author: Harifun
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_


/*
	return false when the procedure was failed, true when success
*/
bool write_byte_eeprom(uint8_t addr, uint8_t data);

#endif /* EEPROM_H_ */