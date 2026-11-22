# ESP32-IDF Peripheral Drivers

Modular peripheral drivers written in C using the official **ESP-IDF** framework and **FreeRTOS**. Each module separates low-level hardware bus/register operations from application logic for clean reusability.

## Drivers & Progress

| Module | Protocol | Target / Hardware | Key Concepts | Status |
| :--- | :--- | :--- | :--- | :--- |
| `01_uart_driver` | UART | GPS NMEA Parser | FreeRTOS Event Queues, `strtok_r`, XOR Checksum | Verified (Simulated Data) / Hardware Pending |
| `02_i2c_mpu6050` | I2C | MPU6050 IMU | `i2c_master` Bus/Device API, Burst Read, Struct Pointers | Code Complete / Hardware Pending |
| `03_spi_driver` | SPI | *Upcoming* | Full-Duplex Transactions, CS Management, DMA | Planned |

## Architecture Principles

* **Driver-as-a-Component:** Public headers (`.h`) expose only clean APIs and data structures; register maps, bit manipulation, and parsing logic stay encapsulated in `.c` files.
* **Hardware-Decoupled Parsing:** Data conversion and frame validation functions are separated from bus reads so logic can be verified independently without physical sensors attached.
* **Defensive Error Handling:** Fail-fast `esp_err_t` propagation and null-pointer checks across all driver entry points.
