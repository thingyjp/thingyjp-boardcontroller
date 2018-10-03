#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "console.h"

#define USART_CONSOLE	USART1
#define USART_TX_PIN	GPIO2
#define USART_TX_AF		GPIO_AF1

int _write(int file, char *ptr, int len);

int _write(int file, char *ptr, int len) {
	int i;

	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		for (i = 0; i < len; i++) {
			if (ptr[i] == '\n') {
				usart_send_blocking(USART_CONSOLE, '\r');
			}
			usart_send_blocking(USART_CONSOLE, ptr[i]);
		}
		return i;
	}
	errno = EIO;
	return -1;
}

void console_init() {
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, USART_TX_PIN);
	gpio_set_af(GPIOA, USART_TX_AF, USART_TX_PIN);

	rcc_periph_clock_enable(RCC_USART1);
	usart_set_baudrate(USART_CONSOLE, 115200);
	usart_set_databits(USART_CONSOLE, 8);
	usart_set_stopbits(USART_CONSOLE, USART_STOPBITS_1);
	usart_set_mode(USART_CONSOLE, USART_MODE_TX);
	usart_set_parity(USART_CONSOLE, USART_PARITY_NONE);
	usart_set_flow_control(USART_CONSOLE, USART_FLOWCONTROL_NONE);
	usart_enable(USART_CONSOLE);
}
