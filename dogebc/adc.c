#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>

#include <stdio.h>
#include <math.h>
#include <inttypes.h>

#include "adc.h"
#include "utils.h"

#define TEMP110_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7C2))
#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7B8))
#define VDD_CALIB ((uint16_t) (330))
#define VDD_APPLI ((uint16_t) (330))

static uint8_t channel_array[] = {
// user channels
		1,
// our channels
		4, 5, 6, 7,
// for fun
		ADC_CHANNEL_TEMP,
		ADC_CHANNEL_VREF };
static uint16_t results[N_ELEMENTS(channel_array)];

int32_t adc_temp(void) {
	uint16_t cal30 = *TEMP30_CAL_ADDR;
	uint16_t cal110 = *TEMP110_CAL_ADDR;
	int32_t temperature = (((int32_t) adc_getlastresult(ADC_CHANNEL_TEMP)
			* VDD_APPLI / VDD_CALIB) - cal30);
	temperature = temperature * (int32_t) (110 - 30);
	temperature = temperature / (int32_t) (cal110 - cal30);
	temperature = temperature + 30;
	return temperature;
}

uint32_t adc_tovoltage(uint32_t adcreading, uint32_t multiplier) {
	return ((3300000 / (pow(2, 12) - 1)) * adcreading) * multiplier;
}

void adc_init() {
	rcc_periph_clock_enable(RCC_ADC);

	adc_power_off(ADC1);
	adc_set_clk_source(ADC1, ADC_CLKSOURCE_ADC);
	adc_calibrate(ADC1);
	adc_set_operation_mode(ADC1, ADC_MODE_SCAN);
	adc_disable_external_trigger_regular(ADC1);
	adc_set_right_aligned(ADC1);
	adc_enable_temperature_sensor();
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_239DOT5);
	adc_set_regular_sequence(ADC1, N_ELEMENTS(channel_array), channel_array);
	adc_set_resolution(ADC1, ADC_RESOLUTION_12BIT);
	adc_disable_analog_watchdog(ADC1);
	adc_power_on(ADC1);

	while (!(ADC_ISR(ADC1) & ADC_ISR_ADRDY))
		__asm__("nop");
}

void adc_poll() {
	adc_start_conversion_regular(ADC1);
// read results out
	for (unsigned i = 0; i < N_ELEMENTS(channel_array); i++) {
		while (!(adc_eoc(ADC1)))
			;
		results[i] = adc_read_regular(ADC1);
	}
	ADC_ISR(ADC1) = ADC_ISR_EOS;
}

uint16_t adc_getlastresult(unsigned channel) {
	for (unsigned i = 0; i < N_ELEMENTS(channel_array); i++) {
		if (channel_array[i] == channel)
			return results[i];
	}

	return 0;
}
