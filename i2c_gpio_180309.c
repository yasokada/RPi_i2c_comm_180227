#include "gpio_RPi_180301.h"
#include "i2c_gpio_180309.h"

/*
 * v0.1 2018/03/09
 *   - add i2c_gpio_initSDA()
 *   - add i2c_gpio_initSCL()
 *   - add i2c_gpio_setSDA_level()
 *   - add i2c_gpio_setSCL_level()
 *   - add i2c_gpio_isSDA_high()
 *   - add i2c_gpio_isSCL_high()
 *   - add i2c_gpio_setSDA_direction()
 *   - add i2c_gpio_setSCL_direction()
 *   - add i2c_gpio_terminateSDA()
 *   - add i2c_gpio_terminateSCL()
 *   - define [GPIO_SDA], [GPIO_SCL]
 */

#define GPIO_SDA (19) // Pin# 35
#define GPIO_SCL (26) // Pin# 37

void i2c_gpio_initSDA(void)
{
    gpio_setExport(GPIO_SDA, true);
}

void i2c_gpio_initSCL(void)
{
    gpio_setExport(GPIO_SCL, true);
}

void i2c_gpio_setSDA_level(bool bfHigh)
{
    if (bfHigh) {
        gpio_setLevel(GPIO_SDA, GPIO_HIGH);
    } else {
        gpio_setLevel(GPIO_SDA, GPIO_LOW);
    }
}

void i2c_gpio_setSCL_level(bool bfHigh)
{
    if (bfHigh) {
        gpio_setLevel(GPIO_SCL, GPIO_HIGH);
    } else {
        gpio_setLevel(GPIO_SCL, GPIO_LOW);
    }
}

bool i2c_gpio_isSDA_high(void)
{
    return gpio_isHigh(GPIO_SDA);
}

bool i2c_gpio_isSCL_high(void)
{
    return gpio_isHigh(GPIO_SCL);
}

void i2c_gpio_setSDA_direction(bool bfOut)
{
    gpio_setDirection(GPIO_SDA, bfOut);
}

void i2c_gpio_setSCL_direction(bool bfOut)
{
    gpio_setDirection(GPIO_SCL, bfOut);
}

void i2c_gpio_terminateSDA(void)
{
    gpio_setExport(GPIO_SDA, false);
}

void i2c_gpio_terminateSCL(void)
{
    gpio_setExport(GPIO_SCL, false);
}
