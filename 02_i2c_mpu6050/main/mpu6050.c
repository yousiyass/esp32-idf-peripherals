#include "mpu6050.h"
#include "esp_log.h"

static const char *TAG = "MPU6050";

esp_err_t mpu6050_parse_accel(const uint8_t raw_data[6], mpu6050_accel_t *accel_out) {
    if (raw_data == NULL || accel_out == NULL) {
        return ESP_ERR_INVALID_ARG;
    } 

    int16_t raw_x = (int16_t)((raw_data[0] << 8) | raw_data[1]);
    int16_t raw_y = (int16_t)((raw_data[2] << 8) | raw_data[3]);
    int16_t raw_z = (int16_t)((raw_data[4] << 8) | raw_data[5]);

    accel_out->x = raw_x / MPU6050_ACCEL_SCALE_2G;
    accel_out->y = raw_y / MPU6050_ACCEL_SCALE_2G;
    accel_out->z = raw_z / MPU6050_ACCEL_SCALE_2G;

    return ESP_OK;
}

esp_err_t mpu6050_init(i2c_master_bus_handle_t bus_handle, mpu6050_dev_t *dev, uint8_t dev_addr) {
    if (bus_handle == NULL || dev == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr,
        .scl_speed_hz = 400000
    };

    esp_err_t err = i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev->i2c_dev);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C cihaz ekleme hatasi: %s", esp_err_to_name(err));
        return err;
    }

    uint8_t reg = MPU6050_REG_WHO_AM_I;
    uint8_t who_am_i = 0;
    err = i2c_master_transmit_receive(dev->i2c_dev, &reg, 1, &who_am_i, 1, 100);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "WHO_AM_I okuma hatasi: %s", esp_err_to_name(err));
        return err;
    }

    if (who_am_i != MPU6050_WHO_AM_I_VAL) {
        ESP_LOGE(TAG, "Beklenmeyen kimlik degeri: 0x%02X", who_am_i);
        return ESP_ERR_NOT_FOUND;
    }

    uint8_t wake_cmd[2] = { MPU6050_REG_PWR_MGMT_1, 0x00};
    err = i2c_master_transmit(dev->i2c_dev, wake_cmd, sizeof(wake_cmd), 100);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "uyandirma komutu gonderilemedi: %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

esp_err_t mpu6050_read_accel(mpu6050_dev_t *dev, mpu6050_accel_t *accel_out) {
    if (dev == NULL || dev->i2c_dev == NULL || accel_out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t reg = MPU6050_REG_ACCEL_XOUT_H;
    uint8_t raw_data[6] = {0};

    esp_err_t err = i2c_master_transmit_receive(dev->i2c_dev, &reg, 1, raw_data, sizeof(raw_data), 100);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ivme verisi okunamadi: %s", esp_err_to_name(err));
        return err;
    }

    return mpu6050_parse_accel(raw_data, accel_out);
}