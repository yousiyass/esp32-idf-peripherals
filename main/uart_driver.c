#include "uart_driver.h"
#include "driver/uart.h"
#include "esp_log.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "UART_DRIVER";

esp_err_t uart_driver_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 1024, 1024 * 2, 0, NULL, 0));

    ESP_LOGI(TAG, "UART0 basariyla baslatildi (115200 baud)");
    return ESP_OK;
}

void uart_driver_send_str(const char *str) {
    if (str != NULL) {
        uart_write_bytes(UART_NUM_0, str, strlen(str));
    }
}

int uart_driver_read_bytes(uint8_t *buf, size_t max_len, uint32_t timeout_ms) {
    if (buf == NULL || max_len == 0) {
        return -1;
    }
    return uart_read_bytes(UART_NUM_0, buf, max_len, pdMS_TO_TICKS(timeout_ms));
}

