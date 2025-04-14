#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "mpu6050.h"

const int I2C_CHIP_ADDRESS = 0x68;
const int I2C_SDA_GPIO = 20;
const int I2C_SCL_GPIO = 21;

void i2c_task(void *p) {
    i2c_init(i2c_default, 400000);
    gpio_set_function(I2C_SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_GPIO, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_GPIO);
    gpio_pull_up(I2C_SCL_GPIO);

    uint8_t buf_write[2];
    buf_write[0] = MPUREG_PWR_MGMT_1;
    buf_write[1] = 1 << 7;
    i2c_write_blocking(i2c_default, I2C_CHIP_ADDRESS, buf_write, 2, false);

    buf_write[0] = MPUREG_ACCEL_CONFIG;
    buf_write[1] = 1 << 3;
    i2c_write_blocking(i2c_default, I2C_CHIP_ADDRESS, buf_write, 2, false);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

int main() {
    stdio_init_all();
    printf("Start RTOS \n");

    xTaskCreate(i2c_task, "i2c task", 4095, NULL, 1, NULL);
    vTaskStartScheduler();

    while (true) {}
}