
/**
 * Ejemplo de Deep Sleep con timers.
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#define STORAGE_NAMESPACE "storage"

#define NVS_NAME    "DSleep"
#define KEY_CAVA    "cava_save"



void task_1(void *pvParameter);
void task_2(void *pvParameter);
void task_3(void *pvParameter);
void task_4(void *pvParameter);

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

typedef enum {
    WAKEUP_FROM_SET = 0,
    WAKEUP_FROM_SLEEP
    
} wakeup_type_t;

typedef enum{
BUSSY_CAVA = 0,
FREE_CAVA,
Def_CAVA
}occupancy_t;

typedef struct
{
    double lat;
    double lon;
    char time[10];
    char date[7];
    NMEA_state_t NMEA_state;
} GNSSData_t;


// Analizar Si la posición GNSSData_t puede ser un arreglo de 10 valores. 
typedef struct
{
    occupancy_t occupancy;
    GNSSData_t position;
    int task_id;
} CAVA_DATA_t;

static CAVA_DATA_t cava_data_saved;
static wakeup_type_t wakeup_type = WAKEUP_FROM_SET;

// Función para guardar el estado de la tarea en la NVS
void save_task_state(int task_id);

// Función para obtener el estado de la tarea de la NVS
int get_task_state();

// Función para entrar en deep sleep
void enter_deep_sleep(int task_id);
int task_id = 0;

static esp_err_t save_cava_state(CAVA_DATA_t * cava_data);
static esp_err_t get_cava_state(CAVA_DATA_t * cava_data);
static esp_err_t enter_deep_sleep_2(CAVA_DATA_t * cava_data);

void app_main(void) {
    // Inicializar NVS (Non-Volatile Storage)
    printf("Inicializando NVS \n");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Verificar la causa del wakeup
    printf("Verificando causa del wakeup...\n");
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_TIMER) {
        wakeup_type = WAKEUP_FROM_SLEEP;
        get_cava_state(&cava_data_saved);
        printf("Wake up from deep sleep, returning to task: %d\n", task_id);
        // Aquí puedes restaurar el estado de la tarea y continuar su ejecución
        //xTaskResumeAll();
        xTaskCreate(task_1, "task1", 2048, (void *) 1, 5, NULL);
        xTaskCreate(task_2, "task2", 2048, (void *) task_id, 5, NULL);
        xTaskCreate(task_4, "task4", 2048, (void *) &cava_data_saved, 5, NULL);
    } else {
        wakeup_type = WAKEUP_FROM_SET;
        printf("Not a deep sleep wakeup\n");
        // Aquí puedes inicializar las tareas
        printf("Creando tareas\n");
        xTaskCreate(task_1, "task1", 2048, (void *) 1, 5, NULL);
        xTaskCreate(task_2, "task2", 2048, (void *) task_id, 5, NULL);
        xTaskCreate(task_3, "task3", 2048, (void *) 3, 5, NULL);
    }

    // Crear tareas
    // Ejemplo de creación de una tarea
    
    // Crea tus otras tareas aquí...

    // Ejemplo de uso de deep sleep en una tarea
    // enter_deep_sleep(1);
}

// Función de una tarea de ejemplo
void task_1(void *pvParameter) {
    int task_id = (int) pvParameter;
    while (1) {
        printf("Task %d running\n", task_id);
        // Simula trabajo
        vTaskDelay(pdMS_TO_TICKS(1000));
        
    }
}

void task_2(void *pvParameter) {
    int task_id = (int) pvParameter;
    //int32_t cont = 0;
    int cont = task_id;
    while (1) {
        
        while(cont >= 0){
            
            printf("Task %d running. Counter: %d\n", task_id, cont);
            cont++;
            if(cont == 11)
            {
                cava_data_saved.position.lat = 10.111111;
                cava_data_saved.position.lon = -74.111111;
                cava_data_saved.task_id = 4;
                enter_deep_sleep_2(&cava_data_saved);
            }
            
            if (cont == 5)
            {
                cava_data_saved.position.lat = 0.000;
                cava_data_saved.position.lon = 0.000;
                //cont = 0;
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
       
    }
}

void task_3(void *pvParameter) {
    int task_id = (int) pvParameter;
    pvParameter += 1;
    while (1) {
        printf("Task %d running\n", task_id);
        // Simula trabajo
        xTaskCreate(task_4, "task3", 2048, (void *) pvParameter, 5, NULL);
        vTaskDelay(pdMS_TO_TICKS(1000));
        vTaskDelete(NULL);
    }
}

void task_4(void *pvParameter) {
    //int task_id = (int) pvParameter;
    int cont = 0;
    CAVA_DATA_t cava_data = cava_data_saved;
    while (1) {
        printf("Task %d running. lat: %lf, lng: %lf\n", cava_data.task_id, cava_data.position.lat, cava_data.position.lon);
        // Simula trabajo
        //printf("Task 4 runing. Counter: %d\n", cont);
        cont++;

        if (cont == 5)
            cont = 0;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


// Función para guardar el estado de la tarea en la NVS
void save_task_state(int task_id) {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        err = nvs_set_i32(my_handle, "task_id", task_id);
        if (err != ESP_OK) {
            printf("Error (%s) setting task_id!\n", esp_err_to_name(err));
        }
        nvs_commit(my_handle);
        nvs_close(my_handle);
    }
}

// Función para obtener el estado de la tarea de la NVS
int get_task_state() {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    int32_t task_id = 0;
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        err = nvs_get_i32(my_handle, "task_id", &task_id);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
            printf("Error (%s) getting task_id!\n", esp_err_to_name(err));
        }
        nvs_close(my_handle);
    }
    return task_id;
}

// Función para entrar en deep sleep
void enter_deep_sleep(int task_id) {
    save_task_state(task_id);
    // Configurar el tiempo de sleep
    esp_sleep_enable_timer_wakeup(1000000 * 10);  // 10 segundos
    esp_deep_sleep_start();
}


static esp_err_t save_cava_state(CAVA_DATA_t * cava_data) 
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAME, NVS_READWRITE, &nvs_handle);

    if (ESP_OK == err) 
    {
        err = nvs_set_blob(nvs_handle, KEY_CAVA, (const void *) cava_data, sizeof(CAVA_DATA_t));
        if (ESP_OK == err)  
            nvs_commit(nvs_handle);
        else
            return err; 
    }
    else 
        return err; 

    nvs_close(nvs_handle);
    return err; 
}

// Función para recuperar el estado de la tarea de la NVS
static esp_err_t get_cava_state(CAVA_DATA_t * cava_data) 
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAME, NVS_READWRITE, &nvs_handle);

    size_t len = sizeof(CAVA_DATA_t);
    
    if (ESP_OK == err) 
        err = nvs_get_blob(nvs_handle, KEY_CAVA, (void *) cava_data, &len);
    else 
        return err;

    nvs_close(nvs_handle);

    return err; 
}

// Función para entrar en deep sleep
static esp_err_t enter_deep_sleep_2(CAVA_DATA_t * cava_data) {
    esp_err_t err = save_cava_state(cava_data);
    esp_sleep_enable_timer_wakeup(1000000 * 10);  // Tiempo de sleep: 10 segundos
    //wakeup_type = WAKEUP_FROM_SLEEP;
    //lcd_off();
    if (ESP_OK == err)
        esp_deep_sleep_start();
    
    return err;    
}
