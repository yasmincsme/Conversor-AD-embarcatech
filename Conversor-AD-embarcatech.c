#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <pico/multicore.h>
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

#define JOYSTICK_X 26  
#define JOYSTICK_Y 27 

#define LED_RED 13  
#define LED_BLUE 12 
#define LED_GREEN 11 

#define DEBOUNCE_TIME_US 200000

static volatile uint32_t last_press_A = 0;
static volatile uint32_t last_press_B = 0;
static volatile uint32_t last_press_PB = 0;

static volatile uint8_t trigger = 0;

void display_task() {

}


void leds_init() {
	gpio_init(LED_GREEN);
	gpio_set_dir(LED_GREEN, GPIO_OUT);
	gpio_put(LED_GREEN, 0);

    // Configura os pinos dos LEDs como PWM
    gpio_set_function(LED_RED, GPIO_FUNC_PWM);
    gpio_set_function(LED_BLUE, GPIO_FUNC_PWM);

    // Obtém o slice de PWM
    uint slice_num = pwm_gpio_to_slice_num(LED_RED);

    // Configura o PWM
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 255); 
    pwm_init(slice_num, &config, true);
}

void led_set_value(uint16_t adc_value, uint8_t led, uint8_t trigger) {
    uint slice_num = pwm_gpio_to_slice_num(led); 
    uint channel_num = pwm_gpio_to_channel(led); 

	if(trigger) {
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
	else {
		pwm_set_chan_level(slice_num, channel_num, 0);
	}
}

void ADC_init() {
	adc_init();
	adc_gpio_init(JOYSTICK_X);
	adc_gpio_init(JOYSTICK_Y);

	gpio_init(JOYSTICK_PB);
	gpio_set_dir(JOYSTICK_PB, GPIO_IN);
	gpio_pull_up(JOYSTICK_PB); 
}

void ADC_read(uint16_t* adc_value_x, uint16_t* adc_value_y) {
	adc_select_input(1); // Seleciona o canal do eixo Y (GPIO 27)
    *adc_value_x = adc_read(); // Lê o valor do eixo Y
    adc_select_input(0); // Seleciona o canal do eixo X (GPIO 26)
    *adc_value_y = adc_read(); // Lê o valor do eixo X
}

void I2C_init() {
	i2c_init(I2C_PORT, 400 * 1000);
	gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); 
	gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); 
	gpio_pull_up(I2C_SDA); 
	gpio_pull_up(I2C_SCL); 
}

static void gpio_irq_handler(uint gpio, uint32_t events) {
	uint32_t current_time = to_us_since_boot(get_absolute_time());

	if(gpio == BUTTON_A && (events & GPIO_IRQ_EDGE_FALL)) {
		if(current_time - last_press_A > DEBOUNCE_TIME_US) {
			last_press_A = current_time;
			if(trigger) {
				trigger = 0;
			} else {
				trigger = 1;
			}
		}
	}
	else if(gpio == BUTTON_B && (events & GPIO_IRQ_EDGE_FALL)) {
		if(current_time - last_press_B > DEBOUNCE_TIME_US) {
			last_press_B = current_time;
			reset_usb_boot(0, 0);
		}
	}
	else if(gpio == JOYSTICK_PB && (events & GPIO_IRQ_EDGE_FALL)) {
		if(current_time - last_press_PB > DEBOUNCE_TIME_US) {
			last_press_PB = current_time;
			if(gpio_get(LED_GREEN)) {
				gpio_put(LED_GREEN, 0);
			}
			else {
				gpio_put(LED_GREEN, 1);
			}
		}
	}
}

int main() {

	stdio_init_all();
	buttons_init();
	leds_init(); 
	ADC_init();
	I2C_init();

	gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
	gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
	gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

	multicore_launch_core1(display_task);

	uint16_t adc_value_x;
	uint16_t adc_value_y;  

	char str_x[5];  
	char str_y[5];  

	ssd1306_t ssd;
	bool cor = true;

	while (true) {
		ADC_read(&adc_value_x, &adc_value_y);

		sprintf(str_x, "%d", adc_value_x); 
		sprintf(str_y, "%d", adc_value_y);  

		led_set_value(adc_value_x, LED_RED, trigger);
		led_set_value(adc_value_y, LED_BLUE, trigger);

		printf("x: %d\n", adc_value_x);
		printf("y: %d\n", adc_value_y);

		sleep_ms(100);
	}
}