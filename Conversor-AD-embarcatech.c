#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <pico/multicore.h>
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "IO_handler.h"

ssd1306_t ssd;
bool cor = true;

static uint16_t value_x = 60; 
static uint16_t value_y = 25; 


void display_task() {
	while (true) {
		ssd1306_fill(&ssd, !cor); //Limpa o display
		ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); //Desenha a borda

		ssd1306_draw_char(&ssd, '|', value_x, value_y);

		ssd1306_send_data(&ssd); //Atualiza o display
		sleep_ms(1000);
	}
	
}

int main() {
	stdio_init_all();
	joystick_init();
	buttons_init();
	leds_init();
	I2C_init();
	SSD1306_init(&ssd);

	gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
	gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
	gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

	multicore_launch_core1(display_task); 

	uint16_t adc_value_x;
	uint16_t adc_value_y; 

	char str_x[5];  
	char str_y[5];  

	while (true) {
		joystick_read(&adc_value_x, &adc_value_y);

		display_set(adc_value_x, adc_value_y, &value_x, &value_y);

		sprintf(str_x, "%d", adc_value_x); 
		sprintf(str_y, "%d", adc_value_y);  

		leds_set(adc_value_x, LED_RED, trigger);
		leds_set(adc_value_y, LED_BLUE, trigger);

		printf("x: %d\n", adc_value_x);
		printf("y: %d\n", adc_value_y);

		sleep_ms(500);
	}
}