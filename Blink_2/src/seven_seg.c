
#include "seven_seg.h"




void seg_7_init(seg_7_handler_t *seg_7_handler, 
                uint8_t a, 
                uint8_t b, 
                uint8_t c, 
                uint8_t d, 
                uint8_t e, 
                uint8_t f, 
                uint8_t g, 
                uint8_t p){

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