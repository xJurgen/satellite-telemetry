/* This project is based on libopencm3 STM32F4 usart_irq example */
/* Modified by: Jiří Veverka (xvever12@vutbr.cz) */

#include <string.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#include "adc_light.h"

#define IRQ_PRI_USBUSART        (1 << 4)

static void clock_setup(void)
{
	// Enable GPIOC clock for LED & USARTs.
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOA);

	// Enable clocks for USART1.
	rcc_periph_clock_enable(RCC_USART1);
}

static void usart_setup(void)
{
	// Setup GPIO pins for USART1 transmit.
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);

	// Setup GPIO pins for USART1 receive.
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO10);

	// Setup USART1 TX and RX pin as alternate function.
	gpio_set_af(GPIOA, GPIO_AF7, GPIO9);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO10);

	// Setup USART1 parameters.
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	// Enable USART1 Receive interrupt.
	usart_enable_rx_interrupt(USART1);

	// Enable the USART.
	usart_enable(USART1);

    // Enable interrupt
    //USART1_CR1 |= USART_CR1_RXNEIE;
    //nvic_set_priority(NVIC_USART1_IRQ, IRQ_PRI_USBUSART);
    nvic_enable_irq(NVIC_USART1_IRQ);
}

static void gpio_setup(void)
{
	// Setup GPIO pin GPIO12 on GPIO port C for LED.
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
}

int main(void)
{
	clock_setup();
	gpio_setup();
	usart_setup();

	while (1) {
		//gpio_toggle(GPIOC, GPIO13);     /* LED on/off */
        //for (int i = 0; i < 1000000; i++) { /* Wait a bit. */
            __asm__("nop");
        //}
	}

    //Should not ever get there...
	return 0;
}

#define MAX_BUFFER_SIZE		25
static uint8_t recv_buffer[MAX_BUFFER_SIZE];
static uint8_t command = false;
static uint8_t delete = false;
static uint8_t i = 0;

static char *version = "1.0";

void clear_buffer() {
	for (int j = 0; j < MAX_BUFFER_SIZE; j++) {
		recv_buffer[j] = 0;
	}
}

void send_message(char *message, size_t size) {
	for (size_t j = 0; j < size; j++) {
		usart_send_blocking(USART1, message[j]);
	}
	usart_send_blocking(USART1, '\r');
	usart_send_blocking(USART1, '\n');
}

void parse_command() {
	static char *message;

	if (strcmp((char *)recv_buffer, "print buffer") == 0) { //TEST FUNCTION
		message = "RESPONSE!";
		send_message(message, strlen(message));
	} else if (strcmp((char *)recv_buffer, "get version") == 0) {
		message = version;
		send_message(message, strlen(message));
	}
	clear_buffer();
}

void usart1_isr(void)
{
	static uint8_t data = 'A';

	// Check if we were called because of RXNE.
	if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) &&
	    ((USART_SR(USART1) & USART_SR_RXNE) != 0)) {

		// Indicate that we got data.
		gpio_toggle(GPIOC, GPIO13);

		// Retrieve the data from the peripheral.
		data = usart_recv(USART1);

		if ((int)((char)data) == 13 || (int)((char)data) == 10) { //Enter
			command = true;
		} else if ((int)((char)data) == 127) { //Delete last char
			recv_buffer[i] = 0;
			i--;
			delete = true;
		} else if (i < MAX_BUFFER_SIZE) { //If we don't want to execute the command or delete the entry, add the char to the buffer...
			recv_buffer[i] = data;
			i++;
		}
		// Enable transmit interrupt so it sends back the data.
		usart_enable_tx_interrupt(USART1);
	}

	// Check if we were called because of TXE.
	if (((USART_CR1(USART1) & USART_CR1_TXEIE) != 0) &&
	    ((USART_SR(USART1) & USART_SR_TXE) != 0)) {

		// Put data into the transmit register.
		if (command) {
			usart_send_blocking(USART1, '\r');
			usart_send_blocking(USART1, '\n');
			i = 0;
			command = false;
			parse_command();
		} else if (delete) {
			usart_send_blocking(USART1, '\b');
			usart_send_blocking(USART1, ' ');
			usart_send_blocking(USART1, '\b');
			delete = false;
		} else if (i < MAX_BUFFER_SIZE) {
			usart_send(USART1, data);
		}

		// Disable the TXE interrupt as we don't need it anymore.
		usart_disable_tx_interrupt(USART1);
	}
}


