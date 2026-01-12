#ifndef MONCTRLMACROS_H_
#define MONCTRLMACROS_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2011
*
*This library is free software; you can redistribute it and/or
*modify it under the terms of the GNU Lesser General Public
*License as published by the Free Software Foundation; either
*version 2.1 of the License, or (at your option) any later version.
*
*This library is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*Lesser General Public License for more details.
*
*You should have received a copy of the GNU Lesser General Public
*License along with this library; if not, write to the Free Software
*Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
*
*/

/************************************************************************
 * Pre-processor macros to aid implementation of CAN bus device drivers
 *   for the Front End,  LORTM, Compressor, and Power Supply
 *----------------------------------------------------------------------
 */


// Macros which support a naming and usage convention for classes derived from FEHardwareDevice:
// the RCAs are in variables like 'monitorPoint_RCA'
// the FEStatus bytes are like 'monitorPoint_status'
// the 'MON' macros fail if the monitor thread has been told to stop.

/// log a float transaction for NAME
#define LOG_FLOAT(TRANS, NAME, TEXT, VALUE) \
    getLogger().log(TRANS, TEXT, NAME##_RCA, (signed char) NAME##_status, 0, VALUE);

///log an integer transaction for NAME
#define LOG_INT(TRANS, NAME, TEXT, VALUE) \
    getLogger().log(TRANS, TEXT, NAME##_RCA, (signed char) NAME##_status, VALUE, 0.0);

/// log a bool transaction for NAME
#define LOG_BOOL(TRANS, NAME, TEXT, VALUE) \
    getLogger().log(TRANS, TEXT, NAME##_RCA, (signed char) NAME##_status, VALUE ? 1 : 0, 0.0);

/// log a single-byte transaction for NAME
#define LOG_BYTE(TRANS, NAME, TEXT, VALUE) \
    getLogger().log(TRANS, TEXT, NAME##_RCA, (signed char) NAME##_status, (int) VALUE, 0.0);

/// synchronous monitor for a NAME and a TARGET variable and corresponding NAME_status result
#define SYNCMON(NAME, TARGET) \
    NAME##_status = syncMonitorWithRetry(NAME##_RCA, TARGET);

/// synchronous monitor with averagin for a NAME and a TARGET variable and corresponding NAME_status result
#define SYNCMON_AVG(NAME, TARGET, AVERAGE) \
    NAME##_status = syncMonitorAverage(NAME##_RCA, TARGET, AVERAGE);

/// synchronous monitor, log, and return a float, given the parameter NAME and logging TEXT.
#define SYNCMON_LOG_FLOAT(NAME, TEXT) { \
    float target; \
    SYNCMON(NAME, target) \
    if (NAME##_status == FEMC_AMB_ERROR) target = 0.0; \
    LOG_FLOAT(FEMC_LOG_MONITOR, NAME, TEXT, target); \
    return target; }

/// synchronous monitor with averaging, log, and return a float, given the parameter NAME and logging TEXT.
#define SYNCMON_AVG_LOG_FLOAT(NAME, TEXT, AVERAGE) { \
    float target; \
    SYNCMON_AVG(NAME, target, AVERAGE) \
    if (NAME##_status == FEMC_AMB_ERROR) target = 0.0; \
    LOG_FLOAT(FEMC_LOG_MONITOR, NAME, TEXT, target); \
    return target; }

/// synchronous monitor, log, and return an integer TYPE, given the parameter NAME and logging TEXT.
#define SYNCMON_LOG_INT(NAME, TYPE, TEXT) { \
    TYPE target; \
    SYNCMON(NAME, target) \
    if (NAME##_status == FEMC_AMB_ERROR) target = 0; \
    LOG_INT(FEMC_LOG_MONITOR, NAME, TEXT, target); \
    return target; }

/// synchronous monitor, log, and return a bool, given the parameter NAME and logging TEXT.
#define SYNCMON_LOG_BOOL(NAME, TEXT) { \
    bool target; \
    SYNCMON(NAME, target) \
    if (NAME##_status == FEMC_AMB_ERROR) target = false; \
    LOG_BOOL(FEMC_LOG_MONITOR, NAME, TEXT, target); \
    return target; }

/// synchronous monitor, log, and return a byte, given the parameter NAME and logging TEXT.
#define SYNCMON_LOG_BYTE(NAME, TEXT) { \
    unsigned char target; \
    SYNCMON(NAME, target) \
    if (NAME##_status == FEMC_AMB_ERROR) target = 0; \
    LOG_BYTE(FEMC_LOG_MONITOR, NAME, TEXT, target); \
    return target; }

/// skip monitoring and instead return a fixed value.
#define MONRETURN(NAME, TYPE, VALUE) {\
    TYPE target = VALUE;\
    NAME##_status = 0;\
    return target; }

/// synchronous command for a NAME and a VALUE.
#define SYNCCMD(NAME, VALUE) \
    NAME##_status = syncCommand(NAME##_RCA + 0x10000, VALUE);

/// synchronous command and log a float, given the parameter NAME, VALUE, and logging TEXT.
#define SYNCCMD_LOG_FLOAT(NAME, VALUE, TEXT) \
    SYNCCMD(NAME, VALUE) \
    LOG_FLOAT(FEMC_LOG_COMMAND, NAME, TEXT, VALUE)

/// synchronous command and log an integer type, given the parameter NAME, VALUE, and logging TEXT.
#define SYNCCMD_LOG_INT(NAME, VALUE, TEXT) \
    SYNCCMD(NAME, VALUE) \
    LOG_INT(FEMC_LOG_COMMAND, NAME, TEXT, VALUE)

/// synchronous command and log a bool, given the parameter NAME, VALUE, and logging TEXT.
#define SYNCCMD_LOG_BOOL(NAME, VALUE, TEXT) \
    SYNCCMD(NAME, VALUE) \
    LOG_BOOL(FEMC_LOG_COMMAND, NAME, TEXT, VALUE)

/// synchronous command and log a byte, given the parameter NAME, VALUE, and logging TEXT.
#define SYNCCMD_LOG_BYTE(NAME, VALUE, TEXT) \
    SYNCCMD(NAME, VALUE) \
    LOG_BYTE(FEMC_LOG_COMMAND, NAME, TEXT, VALUE)

//---------------------------------------------------------------------------------------

// The "2" versions take the RCA explicitly rather than deriving it from a NAME.
// These were added to support the LORTM which does not return a status byte with each message.

/// log a float transaction for NAME
#define LOG2_FLOAT(TRANS, RCA, TEXT, STATUS, VALUE) \
    getLogger().log(TRANS, TEXT, RCA, STATUS, 0, VALUE);

///log an integer transaction for NAME
#define LOG2_INT(TRANS, RCA, TEXT, STATUS, VALUE) \
    getLogger().log(TRANS, TEXT, RCA, STATUS, VALUE, 0.0);

///log a bool transaction for NAME
#define LOG2_BOOL(TRANS, RCA, TEXT, STATUS, VALUE) \
    getLogger().log(TRANS, TEXT, RCA, STATUS, VALUE, 0.0);

/// synchronous monitor for a NAME and TARGET for a given RCA.  Ignores status result.
#define SYNCMON2(RCA, STATUS, TARGET) { \
    sem_t synchLock; sem_init(&synchLock, 0, 0); \
    STATUS = syncMonitor(RCA, TARGET, synchLock); \
    sem_destroy(&synchLock); }

/// synchronous monitor, log, and return a float, given the parameter NAME and logging TEXT.
#define SYNCMON2_LOG_FLOAT(RCA, TEXT) { \
    float target; \
    FEMC_ERROR feStatus = FEMC_NO_ERROR; \
    SYNCMON2(RCA, feStatus, target) \
    LOG2_FLOAT(FEMC_LOG_MONITOR, RCA, TEXT, feStatus, target); \
    return target; }

/// synchronous monitor, log, and return an integer TYPE, given the parameter NAME and logging TEXT.
#define SYNCMON2_LOG_INT(RCA, TYPE, TEXT) { \
    TYPE target; \
    FEMC_ERROR feStatus = FEMC_NO_ERROR; \
    SYNCMON2(RCA, feStatus, target) \
    LOG2_INT(FEMC_LOG_MONITOR, RCA, TEXT, feStatus, target); \
    return target; }

/// synchronous monitor, log, and return a bool, given the parameter NAME and logging TEXT.
#define SYNCMON2_LOG_BOOL(RCA, TEXT) { \
    unsigned char target; \
    FEMC_ERROR feStatus = FEMC_NO_ERROR; \
    SYNCMON2(RCA, feStatus, target) \
    bool ret(target != 0); \
    LOG2_BOOL(FEMC_LOG_MONITOR, RCA, TEXT, feStatus, ret); \
    return ret; }

/// synchronous monitor, log, and return a float, given the parameter NAME and logging TEXT.
#define SYNCMON2_LOG_FLOAT_CONV(RCA, TEXT, CONV) { \
    unsigned short target; \
    FEMC_ERROR feStatus = FEMC_NO_ERROR; \
    SYNCMON2(RCA, feStatus, target) \
    float val = CONV(target); \
    LOG2_FLOAT(FEMC_LOG_MONITOR, RCA, TEXT, feStatus, val); \
    return val; }

/// synchronous command for a NAME and a RCA.  Ignores status result.
#define SYNCCMD2(RCA, VALUE) \
    syncCommand(RCA, VALUE);

/// synchronous command and log a byte, given the parameter NAME, VALUE, and logging TEXT.
#define SYNCCMD2_LOG_BYTE(RCA, VALUE, TEXT) \
    SYNCCMD2(RCA, VALUE) \
    LOG2_INT(FEMC_LOG_COMMAND, RCA, TEXT, 0, VALUE)

/// synchronous command and log a float, given the parameter NAME, VALUE, and logging TEXT.
#define SYNCCMD2_LOG_FLOAT(RCA, VALUE, TEXT) \
    SYNCCMD2(RCA, VALUE) \
    LOG2_FLOAT(FEMC_LOG_COMMAND, RCA, TEXT, 0, VALUE)

/// synchronous command and log an integer type, given the parameter NAME, VALUE, and logging TEXT.
#define SYNCCMD2_LOG_INT(RCA, VALUE, TEXT) \
    SYNCCMD2(RCA, VALUE) \
    LOG2_INT(FEMC_LOG_COMMAND, RCA, TEXT, 0, VALUE)



#endif /* MONCTRLMACROS_H_ */
