#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lcd_i2c.h"  // Librería para el uso del LCD con modulo I2C PCF8574


void app_main()
{
    // Inicialización de la pantalla LCD
    lcd_init(); 
    
    // Se pone la pantalla LCD en blanco
    lcd_clear(); 
   
    while(1){

        // Se pone la pantalla LCD en blanco cada nuevo ciclo
        lcd_clear(); 
        
        // Se coloca el cursor en la posición (0, 0) de la pantalla (fila, columna)
        lcd_cursor(0, 0); 

        // Se escribe el texto en pantalla
        lcd_write_string("Hola Mundo..."); 
        
        vTaskDelay(3000/portTICK_PERIOD_MS); 
        
        // Se pone la pantalla LCD en blanco
        lcd_clear(); 
        
        // Se coloca el cursor en la posición (1, 0) de la pantalla
        lcd_cursor(1, 0); 
        
        // Se escribe el texto en pantalla
        lcd_write_string("...Chao Mundo");  
        
        vTaskDelay(3000/portTICK_PERIOD_MS);
    }
}






















/* 7-Segmentoso excersice

#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char* TAG = "7-segmentos";

// Matriz de números para cátodo común
uint8_t  caracter[10][8] =
{
// a, b, c, d, e, f, g, p
  {1, 1, 1, 1, 1, 1, 0, 0},     // 0 
  {0, 1, 1, 0, 0, 0, 0, 0},     // 1
  {1, 1, 0, 1, 1, 0, 1, 0},     // 2
  {1, 1, 1, 1, 0, 0, 1, 0},     // 3
  {0, 1, 1, 0, 0, 1, 1, 0},     // 4
  {1, 0, 1, 1, 0, 1, 1, 0},     // 5
  {1, 0, 1, 1, 1, 1, 1, 0},     // 6
  {1, 1, 1, 0, 0, 0, 0, 0},     // 7
  {1, 1, 1, 1, 1, 1, 1, 0},     // 8
  {1, 1, 1, 0, 0, 1, 1, 0}      // 9
};

typedef struct {
    uint8_t a; 
    uint8_t b;
    uint8_t c; 
    uint8_t d; 
    uint8_t e; 
    uint8_t f; 
    uint8_t g; 
    uint8_t p;
} seg_7_handler_t;


void seg_7_init(seg_7_handler_t *seg_7_handler, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t p);
void seg_7_write(seg_7_handler_t *seg_t_handler, uint8_t * caracter);

seg_7_handler_t seg_7_handler;

void app_main() 
{
    seg_7_init(&seg_7_handler, 2, 4, 16, 5, 18, 19, 21, 32);

    while(1)
    {
        for(uint8_t i = 0; i < 8; i++)
        {
            seg_7_write(&seg_7_handler, caracter[i]);
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
        
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
    


}

void seg_7_init(seg_7_handler_t *seg_7_handler, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t p)
{

    seg_7_handler->a = a;
    seg_7_handler->b = b;
    seg_7_handler->c = c;
    seg_7_handler->d = d; 
    seg_7_handler->e = e; 
    seg_7_handler->f = f; 
    seg_7_handler->g = g;
    seg_7_handler->p = p;
    
    gpio_set_direction(seg_7_handler->a, GPIO_MODE_OUTPUT);
    gpio_set_direction(seg_7_handler->b, GPIO_MODE_OUTPUT);
    gpio_set_direction(seg_7_handler->c, GPIO_MODE_OUTPUT);
    gpio_set_direction(seg_7_handler->d, GPIO_MODE_OUTPUT);
    gpio_set_direction(seg_7_handler->e, GPIO_MODE_OUTPUT);
    gpio_set_direction(seg_7_handler->f, GPIO_MODE_OUTPUT);
    gpio_set_direction(seg_7_handler->g, GPIO_MODE_OUTPUT);
    gpio_set_direction(seg_7_handler->p, GPIO_MODE_OUTPUT);

}

void seg_7_write(seg_7_handler_t *seg_t_handler, uint8_t * caracter)
{

    uint8_t pins[8] = 
	{
		seg_t_handler->a,
		seg_t_handler->b,
		seg_t_handler->c,
		seg_t_handler->d,
		seg_t_handler->e,
		seg_t_handler->f,
		seg_t_handler->g,
		seg_t_handler->p
	};

	for (uint8_t i = 0; i < 8; i++)
	{
		gpio_set_level(pins[i], !caracter[i]);
        //ESP_LOGW(TAG, "Pin a %d -- Level: %d", pins[i], !caracter[i]);
	}

}

*/