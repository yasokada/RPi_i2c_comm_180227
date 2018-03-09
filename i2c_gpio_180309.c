#include "gpio_handle_180301.h"
#include "i2c_gpio_180309.h"

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

void i2c_gpio_setSCL_level(bool bfHigh);
bool i2c_gpio_isSDA_high(void);
bool i2c_gpio_isSCL_high(void);
void i2c_gpio_setSDA_direction(bool bfOut);
void i2c_gpio_setSCL_direction(bool bfOut);
void i2c_gpio_terminateSDA(void);
void i2c_gpio_terminateSCL(void);

