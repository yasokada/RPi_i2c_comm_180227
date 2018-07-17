#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "i2c_comm_180302.h"
#include "wait_msec_nsec_180301.h"

/* 
 * v1.1 Jul. 17, 2018
 *   - remove wait_clock_stretching_release()
 *   - add Test_sendCommand()
 *   - add send_stopContinuousMeasurement()
 *   - add wait_clock_stretching_release()
 *   - change commands for different slave device
 *   - change address for different slave device (0x44 -> 0x61)
 * ===== branched from [RPi_i2c_comm_180227.c] =====
 * v1.0 Mar. 09, 2018
 *   - include "wait_msec_nsec_180301.h"
 *   - rename [gpio_handle_180309] to [gpio_RPi_180309]
 *   - add [i2c_gpio_180309] to be used in [i2c_comm_180302]
 *     + to separate low level implementations for SDA, SCL
 * v0.9 Mar. 02, 2018
 *   - remove myDelay()
 *   - move i2c_XXX() to [i2c_comm_180302.c]
 * 	 - gpio_handle_180301.h: has gpio_isHigh()
 *     + instead of gpio_getLevel()
 *     + redefine [BOOL_ACL], [BOOL_NAK]
 *   - refactor > remove comments "just in case"
 * v0.8 Mar. 02, 2018 can obtain sensor data
 *   - refactor > i2c_readData() > for() does not have DIR out
 *   - i2c_readData() returns [char] type
 *   - i2c_readData() removes [dstPtr] arg
 *   - refactor > rename to i2c_sendStopCondition()
 *   - refactor > rename to i2c_sendStartCondition()
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

void wait_clock_stretching_release(void);
static void Test_sendCommand(int slvAdr);
static void send_stopContinuousMeasurement(int slvAdr);

static const int kMaxloop_SCL_stretch=1000; // maximum loop to check SCL=LOW

int main(void)
{
    int slvAdr = 0x61; // Slave address
    char vals[10] = {0};
    int idx;

    //test_clockout_ioin();

    // 1. init
    i2c_Setup();

#if 1 // 2018-07-17
	send_stopContinuousMeasurement(slvAdr);
#else
    Test_sendCommand(slvAdr);
#endif

    // 3. wait for measurement
//    Wait_millisecond(15); // 15: arbitrary

    // 4. obtain data
    // read header
//    i2c_SendStartCondition(/* withInit=*/false);
//    i2c_SendSlaveAddress(slvAdr, /*bfRead=*/true);
//    if (i2c_IsACK()) {
//        printf("ACK\n");
//    };
//    for(idx=0; idx<6; idx++) {
//        vals[idx] = i2c_ReadData(/* isLast=*/(idx==5));
//        printf("%d\n", vals[idx]);
//    }
//    i2c_SendStopCondition();

    // 5. finish
    i2c_Teardown();

    return 0;
}

static void Test_sendCommand(int slvAdr)
{
    // 2. start measurement
    // write header
    i2c_SendStartCondition(/* withInit=*/true);
    i2c_SendSlaveAddress(slvAdr, /*bfRead=*/false);
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };
    // send command
    i2c_SendData(0x46); // 0x46: arbitrary
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };

    i2c_SendData(0x00); // 0x00: arbitrary
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };

    i2c_SendData(0x00); // 0x00: arbitrary
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };

    i2c_SendData(0x02); // 0x02: arbitrary
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };

    i2c_SendData(0xE3); // 0xE3: CRC
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };

    i2c_SendStopCondition();	
}

static void send_stopContinuousMeasurement(int slvAdr)
{
    // write header
    i2c_SendStartCondition(/* withInit=*/true);
    i2c_SendSlaveAddress(slvAdr, /*bfRead=*/false);
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };
    // send command
    i2c_SendData(0x06); // Function Code
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };

    i2c_SendData(0x00); // Address MSB
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };

    i2c_SendData(0x37); // Address LSB
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };

    i2c_SendData(0x00); // Content MSB
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };

    i2c_SendData(0x01); // Content LSB
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };

    i2c_SendData(0xF0); // CRC LSB
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };

    i2c_SendData(0x64); // CRC MSB
    if (i2c_clockStretching_IsACK(kMaxloop_SCL_stretch)) {
        printf("ACK\n");
    };

    i2c_SendStopCondition();
}

