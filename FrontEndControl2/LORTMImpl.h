#ifndef LORTMIMPL_H_
#define LORTMIMPL_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2008
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

#include "FEBASE/LORTMImplBase.h"

/// implementation class for the LORTM module

class LORTMImpl : public LORTMImplBase {
public:
    LORTMImpl(unsigned long channel, unsigned long nodeAddress);
    virtual ~LORTMImpl()
      {}

    void setFreqRefLaser(double freqGHz)
      { freqRefLaser_m = freqGHz; }
    
    void getFirmwareInfo(std::string &serialNum,
                         std::string &protocolRev,
                         std::string &firmwareRev);
    
    void getAMBSIInfo(unsigned long &numErrors, 
                      unsigned long &numTransactions) const;
    
    virtual unsigned short SystemGetStatus();
    
    void getErrorList(std::string &errorList);
    
    virtual void SystemClearErrors();
    
    enum SystemStates {
        STARTUP     = 0,
        INTERLOCK   = 1,
        STANDBY     = 2,
        LOCKING     = 3,
        OPERATIONAL = 4,
        MANUAL      = 5
    };
    
    unsigned char getSystemState() const
      { return LORTMImplBase::systemState; }
    
    bool getInterlockStatus() const
      { return LORTMImplBase::SystemGetInterlockStatus_value; }

    bool getPhaselockStatus() const
      { return LORTMImplBase::phaselockLocked; }
    
    bool getTuningReady() const
      { return (LORTMImplBase::systemState == LOCKING || LORTMImplBase::systemState == OPERATIONAL) && LORTMImplBase::phaselockTuningReady; }
    
    bool getRFInputReady() const
      { return LORTMImplBase::phaselockRFInputReady; }
    
    bool PhaseLockTuningInit(short band, double freqLO1, double freqLO2, double freqFLOOG, short sbLock);
        
    bool PhaseLockGetTuning(double &freqLSB, double &freqUSB,
                            double &freqRef1, double &freqRef2,
                            double *freqBeatNote = NULL, double *freqSlaveLaser = NULL,
                            short *factorM = NULL) const;

    bool laserEnable(int laser, bool enable);

    bool laserSetFrequency(int laser, double freq);
    
// LORTM status and error monitoring:
    struct LORTMStatus_t {
        unsigned char systemState;
        bool systemWarning;
        bool systemError;
        bool operationPending;
        bool interlockStatus;
        short warningCode;
        short errorCode;
        bool phaselockLocked;
        bool phaselockTuningReady;
        bool phaselockRFInputReady;        
        unsigned char selectedLaser;
        unsigned char selectedBand;
    };
    bool getMonitorLORTMStatus(LORTMStatus_t &target);
    
// LORTM laser status monitoring:
    struct LORTMLasers_t {    
        unsigned char LaserGetStatus1_value;
        unsigned char LaserGetStatus2_value;
        unsigned char LaserGetStatus3_value;
        unsigned char LaserGetStatus4_value;
        float LaserBiasMon1_value;
        float LaserBiasMon2_value;
        float LaserBiasMon3_value;
        float LaserBiasMon4_value;
        float LaserTempMon1_value;
        float LaserTempMon2_value;
        float LaserTempMon3_value;
        float LaserTempMon4_value;
        float LaserSlowCorrMon_value;
    };
    bool getMonitorLORTMLasers(LORTMLasers_t &target);
    
// LORTM other low-priority monitor points:
    struct LORTMOther_t {
        float LaserTecIMon1_value;
        float LaserTecIMon2_value;
        float LaserTecIMon3_value;
        float LaserTecIMon4_value;
        float LaserPowMon1_value;
        float LaserPowMon2_value;
        float LaserPowMon3_value;
        float LaserPowMon4_value;
        float PhMixBiasMon1_value;
        float PhMixBiasMon2_value;
        float PhMixBiasMon3_value;
        float PhMixBiasMon4_value;
        float RFAGCGainMon_value;
        float RFPowMon34dB_value;
        float ExternThermMon_value;
        float InternThermMon_value;
    };
    bool getMonitorLORTMOther(LORTMOther_t &target);
    
private:
    static const double wlRefLaser;     ///< reference laser nominal wavelength
    static const double c;              ///< m/s speed of light in vacuum
    
    double freqFLOOG_m;
    double freqLSB_m;
    double freqUSB_m;
    double freqRef1_m;
    double freqRef2_m;
    double freqRefLaser_m;
    double freqBeatNote_m;
    double freqSlaveLaser_m;
    short sbLock_m;
    short band_m;
    short factorM_m;
    
    std::string errorList_m;

    double getRef2DivideBy(short band) const;
    ///< helper to find the reference 2 divide-by value 

    void resetTuning();
    ///< helper to clear the tuning frequency state.
};

#endif /*LORTMIMPL_H_*/
