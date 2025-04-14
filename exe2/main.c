#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "mpu6050.h"

#define MPU_ADDR        0x68
#define SDA_PIN         20
#define SCL_PIN         21
#define WHO_AM_I_REG    0x75
#define INT_ENABLE_REG  0x38
#define I2C_FREQ        400000

static uint8_t mpu_read_reg(i2c_inst_t *i2c, uint8_t reg) {
    uint8_t val;
    i2c_write_blocking(i2c, MPU_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c, MPU_ADDR, &val, 1, false);
    return val;
}

void i2c_task(void *p) {
    i2c_init(i2c_default, I2C_FREQ);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    uint8_t who = mpu_read_reg(i2c_default, WHO_AM_I_REG);
    printf("WHOAMI: 0x%X\n", who);

    uint8_t inten = mpu_read_reg(i2c_default, INT_ENABLE_REG);
    printf("INT_ENABLE: 0x%X\n", inten);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

int main() {
    stdio_init_all();
    printf("Start RTOS\n");

    xTaskCreate(i2c_task, "i2c task", 4096, NULL, 1, NULL);
    vTaskStartScheduler();

    while (true) {}
}