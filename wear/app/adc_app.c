/*
 * adc.c
 *
 * Created: 2014/8/6 21:34:13
 *  Author: Harifun
 */
#include "asf.h"
#include <config_app.h>
#include <adc_app.h>
#include <ffft.h> 
#include <eeprom.h>
#include <wear.h>
#include <ws2812.h>
#include <FHT.h>
#include <uart.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#ifndef FHT_N // number of samples
#define FHT_N 256
#endif

#define order_low 30
#define order_high 15
#define order_mid 150
uint8_t lowf[order_low] = {[0 ... order_low-1] = 128};
uint8_t midf[order_mid] = {[0 ... order_mid-1] = 128};
uint8_t highf[order_high] = {[0 ... order_high-1] = 128};
int point_low = 0;
int point_mid= 0;
int point_high = 0;

uint8_t ledLevelLow;
uint8_t ledLevelMid;
uint8_t ledLevelHigh;

int lows = 3;
int mids = 18;
int highs = 107;
int gain = 80;
float maxim = 1;
float min = 0;
float vrem;

float low_power = 0, mid_power = 0, high_power = 0;
float low_power_old = 0, mid_power_old = 0, high_power_old = 0;


float damp = 0.99;
float agc = 0.9;

volatile uint16_t fft_num = 0;

int16_t capture[FFT_N];
complex_t bfly_buff[FFT_N];
uint16_t spektrum[FFT_N/2];
bool aaaa = false;
volatile bool flag_fft = false;

//bubble cans
volatile uint16_t bubble_low_l[BUBBLE_NUM];
volatile uint16_t bubble_med_l[BUBBLE_NUM];
volatile uint16_t bubble_high_l[BUBBLE_NUM];

volatile uint16_t bubble_low_r[BUBBLE_NUM];
volatile uint16_t bubble_med_r[BUBBLE_NUM];
volatile uint16_t bubble_high_r[BUBBLE_NUM];

volatile uint8_t mean_low_l;
volatile uint8_t mean_med_l;
volatile uint8_t mean_high_l;

volatile uint8_t mean_low_r;
volatile uint8_t mean_med_r;
volatile uint8_t mean_high_r;

volatile uint8_t bubble_num = 0;

volatile uint8_t LOW_DIVIDER = 100;
volatile uint8_t MID_DIVIDER = 100;
volatile uint8_t HIGH_DIVIDER = 100;

void init_adc(uint8_t adc_status)
{
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;
	
//	adc_disable(&ADCA);
	
	/* Configure the ADC module:
	 * - unsigned, 12-bit results
	 * - voltage reference = VCC / 1.6 = 3.3V / 1.6
	 * - 2 MHz maximum clock rate
	 * - freerun conversion triggering
	 * - 200 khz maximum sampling rate
	 */
	adc_read_configuration(&ADCA, &adc_conf);
	adc_enable_internal_input(&adc_conf, ADC_INT_BANDGAP);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12, ADC_REF_VCC);
	adc_set_clock_rate(&adc_conf, 2000000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_sample_value(&ADCA, 8);
	adc_write_configuration(&ADCA, &adc_conf);
	
	/* Configure ADC channel:
	 * - Single ended mode
	 * - Input voltage on ADC0 pin (PA0 pin)
	 * - 1x gain
	 * - interrupt flag set on completed conversion
	 */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	switch(adc_status)
	{
		case STATUS_CHARGE:
			adcch_disable_averaging(&adcch_conf);
			adcch_set_input(&adcch_conf, ADCCH_POS_CHARGE, ADCCH_NEG_NONE, 1);
			break;
		case STATUS_FFT_L:
			adcch_disable_averaging(&adcch_conf);
			adcch_set_input(&adcch_conf, ADCCH_POS_FFT_L, ADCCH_NEG_FFT_L, 4);
			break;
		case STATUS_FFT_R:
			adcch_disable_averaging(&adcch_conf);
			adcch_set_input(&adcch_conf, ADCCH_POS_FFT_R, ADCCH_NEG_FFT_R, 4);
			break;
		case STATUS_LIGHT_READ:
			adcch_set_input(&adcch_conf, ADCCH_POS_LIGHT, ADCCH_NEG_NONE, 1);
			break;
		case STATUS_BATT_READ:
			adcch_set_input(&adcch_conf, ADCCH_POS_BANDGAP, ADCCH_NEG_NONE, 1);
			adcch_enable_averaging(&adcch_conf, ADC_SAMPNUM_64X);
		default:
			break;
	}	
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_disable_correction(&adcch_conf);
	adcch_disable_interrupt(&adcch_conf);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);
	
	/* Enable ADC */
	adc_enable(&ADCA);
	
	/* Do useful conversion */
	adc_start_conversion(&ADCA, ADC_CH0);
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
}

void change_adc_channel(uint8_t adc_status)
{
	struct adc_channel_config adcch_conf;
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	switch(adc_status)
	{
		case STATUS_CHARGE:
			adcch_disable_averaging(&adcch_conf);
			adcch_set_input(&adcch_conf, ADCCH_POS_CHARGE, ADCCH_NEG_NONE, 1);
			//adcch_enable_averaging(&adcch_conf, ADC_SAMPNUM_4X);
			break;
		case STATUS_FFT_L:
			adcch_disable_averaging(&adcch_conf);
			adcch_set_input(&adcch_conf, ADCCH_POS_FFT_L, ADCCH_NEG_FFT_L, 4);
			break;
		case STATUS_FFT_R:
			adcch_disable_averaging(&adcch_conf);
			adcch_set_input(&adcch_conf, ADCCH_POS_FFT_R, ADCCH_NEG_FFT_R, 4);
			break;
		case STATUS_LIGHT_READ:
			adcch_set_input(&adcch_conf, ADCCH_POS_LIGHT, ADCCH_NEG_NONE, 1);
			break;
		case STATUS_BATT_READ:
			adcch_set_input(&adcch_conf, ADCCH_POS_BANDGAP, ADCCH_NEG_NONE, 1);
			adcch_enable_averaging(&adcch_conf, ADC_SAMPNUM_64X);
		default:
			break;
	}
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);
}

void adc_enable_interrupt(void)
{
	struct adc_channel_config adcch_conf;
	
	adc_disable(&ADCA);
	
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	
	/* Enable PMIC interrupt level low. */
	pmic_enable_level(PMIC_LVL_LOW);
	adc_set_callback(&ADCA, adc_callback);
	adcch_enable_interrupt(&adcch_conf);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);
	
	/* Enable ADC */
	adc_enable(&ADCA);

	/* Enable global interrupts. */
	sei();
}

void adc_callback(ADC_t *adc, uint8_t ch_mask, adc_result_t res)
{
	switch(core_status)
	{
		case STATUS_CHARGE:
			if (ischarging() == false)
			{
				old_core_status = core_status;
				core_status = STATUS_BATT_READ;
				change_adc_channel(core_status);
				tc45_enable(&TCC4);
				tc45_disable(&TCC5);
				load_data_color_led();
			}
	
			break;
		case STATUS_FFT_L:
			fht_input[fft_num++] = res;
			if (fft_num >= (FHT_N))
			{
//				usart_putchar(M_USART, (res & 0xff00 ) >> 8);
//				usart_putchar(M_USART, (res & 0x00ff ) >> 0);
				fft_num = 0;
				flag_fft = true;
				tc45_disable(&TCC4);				
			}
			break;
		case STATUS_FFT_R:
			fht_input[fft_num++] = res;
			if (fft_num >= (FHT_N))
			{
				fft_num = 0;
				flag_fft = true;
				tc45_disable(&TCC4);		
			}
			break;
			
		case STATUS_LIGHT_READ:
			value_light = res;
			flag_light = true;
			core_status = old_core_status;
			change_adc_channel(core_status);
			break;
			
		case STATUS_BATT_READ:
			value_batt = res;
			flag_batt = true;
			if (ischarging())
			{
				twinkle(0, 0, 255);
				old_core_status = core_status;
				core_status = STATUS_CHARGE;
				change_adc_channel(core_status);
				tc45_disable(&TCC4);
				tc45_enable(&TCC5);
			}
			break;
			
		default:
			break;
	}
}

void start_fft(void)
{
	fft_input(capture, bfly_buff);
	fft_execute(bfly_buff);
	fft_output(bfly_buff, spektrum);
}

void send_fft(void)
{
//	usart_putchar(M_USART, (spektrum[EEPROM_LOW_TH_FRE_FFT_L] & 0xff00 ) >> 8);
//	usart_putchar(M_USART, (spektrum[EEPROM_LOW_TH_FRE_FFT_L] & 0x00ff ) >> 0);
}

void update_led_fft_l(void)
{
	M_LED0.r = mean_low_l;
	M_LED0.g = 0;//spektrum[EEPROM_LOW_TH_FRE_FFT_L];
	M_LED0.b = 0;//spektrum[EEPROM_LOW_TH_FRE_FFT_L];
	
	M_LED1.r = 0;//spektrum[EEPROM_MED_TH_FRE_FFT_L];
	M_LED1.g = mean_med_l;
	M_LED1.b = 0;//spektrum[EEPROM_MED_TH_FRE_FFT_L];
	
	M_LED2.r = 0;//spektrum[EEPROM_HIGH_TH_FRE_FFT_L];
	M_LED2.g = 0;//spektrum[EEPROM_HIGH_TH_FRE_FFT_L];
	M_LED2.b = mean_high_l;

	set_flash_ws2812(m_led_struct, 6);
}

void update_led_fft_r(void)
{
	M_LED3.r = mean_low_r;
//	M_LED3.g = spektrum[EEPROM_LOW_TH_FRE_FFT_R];
//	M_LED3.b = spektrum[EEPROM_LOW_TH_FRE_FFT_R];
	
//	M_LED4.r = spektrum[EEPROM_MED_TH_FRE_FFT_R];
	M_LED4.g = mean_med_r;
//	M_LED4.b = spektrum[EEPROM_MED_TH_FRE_FFT_R];
	
//	M_LED5.r = spektrum[EEPROM_HIGH_TH_FRE_FFT_R];
//	M_LED5.g = spektrum[EEPROM_HIGH_TH_FRE_FFT_R];
	M_LED5.b = mean_high_r;

	set_flash_ws2812(m_led_struct, 6);
}

void get_fft_l(void)
{
//	bubble_low_l[bubble_num] = spektrum[EEPROM_LOW_TH_FRE_FFT_L];
//	bubble_med_l[bubble_num] = spektrum[EEPROM_MED_TH_FRE_FFT_L];
//	bubble_high_l[bubble_num] = spektrum[EEPROM_HIGH_TH_FRE_FFT_L];
}

void get_fft_r(void)
{
//	bubble_low_r[bubble_num] = spektrum[EEPROM_LOW_TH_FRE_FFT_R];
//	bubble_med_r[bubble_num] = spektrum[EEPROM_MED_TH_FRE_FFT_R];
//	bubble_high_r[bubble_num] = spektrum[EEPROM_HIGH_TH_FRE_FFT_R];
}

uint8_t start_bubble(uint16_t * can)
{
	uint16_t temp;
	
	for (uint8_t i=0; i<BUBBLE_NUM; i++)
	{
		for (uint8_t j=0; j<BUBBLE_NUM; j++)
		{
			if (can[j] > can[j+1])
			{
				temp = can[j];
				can[j] = can[j+1];
				can[j+1] = temp;
			}
		}
	}
	
	for (uint8_t k = 0; k<(BUBBLE_NUM-2); k++)
	{
		temp += can[k+1];
	}
	
	temp = temp/(BUBBLE_NUM-2);
	
	if (temp > 255)
	{
		return 255;
	} 
	else
	{
		return temp&0x00FF;
	}
}

void bubble(void)
{
	mean_low_l = start_bubble(bubble_low_l);
	mean_med_l = start_bubble(bubble_med_l);
	mean_high_l = start_bubble(bubble_high_l);
	
	mean_low_r = start_bubble(bubble_low_r);
	mean_med_r = start_bubble(bubble_med_r);
	mean_high_r = start_bubble(bubble_high_r);
}



uint8_t mean(uint8_t* a, uint8_t order) 
{
	uint16_t m = 0;
	for (int i=0; i<order; i++) 
	{
		m += a[i];
	}
	return m/order;
}

static inline uint8_t getSmallestColorIndex(void) {
	uint8_t a = colors[0];
	uint8_t b = colors[1];
	uint8_t c = colors[2];
	
	uint8_t smallest = a;
	
	if (smallest < b) {
		smallest = b;
	}
	
	if (smallest < c) {
		smallest = c;
	}
	
	if (smallest == a) {
		return 1;
	}
	else if (smallest == b) {
		return 2;
	}
	else {
		return 3;
	}
}

void updateLEDs()
{

//	bool playing = isplaying();
//	bool charging = ischarging();

		// get and bin the power of frequencies to colors
		low_power_old = low_power;
		mid_power_old = mid_power;
		high_power_old = high_power;

		low_power = 0;
		mid_power = 0;
		high_power = 0;

		for (int i = 0; i < lows; i++){
			low_power += fht_lin_out[i];
		}
		for (int i = lows; i < lows+mids; i++){
			mid_power += fht_lin_out[i];
		}
		for (int i = lows+mids; i < lows+mids+highs; i++){
			high_power += fht_lin_out[i];
		}
		// normalize for bass, mids and highs

		// Decaying - dont change the value too fast 
		/*if (low_power < low_power_old*damp){
			low_power = low_power_old*damp;
		}
		if (mid_power < mid_power_old*damp){
			mid_power = mid_power_old*damp;
		}
		if (high_power < high_power_old*damp){
			high_power = high_power_old*damp;
		}*/

		// AGC - automatic gain control
		if ( low_power > maxim || mid_power > maxim || high_power > maxim)  {
	    	vrem = max(low_power, mid_power);
	    	vrem = max(vrem, high_power);
	    	maxim = vrem;
	  	} 
	  	else {
	    	maxim *= agc;
	  	}
		// map values to agc adjusted
		float slope = 255/(maxim-min);
		
	  	ledLevelLow = (uint8_t)(low_power*slope);
	  	ledLevelMid = (uint8_t)(mid_power*slope);
	  	ledLevelHigh = (uint8_t)(high_power*slope);
		
     lowf[point_low] = ledLevelLow;
	 midf[point_mid] = ledLevelMid;
	 highf[point_high] = ledLevelHigh;
	 point_low++;
	 point_mid++;
	 point_high++;
	 point_low %= order_low;
	 point_mid %= order_mid;
	 point_high %= order_high;
	 
	ledLevelHigh = mean(highf, order_high);
	ledLevelMid = mean(midf, order_mid);
	ledLevelLow = mean(lowf, order_low);
		 
	ledLevelLow = 100*ledLevelLow/LOW_DIVIDER;
	ledLevelMid = 100*ledLevelMid/MID_DIVIDER;
	ledLevelHigh = 100*ledLevelHigh/HIGH_DIVIDER;
	
	if (ledLevelHigh > 255){
		ledLevelHigh = 255;
	}
	if (ledLevelMid > 255){
		ledLevelMid = 255;
	}
	if (ledLevelLow > 255){
		ledLevelLow = 255;
	}
	
	if (colors[0] <= colors[1] && colors[0] <= colors[2]) {
		twinkle(0, ledLevelHigh, ledLevelLow);
	}
	else if (colors[1] <= colors[0] && colors[1] <= colors[2]) {
		twinkle(ledLevelHigh, 0, ledLevelLow);
	}
	else if (colors[2] <= colors[0] && colors[2] <= colors[1]) {
		twinkle(ledLevelHigh, ledLevelLow, 0);
	}
	else {
		twinkle(0, ledLevelHigh, ledLevelLow);
	}
	
	// update LEDs

}

void run(){

	fht_window(); // window the data for better frequency response
	fht_reorder(); // reorder the data before doing the fht
	fht_run(); // process the data in the fht
	fht_mag_lin(); // take the output of the fht
	
	updateLEDs();
	flag_fft = false;
}