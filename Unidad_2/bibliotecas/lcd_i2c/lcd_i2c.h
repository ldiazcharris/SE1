
/**
 * 
 * Librería adaptada de: https://controllerstech.com/i2c-in-esp32-esp-idf-lcd-1602/ y 
 * https://controllerstech.com/i2c-lcd-in-stm32/, para el control de pantalla LCD con 
 * El módulo I2C de referencia: PCF8574 
 * Autor: ControllersTech Shop & Learn. 
 * 
*/


#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_LCD_PORT I2C_NUM_0
#define I2C_LCD_SLAVE_ADDRESS 0x4E // para la pantalla Grove 0x3E

static const char* TAG = "LIB_LCD_I2C";

void lcd_init ();
void lcd_send_cmd(char cmd);
void lcd_write_data(char data);
void lcd_cursor(uint8_t row, uint8_t col);
void lcd_write_string(char *str);
void lcd_clear();
