#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gpio_handle_180301.h"
#include "wait_msec_nsec_180301.h"

/*
 * v0.8 Mar. 02, 2018
 *   - fix bug > i2c_sendAckNak() was incorrect
 *   - fix bug > i2c_readData() > gpio_getLevel() had no arg
 *   - i2c_isACK() does not set GPIO_SDA direction outward
 *   - i2c_setStartCondition() has [withInit] argument
 *   - add i2c_readData()
 *   - add i2c_sendAckNak()
 *   - fix bug > i2c_isACK() did not set direction inward
 * v0.7 Mar. 01, 2018
 *   - add i2c_sendData()
 *   - add i2c_isACK()
 *   - add [BOOL_ACK], [BOOL_NAK]
 *   - add i2c_sendSlaveAddress()
 * v0.6 Mar. 01, 2018
 *   - add i2c_setStopCondition()
 *   - add i2c_setStartCondition()
 *   - move test feature to test_clockout_ioin()
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

#define BOOL_ACK (0)
#define BOOL_NAK (1)

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

void i2c_setStartCondition(bool withInit)
{
    // just in case
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

void i2c_setStopCondition(void)
{
    // just in case
    gpio_setDirection(GPIO_SDA, /* bfOut=*/true);

    gpio_setLevel(GPIO_SCL, GPIO_LOW);
    gpio_setLevel(GPIO_SDA, GPIO_LOW);
    myDelay();
    gpio_setLevel(GPIO_SCL, GPIO_HIGH);
    myDelay();
    gpio_setLevel(GPIO_SDA, GPIO_HIGH);
    myDelay();
}

void i2c_sendSlaveAddress(int address_7bit, bool bfRead)
{
    int loop;
    bool bitVal;
    char slvAdr;

    slvAdr = address_7bit;

    // just in case
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

void i2c_sendData(char dataCode)
{
    int loop;
    bool bitVal;

    // just in case
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

void i2c_sendAckNak(bool isAck)
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

bool i2c_isACK(void)
{
    int pinlvl; // pin level

    gpio_setLevel(GPIO_SCL, GPIO_LOW);
    gpio_setDirection(GPIO_SDA, /* bfOut=*/false);
    myDelay();
    gpio_setLevel(GPIO_SCL, GPIO_HIGH);
    pinlvl = gpio_getLevel(GPIO_SDA);
    myDelay();

    return (pinlvl == BOOL_ACK);
}

void i2c_readData(char *dstPtr, bool isLast)
{
    char code;
    int loop;

    if (dstPtr == NULL) {
        return; // error
    }

    code = 0;
    for (loop=0; loop<8; loop++) {
        // gpio_setLevel(GPIO_SCL, GPIO_LOW);
        if (loop == 0) {
            gpio_setDirection(GPIO_SDA, /* bfOut=*/false);
        }
        gpio_setLevel(GPIO_SCL, GPIO_LOW);
        myDelay();
        gpio_setLevel(GPIO_SCL, GPIO_HIGH);
        if (gpio_getLevel(GPIO_SDA)) { // High
            code |= 0x01;
        }
        myDelay();
        if (loop < (8-1)) { // other than last digit
            code <<= 1;
        }
    }
    
    *dstPtr = code;
    
    if (isLast) {
        i2c_sendAckNak(/* isAck=*/false);
    } else {
        i2c_sendAckNak(/* isAck=*/true);
    }
}

void test_clockout_ioin(void)
{
    int loop;
    int pinlvl; // pin level

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
}

int main()
{
    int slvAdr = 0x44; // Slave address
    char vals[10] = {0};
    int idx;

    //test_clockout_ioin();

    i2c_setup();
    // write header
    i2c_setStartCondition(/* withInit=*/true);
    i2c_sendSlaveAddress(slvAdr, /*bfRead=*/false);
    if (i2c_isACK()) {
        printf("ACK\n");
    };
    // send command
    i2c_sendData(0x2C); // 0x2C: arbitrary
    if (i2c_isACK()) {
        printf("ACK\n");
    };
    i2c_sendData(0x06); // 0x06: arbitrary
    if (i2c_isACK()) {
        printf("ACK\n");
    };

    i2c_setStopCondition();

    // wait for measurement
    Wait_millisecond(15); // 15: arbitrary

    // read header
    i2c_setStartCondition(/* withInit=*/false);
    i2c_sendSlaveAddress(slvAdr, /*bfRead=*/true);
    if (i2c_isACK()) {
        printf("ACK\n");
    };
    for(idx=0; idx<6; idx++) {
        i2c_readData(&vals[idx], /* isLast=*/(idx==5));
        printf("%d\n", vals[idx]);
    }

    i2c_setStopCondition();

    i2c_teardown();

    return 0;
}

