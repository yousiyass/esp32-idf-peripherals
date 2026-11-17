#include "uart_driver.h"
#include "driver/uart.h"
#include "esp_log.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static const char *TAG = "UART_DRIVER";

static QueueHandle_t uart_queue = NULL;

static uart_callback_t s_registered_function = NULL;
void uart_callback_save(uart_callback_t new_function) {
    s_registered_function = new_function;
}

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
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 1024 * 2, 1024 * 2, 20, &uart_queue, 0));

    ESP_LOGI(TAG, "UART0 initialized successfully (115200 baud)");
    return ESP_OK;
}

void uart_listening_task(void *pvParameters) {
    uart_event_t event;
    uint8_t temp_buf[128];
    char line_buf[128];
    int line_idx = 0;

    while (1) {
        if (xQueueReceive(uart_queue, (void *)&event, portMAX_DELAY)) {
            switch(event.type) {
                case UART_DATA: {
                    int len = uart_read_bytes(UART_NUM_0, temp_buf, event.size, portMAX_DELAY);
                    for (int i = 0; i < len; i++) {
                        char c = (char)temp_buf[i];

                        if (c == '\n' || c == '\r') {
                            if (line_idx > 0) {
                                line_buf[line_idx] = '\0';
                                ESP_LOGI(TAG, "received line: %s", line_buf);

                                if (s_registered_function != NULL) {
                                    s_registered_function(line_buf);
                                }

                                line_idx = 0;
                            }
                        }
                        else if (line_idx < sizeof(line_buf) - 1) {
                            line_buf[line_idx] = c;
                            line_idx++;
                        }
                    }
                    break;
                }
            
                case UART_FIFO_OVF:
                case UART_BUFFER_FULL:
                    uart_flush_input(UART_NUM_0);
                    xQueueReset(uart_queue);
                    line_idx = 0;
                    break;

                default:
                    break;
            }
        }
    }
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

