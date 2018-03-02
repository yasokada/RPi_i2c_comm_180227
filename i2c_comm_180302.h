#ifndef I2C_COMM_180302_H
#define I2C_COMM_180302_H

void i2c_Setup(void);
void i2c_Teardown(void);
void i2c_SendStartCondition(bool withInit);
void i2c_SendStopCondition(void);
void i2c_SendSlaveAddress(int address_7bit, bool bfRead);
void i2c_SendData(char dataCode);
void i2c_SendAckNak(bool isAck);
bool i2c_IsACK(void);
char i2c_ReadData(bool isLast);

#endif
