#include "freertos/Freertos.h"
#include <stdio.h>


typedef struct{
    int adc_chanel;
    int width;
    int atten; //Atenuación rango de tensión que recibe el pin ESP32

}lm35_handler

int lm35_init(lm35_handler lm35);