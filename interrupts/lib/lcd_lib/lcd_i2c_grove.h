
/**
 * ESP-IDF Framework Library for "Grove-LCD RGB Backlight V4.0"
 * 
 * This library was adapted from diferent source codes: 
 * https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight
 * https://controllerstech.com/i2c-in-esp32-esp-idf-lcd-1602/ y 
 * https://controllerstech.com/i2c-lcd-in-stm32/
 * 
 * This software is written by ldiazcharris(lddiazcharris@gmail.com) and is licensed 
 * under The MIT License. Check License.txt for more information.
 *  
 * 
*/


#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_LCD_PORT    I2C_NUM_0

// para la pantalla Grove 0x3E - Para la LCD Generica es 0x27
#define I2C_LCD_SLAVE_ADDRESS         0x3E 
#define I2C_LCD_SLAVE_RGB_ADDRESS     0x62
#define REG_MODE1       0x00
#define REG_MODE2       0x01
#define REG_OUTPUT      0x08

#define REG_RED         0x04
#define REG_GREEN       0x03
#define REG_BLUE        0x02


/**
 * @brief Esta función incializa las funcionalidades I2C y la configuración de escritura de la pantalla
 */
void lcd_init();

void lcd_write_char(char character);
void lcd_cursor(uint8_t row, uint8_t col);
void lcd_write_string(char *str);

/**
 * @brief Esta función escribe enla pantalla LCD el texto deseado.
 * 
 * @param row: Columna desde la cual se inica la escritura
 * @param column: Fila sobre la cual se desea escribir
 * @param str: Texto que se desea escribir
 */
void lcd_write(uint8_t row, uint8_t column, char *str);

/**
 * @brief Esta función limpia o borra todos los caracteres de la pantalla LCD.
 */
void lcd_clear();

/**
 * @brief Esta función cambia el color de fondo de la pantalla LCD según el modelo de color RGB.
 * 
 * @param r: valor para cantidad de color rojo.
 * @param g: valor para cantidad de color verde.
 * @param b: valor para cantidad de color azul.
 */
void lcd_set_RGB(unsigned char r, unsigned char g, unsigned char b);

