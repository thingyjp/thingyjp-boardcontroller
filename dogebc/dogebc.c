#include <libopencm3/stm32/rcc.h>
#include <libopencmsis/core_cm3.h>

#include <stdio.h>

#include "time.h"
#include "watchdog.h"
#include "adc.h"
#include "console.h"
#include "pwrctrl.h"
#include "host.h"

static void clocks_init(void) {
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOF);
}

int main(void) {
	clocks_init();
	time_init();
	watchdog_init();
	adc_init();
	pwrctrl_init();
	host_init();
	console_init();

	printf("dogebc starting....\n");

	while (true) {
		adc_poll();
		pwrctrl_poll();
		__WFI();
	}

	return 0;
}
