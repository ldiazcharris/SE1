#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"


#define BUSSY_PILOT           27
#define FREE_PILOT            13
#define FREE_BUTTON           39 
#define BUSSY_BUTTON          36
//#define EN_GNSS_PIN           17
#define EN_4G_PIN             33
#define BOARD_LED              2

#define UART0 UART_NUM_0
#define UART1 UART_NUM_1

// A funturo cambiar a "proyectoLuis/cava001/datos" Para tener un control numérico de las cavas
#define CAVA_REF            "C001"
#define CAVA_TOPIC          "proyectoLuis/cava/datos" 

#define CMQTT_START         "AT+CMQTTSTART\r\n"
#define CMQTT_CLIENT        "AT+CMQTTACCQ=0,\"gnss_cavas\",0\r\n"

// Incluir en el manual como una configuración
#define CMQTT_CONNECT        "AT+CMQTTCONNECT=0,\"tcp://18.212.130.131:1883\",300,0,\"test\",\"CloudTech*\"\r\n"
#define CMQTT_TOPIC          "AT+CMQTTTOPIC=0,23\r\n"
#define CMQTT_PAYLOAD        "AT+CMQTTPAYLOAD=0,%d\r\n"
#define MQTT_PAYLOAD_FORMAT  "{\"lat\":\"%.6f\", \"long\":\"%.6f\", \"occup\":\"%d\", \"NMEA_st\":\"%d\", \"Cref\":\"%s\", \"time\":\"%s\", \"date\":\"%s\"}\r\n"
#define MQTT_PUBLISH         "AT+CMQTTPUB=0,0,60,0,0\r\n"

#define RADIO_TIERRA_EQUIVOLUMEN_KM    6371.0
#define RADIO_TIERRA_ECUATORIAL_KM     6378.0
#define RADIO_TIERRA_POLAR_KM          6357.0

typedef enum {
    NMEA_PARSER_OK = 0,
    NMEA_NO_VALID,
    NMEA_NO_RMC,
    NMEA_VOID_FIELD,
    NMEA_PARSER_ERR
} NMEA_state_t;

typedef enum {
    MQTT_MSG_OK = 0,
    MQTT_MSG_FAIL,
    MQTT_TOPIC_OK,
    MQTT_TOPIC_FAIL,
    MQTT_MSG_ERROR
} mqtt_msg_state_t;

typedef enum {
    MQTT_SERVER_OK = 0,
    MQTT_FAIL_INIT_SERVICE,
    MQTT_FAIL_ADQ_CLIENT,
    MQTT_FAIL_INIT_SERVER,
    MQTT_SERVER_ERR
} mqtt_server_state_t;

typedef enum{
BUSSY_CAVA = 0,
FREE_CAVA,
Def_CAVA
}occupancy_t;

typedef struct
{
    float lat;
    float lon;
    char time[10];
    char date[7];
    NMEA_state_t NMEA_state;
    
} GNSSData_t;


// Analizar Si la posición GNSSData_t puede ser un arreglo de 10 valores. 
typedef struct
{
    occupancy_t occupancy;
    GNSSData_t position;

} CAVA_DATA_t;

typedef struct 
{
    CAVA_DATA_t cava_data;
    mqtt_msg_state_t msg_state;
    mqtt_server_state_t server_state;

} LCD_data_t;




/**
 * @brief explican que hace la función
 * 
 * @param baud_rate: 
 * @param 
 * 
 * @return 
*/

void uart_init(  uart_port_t     uart_num, 
                        int             baud_rate, 
                        int             rx_buffer_size, 
                        int             tx_buffer_size, 
                        int             queue_size, 
                        QueueHandle_t   *uart_queue, 
                        int             intr_alloc_flags
                     );
void uart_transmit(uart_port_t uart_num, const void *src, size_t size);

void uart_receive(uart_port_t uart_num, void *buf, uint32_t length);

void delay(const TickType_t delay_ms);



/**
 * @brief Función que permite parsear las cadenas tipo GNRMC recibidas por el módulo Quectel L76
 *        Es la versión de tipo "reentrant".
 * @param nmeaString: string que priviene del módulo GNSS en formato de trama NMEA, que será parseado para extraer la 
 *                    información necesaria
 * @param gnssData: estructura de datos del tipo GNSSData_t, que permite almacenar hora, latitud, longitud y resultao
 *                  del proceso de parseo
 * 
*/
NMEA_state_t nmea_rmc_parser_r(char *nmeaString, GNSSData_t *gnssData);

/**
 * @brief Esta función permite inicializar un pin GPIO de la ESP32 para usarlo como interrupición
 * 
*/
void ocupancy_buttons_init();


void write_position(char * lat, char * lon);

/**
 * @brief Esta función permite inicializar un los pines GPIO de la ESP32 que serán usados para conectar los pilots
 * 
*/
void occupancy_pilots_init();

/**
 * @brief Inicializa el pin GPIO donde se conecta el pin de EN (enable) del módulo 4G SIM A7670SA
 *        Permite controlar reinicios del módulo por software. 
*/
void enable_pin_4g_init();

/**
 * @brief convierte un estado de la estructura NMEA_state_t en un string.
 * @param nmea_state: estado NMEA a convertir en string.
 * @param str: puntero a char de destino. 
 */
void nmea_state_to_str(NMEA_state_t nmea_state, char * str);


/**
 * @brief convierte un estado de la estructura mqtt_msg_state_t en un string.
 * @param mqtt_msg_st: estado del mensaje MQTT a convertir en string.
 * @param str: puntero a char de destino. 
 */
void mqtt_msg_state_to_string(mqtt_msg_state_t mqtt_msg_st, char * str);

/**
 * @brief convierte un estado de la estructura occupancy_t en un string.
 * @param occupancy: estado de la occupancy a convertir en string.
 * @param str: puntero a char de destino. 
 */
void occupancy_to_string(occupancy_t occupancy, char * str);



/**
 * Debounce functions
*/

typedef enum{
	BUTTON_UP = 0,
	BUTTON_RISING,
	BUTTON_DOWN,
	BUTTON_FALLING
} debounce_t;

bool debounce_init();

debounce_t debounce_up(int pinState);

/**
 * Delay_t functions
*/

typedef struct{
	uint32_t startTime;
	uint32_t duration;
	bool running;
} delay_t;


void delay_init(delay_t * delay, uint32_t duration);
bool delay_read(delay_t * delay);
void delay_write(delay_t * delay, uint32_t duration);

double haversine(double lat1, double lon1, double lat2, double lon2);




