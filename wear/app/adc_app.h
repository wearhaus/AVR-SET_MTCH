/*
 * adc.h
 *
 * Created: 2014/8/6 21:34:22
 *  Author: Harifun
 */ 


#ifndef ADC_APP_H_
#define ADC_APP_H_

void adc_callback(ADC_t *adc, uint8_t ch_mask, adc_result_t res);
void init_adc(uint8_t adc_status);
void adc_enable_interrupt(void);
void start_fft(void);
void send_fft(void);
void update_led_fft_l(void);
void update_led_fft_r(void);
void start_adc(uint8_t adc_status);
void change_adc_channel(uint8_t adc_status);
void get_fft_l(void);
void get_fft_r(void);
void bubble(void);

//extern uint16_t fft_num;
extern volatile bool flag_fft;

//bubble cans
extern volatile uint16_t bubble_low_l[BUBBLE_NUM];
extern volatile uint16_t bubble_med_l[BUBBLE_NUM];
extern volatile uint16_t bubble_high_l[BUBBLE_NUM];

extern volatile uint16_t bubble_low_r[BUBBLE_NUM];
extern volatile uint16_t bubble_med_r[BUBBLE_NUM];
extern volatile uint16_t bubble_high_r[BUBBLE_NUM];

extern volatile uint8_t mean_low_l;
extern volatile uint8_t mean_med_l;
extern volatile uint8_t mean_high_l;

extern volatile uint8_t mean_low_r;
extern volatile uint8_t mean_med_r;
extern volatile uint8_t mean_high_r;

extern volatile uint8_t bubble_num;

extern volatile uint8_t LOW_DIVIDER;
extern volatile uint8_t MID_DIVIDER;
extern volatile uint8_t HIGH_DIVIDER;

void updateLEDs(void);
void run(void);
uint8_t mean(uint8_t* a, uint8_t order);

#endif /* ADC_H_ */