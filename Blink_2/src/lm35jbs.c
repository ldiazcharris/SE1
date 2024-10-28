//CODIGO LM35JBS LIBRERIA.c

#include "lm35jbs.h"


void lm35jbs_inicia(adc1_channel_t channel, adc_atten_t atten)
{
   
    adc1_config_width(ADC_WIDTH_BIT_12);
    
    adc1_config_channel_atten(channel, atten);
}

float lm35jbs_leer(adc1_channel_t channel)
{
    uint32_t leer_adc = adc1_get_raw(channel);

    float volt = (leer_adc / (float)4095) * 3.3;

    float temp = (volt * 1000) / 10;

    return temp;
}
