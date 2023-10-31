

#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

/**
 * @brief 
*/

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

/**
 * @brief 
*/
void seg_7_init(seg_7_handler_t *seg_7_handler, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t p);


/**
 * @brief 
*/
void seg_7_write(seg_7_handler_t *seg_t_handler, uint8_t * caracter);