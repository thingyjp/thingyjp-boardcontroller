#pragma once

void adc_init(void);
void adc_poll(void);
int32_t adc_temp(void);
uint16_t adc_getlastresult(unsigned channel);
uint32_t adc_tovoltage(uint32_t adcreading, uint32_t multiplier);
