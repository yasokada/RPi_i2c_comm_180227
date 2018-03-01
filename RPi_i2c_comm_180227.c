#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gpio_handle_180301.h"
#include "wait_msec_nsec_180301.h"

/*
 * v0.6 Mar. 01, 2018
 *   - add i2c_teardown()
 *   - add i2c_setup()
 *   - add myDelay()
 * v0.5 Mar. 01, 2018
 *   - move functions related to GPIO to [gpio_handle_180301.c]
 * v0.4 Mar. 01, 2018
 *   - add gpio_getLevel()
 * v0.3 Mar. 01, 2018
 *   - include [wait_msec_nsec_180301.h]
 * v0.2 Mar. 01, 2018
 *   - add s_zbuf[]
 *   - add [GPIO_HIGH], [GPIO_LOW]
 *   - refactor > change gpio_setHigh() to gpio_setLevel()
 * v0.1 Feb. 27, 2018
 *   - test > set High for [GPIO_SCL]
 *   - add gpio_setDirection()
 *   - add gpio_setHigh()
 *   - add gpio_setExport()
*/


#define GPIO_SDA (19) // Pin# 35
#define GPIO_SCL (26) // Pin# 37

void myDelay(void)
{
    Wait_about200usec();
}

void i2c_setup(void)
{
    gpio_setExport(GPIO_SCL, /* bfOn=*/true);
    gpio_setExport(GPIO_SDA, /* bfOn=*/true);

    gpio_setDirection(GPIO_SCL, /* bfOut=*/true);
    gpio_setDirection(GPIO_SDA, /* bfOut=*/true);
}

void i2c_teardown(void)
{
    gpio_setDirection(GPIO_SDA, /* bfOut=*/false);
    gpio_setDirection(GPIO_SCL, /* bfOut=*/false);

    gpio_setExport(GPIO_SDA, /* bfOn=*/false);	
    gpio_setExport(GPIO_SCL, /* bfOn=*/false);
}

int main(){
    int loop;
    int pinlvl; // pin level

#if 1 //
    i2c_setup();
    i2c_teardown();
#else
    // 1. output clock at [GPIO_SCL]
    gpio_setExport(GPIO_SCL, /* bfOn=*/true);
    gpio_setDirection(GPIO_SCL, /* bfOut=*/true);        
    for(loop=0; loop<5; loop++) {
        gpio_setLevel(GPIO_SCL, GPIO_HIGH);
        myDelay();
        gpio_setLevel(GPIO_SCL, GPIO_LOW);
        myDelay();
    }
    gpio_setExport(GPIO_SDA, /* bfOn=*/false);

    // 2. read at [GPIO05]
    gpio_setExport(5, /* bfOn=*/true);
    pinlvl = gpio_getLevel(5);
    printf("GPIO05:%d\n", pinlvl);
    gpio_setExport(5, /* bfOn=*/false);
#endif

    return 0;
}

