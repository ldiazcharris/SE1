// *************** Código para el MASTER*********************************** //

#include <stdio.h>
#include "driver/spi_master.h"
#include "esp_log.h"
#include "utilities.h"

// Definir los pines de SPI
#define MISO_PIN 25
#define MOSI_PIN 23
#define CLK_PIN  19
#define CS_PIN   22

void app_main(void)
{
    spi_device_handle_t spi;

    spi_bus_config_t bus_config = {
        .miso_io_num = MISO_PIN,
        .mosi_io_num = MOSI_PIN,
        .sclk_io_num = CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    spi_device_interface_config_t device_config={
        .mode = 0,                            //Polaridad y fase del reloj
        .clock_speed_hz = 1000000,           //Velocidad del reloj
        .duty_cycle_pos = 128,              // 50% duty cycle
        .spics_io_num = CS_PIN,           //Pin de selección de chip
        .queue_size = 3,                      //Número de transacciones en la cola de SPI
        .cs_ena_posttrans = 3,              // Keep the CS low 3 cycles after transaction
        .pre_cb = NULL,
        .post_cb = NULL,
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0
    };

    //Configurar bus SPI
    spi_bus_initialize(HSPI_HOST, &bus_config, 1);

    //Agregar dispositivos SPI al bus
    spi_bus_add_device(HSPI_HOST, &device_config, &spi);

    //Enviar y recibir datos
    uint8_t tx_data[] = {'H', 'O', 'L', 'A'};

    while (1)
    {

        spi_transaction_t trans = {
            .length = 8 * sizeof(tx_data),
            .tx_buffer = tx_data,
            .rx_buffer = NULL
        };

        spi_device_transmit(spi, &trans);
        printf("Was sent %s\n", tx_data);
        delay(2000);
    }
}
