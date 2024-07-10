#include "esp_sleep.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Definir etiquetas de almacenamiento
#define STORAGE_NAMESPACE "storage"

// Función para guardar el estado de la tarea en la NVS
void save_task_state(int task_id, int task_data) {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        err = nvs_set_i32(my_handle, "task_id", task_id);
        if (err != ESP_OK) {
            printf("Error (%s) setting task_id!\n", esp_err_to_name(err));
        }
        err = nvs_set_i32(my_handle, "task_data", task_data);
        if (err != ESP_OK) {
            printf("Error (%s) setting task_data!\n", esp_err_to_name(err));
        }
        nvs_commit(my_handle);
        nvs_close(my_handle);
    }
}

// Función para obtener el estado de la tarea de la NVS
void get_task_state(int *task_id, int *task_data) {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        err = nvs_get_i32(my_handle, "task_id", task_id);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
            printf("Error (%s) getting task_id!\n", esp_err_to_name(err));
        }
        err = nvs_get_i32(my_handle, "task_data", task_data);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
            printf("Error (%s) getting task_data!\n", esp_err_to_name(err));
        }
        nvs_close(my_handle);
    }
}

// Función de inicialización del ULP (ejemplo simple)
void init_ulp_program() {
    // Aquí se debería cargar y configurar el programa ULP
}

// Función para entrar en deep sleep
void enter_deep_sleep(int task_id, int task_data) {
    save_task_state(task_id, task_data);
    init_ulp_program();
    esp_sleep_enable_ulp_wakeup();
    esp_deep_sleep_start();
}

void app_main(void) {
    // Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Verificar la causa del wakeup
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_ULP) {
        int task_id, task_data;
        get_task_state(&task_id, &task_data);
        printf("Wake up from ULP deep sleep, returning to task: %d with data: %d\n", task_id, task_data);
        // Aquí puedes restaurar el estado de la tarea y continuar su ejecución
    } else {
        printf("Not a ULP deep sleep wakeup\n");
        // Inicializar las tareas y su estado inicial
    }

    // Crear tareas
    xTaskCreate(task_function, "task1", 2048, (void *) 1, 5, NULL);
    // Crea tus otras tareas aquí...

    // Ejemplo de uso de deep sleep en una tarea
    // enter_deep_sleep(1, 100); // Pasar id y datos de la tarea
}

// Función de una tarea de ejemplo
void task_function(void *pvParameter) {
    int task_id = (int) pvParameter;
    int task_data = 0; // Datos de la tarea
    while (1) {
        printf("Task %d running with data %d\n", task_id, task_data);
        task_data++;
        // Simula trabajo
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (task_id == 1 && task_data > 10) { // Condición para entrar en deep sleep
            enter_deep_sleep(task_id, task_data);
        }
    }
}
