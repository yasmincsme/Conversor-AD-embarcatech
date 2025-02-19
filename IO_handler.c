#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "inc/ssd1306.h"
#include "IO_handler.h"

static volatile uint32_t last_press_A = 0;
static volatile uint32_t last_press_B = 0;
static volatile uint32_t last_press_PB = 0;

volatile uint8_t trigger = 0; 

void joystick_init() {
	adc_init();
	adc_gpio_init(JOYSTICK_X);
	adc_gpio_init(JOYSTICK_Y);

	gpio_init(JOYSTICK_PB);
	gpio_set_dir(JOYSTICK_PB, GPIO_IN);
	gpio_pull_up(JOYSTICK_PB);
}

void joystick_read(uint16_t* adc_value_x, uint16_t* adc_value_y) {
	adc_select_input(1); // Seleciona o canal do eixo Y (GPIO 27)
    *adc_value_x = adc_read(); // Lê o valor do eixo Y
    adc_select_input(0); // Seleciona o canal do eixo X (GPIO 26)
    *adc_value_y = adc_read(); // Lê o valor do eixo X
}

void buttons_init() {
	gpio_init(BUTTON_A);
	gpio_set_dir(BUTTON_A, GPIO_IN);
	gpio_pull_up(BUTTON_A);

	gpio_init(BUTTON_B);
	gpio_set_dir(BUTTON_B, GPIO_IN);
	gpio_pull_up(BUTTON_B);
}

void leds_init() {
	gpio_init(LED_GREEN);
	gpio_set_dir(LED_GREEN, GPIO_OUT);
	gpio_put(LED_GREEN, 0);

    gpio_set_function(LED_RED, GPIO_FUNC_PWM);
    gpio_set_function(LED_BLUE, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(LED_RED);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 255); 
    pwm_init(slice_num, &config, true);
}

void leds_set(uint16_t adc_value, uint8_t led, uint8_t trigger) {
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

void display_set(uint16_t adc_value_x, uint16_t adc_value_y, uint16_t* value_x, uint16_t* value_y) {
    const uint16_t MIN_X = 5, MAX_X = 115; // Borda de 5 pixels no X
    const uint16_t MIN_Y = 5, MAX_Y = 51;  // Borda de 5 pixels no Y

    // Mapeia os valores do joystick para a área útil do display
    *value_x = MIN_X + ((adc_value_x * (MAX_X - MIN_X)) / 4095);
    *value_y = MAX_Y - ((adc_value_y * (MAX_Y - MIN_Y)) / 4095); // Inverte o eixo Y

    // Garante que os valores estejam dentro dos limites
    if (*value_x < MIN_X) *value_x = MIN_X;
    if (*value_x > MAX_X) *value_x = MAX_X;
    if (*value_y < MIN_Y) *value_y = MIN_Y;
    if (*value_y > MAX_Y) *value_y = MAX_Y;
}

void I2C_init() {
	i2c_init(I2C_PORT, 400 * 1000);
	gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); 
	gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); 
	gpio_pull_up(I2C_SDA); 
	gpio_pull_up(I2C_SCL); 
}

void SSD1306_init(ssd1306_t* ssd) {
	ssd1306_init(ssd, WIDTH, HEIGHT, false, ADDRESS, I2C_PORT);
	ssd1306_config(ssd);
	ssd1306_send_data(ssd); 
	ssd1306_fill(ssd, false);
  	ssd1306_send_data(ssd);	
}

void gpio_irq_handler(uint gpio, uint32_t events) {
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
