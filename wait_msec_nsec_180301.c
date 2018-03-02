#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "wait_msec_nsec_180301.h"

/*
 * v0.3 2017/03/01
 *   - add Wait_about200usec()
 *   - remove Wait_microsecond()
 *   - remove Wait_nanosecond()
 * v0.2 2017/03/01
 *   - add Wait_microsecond()
 *   - add [MICRO_SEC]
 * v0.1 2018/03/01
 *   - add Wait_nanosecond()
 *   - add Wait_millisecond()
 *   - add [MILLI_SEC]
 */

#define MILLI_SEC (1000000)
//#define MICRO_SEC (1000)

void Wait_millisecond(int wait_msec)
{
    int loop;
    struct timespec req = {0, MILLI_SEC};

    for(loop=0; loop < wait_msec; loop++) {
        nanosleep(&req, NULL);
    }
}

/*
void Wait_nanosecond(int wait_nsec)
{
    int loop;
    struct timespec req;

    req.tv_sec = 0;	
    req.tv_nsec = wait_nsec;	
    nanosleep(&req, NULL);
}
*/

/*
void Wait_microsecond(int wait_usec)
{
    int loop;
    struct timespec req;

    req.tv_sec = 0;
    req.tv_nsec = wait_usec * MICRO_SEC;	
    nanosleep(&req, NULL);	
}
*/

void Wait_about200usec(void)
{
    /*
     * This will wait about 200 usec for Raspberry Pi 2 Model B
     * using raspbian Jessie
     * 
     * (1st: 260usec)
     * (2nd and after: 160usec)
     */
    int loop;
    struct timespec req = { 0, 1 };

    nanosleep(&req, NULL);		
}
