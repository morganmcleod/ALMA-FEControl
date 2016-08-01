/*
 * portable.h
 *
 *  Created on: Jul 29, 2011
 *      Author: mmcleod
 */

#ifndef PORTABLE_H_
#define PORTABLE_H_

#ifdef OSX
    #include <unistd.h>
    #define SLEEP(MSEC) usleep(MSEC * 1000)
#else
    #include <windows.h>
    #include <mmsystem.h>
    #define SLEEP(MSEC) Sleep(MSEC)
    #define GETTIMER() timeGetTime()
#endif

#endif /* PORTABLE_H_ */
