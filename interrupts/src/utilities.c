#include "utilities.h"
#include "lcd_i2c_grove.h"
#include <math.h>

// Uncomment for debugging with UART_0
//static const char *TAG = "NMEA_PARSER";


void uart_init(  uart_port_t     uart_num, 
                        int             baud_rate, 
                        int             rx_buffer_size, 
                        int             tx_buffer_size, 
                        int             queue_size, 
                        QueueHandle_t   *uart_queue, 
                        int             intr_alloc_flags
                     ){
    
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(    uart_num, 
                                            rx_buffer_size*2, 
                                            tx_buffer_size,
                                            queue_size, 
                                            uart_queue, 
                                            intr_alloc_flags
                                         )
                     );

    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
}



void uart_transmit(uart_port_t uart_num, const void *src, size_t size)
{
    uart_write_bytes(uart_num, src, size);
}



void uart_receive(uart_port_t uart_num, void *buf, uint32_t length)
{
    uart_read_bytes(uart_num, buf, length, portMAX_DELAY);
}



void delay(const TickType_t delay_ms)
{
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
}



NMEA_state_t nmea_rmc_parser_r(char *nmeaString, GNSSData_t *gnssData)
{
    NMEA_state_t result_parser = NMEA_PARSER_ERR;
    // Verificar que la cadena comience con '$'
    if (NULL == strstr(nmeaString, "$"))
    {
        // Cadena NMEA no válida, no comienza con $
        result_parser = NMEA_NO_VALID;
    }
    else
    {
        // Se utiliza strtok para dividir la cadena en tokens usando "," como divisor
        char *token;
        char* rest = (char *)nmeaString;
        token = strtok_r(nmeaString, ",", &rest);
       

        // Comprobar si el primer token contiene "RMC"
        if (NULL == strstr(token, "GNRMC"))
        {
            // Cadena NMEA no válida, no es un mensaje GNRMC
            result_parser = NMEA_NO_RMC;
        }
        else
        {    // Si todo sale bien se itera a través de los tokens
            for (int i = 1; i < 12; i++)
            {
                token = strtok_r(NULL, ",", &rest);

                switch (i)
                {
                case 1:
                    // Obtener la hora en formato HHMMSS
                    strncpy(gnssData->time, token, 10);
                    break;
                case 2:
                    // 
                    if(NULL == strstr(token, "A"))
                        return NMEA_NO_VALID;

					break;

                case 3:
                    // Obtener la latitud en formato DMS (DDMM.MMMM) y convertir a formato DD
                    float lat_float_1 = atof(token) / 100;
                    int lat_deg = (int)lat_float_1;
                    float lat_float_2 = (lat_float_1 - lat_deg)*100;
                    int lat_min = (int)(lat_float_2);
                    float lat_sec = (lat_float_2 - lat_min)*10;
                    gnssData->lat = (float)lat_deg + ((float)lat_min/60) + (lat_sec/3600);
                    break;
                
                case 4:
                    // Obtener la dirección de latitud Norte o Sur
                    if (0 == strcmp("S", token))
                        gnssData->lat *=-1;
					break;
					
                case 5:
                    // Obtener la longitud en formato DMS (DDDMM.MMMM) y convertir a formato DD
                    float lon_float_1 = atof(token) / 100;
                    int lon_deg = (int)lon_float_1;
                    float lon_float_2 = (lon_float_1 - lon_deg)*100;
                    int lon_min = (int)(lon_float_2);
                    float lon_sec = (lon_float_2 - lon_min)*10;
                    gnssData->lon = (float)lon_deg + ((float)lon_min/60) + (lon_sec/3600);
                    
                    result_parser = NMEA_PARSER_OK;
                    break;
					
                case 6:
                    // Obtener la dirección de longitud Este u Oeste
                    if (0 == strcmp("W", token))
                        gnssData->lon *=-1;
                    break;
				case 9:
                    // Obtener la fecha
                    strncpy(gnssData->date, token, 7);
                    break;

                default:
                    break;
                }
            }
        }
    }
    gnssData->NMEA_state = result_parser;
    return result_parser;
}


void ocupancy_buttons_init()
{
    gpio_reset_pin(BUSSY_BUTTON);
    gpio_reset_pin(FREE_BUTTON);
    gpio_config_t occupancy_pin_config;
    occupancy_pin_config.intr_type = GPIO_INTR_POSEDGE;
    occupancy_pin_config.pin_bit_mask = ((1ULL<<BUSSY_BUTTON) | (1ULL<<FREE_BUTTON));
    occupancy_pin_config.mode = GPIO_MODE_INPUT;
    occupancy_pin_config.pull_up_en = GPIO_PULLUP_DISABLE;
    occupancy_pin_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //gpio_config(occupancy_pin_config);
    esp_err_t error = gpio_config(&occupancy_pin_config);
    if(error!=ESP_OK){
        gpio_set_level(BUSSY_PILOT, 1);
        gpio_set_level(FREE_PILOT, 1);
    }

}



void occupancy_pilots_init()
{    
    gpio_reset_pin(BUSSY_PILOT);
    gpio_reset_pin(FREE_PILOT);
    gpio_set_direction(BUSSY_PILOT, GPIO_MODE_OUTPUT);
    gpio_set_direction(FREE_PILOT, GPIO_MODE_OUTPUT);
    gpio_pulldown_dis(BUSSY_PILOT);
    gpio_pulldown_dis(FREE_PILOT);
    gpio_pullup_dis(BUSSY_PILOT);
    gpio_pullup_dis(FREE_PILOT);
    //gpio_set_level(BUSSY_PILOT, 1);
    //gpio_set_level(FREE_PILOT, 1);
}



void enable_pin_4g_init()
{
    gpio_reset_pin(EN_4G_PIN);
    gpio_set_direction(EN_4G_PIN, GPIO_MODE_OUTPUT_OD);
    gpio_set_pull_mode(EN_4G_PIN, GPIO_FLOATING);
    gpio_set_level(EN_4G_PIN, 1);
}



void write_position(char * lat, char * lon)
{
    //lcd_clear();
    lcd_cursor(0, 0);
    lcd_write_string(lat);
    lcd_cursor(1, 0);
    lcd_write_string(lon);
}



void nmea_state_to_str(NMEA_state_t nmea_state, char * str)
{
    switch (nmea_state)
    {
    case NMEA_PARSER_OK:
        bzero(str, 16);
        strcpy(str, "NMEA_PARSER_OK");
        break;
    case NMEA_NO_VALID:
        bzero(str, 16);
        strcpy(str, "NMEA_NO_VALID");
        break;
    case NMEA_NO_RMC:
        bzero(str, 16);
        strcpy(str, "NMEA_NO_RMC");
        break;
    case NMEA_VOID_FIELD:
        bzero(str, 16);
        strcpy(str, "NMEA_VOID_FIELD");
        break;
    case NMEA_PARSER_ERR:
        bzero(str, 16);
        strcpy(str, "NMEA_PARSER_ERR");
        break;
    default:
        bzero(str, 16);
        strcpy(str, "NMEA_default");
    }
}



void mqtt_msg_state_to_string(mqtt_msg_state_t mqtt_msg_st, char *str)
{
    switch (mqtt_msg_st)
    {
    case MQTT_MSG_OK:
        bzero(str, 16);
        sprintf(str, "MQTT Msg OK");
        lcd_set_RGB(0, 255, 0); // LCD color verde
        break;
    case MQTT_MSG_FAIL:
        bzero(str, 16);
        sprintf(str, "MQTT Msg FAIL");
        lcd_set_RGB(125, 2, 0); // LCD color rojo
        break;
    case MQTT_TOPIC_OK:
        bzero(str, 16);
        sprintf(str, "MQTT Topic OK");
        lcd_set_RGB(255, 255, 0); // LCD color amarillo
        break;
    case MQTT_TOPIC_FAIL:
        bzero(str, 16);
        sprintf(str, "MQTT Topic FAIL");
        lcd_set_RGB(125, 2, 0); // LCD color rojo
        break;
    case MQTT_MSG_ERROR:
        bzero(str, 16);
        sprintf(str, "MQTT Msg ERROR");
        lcd_set_RGB(125, 2, 0); // LCD color rojo
        break;

    default:
        bzero(str, 16);
        sprintf(str, "Msg st not recv");
        lcd_set_RGB(0, 0, 255); // LCD color azul
        break;
    }
}



void occupancy_to_string(occupancy_t occupancy, char * str)
{
    switch (occupancy)
    {
    case BUSSY_CAVA:
        strcpy(str, "OCUPADA");
        break;
    case FREE_CAVA:
        strcpy(str, "LIBRE");
        break;
    case Def_CAVA:
        strcpy(str, "Defect");
        break;
    default:
        strcpy(str, "NaN");
        break;
    }
}



static double deg_to_rad(double deg) {
    return deg * (M_PI / 180.0);
}

double haversine(double lat1, double lon1, double lat2, double lon2) 
{
    lat1 = deg_to_rad(lat1);
    lon1 = deg_to_rad(lon1);
    lat2 = deg_to_rad(lat2);
    lon2 = deg_to_rad(lon2);

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double dist = RADIO_TIERRA_EQUIVOLUMEN_KM * c;

    return dist;
}