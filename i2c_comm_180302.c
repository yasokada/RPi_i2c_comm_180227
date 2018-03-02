#include <stdbool.h>
#include "gpio_handle_180301.h"
#include "i2c_comm_180302.h"

/*
 * v0.1 2018/03/02
 *   - has i2c_XXX() moved from other source file
 */

// TODO: 0m > move following to some other place 
#define GPIO_SDA (19) // Pin# 35
#define GPIO_SCL (26) // Pin# 37

#define BOOL_ACK (false)
#define BOOL_NAK (true)

// TODO: 0m > where to define myDelay()?

void i2c_Setup(void)
{
    gpio_setExport(GPIO_SCL, /* bfOn=*/true);
    gpio_setExport(GPIO_SDA, /* bfOn=*/true);

    gpio_setDirection(GPIO_SCL, /* bfOut=*/true);
    gpio_setDirection(GPIO_SDA, /* bfOut=*/true);
}

void i2c_Teardown(void)
{
    gpio_setDirection(GPIO_SDA, /* bfOut=*/false);
    gpio_setDirection(GPIO_SCL, /* bfOut=*/false);

    gpio_setExport(GPIO_SDA, /* bfOn=*/false);	
    gpio_setExport(GPIO_SCL, /* bfOn=*/false);
}

void i2c_SendStartCondition(bool withInit)
{
    gpio_setDirection(GPIO_SDA, /* bfOut=*/true);

    if (withInit) {
        gpio_setLevel(GPIO_SDA, GPIO_HIGH);
        gpio_setLevel(GPIO_SCL, GPIO_HIGH);
        myDelay();
        myDelay();
        myDelay();
    }
    // start condition
    gpio_setLevel(GPIO_SDA, GPIO_LOW);
    myDelay();
    gpio_setLevel(GPIO_SCL, GPIO_LOW);
    myDelay();
}

void i2c_SendStopCondition(void)
{
    gpio_setDirection(GPIO_SDA, /* bfOut=*/true);

    gpio_setLevel(GPIO_SCL, GPIO_LOW);
    gpio_setLevel(GPIO_SDA, GPIO_LOW);
    myDelay();
    gpio_setLevel(GPIO_SCL, GPIO_HIGH);
    myDelay();
    gpio_setLevel(GPIO_SDA, GPIO_HIGH);
    myDelay();
}

void i2c_SendSlaveAddress(int address_7bit, bool bfRead)
{
    int loop;
    bool bitVal;
    char slvAdr;

    slvAdr = address_7bit;

    gpio_setDirection(GPIO_SDA, /* bfOut=*/true);

    // 1. slave address
    for(loop=0; loop<7; loop++) { // 7bit
        bitVal = (slvAdr & 0x40);
        gpio_setLevel(GPIO_SCL, GPIO_LOW);
        if (bitVal) {
            gpio_setLevel(GPIO_SDA, GPIO_HIGH);
        } else {
            gpio_setLevel(GPIO_SDA, GPIO_LOW);
        }
        myDelay();
        gpio_setLevel(GPIO_SCL, GPIO_HIGH);
        myDelay();
        slvAdr <<= 1;
    }

    // 2. Read/Write-bit
    gpio_setLevel(GPIO_SCL, GPIO_LOW);
    if (bfRead) {
        gpio_setLevel(GPIO_SDA, GPIO_HIGH);
    } else {
        gpio_setLevel(GPIO_SDA, GPIO_LOW);
    }
    myDelay();
    gpio_setLevel(GPIO_SCL, GPIO_HIGH);
    myDelay();
}

void i2c_SendData(char dataCode)
{
    int loop;
    bool bitVal;

    gpio_setDirection(GPIO_SDA, /* bfOut=*/true);

    for(loop=0; loop<8; loop++) { // 8bit
        bitVal = (dataCode & 0x80);
        gpio_setLevel(GPIO_SCL, GPIO_LOW);
        if (bitVal) {
            gpio_setLevel(GPIO_SDA, GPIO_HIGH);
        } else {
            gpio_setLevel(GPIO_SDA, GPIO_LOW);
        }
        myDelay();
        gpio_setLevel(GPIO_SCL, GPIO_HIGH);
        myDelay();
        dataCode <<= 1;
    }
}

void i2c_SendAckNak(bool isAck)
{
    gpio_setLevel(GPIO_SCL, GPIO_LOW);
    gpio_setDirection(GPIO_SDA, /* bfOut=*/true);
    if (isAck) {
        gpio_setLevel(GPIO_SDA, GPIO_LOW); // ACK
    } else {
        gpio_setLevel(GPIO_SDA, GPIO_HIGH); // NAK
    }
    myDelay();
    gpio_setLevel(GPIO_SCL, GPIO_HIGH);
    myDelay();
    gpio_setLevel(GPIO_SCL, GPIO_LOW);
}

bool i2c_IsACK(void)
{
    bool pinIsH;

    gpio_setLevel(GPIO_SCL, GPIO_LOW);
    gpio_setDirection(GPIO_SDA, /* bfOut=*/false);
    myDelay();
    gpio_setLevel(GPIO_SCL, GPIO_HIGH);
    pinIsH = gpio_isHigh(GPIO_SDA);
    myDelay();

    return (pinIsH == BOOL_ACK);
}

char i2c_ReadData(bool isLast)
{
    char code;
    int loop;

    gpio_setDirection(GPIO_SDA, /* bfOut=*/false);

    code = 0;
    for (loop=0; loop<8; loop++) {
        gpio_setLevel(GPIO_SCL, GPIO_LOW);
        myDelay();
        gpio_setLevel(GPIO_SCL, GPIO_HIGH);
        if (gpio_isHigh(GPIO_SDA)) {
            code |= 0x01;
        }
        myDelay();
        if (loop < (8-1)) { // other than last digit
            code <<= 1;
        }
    }
        
    if (isLast) {
        i2c_SendAckNak(/* isAck=*/false);
    } else {
        i2c_SendAckNak(/* isAck=*/true);
    }
    
    return code;
}

/*
void test_clockout_ioin(void)
{
    int loop;
    int pinlvl; // pin level

    // 1. output clock at [GPIO_SCL]
    gpio_setExport(GPIO_SCL, true);
    gpio_setDirection(GPIO_SCL, true); // true=bfOut
    for(loop=0; loop<5; loop++) {
        gpio_setLevel(GPIO_SCL, GPIO_HIGH);
        myDelay();
        gpio_setLevel(GPIO_SCL, GPIO_LOW);
        myDelay();
    }
    gpio_setExport(GPIO_SDA, false);

    // 2. read at [GPIO05]
    gpio_setExport(5, true);
    pinlvl = gpio_isHigh(5);
    printf("GPIO05:%d\n", pinlvl);
    gpio_setExport(5, false);	
}
*/
