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
	#define GETTIMER() 0 // implement based on gettimeofday()
#endif /* OSX */

#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
    #include <mmsystem.h>
    #define SLEEP(MSEC) Sleep(MSEC)
    #define GETTIMER() timeGetTime()
#endif /* _WIN32      */
#endif /* PORTABLE_H_ */
