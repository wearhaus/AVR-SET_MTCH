/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include <config_app.h>
#include <adc_app.h>
#include <timer_app.h>
#include <wear.h>
#include <uart.h>
#include <mtch6301.h>
#include <util/delay.h>
#include <eeprom.h>

uint8_t new_message_ID;
uint8_t fht_flag = false;		//judge if the fht led function was turned on or off
char response[2] = {0, 0};

/*
uint8_t playPauseColor[9] = {0, 128, 128, 0, 128, 128, 0, 128, 128};
uint8_t forwardColor[9] = {0, 128, 128, 0, 0, 0, 0, 0, 0};
uint8_t backwardColor[9] = {0, 0, 0, 0, 128, 128, 0, 0, 0};
uint8_t volumeUpColor[9] = {0, 0, 0, 0, 0, 0, 0, 128, 128};
uint8_t volumeDownColor[9] = {0, 128, 128, 0, 128, 128, 0, 0, 0};
uint8_t connDiscColor[9] = {0, 128, 0, 0, 0, 0, 0, 128, 0};
uint8_t inquiryColor[9] = {0, 0, 0, 128, 128, 0, 128, 128, 0};
*/

static void test_pin(void) {
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTA, 6), IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	twinkle(0, 255, 255);
	ioport_set_pin_high(IOPORT_CREATE_PIN(PORTA, 6));
	_delay_ms(2000);
	ioport_set_pin_low(IOPORT_CREATE_PIN(PORTA, 6));
	twinkle(0, 0, 0);
}

void set_temp_color(uint8_t* tempcolor) {
	m_led_struct[0].r = tempcolor[0];
	m_led_struct[0].g = tempcolor[1];
	m_led_struct[0].b = tempcolor[2];
	m_led_struct[3].r = tempcolor[0];
	m_led_struct[3].g = tempcolor[1];
	m_led_struct[3].b = tempcolor[2];
	
	m_led_struct[1].r = tempcolor[3];
	m_led_struct[1].g = tempcolor[4];
	m_led_struct[1].b = tempcolor[5];
	m_led_struct[4].r = tempcolor[3];
	m_led_struct[4].g = tempcolor[4];
	m_led_struct[4].b = tempcolor[5];
	
	m_led_struct[2].r = tempcolor[6];
	m_led_struct[2].g = tempcolor[7];
	m_led_struct[2].b = tempcolor[8];
	m_led_struct[5].r = tempcolor[6];
	m_led_struct[5].g = tempcolor[7];
	m_led_struct[5].b = tempcolor[8];

	set_flash_ws2812(m_led_struct, 6);
}


int main(void)
{
	//ioport_configure_pin(IOPORT_CREATE_PIN(PORTA, 6), IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	// Insert system clock initialization code here (sysclk_init()).
	sysclk_init();
	//board_init();
//	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 3), IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);

#ifdef ZXL_WDT_ENABLE
	/* Set the timeout period for the watchdog - 8 ms */
	wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_2KCLK);
    wdt_reset(); 
	wdt_enable();
#endif	
	
	init_uart();
	init_ws2812(IOPORT_CREATE_PIN(PORTC, 6));
	clear_led();
	init_chargingpin(IOPORT_CREATE_PIN(PORTR, 1));
	
	volatile uint8_t eep0 = nvm_eeprom_read_byte(0);
	volatile uint8_t eep1 = nvm_eeprom_read_byte(1);
	volatile uint8_t eep2 = nvm_eeprom_read_byte(2);
	volatile uint8_t eep3 = nvm_eeprom_read_byte(3);
	volatile uint8_t eep4 = nvm_eeprom_read_byte(4);
	volatile uint8_t eep5 = nvm_eeprom_read_byte(5);
	volatile uint8_t eep6 = nvm_eeprom_read_byte(6);
	volatile uint8_t eep7 = nvm_eeprom_read_byte(7);
	volatile uint8_t eep8 = nvm_eeprom_read_byte(8);
	volatile uint8_t eep9 = nvm_eeprom_read_byte(9);

	// init the mtch 6301
	init_mtchio();
	init_i2c();
	init_mtch6301();
		
	// restore the state stored in the eeprom
	// restore_state_eep();
	
	//rainbow();
	restore_led_from_eeprom();
	
	core_status = STATUS_CHARGE;
	change_adc_channel(core_status);
	flag_initcharge = true;
	tc45_disable(&TCC4);
	tc45_enable(&TCC5);
		
	init_adc(core_status);
	adc_enable_interrupt();
	init_timer5();
	init_timer4();		
		
	if /*(ischarging())
	{
		old_core_status = core_status;
		core_status = STATUS_CHARGE;
		change_adc_channel(core_status);
		flag_initcharge = true;
		tc45_disable(&TCC4);
		tc45_enable(&TCC5);
	}
	else*/
	(!ischarging()){
		old_core_status = core_status;
		core_status = STATUS_BATT_READ;
		change_adc_channel(core_status);
		tc45_disable(&TCC5);
		tc45_enable(&TCC4);
	//	usart_putchar(M_USART, 0x23);
	}
	
	/* Charging and watchdog timer - Using two compare channels with single timer */
	//init_timerd5();
	
	// Insert application code here, after the board has been initialized.
	while(1)
	{
        #ifdef ZXL_WDT_ENABLE
                wdt_reset(); 
        #endif
		if (latest_gesture) {
			//twinkle(255, 0, 255);
			switch (latest_gesture) {
				case PLAY:
					ioport_set_pin_high(IOPORT_CREATE_PIN(PORTA, 5));
					//twinkle(255, 0, 0);
					//set_temp_color(playPauseColor);
					_delay_ms(2000); //twinkle(0, 0, 0);
					ioport_set_pin_low(IOPORT_CREATE_PIN(PORTA, 5));
					break;
				case NEXT:
					ioport_set_pin_high(IOPORT_CREATE_PIN(PORTA, 6));
					//twinkle(255, 255, 0);
					//set_temp_color(forwardColor);
					_delay_ms(2000); //twinkle(0, 0, 0);
					ioport_set_pin_low(IOPORT_CREATE_PIN(PORTA, 6));
					break;
				case PREV:
					ioport_set_pin_high(IOPORT_CREATE_PIN(PORTA, 7));
					//twinkle(0, 255, 0);
					//set_temp_color(backwardColor);
					_delay_ms(2000); //twinkle(0, 0, 0);
					ioport_set_pin_low(IOPORT_CREATE_PIN(PORTA, 7));
					break;
				case VOLUP:
					ioport_set_pin_high(IOPORT_CREATE_PIN(PORTD, 6));
					//twinkle(0, 255, 255);
					//set_temp_color(volumeUpColor);
					_delay_ms(2000); //twinkle(0, 0, 0);
					ioport_set_pin_low(IOPORT_CREATE_PIN(PORTD, 6));
					break;
				case VOLDOWN:
					ioport_set_pin_high(IOPORT_CREATE_PIN(PORTR, 0));
					//twinkle(0, 0, 255);
					//set_temp_color(volumeDownColor);
					_delay_ms(2000); //twinkle(0, 0, 0);
					ioport_set_pin_low(IOPORT_CREATE_PIN(PORTR, 0));
					break;
				case CONNDISC:
					ioport_set_pin_high(IOPORT_CREATE_PIN(PORTD, 0));
					//twinkle(255, 0, 255);
					//set_temp_color(connDiscColor);
					_delay_ms(2000); //twinkle(0, 0, 0);
					ioport_set_pin_low(IOPORT_CREATE_PIN(PORTD, 0));
					break;
				case INQUIRE:
					ioport_set_pin_high(IOPORT_CREATE_PIN(PORTD, 1));
					//twinkle(255, 255, 255);
					//set_temp_color(inquiryColor);
					_delay_ms(2000); //twinkle(0, 0, 0);
					ioport_set_pin_low(IOPORT_CREATE_PIN(PORTD, 1));
					break;
				default:
				break;
			}
			latest_gesture = NONE;
			//twinkle(0, 0, 0);
		}
		
		if (ischarging()) {
			if (core_status != STATUS_CHARGE)
			{
				twinkle(0, 0, 255);
				shutdown_received = false;
				old_core_status = core_status;
				core_status = STATUS_CHARGE;
				change_adc_channel(core_status);
				flag_initcharge = true;
				tc45_disable(&TCC4);
				tc45_enable(&TCC5);
			}
		}
		else {
			if (core_status == STATUS_CHARGE) {
				old_core_status = core_status;
				core_status = STATUS_BATT_READ;
				change_adc_channel(core_status);
				tc45_disable(&TCC5);
				tc45_enable(&TCC4);
				load_data_color_led();
			}
			
			if (get_pulse_state() && core_status != STATUS_FFT_L && core_status != STATUS_FFT_R) {
				trigger_pulse_state_changed();
			}
			
			if (shutdown_received) {
				old_core_status = core_status;
				core_status = STATUS_NULL;
				tc45_disable(&TCC4);
				save_led_to_eeprom();
				twinkle(0, 0, 0);
			}
			else if (get_and_clear_pulse_state_changed()) {
				if (get_pulse_state()) {
					// If new message is to turn on visualization and if it is already on then ignore it
					
					// Initialize the ADC by changing to correct channel
					fht_flag = true;
					
					old_core_status = core_status;
					core_status = STATUS_FFT_L;
					change_adc_channel(core_status);
					// Switch on visualization interrupt
					tc45_enable(&TCC4);
					// save state to CSR- the fact that visualization is on
					write_byte_eeprom(VISUALIZATION_STATE_BYTE, 0x01);
					// respond to CSR
					//send_response(new_message_ID, 0x01);
				}
				else {
					fht_flag = false;
					old_core_status = core_status;
					core_status = STATUS_BATT_READ;
					change_adc_channel(core_status);
					clear_led();
					// save the state to eeprom (Clear visualization was on)
					write_byte_eeprom(VISUALIZATION_STATE_BYTE, 0x00);
					// Update LED from led_color_buf_data to last saved state
					load_data_color_led();
					//send_response(new_message_ID, 0x01);
				}
			}
			else {
				// if the fht led function was turned on
				if (fht_flag == true)
				{
					if (flag_fft == true)
					{
						
						if (core_status == STATUS_FFT_L)
						{
							//	usart_putchar(M_USART, 0x55);

							run();
							old_core_status = core_status;
							core_status = STATUS_FFT_R;
							change_adc_channel(core_status);
							tc45_enable(&TCC4);
						}
						else if (core_status == STATUS_FFT_R)
						{
							//	usart_putchar(M_USART, 0x66);

							run();
							if (ischarging())
							{
								old_core_status = core_status;
								core_status = STATUS_CHARGE;
								change_adc_channel(core_status);
								flag_initcharge = true;
								tc45_disable(&TCC4);
								tc45_enable(&TCC5);
							}
							else
							{
								old_core_status = core_status;
								core_status = STATUS_FFT_L;
								change_adc_channel(core_status);
								tc45_enable(&TCC4);
							}
							
						}
						
						flag_fft = false;
					}
					
				}
				else {
					//load_data_color_led();
				}
			}
		}
		/*
		if (flag_timer5) {
			flag_timer5 = false;
			update_led_charge();
		}
		*/
		
		
		
		
		/*
//		detect_led();
		if (ischarging())
		{
			if (core_status != STATUS_CHARGE)
			{
				core_status = STATUS_CHARGE;
				change_adc_channel(core_status);
				tc45_enable(&TCC4);
				tc45_enable(&TCC5);
			}			
		} 
		else
		{
			if (new_message_exists())
			{
				// Check new message against existing LED_STATUS - if it is the exact same ignore
				new_message_ID = get_message_ID();
				
				switch(new_message_ID)
				{
					case VISUAL_ON_ID:
						// If new message is to turn on visualization and if it is already on then ignore it
						
						// Initialize the ADC by changing to correct channel
						fht_flag = true;
							
						core_status = STATUS_FFT_L;
						change_adc_channel(core_status);
						// Switch on visualization interrupt
						tc45_enable(&TCC4);
						// save state to CSR- the fact that visualization is on 
						write_byte_eeprom(VISUALIZATION_STATE_BYTE, 0x01);
						// respond to CSR 
						send_response(new_message_ID, 0x01);						
						break;
					case LED_COLOR_ID:
						// if current state is music visualization 
						// turn off fft interrupt 
						fht_flag = false;
						
						core_status = STATUS_NULL;						
						tc45_disable(&TCC4);
						clear_led();
						
						// update the LED's 
						load_buff_data_color_led();
						// save the state to EEPROM (including the latest LED colors) 
				//		nvm_eeprom_erase_and_write_buffer((uint16_t)EEPROM_LED1_R_BYTE, &buffer_data[2], 9);
						write_byte_eeprom(1, buffer_data[2]);
						write_byte_eeprom(2, buffer_data[3]);
						write_byte_eeprom(3, buffer_data[4]);
						
						write_byte_eeprom(4, buffer_data[5]);
						write_byte_eeprom(5, buffer_data[6]);
						write_byte_eeprom(6, buffer_data[7]);
						
						write_byte_eeprom(7, buffer_data[8]);
						write_byte_eeprom(8, buffer_data[9]);
						write_byte_eeprom(9, buffer_data[10]);
						// respond to CSR 
						send_response(new_message_ID, 0x01);
						break;
					case VISUAL_OFF_ID:
						core_status = STATUS_NULL;
						tc45_disable(&TCC4);
						clear_led();
						// save the state to eeprom (Clear visualization was on) 
						write_byte_eeprom(VISUALIZATION_STATE_BYTE, 0x00);
						// Update LED from led_color_buf_data to last saved state 
						restore_led_from_eeprom();
						send_response(new_message_ID, 0x01);
						break;
					case VBAT_ID:
						// DO NOT UPDATE FIRMWARE- HEADPHONES ARE NOT CHARGING 
						send_response(new_message_ID, 0x00);
						break;
					case LIGHT_ID:
						tc45_disable(&TCC4);
						
						old_core_status = core_status;
						core_status = STATUS_LIGHT;
						
						adc_flush(&ADCA);
						change_adc_channel(core_status);
						adc_start_conversion(&ADCA, ADC_CH0);
				
						while(flag_light == false);
						flag_light = false;
					
						send_light_data();
						
						core_status = old_core_status;
						change_adc_channel(core_status);						
						if ((core_status == STATUS_FFT_L) ||(core_status == STATUS_FFT_R))
						{
							tc45_enable(&TCC4);
						}						
						
						send_response(new_message_ID, 0x01);
						break;
					default:
						break;
				}
			} 
			else
			{
				// if the fht led function was turned on 
				if (fht_flag == true)
				{
					if (flag_fft == true)
					{
						if (core_status == STATUS_FFT_L)
						{
							//	usart_putchar(M_USART, 0x55);

							run();
							core_status = STATUS_FFT_R;
							change_adc_channel(core_status);
							tc45_enable(&TCC4);
						}
						else if (core_status == STATUS_FFT_R)
						{
							//	usart_putchar(M_USART, 0x66);

							run();
							if (ischarging())
							{
								core_status = STATUS_CHARGE;
								change_adc_channel(core_status);
								tc45_enable(&TCC4);
								tc45_enable(&TCC5);
							}
							else
							{
								core_status = STATUS_FFT_L;
								change_adc_channel(core_status);
								tc45_enable(&TCC4);
							}
							
						}
						flag_fft = false;
					}
				}
				
			}		
			
		}*/		
	}
}
