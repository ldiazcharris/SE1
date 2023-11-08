

/**
 * 
 * Librería adaptada de: https://controllerstech.com/i2c-in-esp32-esp-idf-lcd-1602/ y 
 * https://controllerstech.com/i2c-lcd-in-stm32/
 * Autor: ControllersTech Shop & Learn. 
 * 
*/

#include "lcd_i2c.h"
#include "unistd.h"
//#define portTICK_PERIOD_US              ( ( TickType_t ) 1000000 / configTICK_RATE_HZ )
static const char* TAG = "LIB_LCD_I2C";

static esp_err_t i2c_chanell_init();
//static void usleep(uint32_t value);

void lcd_send_cmd(char cmd)
{
	esp_err_t err;
	char data_h, data_l;
	uint8_t data_t[4];
	data_h = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_h|0x0C;  //en = 1, rs = 0
	data_t[1] = data_h|0x08;  //en = 0, rs = 0
	data_t[2] = data_l|0x0C;  //en = 1, rs  =0
	data_t[3] = data_l|0x08;  //en = 0, rs = 0

	err= i2c_master_write_to_device(I2C_LCD_PORT, I2C_LCD_SLAVE_ADDRESS, data_t, 4, 1000);
	if (err != 0) ESP_LOGI(TAG, "Error no. %d in command", err);
}

void lcd_write_data(char data)
{
	esp_err_t err;
	char data_h, data_l;
	uint8_t data_t[4];
	data_h = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_h|0x0D; // en = 1, rs = 0
	data_t[1] = data_h|0x09; // en = 0, rs = 0
	data_t[2] = data_l|0x0D; // en = 1, rs = 0
	data_t[3] = data_l|0x09; // en = 0, rs = 0
	err = i2c_master_write_to_device(I2C_LCD_PORT, I2C_LCD_SLAVE_ADDRESS, data_t, 4, 1000);
	if (err != 0) ESP_LOGI(TAG, "Error no. %d in command", err);
}

void lcd_init()
{

	// Se inicializa el canal I2C para el LCD. 
	i2c_chanell_init();

	// inicialización de 4 bits
	usleep(50000);  // wait for >40ms
    lcd_send_cmd (0x30);
    usleep(5000);  // wait for >4.1ms
    lcd_send_cmd (0x30);
    usleep(200);  // wait for >100us
    lcd_send_cmd (0x30);
    usleep(10000);
    lcd_send_cmd (0x20);  // 4bit mode
    usleep(10000);

  // dislay initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	usleep(1000);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	usleep(1000);
	lcd_send_cmd (0x01);  // clear display
	usleep(1000);
	usleep(1000);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	usleep(1000);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
	usleep(2000);
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
    lcd_send_cmd(0x80 | address);
}



/*
void lcd_cursor(uint8_t row, uint8_t col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }
    lcd_send_cmd(col);
}


*/
void lcd_write_string(char *str)
{
	while(*str) 
		lcd_write_data(*str++);
}

void lcd_clear()
{
	lcd_send_cmd(0x01);
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

/*
static void usleep(uint32_t value)
{
	vTaskDelay(value/portTICK_PERIOD_US); //portTICK_PERIOD_US
}
*/
