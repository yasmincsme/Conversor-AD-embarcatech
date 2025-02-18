#include "button.h"

void buttons_init() {
	gpio_init(BUTTON_A);
	gpio_set_dir(BUTTON_A, GPIO_IN);
	gpio_pull_up(BUTTON_A);

	gpio_init(BUTTON_B);
	gpio_set_dir(BUTTON_B, GPIO_IN);
	gpio_pull_up(BUTTON_B);

	gpio_init(JOYSTICK_PB);
	gpio_set_dir(JOYSTICK_PB, GPIO_IN);
	gpio_pull_up(JOYSTICK_PB);
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