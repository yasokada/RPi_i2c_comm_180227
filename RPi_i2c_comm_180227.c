#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <stdbool.h>
#include <string.h>
#include "wait_msec_nsec_180301.h"

/*
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
#define GPIO_HIGH (true)
#define GPIO_LOW (false)

// 12345678901234567890123456789012
// /sys/class/gpio/gpioxx/direction
static char s_zbuf[40]; // buffer for sprintf()

void gpio_setExport(int index, bool bfOn)
{
    int wfd;
    
    if (bfOn) {
        wfd = open("/sys/class/gpio/export", O_WRONLY);
    } else {
        wfd = open("/sys/class/gpio/unexport", O_WRONLY);
    }
    sprintf(s_zbuf, "%d", index);
    write(wfd, s_zbuf, (int)strlen(s_zbuf));
    close(wfd);
}

void gpio_setLevel(int index, bool bfHigh)
{
    int wfd;

    sprintf(s_zbuf, "/sys/class/gpio/gpio%d/value", index);
    wfd = open(s_zbuf, O_WRONLY);
    if (bfHigh) {
        write(wfd, "1", 1);
    } else {
        write(wfd, "0", 1);
    }
    close(wfd);
}

int gpio_getLevel(int index)
{
    int rfd;
    char code;

    sprintf(s_zbuf, "/sys/class/gpio/gpio%d/value", index);
    rfd = open(s_zbuf, O_RDONLY);
    if (rfd == -1) {
        return 0; // return as Low level
    }
    read(rfd, &code, 1);
    close(rfd);
    
    return code == '1';
}

void gpio_setDirection(int index, bool bfOut)
{
    int wfd;

    sprintf(s_zbuf, "/sys/class/gpio/gpio%d/direction", index);
    wfd = open(s_zbuf, O_WRONLY);
    if (bfOut) {
        write(wfd, "out", 3);
    } else {
        write(wfd, "in", 2);
    }
    close(wfd);
}

int main(){
    int loop;
    int pinlvl; // pin level

    // 1. output clock at [GPIO_SCL]
    gpio_setExport(GPIO_SCL, /* bfOn=*/true);
    gpio_setDirection(GPIO_SCL, /* bfOut=*/true);        
    for(loop=0; loop<5; loop++) {
        gpio_setLevel(GPIO_SCL, GPIO_HIGH);
        Wait_about200usec();
        gpio_setLevel(GPIO_SCL, GPIO_LOW);
        Wait_about200usec();
    }
    gpio_setExport(GPIO_SDA, /* bfOn=*/false);

    // 2. read at [GPIO05]
    gpio_setExport(5, /* bfOn=*/true);
    pinlvl = gpio_getLevel(5);
    printf("GPIO05:%d\n", pinlvl);
    gpio_setExport(5, /* bfOn=*/false);

    return 0;
}

