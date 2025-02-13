#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "pico/bootrom.h"
#include "hardware/pwm.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define JOYSTICK_X_PIN 26  
#define JOYSTICK_Y_PIN 27 
#define JOYSTICK_PB 22 

#define BUTTON_A 5 
#define BUTTON_B 6

#define LED_RED 13
#define LED_BLUE 12
#define LED_GREEN 11

void gpio_irq_handler(uint gpio, uint32_t events) {
	reset_usb_boot(0, 0);
}

void buttons_init() {
	gpio_init(BUTTON_A);
	gpio_set_dir(BUTTON_A, GPIO_IN);
	gpio_pull_up(BUTTON_A);

	gpio_init(BUTTON_B);
	gpio_set_dir(BUTTON_B, GPIO_IN);
	gpio_pull_up(BUTTON_B);
}

void led_set_value(uint16_t adc_value, uint8_t led) {
    gpio_set_function(led, GPIO_FUNC_PWM); 
    uint slice_num = pwm_gpio_to_slice_num(led); 
    uint channel_num = pwm_gpio_to_channel(led); 

    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 255); 
    pwm_init(slice_num, &config, true); 

    if (adc_value == 2048) {
        pwm_set_chan_level(slice_num, channel_num, 0); // LED apagado
    } else if (adc_value > 2048) {
        uint8_t brightness = (adc_value - 2048) / 8; // Mapeia para 0-255
        pwm_set_chan_level(slice_num, channel_num, brightness);
    } else {
        uint8_t brightness = (2048 - adc_value) / 8; // Mapeia para 0-255
        pwm_set_chan_level(slice_num, channel_num, brightness);
    }
}

void ADC_init() {
	adc_init();
	adc_gpio_init(JOYSTICK_X_PIN);
	adc_gpio_init(JOYSTICK_Y_PIN);

	gpio_init(JOYSTICK_PB);
	gpio_set_dir(JOYSTICK_PB, GPIO_IN);
	gpio_pull_up(JOYSTICK_PB); 
}

void ADC_read(uint16_t* adc_value_x, uint16_t* adc_value_y) {
	adc_select_input(1); 
    *adc_value_x = adc_read(); 
    adc_select_input(0);
    *adc_value_y = adc_read();
}

void I2C_init() {
	i2c_init(I2C_PORT, 400 * 1000);
	gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); 
	gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); 
	gpio_pull_up(I2C_SDA); 
	gpio_pull_up(I2C_SCL); 
}

int main() {
	stdio_init_all();
	buttons_init();
	ADC_init();
	I2C_init();

	gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

	uint16_t adc_value_x;
	uint16_t adc_value_y;  

	char str_x[5];  
	char str_y[5];  

	bool cor = true;

	while (true) {
		ADC_read(&adc_value_x, &adc_value_y);

		sprintf(str_x, "%d", adc_value_x); 
		sprintf(str_y, "%d", adc_value_y);  

		led_set_value(adc_value_x, LED_RED);
		led_set_value(adc_value_y, LED_BLUE);

		printf("x: %d\n", adc_value_x);
		printf("y: %d\n", adc_value_y);

		sleep_ms(100);
	}
}