#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "uart_driver.h"
#include <string.h>
#include "errno.h"
#include <stdlib.h>
#include <stdbool.h>
#include "esp_log.h"

uint8_t checksum_calculator(const char *incoming_data, int data_length) {
    uint8_t calculated = 0;
    for (int i = 0; i < data_length; i++) {
        calculated ^= incoming_data[i];
    }
    return calculated;
}

bool is_valid_packet(char *incoming_packet) {
    char *star_pointer = strchr(incoming_packet, '*');
    if (star_pointer != NULL) {
        *star_pointer = '\0';
        if (incoming_packet == NULL || incoming_packet[0] != '$') return false;
        char *original_data = incoming_packet + 1;
        char *checksum_data = star_pointer + 1;

        if (*checksum_data == '\0') {
            return false;
        }

        uint8_t incoming_checksum = (uint8_t)strtol(checksum_data, NULL, 16);
        uint8_t our_checksum_calculation = checksum_calculator(original_data, strlen(original_data));

        ESP_LOGD("GPS_DEBUG", "Incoming: 0x%02X | Calculated: 0x%02X", incoming_checksum, our_checksum_calculation);
        if (incoming_checksum == our_checksum_calculation) {
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}

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

    if (!is_valid_packet(incoming_text)) { ESP_LOGW("GPS", "Invalid package!"); return; }

    char *saveptr;
    char *piece = strtok_r(incoming_text, ",", &saveptr);  //for example incoming_text: "$GPS,598.5,65.6*70" should true
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

    uart_driver_send_str("\r\n--- ESP32 NMEA GPS Parser Ready ---\r\n");

}