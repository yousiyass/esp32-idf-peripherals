#ifndef MPU6050_H
#define MPU6050_H

#include "driver/i2c_master.h"

#include <stdint.h>
#include "esp_err.h"

#define MPU6050_I2C_ADDR 0x68
#define MPU6050_REG_WHO_AM_I 0x75
#define MPU6050_WHO_AM_I_VAL 0x68
#define MPU6050_REG_PWR_MGMT_1 0x6B
#define MPU6050_REG_ACCEL_XOUT_H 0x3B
#define MPU6050_ACCEL_SCALE_2G 16384.0f

typedef struct {
    i2c_master_dev_handle_t i2c_dev;
}mpu6050_dev_t;

typedef struct {
    float x;
    float y;
    float z;
}mpu6050_accel_t;

esp_err_t mpu6050_init(i2c_master_bus_handle_t bus_handle, mpu6050_dev_t *dev, uint8_t dev_addr);
esp_err_t mpu6050_parse_accel(const uint8_t raw_data[6], mpu6050_accel_t *accel_out);
esp_err_t mpu6050_read_accel(mpu6050_dev_t *dev, mpu6050_accel_t *accel_out);

#endif