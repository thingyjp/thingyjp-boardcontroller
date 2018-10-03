#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>

#include "time.h"

static uint32_t time = 0;

void sys_tick_handler(void) {
	time += 20;
}

void time_init() {
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_set_reload(8000000 / 50);
	systick_interrupt_enable();
	systick_counter_enable();
}

uint32_t time_get() {
	return time;
}
