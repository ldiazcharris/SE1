#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_err.h"

// Define the structure
typedef struct {
    int id;
    float temperature;
    char name[20];
} sensor_data_t;

// Function to save the structure
void save_sensor_data(const char* key, const sensor_data_t* data) {
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open NVS handle
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }

    // Calculate the size of the structure
    size_t required_size = sizeof(sensor_data_t);

    // Write the structure
    err = nvs_set_blob(my_handle, key, data, required_size);
    if (err != ESP_OK) {
        printf("Error (%s) writing to NVS!\n", esp_err_to_name(err));
    } else {
        // Commit written value
        err = nvs_commit(my_handle);
        if (err != ESP_OK) {
            printf("Error (%s) committing to NVS!\n", esp_err_to_name(err));
        }
    }

    // Close NVS handle
    nvs_close(my_handle);
}

// Function to load the structure
void load_sensor_data(const char* key, sensor_data_t* data) {
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open NVS handle
    err = nvs_open("storage", NVS_READONLY, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }

    // Calculate the size of the structure
    size_t required_size = sizeof(sensor_data_t);

    // Read the structure
    err = nvs_get_blob(my_handle, key, data, &required_size);
    if (err != ESP_OK) {
        printf("Error (%s) reading from NVS!\n", esp_err_to_name(err));
    }

    // Close NVS handle
    nvs_close(my_handle);
}

void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Create a sensor_data_t instance and fill it with some data
    sensor_data_t sensor = {
        .id = 1,
        .temperature = 25.5,
        .name = "Sensor 1"
    };

    // Save the structure in NVS
    save_sensor_data("sensor_data", &sensor);

    // Create another sensor_data_t instance to load the data
    sensor_data_t loaded_sensor;

    // Load the structure from NVS
    load_sensor_data("sensor_data", &loaded_sensor);

    // Print the loaded data
    printf("Loaded data: id=%d, temperature=%.2f, name=%s\n",
           loaded_sensor.id, loaded_sensor.temperature, loaded_sensor.name);
}
