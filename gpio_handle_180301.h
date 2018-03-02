#ifndef GPIO_HANDLE_180301_H
#define GPIO_HANDLE_180301_H

#define GPIO_HIGH (true)
#define GPIO_LOW (false)

void gpio_setExport(int index, bool bfOn);
void gpio_setLevel(int index, bool bfHigh);
int gpio_getLevel(int index);
void gpio_setDirection(int index, bool bfOut);

#endif
