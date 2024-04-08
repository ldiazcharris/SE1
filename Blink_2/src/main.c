

//*
// I2C Practica SE_2

// *************** Código para el MASTER*********************************** //

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

// Parámetros de configuración para la comunicación I2C
#define SLAVE_ADDR          0x32   
#define MASTER_PORT         I2C_NUM_0 
#define MASTER_SDA          21   
#define MASTER_SCL          22         
#define MASTER_FREQ_HZ      100000     
     
// Función de inicialización del controlador I2C para modo Master
void i2c_master_init() {

    i2c_config_t i2c_master_config = {

    .mode = I2C_MODE_MASTER,
    .sda_io_num = MASTER_SDA,
    .scl_io_num = MASTER_SCL,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = MASTER_FREQ_HZ,
    };
	
    ESP_ERROR_CHECK(i2c_param_config(MASTER_PORT, &i2c_master_config));
    ESP_ERROR_CHECK(i2c_driver_install(MASTER_PORT, i2c_master_config.mode, 0, 0, 0));
}

void app_main() {

    i2c_master_init();
                        //    'H'  'O'   'L',   'A'
    const uint8_t data[4] = {0x48, 0x4F, 0x4C, 0x41}; 
    
    while(1) {

        // Se transmite la información al SLAVE
        // Parámetros que recibe la función i2c_master_write_to_device()
        //                        (i2c_num, device_address, *write_buffer, write_size, ticks_to_wait)
        i2c_master_write_to_device(MASTER_PORT, SLAVE_ADDR, data, (size_t)sizeof(data), 100/portTICK_PERIOD_MS);
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
}
//*/


/*


// *************** Código para el SLAVE*********************************** //

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include <stdlib.h>
#include <string.h>

// Parámetros de configuración para la comunicación I2C
#define SLAVE_RX_BUF        1024
#define SLAVE_ADDR          0x32
#define SLAVE_SCL           22
#define SLAVE_SDA           21


// Función de inicialización del controlador I2C para modo Slave
void i2c_slave_init() {

    i2c_config_t i2c_slave_config = {

    .mode = I2C_MODE_SLAVE,
    .sda_io_num = SLAVE_SDA,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = SLAVE_SCL,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .slave.addr_10bit_en = 0,
    .slave.slave_addr = SLAVE_ADDR,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_slave_config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, i2c_slave_config.mode, SLAVE_RX_BUF*2, 0, 0));
}

void app_main() {

    i2c_slave_init();
    
    size_t size = 4;
    uint8_t data[4];
    int len = 0;

    bzero(data, size);
    
    while(1) {
        
        // Se recibe eternamente información del Master
        len = i2c_slave_read_buffer(I2C_NUM_0, data, size, portMAX_DELAY);
        if(len > 0)
            printf("Received data: %c, %c, %c, %c\n", data[0], data[1], data[2], data[3]);
        
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

/*

#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


#define GPIO_SERVO 18 // Puedes utilizar cualquier pin GPIO de la ESP32

void app_main() {
    // Configurar el pin GPIO para controlar el servo
   
   gpio_set_direction(GPIO_SERVO, GPIO_MODE_OUTPUT);
   gpio_set_pull_mode(GPIO_SERVO, GPIO_PULLDOWN_ONLY);

    uint16_t grades; 
    float dutyCicle;
    const int tMaxGrades = 180;
    const int tMinGrades = 500;
    const float period = 2000.0;
    uint8_t hz;
    int time;
    int cont;
    while (1) {

        grades = 0;
        time = 15000;
        cont = 1;
	
        for (hz = 1; hz <= 50; hz++){
            dutyCicle = (grades*(period/(const float)tMaxGrades))+tMinGrades;
            printf("%i - %i - %i\n", (int)dutyCicle, (int)(period - dutyCicle), cont);
            gpio_set_level(GPIO_SERVO, 1);
            usleep(dutyCicle);
            gpio_set_level(GPIO_SERVO, 0);
            usleep(2000 - dutyCicle);
            cont++;
        }
        sleep(2);

        grades = 180;
        
	
        for (hz = 1; hz <= 50; hz++){
            dutyCicle = (grades*(period/(const float)tMaxGrades))+tMinGrades;
            printf("%i - %i - %i\n", (int)dutyCicle, (int)(period - dutyCicle), cont);
            gpio_set_level(GPIO_SERVO, 1);
            usleep(dutyCicle);
            gpio_set_level(GPIO_SERVO, 0);
            usleep(2000 - dutyCicle);
            cont++;
        }
        sleep(2);
        */
        
/*


        grades = 90;
        for (int hz = 1; hz <= 50; hz++){
            dutyCicle = (grades*(2000.0/(const float)tMaxGrades))+tMinGrades;
            printf("%i - %i\n", (int)dutyCicle, (int)(period - dutyCicle));
            gpio_set_level(GPIO_SERVO, 1);
            usleep((int)dutyCicle);
            gpio_set_level(GPIO_SERVO, 0);
            usleep((int)(period - dutyCicle));
        }
        sleep(2);
        grades = 180;
        for (int hz = 1; hz <= 50; hz++){
            dutyCicle = (grades*period/tMaxGrades)+tMinGrades;
            printf("%f\n", dutyCicle);
            gpio_set_level(GPIO_SERVO, 1);
            usleep((int)dutyCicle);
            gpio_set_level(GPIO_SERVO, 0);
            usleep((int)(period - dutyCicle));
        }
        sleep(2);

        grades = 90;
        for (int hz = 1; hz <= 50; hz++){
            dutyCicle = (grades*period/tMaxGrades)+tMinGrades;
            printf("%f\n", dutyCicle);
            gpio_set_level(GPIO_SERVO, 1);
            usleep((int)dutyCicle);
            gpio_set_level(GPIO_SERVO, 0);
            usleep((int)(period - dutyCicle));
        }
        sleep(2);
        
    }   
    
    
}
*/

/*
#include <stdio.h>
#include "driver/uart.h"

void uart_init() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, 256, 0, 0, NULL, 0);
}

void send_at_command(const char *command) {
    uart_write_bytes(UART_NUM_1, command, strlen(command));
    uart_write_bytes(UART_NUM_1, "\r", 1);
}

void app_main() {
    uart_init();
    send_at_command("AT");
}

*/