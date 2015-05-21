/*
 * eeprom.c
 *
 * Created: 2014/8/6 18:42:43
 *  Author: Harifun
 */ 
#include "asf.h"
#include <eeprom.h>
#include <eeprom_init.h>

/*
	return false when the procedure was failed, true when success
*/
bool write_byte_eeprom(uint8_t addr, uint8_t data)
{
	uint8_t rev_data;
	
	nvm_eeprom_write_byte(addr, data);
	rev_data = nvm_eeprom_read_byte(addr);
	if (rev_data == data)
	{
		return true;
	} 
	else
	{
		return false;
	}	
}