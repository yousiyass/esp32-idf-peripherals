#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "esp_err.h"
#include <stddef.h>

esp_err_t uart_driver_init(void);
void uart_driver_send_str(const char *str);
int uart_driver_read_bytes(uint8_t *buf, size_t max_len, uint32_t timeout_ms);

void uart_listening_task(void *pvParameters);

#endif 