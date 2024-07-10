

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

#include "lcd_i2c_grove.h"
#include "unistd.h"
static const char* TAG = "LCD_I2C";

static esp_err_t i2c_chanell_init();
static void setReg(unsigned char reg_addr, unsigned char data);
static void lcd_command(char cmd);
static void lcd_write_data(uint8_t device_address, uint8_t reg_addr, char data);


static void lcd_write_data(uint8_t device_address, uint8_t reg_addr, char data)
{
    esp_err_t err;
    uint8_t data_t[2];
    data_t[0] = reg_addr;
    data_t[1] = data;
    err= i2c_master_write_to_device(I2C_LCD_PORT, device_address, data_t, 2, 1000);
	if (err != 0) ESP_LOGI(TAG, "Error no. %d in command", err);
}

static void lcd_command(char cmd)
{
    lcd_write_data(I2C_LCD_SLAVE_ADDRESS, 0x80, cmd);
}

void lcd_write_char(char character)
{
   lcd_write_data(I2C_LCD_SLAVE_ADDRESS, 0x40, character);
}

static void setReg(unsigned char reg_addr, unsigned char data)
{
    lcd_write_data(I2C_LCD_SLAVE_RGB_ADDRESS, reg_addr, data);
}


void lcd_init()
{

	// The I2C channel for the LCD is initialized. 
	i2c_chanell_init();

	// 4 bits initialization
	usleep(50000);  // wait for >40ms
    lcd_command (0x30);
    usleep(5000);  // wait for >4.1ms
    lcd_command (0x30);
    usleep(200);  // wait for >100us
    lcd_command (0x30);
    usleep(10000);
    lcd_command (0x20);  // 4bit mode
    usleep(10000);

  // display initialisation
	lcd_command (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	usleep(1000);
	lcd_command (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	usleep(1000);
	lcd_command (0x01);  // clear display
	usleep(1000);
	usleep(1000);
	lcd_command (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	usleep(1000);
	lcd_command (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
	usleep(2000);

  //RGB Display initialization:

    // backlight init
    setReg(REG_MODE1, 0);
    // set LEDs controllable by both PWM and GRPPWM registers
    setReg(REG_OUTPUT, 0xFF);
    // set MODE2 values
    // 0010 0000 -> 0x20  (DMBLNK to 1, ie blinky mode)
    setReg(REG_MODE2, 0x20);

    lcd_set_RGB(255,255,255);
    
}

void lcd_set_RGB(unsigned char r, unsigned char g, unsigned char b)
{
    setReg(REG_RED, r);
    setReg(REG_GREEN, g);
    setReg(REG_BLUE, b);
}

void lcd_cursor(uint8_t row, uint8_t column) {
    uint8_t address;
    switch (row) {
        case 0:
            address = 0x00;
            break;
        case 1:
            address = 0x40;
            break;
        default:
            address = 0x00;
    }
    address += column;
    lcd_command(0x80 | address);
}


void lcd_write_string(char *str)
{
	while(*str) 
		lcd_write_char(*str++);
}

void lcd_write(uint8_t row, uint8_t column, char *str)
{
    lcd_cursor(row, column);
	while(*str) 
		lcd_write_char(*str++);
}

void lcd_clear()
{
	lcd_command(0x01);
	usleep(5000);
}

static esp_err_t i2c_chanell_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_param_config(I2C_NUM_0, &conf);

    return i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
}

