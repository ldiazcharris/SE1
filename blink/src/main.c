

/*

#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "utilities.h"



#define BUF_SIZE 1024
#define LED_1 2
#define LED_2 4
#define LED_3 15
#define UART0 UART_NUM_0
#define UART1 UART_NUM_1



static QueueHandle_t uart0_queue;
static QueueHandle_t uart1_queue;



static void uart_interrupt_task(void *params);
static void led_task(void *params);


void app_main()
{
    gpio_reset_pin(LED_1);
    gpio_reset_pin(LED_2);
    gpio_reset_pin(LED_3);
    gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_3, GPIO_MODE_OUTPUT);

    uart_init(UART0, 115200, BUF_SIZE * 2, BUF_SIZE * 2, 50, &uart0_queue, ESP_INTR_FLAG_LEVEL1);
    //          (UART_NUM, TX, RX, RTS, CTS)
    uart_set_pin(UART0,     1,  3,  22,  19);

    uart_init(UART1, 115200, BUF_SIZE * 2, BUF_SIZE * 2, 50, &uart1_queue, ESP_INTR_FLAG_LEVEL1); //   ESP_INTR_FLAG_IRAM
    //          (UART_NUM, TX, RX, RTS, CTS)
    uart_set_pin(UART1,    33, 26,  14,  12);

    xTaskCreate(uart_interrupt_task,
                "Tarea UART",
                BUF_SIZE * 4,
                NULL,
                12,
                NULL);

    xTaskCreate(led_task,
                "Tarea LED_3",
                BUF_SIZE * 4,
                NULL,
                7,
                NULL);
}



static void uart_interrupt_task(void *params)
{
    uart_event_t uart_event;
    char *uart_recv_data = (char *)malloc(BUF_SIZE);
    char *response_string = (char *)malloc(BUF_SIZE);
    uint16_t light_code; 
    while (1)
    {
        if (xQueueReceive(uart0_queue, (void *)&uart_event, (TickType_t)portMAX_DELAY))
        {
            uart_transmit(UART0, "Tarea UART\n", strlen("Tarea UART\n"));
            bzero(uart_recv_data, BUF_SIZE); // Puntero para guardar la información recibida
            bzero(response_string, BUF_SIZE); 

            switch (uart_event.type)
            {
            case UART_DATA:
                uart_receive(UART0, (void *)uart_recv_data, (uint32_t)uart_event.size);
                uart_transmit(UART0, (const void*)uart_recv_data, (uint32_t)uart_event.size);
                
                light_code = (uint16_t)atoi((const char *)uart_recv_data);
                
                switch(light_code)
                {
                    case 10:
                        sprintf(response_string, "Light Code: 10\n");
                        uart_transmit(UART0, response_string, strlen((const char*)response_string));
                        gpio_set_level(LED_1, 0);
                        break;

                    case 11:
                        sprintf(response_string, "Light Code: 11\n");
                        uart_transmit(UART0, response_string, strlen((const char*)response_string));
                        gpio_set_level(LED_1, 1);
                        break;
                    
                    case 20:
                        sprintf(response_string, "Light Code: 20\n");
                        uart_transmit(UART0, response_string, strlen((const char*)response_string));
                        gpio_set_level(LED_2, 0);
                        break;

                    case 21:
                        sprintf(response_string, "Light Code: 21\n");
                        uart_transmit(UART0, response_string, strlen((const char*)response_string));
                        gpio_set_level(LED_2, 1);
                        break;
                    default:
                        sprintf(response_string, "Light Code ERROR\n");
                        gpio_set_level(LED_2, 0);
                        gpio_set_level(LED_1, 1);
                        delay(500);
                        gpio_set_level(LED_2, 1);
                        gpio_set_level(LED_1, 0);
                        delay(500);
                        gpio_set_level(LED_2, 0);
                        gpio_set_level(LED_1, 0);

                        uart_transmit(UART0, response_string, strlen((const char*)response_string));
                }

                //uart_transmit(UART1, response_string, strlen((const char*)response_string));
                break; 

            default:
                break;
            }
        }
    }

    free(uart_recv_data);
    free(response_string);
}


static void led_task(void *params)
{
    while(1){
        
        gpio_set_level(LED_3, 0);
        delay(500);
        gpio_set_level(LED_3, 1);
        delay(500);
        uart_transmit(UART0, "Tarea LED_3\n", strlen("Tarea LED_3\n"));
    }
    
}


*/







/*

// Incluir bibliotecas standar de C 
#include <stdio.h>   
#include <stdlib.h> // Requerida para usar la función malloc()

// Incluir biblioteca de control de pines GPIO del ESP-IDF
#include "driver/gpio.h"       

// Incluir biblioteca de control de GPTimer del ESP-IDF
#include "driver/gptimer.h"

// Incluir biblioteca del sistema operativo FreeRTOS
#include "freertos/FreeRTOS.h" 
  
// Incluir biblioteca para manejo de tareas del FreeRTOS
#include "freertos/task.h" 

// Inicializar una instancia del temporizador GPTimer
gptimer_handle_t gptimer = NULL;

// Inicializar la estructura para configurar el temporizador
gptimer_config_t timer_config = {

    .clk_src = GPTIMER_CLK_SRC_APB,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
	
};

// Se crea el prototipo de función que será llamada cuando se active la interrupción
static bool call_back_proof(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);

// Se crea un puntero del tipo gptimer_event_callbacks_t, que contendrá el llamado a función
gptimer_event_callbacks_t *call_baks;

    // Estructura para habilitar las alertas por eventos del temporizador
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0, // el contador se cargará con 0 en cada evento
        .alarm_count = 1000000, // periodo = 1s, resolución de 1MHz
        .flags.auto_reload_on_alarm = true, // habilita auto-carga
    };

// Función principal
int app_main()
{
    // Configurar el temporizador según a estructura "timer_config",
    // creada anteriormente. 
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));



    // Carga la configuración de alarma en el temporizador
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    // Se reserva un espacio de memoria eh en el heap para albergar la función de callback
    call_baks = malloc(sizeof(gptimer_event_callbacks_t));

    // Condicional que valida que se asignó correctamente la memoria. 
    // Si malloc() falla, entonces devolverá NULL
    // El programa no puede continuar, debido a que esto puede ocasionar
    // una falla de pánico del sistema, y se reiniciará para fallar nuevamente.
    // Por lo cual, no tiene sentido continuar el programa. 
    if(call_baks == NULL)
    {
        printf("------**ERROR ASIGNANDO MEMORIA**------");
        return -1;
    }

    // Se asigna a la estructura call_baks creada anteriormente 
    // la función que será llamada cuando ocurra el evento del temporizador.
    // la función call_back_proof(), se ejecutará en tiempo de interrupción,
    // Por lo tanto, no es recomendable usar lógica compleja dentro de ella
    // o usar funciones del tipo bloqueante, como delays, printf(), entre otras. 
    call_baks->on_alarm = call_back_proof; 

    // Se carga el evento de callback con la función gptimer_register_event_callbacks()
    // para que pueda ser usada realmente
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, 
                                                    call_baks, NULL));
    
    // Habilita el temporizador
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    
    // Inicia el conteo del temporizador
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    // Si algo sale mal, 
	// Se imprimirá por serial un mensaje 
	// parecido a este: abort() was called at PC 0x40082dff on core 0

    
    // Se configura el GPIO_2 como entrada y salida
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT_OUTPUT);


    return 0;
}

// Definición de la función que será llamada en cada evento del temporizador
static bool call_back_proof(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    // Se hace un "toggle" del pin GPIO_2
    gpio_set_level(GPIO_NUM_2, !gpio_get_level(GPIO_NUM_2));

    return true;
    
}

*/


/*


// Incluir bibliotecas standar de C 
#include <stdio.h>   
#include <string.h>  

// Incluir biblioteca de control de pines GPIO del ESP-IDF
#include "driver/gpio.h"       

// Incluir biblioteca de control de GPTimer del ESP-IDF
#include "driver/gptimer.h"

// Incluir biblioteca del sistema operativo FreeRTOS
#include "freertos/FreeRTOS.h" 
  
// Incluir biblioteca para manejo de tareas del FreeRTOS
#include "freertos/task.h" 


// Inicializar una instancia del temporizador GPTimer
gptimer_handle_t gptimer = NULL;

// Inicializar la estructura para configurar el temporizador
gptimer_config_t timer_config = {

    .clk_src = GPTIMER_CLK_SRC_APB,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
	
};

void app_main(){

    // Configurar el temporizador según a estructura "timer_config",
    // creada anteriormente. 
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    // Habilita el temporizador
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    // Inicia el conteo del temporizador
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    // Variable para almacenar el valor leído en el temporizador
    uint64_t timer_val;

    // Lectura del valor del temporizador
    ESP_ERROR_CHECK(gptimer_get_raw_count(gptimer, &timer_val));

    // Este bloque genera una lectura continua
    while(1){

        // Lectura del valor del temporizador
        ESP_ERROR_CHECK(gptimer_get_raw_count(gptimer, &timer_val));

        // Imprime por serial el valor leído
        printf("El valor leido es: %llu\n", timer_val);

        // Espera un segundo
        vTaskDelay( 1000 / portTICK_PERIOD_MS);
    }

}

*/


/*

// Incluir bibliotecas standar de C 
#include <stdio.h>   
#include <string.h>  

// Incluir biblioteca de control de pines GPIO del ESP-IDF
#include "driver/gpio.h"        		

// Incluir biblioteca del sistema operativo FreeRTOS
#include "freertos/FreeRTOS.h" 
  
// Incluir biblioteca para manejo de tareas del FreeRTOS
#include "freertos/task.h" 
                  
// Crea macro con el número del pin donde está el detector de gas. 
// Este detector de gas se activará con un pulso positivo.
// Se debe hacer uso de la configuración Pull-Down de las entradas de la ESP32
// Pra evitar errores de lectura y activaciones inesperadas. 

#define DETECTOR_DE_GAS  2
 
// Crea macro con el número del pin donde está la alarma. 
// Esta alarma se activará con un pulso positivo.
// Se debe hacer uso de la configuración Pull-Down de las entradas de la ESP32
// Pra evitar errores de lectura y activaciones inesperadas. 
                           
#define ALARMA 4                  

// Macros para representar estados encendido (ON) y apagado (OFF)
#define ON  1
#define OFF 0

// Función principal del programa

void app_main(void)
{
    // Reiniciar el pin del DETECTOR_DE_GAS para que 
	// esté en su estado predeterminado
	 
	gpio_reset_pin(DETECTOR_DE_GAS);   
		
	// Configura el pin del DETECTOR_DE_GAS como entrada
	gpio_set_direction(DETECTOR_DE_GAS, GPIO_MODE_INPUT);

	// Habilita la configuración Pull-Down en el pin del DETECTOR_DE_GAS
	gpio_pulldown_en(DETECTOR_DE_GAS);
		
	// Reiniciar el pin de la ALARMA para que 
	// esté en su estado predeterminado
	  
		gpio_reset_pin(ALARMA);

	// Configura el pin de la ALARMA como salida
	gpio_set_direction(ALARMA, GPIO_MODE_OUTPUT);

    // Habilita la configuración Pull-Downen el pin de la alarma.
    gpio_pulldown_en(ALARMA);
	
	// Bucle infinito que permite que el programa se ejecute indefinidamente.
	while(1)
    {
        // Pregunta si el DETECTOR_DE_GAS está encendido
        if(gpio_get_level(DETECTOR_DE_GAS) == ON)
        {
            // Solo si es verdadero, entonces se enciende la ALARMA
            gpio_set_level(ALARMA, ON);
        }
        // Pregunta si el DETECTOR_DE_GAS está apagado
        if (gpio_get_level(DETECTOR_DE_GAS) == OFF)
        {
            // Solo si es verdadero, entonces se apaga la ALARMA
            gpio_set_level(ALARMA, OFF);
        }
	}
}

*/

/*
#include <stdio.h>   
#include <string.h>      
#include "dht11.h"     
#include "driver/gpio.h"			
#include "freertos/FreeRTOS.h"  
#include "freertos/task.h"    

#define portTICK_US ((TickType_t) 1000000/configTICK_RATE_HZ)

void app_main(void)
{
    //gpio_set_direction(1, GPIO_MODE_OUTPUT);
    DHT11_init(GPIO_NUM_4);

    while(1) {
        printf("Temperature is %d \n", DHT11_read().temperature);
        printf("Humidity is %d\n", DHT11_read().humidity);
        printf("Status code is %d\n", DHT11_read().status);
        vTaskDelay(1000000/portTICK_US); //portTICK_PERIOD_MS
    }
}

*/



/*
#include <stdio.h>              
#include "driver/gpio.h"		
#include "driver/adc.h"			
#include "freertos/FreeRTOS.h"  
#include "freertos/task.h"      
#include "driver/uart.h"       


#define DELAY 	500

void app_main(void)
{
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_6));

    int val = adc1_get_raw(ADC1_CHANNEL_0);

	while(1)
	{
		val = adc1_get_raw(ADC1_CHANNEL_0);

		printf("La lectura ADC_0 es: %d\n", val);

		vTaskDelay( DELAY / portTICK_PERIOD_MS);
	}
}

*/





#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#define TIEMPO      1000
#define BUF_SIZE    1024
#define LED         2

static void uart_init();

void app_main(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED,GPIO_MODE_OUTPUT);
    uart_init();
	uint8_t * datos;
    char* Mensaje1= "\n Encendido \n";
    char* Mensaje2= "\n Apagado \n";
	// &datos guarda el valor leido desde el monitor serial en formato ASCII
    int valor = uart_read_bytes(UART_NUM_0, &datos, 2, portMAX_DELAY); 
        

    while(1){
        valor = uart_read_bytes(UART_NUM_0, &datos, 2, portMAX_DELAY); 

        if (valor > 0) {
            int caso = atoi((char *) &datos);
            
           switch (caso)
           {
            
           case 48: // El valor 0 en ASCII es 48
                gpio_set_level(LED, 0);
                uart_write_bytes(UART_NUM_0,(const char*)Mensaje2, strlen(Mensaje2));
            break;
           case 49: // El valor 1 en ASCII es 49
                gpio_set_level(LED, 1);
                uart_write_bytes(UART_NUM_0,(const char*)Mensaje1, strlen(Mensaje1));
            break;
           }

        }
        
       
    }
}

static void uart_init(){
    
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, ESP_INTR_FLAG_IRAM));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));   
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, 1, 3, 22, 19));
}










/*

#include <stdio.h>              // Biblioteca estándar de C para entrada/salida
#include <string.h>             // Biblioteca estándar de C para manejo de strings
#include "driver/gpio.h"        // Biblioteca de funciones para interactuar con los pines GPIO
#include "freertos/FreeRTOS.h"  // Biblioteca para programación multitarea en ESP32
#include "freertos/task.h"      // Biblioteca para crear tareas en ESP32
#include "driver/uart.h"        // Biblioteca para usar las funciones del UART del ESP32
#define TIEMPO      1000
#define BUF_SIZE    1024

static void uart_init();

void app_main(void)
{
	char* string_test = "Hola, mundo\n";
	uart_init();
	while(1){
		uart_write_bytes(UART_NUM_0, (const char*)string_test, strlen(string_test));
		vTaskDelay( TIEMPO / portTICK_PERIOD_MS);
	}
}

static void uart_init(){
	
	uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity    = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_DEFAULT,
	};

	ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, ESP_INTR_FLAG_IRAM));
	ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, 1, 3, 22, 19));

}

*/


/*
// Llama a la biblioteca GPIO
#include "driver/gpio.h" 	
#include "freertos/FreeRTOS.h" // Biblioteca para programación multitarea en ESP32
#include "freertos/task.h"     // Biblioteca para crear tareas en ESP32


// Crea una macro para indicar el pin donde se conectará el botón
#define BUTTON 4

// Crea una macro para indicar el pin donde se conectará el botón
#define LED 2

// Función principal. En esto difiere de un programa en C propiamente dicho.
void app_main(void)
{
	// Configuración de los pines
	gpio_reset_pin(BUTTON);  // Reiniciar el pin del botón (GPIO4) para que esté en su estado predeterminado 
	
	// Configura el pin del botón (GPIO4) como entrada
	gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
	
	gpio_reset_pin(LED);     // Reiniciar el pin del LED (GPIO2) para que esté en su estado predeterminado  
	
	// Configura el pin del LED (GPIO2) como salida
	gpio_set_direction(LED, GPIO_MODE_OUTPUT);
	
	// Bucle infinito que permite que el programa se ejecute indefinidamente.
	while(1){
		// Lee el estado del botón y actualizar el estado del LED
		gpio_set_level(LED, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_set_level(LED, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}
*/

/*
	#include <stdio.h>             // Biblioteca estándar de C para entrada/salida
	#include "driver/gpio.h"       // Biblioteca de funciones para interactuar con los pines GPIO
	#include "freertos/FreeRTOS.h" // Biblioteca para programación multitarea en ESP32
	#include "freertos/task.h"     // Biblioteca para crear tareas en ESP32

	#define ESPERA 500

	void app_main(void)
	{
		while(1){
			printf("Hola mundo\n");
			vTaskDelay( ESPERA / portTICK_PERIOD_MS);
		}
	}

*/

/*
// Incluir biblioteca del sistema operativo FreeRTOS
#include "freertos/FreeRTOS.h" 
  
// Incluir biblioteca para manejo de tareas del FreeRTOS
#include "freertos/task.h" 

// Incluir biblioteca de control de pines GPIO del ESP-IDF
#include "driver/gpio.h"                        

// Crea macro con el número del pin del LED que se utilizará
#define LED  2
 
// Crea una macro que indica el tiempo de espera (WAIT)                            
#define WAIT 1000                               

// Función principal del programa
void app_main(void)                             
{
	// Reiniciar el pin LED para que esté en su estado predeterminado
    gpio_reset_pin(LED);                        
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);  // Configurar el pin LED como salida

	// Bucle infinito que permite que el programa se ejecute indefinidamente.
    while(1){                                   
        gpio_set_level(LED, 0);                 // Apagar el LED
        vTaskDelay( WAIT / portTICK_PERIOD_MS); // Esperar N milisegundos
        gpio_set_level(LED, 1);                 // Enciende el LED
        vTaskDelay( WAIT / portTICK_PERIOD_MS); // Esperar N milisegundos
    }
}

*/

/*

#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h" // Biblioteca para programación multitarea en ESP32
#include "freertos/task.h"     // Biblioteca para crear tareas en ESP32


#define BUTTON 4
#define LED 2

int64_t time = 0;

void app_main(void){
    // Configuración de los pines
    
    gpio_reset_pin(BUTTON);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    time  = esp_timer_get_time();

    while(1){
        // Leer el estado del botón y actualizar el estado del LED
        pio_set_level(LED, gpio_get_level(BUTTON));

    }
}

*/

