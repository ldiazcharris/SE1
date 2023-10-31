
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

    uint8_t seg_7_pins[8] = 
	{
		seg_7_handler->a,
		seg_7_handler->b,
		seg_7_handler->c,
		seg_7_handler->d,
		seg_7_handler->e,
		seg_7_handler->f,
		seg_7_handler->g,
		seg_7_handler->p
	};
    
   
    for (uint8_t i = 0; i < 8; i++)
	{
		gpio_set_direction(seg_7_pins[i], GPIO_MODE_OUTPUT);
        
	}

}

void seg_7_write(seg_7_handler_t *seg_7_handler, uint8_t * caracter)
{
    uint8_t pins[8] = 
	{
		seg_7_handler->a,
		seg_7_handler->b,
		seg_7_handler->c,
		seg_7_handler->d,
		seg_7_handler->e,
		seg_7_handler->f,
		seg_7_handler->g,
		seg_7_handler->p
	};

	for (uint8_t i = 0; i < 8; i++)
	{
		gpio_set_level(pins[i], !caracter[i]);
        //ESP_LOGW(TAG, "Pin a %d -- Level: %d", pins[i], !caracter[i]);
	}
}