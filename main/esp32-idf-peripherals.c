#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "uart_driver.h"

void app_main(void)
{
    uart_driver_init();

    xTaskCreate(uart_listening_task, "uart_task", 4096, NULL, 10, NULL);

    uart_driver_send_str("\r\n--- ESP32 Echo Test Started (Type something and press Enter) ---\r\n");

    uint8_t rx_buffer[128];

    while (1) {
        int len = uart_driver_read_bytes(rx_buffer, sizeof(rx_buffer) - 1, 100);

        if (len > 0) {
            rx_buffer[len] = '\0'; 
            uart_driver_send_str("TX: ");
            uart_driver_send_str((const char *)rx_buffer);
            uart_driver_send_str("\r\n");
        }
    }
}