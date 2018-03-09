RPi_i2c_comm: RPi_i2c_comm_180227.c
	gcc -Wall -v RPi_i2c_comm_180227.c gpio_RPi_180301.c wait_msec_nsec_180301.c i2c_comm_180302.c i2c_gpio_180309.c -o RPi_i2c_comm_180227
