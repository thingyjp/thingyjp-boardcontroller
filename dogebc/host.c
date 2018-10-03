#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>

#include "host.h"

#define SCL_PIN		GPIO9
#define SDA_PIN		GPIO10
#define INT_PORT	GPIOA
#define INT_PIN		GPIO13

#define V_USR1		GPIO1
#define V_USR2		GPIO3
#define V_USR3		GPIO7

void host_init() {
	// i2c setup
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, SCL_PIN | SDA_PIN);
	gpio_set_af(GPIOA, GPIO_AF4, SCL_PIN | SDA_PIN);
	rcc_periph_clock_enable(RCC_I2C1);
	// int setup
	gpio_mode_setup(INT_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, INT_PIN);
	// user adc setup
	// enable our adcs
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,
	V_USR1 | V_USR2 | V_USR3);
}
