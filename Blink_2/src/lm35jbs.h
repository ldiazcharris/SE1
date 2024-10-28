//CODIGO LM35JBS LIBRERIA.h
#include <stdio.h>
#include "driver/adc.h"

void lm35jbs_inicia(adc1_channel_t channel, adc_atten_t atten);
float lm35jbs_leer(adc1_channel_t channel);