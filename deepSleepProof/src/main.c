
/**
 * Ejemplo de Deep Sleep con timers.
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#define STORAGE_NAMESPACE "storage"

void task_1(void *pvParameter);
void task_2(void *pvParameter);

// Función para guardar el estado de la tarea en la NVS
void save_task_state(int task_id);

// Función para obtener el estado de la tarea de la NVS
int get_task_state();

// Función para entrar en deep sleep
void enter_deep_sleep(int task_id);
int task_id;

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
        task_id = get_task_state();
        printf("Wake up from deep sleep, returning to task: %d\n", task_id);
        // Aquí puedes restaurar el estado de la tarea y continuar su ejecución
    } else {
        printf("Not a deep sleep wakeup\n");
        // Aquí puedes inicializar las tareas
    }

    // Crear tareas
    // Ejemplo de creación de una tarea
    printf("Creando tareas\n");
    xTaskCreate(task_1, "task1", 2048, (void *) 1, 5, NULL);
    xTaskCreate(task_2, "task2", 2048, (void *) task_id, 5, NULL);
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
                enter_deep_sleep(cont);
            
            if (cont == 20)
            {
                cont = 0;
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
       
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