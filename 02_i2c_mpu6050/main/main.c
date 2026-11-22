#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

#include "driver/i2c_master.h"
#include "mpu6050.h"

#define I2C_MASTER_PORT     I2C_NUM_0
#define I2C_MASTER_SDA_IO   GPIO_NUM_21
#define I2C_MASTER_SCL_IO   GPIO_NUM_22

static const char *TAG = "MAIN";

void app_main(void) {
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = I2C_MASTER_PORT,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true
    };

    i2c_master_bus_handle_t bus_handle = NULL;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));

    mpu6050_dev_t sensor = {0};
    mpu6050_accel_t accel = {0};

    esp_err_t err = mpu6050_init(bus_handle, &sensor, MPU6050_I2C_ADDR);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "MPU6050 baslatilamadi: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "MPU6050 hazir, okuma basliyor...");

    while (1) {
        err = mpu6050_read_accel(&sensor, &accel);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Accel -> X: %.2f g | Y: %.2f g | Z: %.2f g",
                accel.x, accel.y, accel.z);
        }
        else {
            ESP_LOGW(TAG, "okuma hatasi: %s", esp_err_to_name(err));
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}