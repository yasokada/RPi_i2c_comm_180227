#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <stdbool.h>
#include <string.h>

/*
 * v0.1 Mar. 01, 2018
 *   - add gpio_setDirection()
 *   - add gpio_getLevel()
 *   - add gpio_setLevel()
 *   - add gpio_setExport()
 *   - add s_zbuf[]
 */


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
