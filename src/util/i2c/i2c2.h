#ifndef UTIL_I2C_I2C2_H
#define UTIL_I2C_I2C2_H

#include "driver/i2c.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

void i2c_master_setup(i2c_port_t num, gpio_num_t sda, gpio_num_t scl, uint32_t freq);
void i2c_release(i2c_port_t num);
esp_err_t i2c_master_cmd_begin_safe(i2c_port_t i2c_num, i2c_cmd_handle_t cmd_handle, TickType_t ticks_to_wait);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: UTIL_I2C_I2C2_H */
