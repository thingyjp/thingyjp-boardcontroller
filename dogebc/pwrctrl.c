#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#include "pwrctrl.h"
#include "adc.h"
#include "time.h"

#define PWRCTRL_PORT	GPIOF
#define RSTOUT_PIN		GPIO0
#define PWRSEQ1_PIN		GPIO1

#define V_SUPPLY		GPIO4	// 4-5v
#define V_IO			GPIO5	// 3.3v
#define V_DDR			GPIO6	// 1.8v
#define V_CORE			GPIO7	// 1.2v

#define V_SUPPLY_BROWNOUT	3500000
#define V_SUPPLY_START_MIN	4000000
#define V_CORE_MIN			600000

#define RSTMS 250

static uint32_t resetstarted;

enum state {
	WAITINGFORVSUPPLY,
	WAITINGFORVIO,
	WAITINGFORVDDR,
	WAITINGFORVCORE,
	RSTDELAY,
	ONLINE
};

static enum state state = WAITINGFORVSUPPLY;

void pwrctrl_init() {
	// reset and power seq setup
	gpio_mode_setup(PWRCTRL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
	RSTOUT_PIN | PWRSEQ1_PIN);

	// enable our adcs
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,
	V_SUPPLY | V_CORE | V_IO | V_DDR);

	gpio_clear(PWRCTRL_PORT, RSTOUT_PIN);
	gpio_clear(PWRCTRL_PORT, PWRSEQ1_PIN);
}

static void pwrctrl_healthcheck(uint32_t vsupply, uint32_t vio, uint32_t vddr,
		uint32_t vcore) {
	if (vsupply <= V_SUPPLY_BROWNOUT)
		printf("v_supply too low,..\n");
	if (vio <= V_CORE)
		printf("v_io too low,..\n");
	if (vddr <= V_CORE)
		printf("v_ddr too low,..\n");
	if (vcore <= V_CORE)
		printf("v_core too low,..\n");
}

static void pwrctrl_changestate(enum state newstate) {
	switch (newstate) {
	case RSTDELAY:
		resetstarted = time_get();
		break;
	default:
		break;
	}
	state = newstate;
}

void pwrctrl_poll() {
	uint32_t vsupply = adc_tovoltage(adc_getlastresult(4), 2);
	uint32_t vio = adc_tovoltage(adc_getlastresult(5), 1);
	uint32_t vddr = adc_tovoltage(adc_getlastresult(6), 1);
	uint32_t vcore = adc_tovoltage(adc_getlastresult(7), 1);
#ifdef DEBUG
	printf("%"PRIu32": rails: v_supply, %"PRIu32", v_io %"PRIu32", v_ddr %"PRIu32", v_core %"PRIu32"\n",
			time_get(),vsupply, vio, vddr, vcore);
#endif

	switch (state) {
	case WAITINGFORVSUPPLY:
		if (vsupply >= V_SUPPLY_START_MIN) {
			printf("v_supply good\n");
			gpio_set(PWRCTRL_PORT, PWRSEQ1_PIN);
			pwrctrl_changestate(WAITINGFORVIO);
		} else
			printf("waiting for v_supply\n");
		break;
	case WAITINGFORVIO:
		if (vcore >= V_CORE_MIN) {
			printf("v_io good\n");
			pwrctrl_changestate(WAITINGFORVDDR);
		} else
			printf("waiting for v_io\n");
		break;
	case WAITINGFORVDDR:
		if (vcore >= V_CORE_MIN) {
			printf("v_ddr good\n");
			pwrctrl_changestate(WAITINGFORVCORE);
		} else
			printf("waiting for v_ddr\n");
		break;
	case WAITINGFORVCORE:
		if (vcore >= V_CORE_MIN) {
			printf("v_core good\n");
			pwrctrl_changestate(RSTDELAY);
		} else
			printf("waiting for v_core\n");
		break;
	case RSTDELAY: {
		uint32_t time = time_get();
		if (time - resetstarted >= RSTMS) {
			printf("releasing reset\n");
			gpio_set(PWRCTRL_PORT, RSTOUT_PIN);
			pwrctrl_changestate(ONLINE);
		} else
			printf("waiting for reset delay to expire\n");
	}
		break;
	case ONLINE:
		pwrctrl_healthcheck(vsupply, vio, vddr, vcore);
		break;
	}
}
