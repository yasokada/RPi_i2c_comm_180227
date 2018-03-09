#ifndef I2C_GPIO_180319_H
#define I2C_GPIO_180319_H

#include <stdbool.h>

void i2c_gpio_initSDA(void);
void i2c_gpio_initSCL(void);
void i2c_gpio_setSDA_level(bool bfHigh);
void i2c_gpio_setSCL_level(bool bfHigh);
bool i2c_gpio_isSDA_high(void);
bool i2c_gpio_isSCL_high(void);
void i2c_gpio_setSDA_direction(bool bfOut);
void i2c_gpio_setSCL_direction(bool bfOut);
void i2c_gpio_terminateSDA(void);
void i2c_gpio_terminateSCL(void);

#endif
