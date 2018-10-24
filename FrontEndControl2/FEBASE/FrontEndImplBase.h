#ifndef FRONTENDIMPLBASE_H_
#define FRONTENDIMPLBASE_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2006
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
 * base class for the FrontEnd monitor and control module
 * This will be replaced by auto-generated code in the Control environment.
 *----------------------------------------------------------------------
 */
 
#include "AMBSIHardwareDevice.h"

class FrontEndImplBase : public AMBSIHardwareDevice {
public:
    FrontEndImplBase(const std::string &name);
    virtual ~FrontEndImplBase()
      { shutdown(); }

    virtual void initialize(unsigned long channel, unsigned long nodeAddress);
    virtual void shutdown();

// monitor properties:
    virtual std::string AMBSILibraryVersion();
    virtual std::string FEMCFirmwareVersion();
    virtual std::string FEMCFPGAVersion();
    virtual unsigned char FEMCGetESNsFound();
    virtual std::string FEMCGetNextESN(bool reverseBytes = false);
    
    virtual unsigned char specialGetSetupInfo();
    // Requesting this monitor point has the side effect of 
    // initializing communications between the AMBSI1 and the FEMC module.

    virtual unsigned char numEnabledModules();
    virtual unsigned char powerEnableModule(int port);

    virtual unsigned short getNumErrors();
    virtual unsigned short getNextError();

    virtual unsigned char getFEMode();

    virtual void getMonitorTimers(unsigned short &monTimer1,
                                  unsigned short &monTimer2,
                                  unsigned short &monTimer3,
                                  unsigned short &monTimer4,
                                  unsigned short &monTimer5,
                                  unsigned short &monTimer6,
                                  unsigned short &monTimer7,
                                  unsigned short &maxTimerValue);
    //< Get the AMBSI1 monitor timers for the seven phases of the last monitor transaction.
    //< maxTimerValue gives the timer starting value, each timer counts down from that value.
    //< If a timer is zero it means a timeout occurred during the last transaction

// control points:
    virtual void powerEnableModule(int port, unsigned char val);
    virtual void specialExitProgram(bool val);
    virtual void specialReadESNs(bool val);
    virtual void setFEMode(unsigned char val);

protected:
    virtual void monitorAction(Time *timestamp_p);
    
    enum MonitorControlOffset {
        SPECIAL_MONITOR         = 0x20000UL,
        SPECIAL_CONTROL         = 0x21000UL,
        // SPECIAL monitor points:
        GET_AMBSI_VERSION_INFO  = 0x0000,   // get the version info for the AMBSI firmware
        GET_SETUP_INFO          = 0x0001,   // Initialize communications between the AMBSI and ARCOM
        GET_VERSION_INFO        = 0x0002,   // Get the version info for the ARCOM Pegasus firmware
        GET_SPECIAL_MONITOR_RCA = 0x0003,   // Get the range of special monitor RCAs
        GET_SPECIAL_CONTROL_RCA = 0x0004,   // Get the range of special control RCAs
        GET_MONITOR_RCA         = 0x0005,   // Get the range of monitor RCAs
        GET_CONTROL_RCA         = 0x0006,   // Get the range of control RCAs
        GET_PPCOMM_TIME         = 0x0007,   // For debugging:  get 8 bytes response as fast as possible
        GET_FPGA_VERSION_INFO   = 0x0008,   // Get the version info for the FEMC FPGA
        GET_ESNS_FOUND          = 0x000A,   // Get the number of ESNs found in the FE
        GET_ESNS                = 0x000B,   // Get the next ESN from the FE queue
        GET_ERRORS_NUMBER       = 0x000C,   // Get the number of errors in the error queue
        GET_NEXT_ERROR          = 0x000D,   // Get the next error from the error queue
        GET_FE_MODE             = 0x000E,   // Get the FE operating mode (operational, troubleshooting, maintenance)

        GET_MON_TIMERS1         = 0x0020,   // Get AMBSI1 monitor timers for phases 1 through 4 of the last monitor transaction.
        GET_MON_TIMERS2         = 0x0021,   // Get AMBSI1 monitor timers for phases 5 through 7 and the MAX_TIMER value

        // SPECIAL control points:      
        SET_EXIT_PROGRAM        = 0x0000,   // For debugging.
        SET_REBOOT              = 0x0001,   // For debugging.
        SET_FE_MODE             = 0x000E,   // Set the FE operating mode.
        SET_READ_ESN            = 0x000F,   // Tell the FEMC module to rescan the 1wire bus for ESNs.
        // Power distribution module RCAs handled by FrontEnd:
        POWER_NUM_MODULES_ENABLED   = 0xA0A0,
        POWER_ENABLE_MODULE         = 0xA00C
    };
    
    std::string AMBSILibraryVersion_value;
    std::string FEMCFirmwareVersion_value;
    std::string FEMCFPGAVersion_value;
    unsigned char FEMCGetESNsFound_value;
    std::string FEMCGetNextESN_value;
    unsigned char specialGetSetupInfo_value;
    unsigned char numEnabledModules_value;
    int numEnabledModules_status;
    unsigned short numErrors_value;
    int numErrors_status;
    unsigned short nextError_value;
    int nextError_status;
    unsigned char FEMode_value;
    int FEMode_status;

    AmbRelativeAddr numEnabledModules_RCA;
    AmbRelativeAddr powerEnableModule_RCA;
    AmbRelativeAddr numErrors_RCA;
    AmbRelativeAddr nextError_RCA;
    AmbRelativeAddr FEMode_RCA;

private:
    // forbid copy constructor:
    FrontEndImplBase(const FrontEndImplBase &other);
    
    Time lastMonitorTime;
    int monitorPhase;
};      


 
#endif /*FRONTENDIMPLBASE_H_*/
