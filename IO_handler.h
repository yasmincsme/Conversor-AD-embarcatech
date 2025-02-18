#ifndef IO_HANDLER_H

#define BUTTON_A 5 
#define BUTTON_B 6

#define JOYSTICK_PB 22 
#define JOYSTICK_X 26  
#define JOYSTICK_Y 27 

#define PADDING 3

#define CENTER_JOYSTICK 2048
#define MAX_JOYSTICK 4095
#define MIN_JOYSTICK 0

#define DEBOUNCE_TIME_US 200000

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ADDRESS 0x3C

#define LED_RED 13  
#define LED_BLUE 12 
#define LED_GREEN 11 

extern volatile uint8_t trigger;

void joystick_init();
void joystick_read(uint16_t* adc_value_x, uint16_t* adc_value_y);
void buttons_init();
void leds_init();
void leds_set(uint16_t adc_value, uint8_t led, uint8_t trigger);
void display_set(uint16_t adc_value_x, uint16_t adc_value_y, uint16_t* value_x, uint16_t* value_y);
void I2C_init();
void SSD1306_init(ssd1306_t* ssd);
void gpio_irq_handler(uint gpio, uint32_t events);

#endif // IO_HANDLER_H