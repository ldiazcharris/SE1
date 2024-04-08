#include "lm35_driver.h"

int lm35_init(lm35_handler lm35){
ESP_ERROR_CHECK(adc1_config_width(lm35.width));
ESP_ERROR_CHECK(adc1_config_channel_atten(lm35.adc_chanel, lm35.atten));



}