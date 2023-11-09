
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

// Función principal
int app_main()
{
	// Configurar el temporizador según a estructura "timer_config",
	// creada anteriormente. 
	ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

	// Estructura para habilitar las alertas por eventos del temporizador
	gptimer_alarm_config_t alarm_config = {
		.reload_count = 0, // el contador se cargará con 0 en cada evento
		.alarm_count = 1000000, // periodo = 1s, resolución de 1MHz
		.flags.auto_reload_on_alarm = true, // habilita auto-carga
	};

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
	// o usar funciones del tipo bloqueante como delays, printf(), entre otras. 
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
	gpio_set_level(2, !gpio_get_level(2));

	return true;
	
}



*/



/*
#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lcd_i2c.h"  // Librería para el uso del LCD con modulo I2C PCF8574



void app_main()
{
    // Inicialización de la pantalla LCD
    lcd_init(); 
    
    // Se pone la pantalla LCD en blanco
    lcd_clear(); 
   
    while(1){

        // Se pone la pantalla LCD en blanco cada nuevo ciclo
        lcd_clear(); 
        
        // Se coloca el cursor en la posición (0, 0) de la pantalla (fila, columna)
        lcd_cursor(0, 0); 

        // Se escribe el texto en pantalla
        lcd_write_string("Hola Mundo..."); 
        
        vTaskDelay(3000/portTICK_PERIOD_MS); 
        
        // Se pone la pantalla LCD en blanco
        lcd_clear(); 
        
        // Se coloca el cursor en la posición (1, 0) de la pantalla
        lcd_cursor(1, 0); 
        
        // Se escribe el texto en pantalla
        lcd_write_string("...Chao Mundo");  
        
        vTaskDelay(3000/portTICK_PERIOD_MS);
    }
}

*/



/*

// 7-Segmentoso excersice

#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "seven_seg.h"

static const char* TAG = "7-segmentos";

// Matriz de números para cátodo común
uint8_t  caracter[10][8] =
{
// a, b, c, d, e, f, g, p
  {1, 1, 1, 1, 1, 1, 0, 0},     // 0 
  {0, 1, 1, 0, 0, 0, 0, 0},     // 1
  {1, 1, 0, 1, 1, 0, 1, 0},     // 2
  {1, 1, 1, 1, 0, 0, 1, 0},     // 3
  {0, 1, 1, 0, 0, 1, 1, 0},     // 4
  {1, 0, 1, 1, 0, 1, 1, 0},     // 5
  {1, 0, 1, 1, 1, 1, 1, 0},     // 6
  {1, 1, 1, 0, 0, 0, 0, 0},     // 7
  {1, 1, 1, 1, 1, 1, 1, 0},     // 8
  {1, 1, 1, 0, 0, 1, 1, 0}      // 9
};

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


void seg_7_init(seg_7_handler_t *seg_7_handler, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t p);
void seg_7_write(seg_7_handler_t *seg_t_handler, uint8_t * caracter);

seg_7_handler_t seg_7_handler;

void app_main() 
{
    seg_7_init(&seg_7_handler, 2, 4, 16, 5, 18, 19, 21, 32);

    while(1)
    {
        for(uint8_t i = 0; i < 8; i++)
        {
            seg_7_write(&seg_7_handler, caracter[i]);
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
        
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
    


}

void seg_7_init(seg_7_handler_t *seg_7_handler, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t p)
{

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

*/



#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


#define GPIO_SERVO 18 // Puedes utilizar cualquier pin GPIO de la ESP32

void app_main() {
    // Configurar el pin GPIO para controlar el servo
    /*
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_SERVO);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    */
   gpio_set_direction(GPIO_SERVO, GPIO_MODE_OUTPUT);
   gpio_set_pull_mode(GPIO_SERVO, GPIO_PULLDOWN_ONLY);

    uint16_t grades; 
    float dutyCicle;

    while (1) {

        grades = 0;
	
        for (int hz = 1; hz <= 50; hz++){
            dutyCicle = (grades*2000.0/180.0)+500;
            gpio_set_level(GPIO_SERVO, 1);
            usleep(dutyCicle);
            gpio_set_level(GPIO_SERVO, 0);
            usleep(20000 - dutyCicle);
        }
        usleep(2000000);
        grades = 90;
        for (int hz = 1; hz <= 50; hz++){
            dutyCicle = (grades*2000.0/180.0)+500;
            gpio_set_level(GPIO_SERVO, 1);
            usleep(dutyCicle);
            gpio_set_level(GPIO_SERVO, 0);
            usleep(20000 - dutyCicle);
        }
        usleep(2000000);
        grades = 180;
        for (int hz = 1; hz <= 50; hz++){
            dutyCicle = (grades*2000.0/180.0)+500;
            gpio_set_level(GPIO_SERVO, 1);
            usleep(dutyCicle);
            gpio_set_level(GPIO_SERVO, 0);
            usleep(20000 - dutyCicle);
        }
        usleep(2000000);

        grades = 90;
        for (int hz = 1; hz <= 50; hz++){
            dutyCicle = (grades*2000.0/180.0)+500;
            gpio_set_level(GPIO_SERVO, 1);
            usleep(dutyCicle);
            gpio_set_level(GPIO_SERVO, 0);
            usleep(20000 - dutyCicle);
        }
        usleep(2000000);

        grades = 0;
        for (int hz = 1; hz <= 50; hz++){
            dutyCicle = (grades*2000.0/180.0)+500;
            gpio_set_level(GPIO_SERVO, 1);
            usleep(dutyCicle);
            gpio_set_level(GPIO_SERVO, 0);
            usleep(20000 - dutyCicle);
        }
        usleep(2000000);

    }
    /*
        // Mover el servo a 0 grados
        gpio_set_level(GPIO_SERVO, 1);
        vTaskDelay(20); // Tiempo para el pulso
        gpio_set_level(GPIO_SERVO, 0);
        vTaskDelay(1980); // Tiempo total de pulso 2000ms (20ms - 2ms = 18ms para 0 grados)

        vTaskDelay(2000); // Espera 2 segundos

        // Mover el servo a 180 grados
        gpio_set_level(GPIO_SERVO, 1);
        vTaskDelay(70); // Tiempo para el pulso
        gpio_set_level(GPIO_SERVO, 0);
        vTaskDelay(1930); // Tiempo total de pulso 2000ms (70ms - 2ms = 68ms para 180 grados)
        
        vTaskDelay(2000); // Espera 2 segundos
    

    */
   
}