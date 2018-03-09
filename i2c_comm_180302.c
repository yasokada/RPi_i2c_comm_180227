#include <stdbool.h>
#include "i2c_comm_180302.h"
#include "wait_msec_nsec_180301.h"
#include "i2c_gpio_180309.h"

/*
 * v0.2 2018/03/09
 *   - use [gpio_i2c_180309] functions
 *     + instead of [gpio_handle_180301]
 * v0.1 2018/03/02
 *   - has myDelay()
 *   - has i2c_XXX() moved from other source file
 */

#define BOOL_ACK (false)
#define BOOL_NAK (true)

static void myDelay(void)
{
    Wait_about200usec();
}

void i2c_Setup(void)
{
    i2c_gpio_initSCL();
    i2c_gpio_initSDA();
    i2c_gpio_setSCL_direction(/* bfOut=*/true);
    i2c_gpio_setSDA_direction(/* bfOut=*/true);
}

void i2c_Teardown(void)
{
    i2c_gpio_setSDA_direction(/* bfOut=*/false);
    i2c_gpio_setSCL_direction(/* bfOut=*/false);
    i2c_gpio_terminateSDA();
    i2c_gpio_terminateSCL();
}

void i2c_SendStartCondition(bool withInit)
{
    i2c_gpio_setSDA_direction(/* bfOut=*/true);

    if (withInit) {
        i2c_gpio_setSDA_level(GPIO_HIGH);
        i2c_gpio_setSCL_level(GPIO_HIGH);
        myDelay();
        myDelay();
        myDelay();
    }
    // start condition
    i2c_gpio_setSDA_level(GPIO_LOW);
    myDelay();
    i2c_gpio_setSCL_level(GPIO_LOW);
    myDelay();
}

void i2c_SendStopCondition(void)
{
    i2c_gpio_setSDA_direction(/* bfOut=*/true);

    i2c_gpio_setSCL_level(GPIO_LOW);
    i2c_gpio_setSDA_level(GPIO_LOW);
    myDelay();
    i2c_gpio_setSCL_level(GPIO_HIGH);
    myDelay();
    i2c_gpio_setSDA_level(GPIO_HIGH);
    myDelay();
}

void i2c_SendSlaveAddress(int address_7bit, bool bfRead)
{
    int loop;
    bool bitVal;
    char slvAdr;

    slvAdr = address_7bit;

    i2c_gpio_setSDA_direction(/* bfOut=*/true);

    // 1. slave address
    for(loop=0; loop<7; loop++) { // 7bit
        bitVal = (slvAdr & 0x40);
        i2c_gpio_setSCL_level(GPIO_LOW);
        if (bitVal) {
            i2c_gpio_setSDA_level(GPIO_HIGH);
        } else {
            i2c_gpio_setSDA_level(GPIO_LOW);
        }
        myDelay();
        i2c_gpio_setSCL_level(GPIO_HIGH);
        myDelay();
        slvAdr <<= 1;
    }

    // 2. Read/Write-bit
    i2c_gpio_setSCL_level(GPIO_LOW);
    if (bfRead) {
        i2c_gpio_setSDA_level(GPIO_HIGH);
    } else {
        i2c_gpio_setSDA_level(GPIO_LOW);
    }
    myDelay();
    i2c_gpio_setSCL_level(GPIO_HIGH);
    myDelay();
}

void i2c_SendData(char dataCode)
{
    int loop;
    bool bitVal;

    i2c_gpio_setSDA_direction(/* bfOut=*/true);

    for(loop=0; loop<8; loop++) { // 8bit
        bitVal = (dataCode & 0x80);
        i2c_gpio_setSCL_level(GPIO_LOW);
        if (bitVal) {
            i2c_gpio_setSDA_level(GPIO_HIGH);
        } else {
            i2c_gpio_setSDA_level(GPIO_LOW);
        }
        myDelay();
        i2c_gpio_setSCL_level(GPIO_HIGH);
        myDelay();
        dataCode <<= 1;
    }
}

void i2c_SendAckNak(bool isAck)
{
    i2c_gpio_setSCL_level(GPIO_LOW);
    i2c_gpio_setSDA_direction(/* bfOut=*/true);
    if (isAck) {
        i2c_gpio_setSDA_level(GPIO_LOW);  // ACK
    } else {
        i2c_gpio_setSDA_level(GPIO_HIGH);  // NAK
    }
    myDelay();
    i2c_gpio_setSCL_level(GPIO_HIGH);
    myDelay();
    i2c_gpio_setSCL_level(GPIO_LOW);
}

bool i2c_IsACK(void)
{
    bool pinIsH;

    i2c_gpio_setSCL_level(GPIO_LOW);
    i2c_gpio_setSDA_direction(/* bfOut=*/false);
    myDelay();
    i2c_gpio_setSCL_level(GPIO_HIGH);
    pinIsH = i2c_gpio_isSDA_high();
    myDelay();

    return (pinIsH == BOOL_ACK);
}

char i2c_ReadData(bool isLast)
{
    char code;
    int loop;

    i2c_gpio_setSDA_direction(/* bfOut=*/false);

    code = 0;
    for (loop=0; loop<8; loop++) {
        i2c_gpio_setSCL_level(GPIO_LOW);
        myDelay();
        i2c_gpio_setSCL_level(GPIO_HIGH);
        if (i2c_gpio_isSDA_high()) {
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

// TODO: remove test_clockout_ioin()

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
