#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "uart_driver.h"
#include <string.h>
#include "errno.h"
#include <stdlib.h>

double parse_safe_double(const char *data) {
    if (data == NULL) return -9999;
    
    char *error_indicator;   
    errno = 0;
    double value = strtod(data, &error_indicator);

    if (error_indicator == data) {
        printf("There are no numbers in the incoming data.");
        return -9999;
    }
    else if (*error_indicator != '\n' && *error_indicator != '\r' && *error_indicator != '\0') {
        printf("There are unnecessary characters in the number.");
        return -9999;
    }
    else if (errno == ERANGE) {
        printf("overflow condition...");
        return -9999;
    }
    else {
        return value;
    }
}

void parse_gps_data(char *incoming_text) {
    char *saveptr;
    char *piece = strtok_r(incoming_text, ",", &saveptr);  //for example incoming_text: "$GPS,40.123,29.456"
    if ((piece != NULL) && (strcmp(piece, "$GPS") == 0)) {
        piece = strtok_r(NULL, ",", &saveptr);
        double latitude = parse_safe_double(piece);

        piece = strtok_r(NULL, ",", &saveptr);
        double longitude = parse_safe_double(piece);

        printf("in the incoming data latitude: %lf, longitude: %lf\n", latitude, longitude);
    }
}

void app_main(void)
{
    uart_callback_save(parse_gps_data);

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