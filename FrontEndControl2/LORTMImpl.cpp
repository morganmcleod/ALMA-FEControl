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

#include "LORTMImpl.h"
#include "ColdCartImpl.h"
#include "logger.h"
#include <iomanip>
#include <math.h>
using namespace std;

const double LORTMImpl::c(299792458.0);     // m/s speed of light in vacuum
const double LORTMImpl::wlRefLaser(1556.2); // nm


LORTMImpl::LORTMImpl(unsigned long channel, 
                     unsigned long nodeAddress)
  : LORTMImplBase("LORTM"),
    freqRefLaser_m(c / wlRefLaser) // GHz
{ 
    LORTMImplBase::initialize(channel, nodeAddress);
    resetTuning();
}

void LORTMImpl::getFirmwareInfo(std::string &serialNum,
                                std::string &protocolRev,
                                std::string &firmwareRev)
{
    serialNum = GetSerialNumber();
    protocolRev = GetProtocolRevision();
    firmwareRev = GetFirmwareVersion();
}

void LORTMImpl::getAMBSIInfo(unsigned long &numErrors, unsigned long &numTransactions) const {
    numErrors = AMBSINumErrors_value;
    numTransactions = AMBSINumTransactions_value;   
}

unsigned short LORTMImpl::SystemGetStatus() {
    unsigned short ret = LORTMImplBase::SystemGetStatus();
    if (systemError) {
        char buf[20];
        sprintf(buf, "%d", (int) SystemGetError());
        if (errorList_m.length())
            errorList_m += " ";
        errorList_m += buf;
    }
    return ret;
}

void LORTMImpl::getErrorList(std::string &errorList) {
    errorList = errorList_m;
    if (!errorList.empty())
        LOG(LM_ERROR) << "LORTM errors: " << errorList << endl;
}

void LORTMImpl::SystemClearErrors() {
    LORTMImplBase::SystemClearErrors();
    errorList_m.clear();    
}

bool LORTMImpl::PhaseLockTuningInit(short band, double freqLO1, double freqLO2, double freqFLOOG, short sbLock) {

    // check parameters:
    if (band < 1 || band > 10) {
        LOG(LM_ERROR) << "LORTMImpl::PhaseLockTuningInit band out of range" << endl; 
        return false;
    }
    
    // divide the LO frequencies by the cartridge cold multiplier to get the LORTM output frequencies:
    double coldMult = (double) ColdCartImpl::getMultiplier(band);
    // min and max LORTM sideband frequencies possible:
    const double freqMinSb = 24.0; 
    const double freqMaxSb = 150.0; 
    // min and max LO frequencies possible:
    double minLO = freqMinSb * coldMult;
    double maxLO = freqMaxSb * coldMult;

    // force frequencies into allowed range or equal to zero:
    bool zeroLO1 = false;
    if (freqLO1 < minLO) {
        freqLO1 = 0;
        zeroLO1 = true;
    } else if (freqLO1 > maxLO) {
        LOG(LM_ERROR) << "LORTMImpl::PhaseLockTuningInit freqLO1=" << freqLO1 << " out of range.  Set to" << maxLO << endl;
        freqLO1 = maxLO;
    }    
    bool zeroLO2 = false;
    if (freqLO2 < minLO) {
        freqLO2 = 0;
        zeroLO2 = true;
    } else if (freqLO2 > maxLO) {
        LOG(LM_ERROR) << "LORTMImpl::PhaseLockTuningInit freqLO2=" << freqLO2 << " out of range.  Set to" << maxLO << endl;
        freqLO2 = maxLO;
    }    
    // generate error message if both LO frequencies were forced to zero:
    if (zeroLO1 && zeroLO2)
        LOG(LM_ERROR) << "LORTMImpl::PhaseLockTuningInit freqLO1 and freqLO2 were both below range and set to zero." << endl;

    // Start assigning variables to object state:
    resetTuning();
    band_m = band;
    freqFLOOG_m = freqFLOOG;
    sbLock_m = sbLock;
    
    // Assign freqLSB and freqUSB so that they are in order or that freqUSB is zero:
    double freq1 = freqLO1 / coldMult;
    double freq2 = freqLO2 / coldMult;
    if (freq1 == 0.0) {
        if (freq2 == 0.0) {
            // error message is printed above.
            return false;
        } else {
            freqLSB_m = freq2;
            freqUSB_m = 0.0;
        }
    } else {
        if (freq2 == 0.0) {
            freqLSB_m = freq1;
            freqUSB_m = 0.0;
        } else if (freq1 < freq2) {
            freqLSB_m = freq1;
            freqUSB_m = freq2;
        } else {
            freqLSB_m = freq2;
            freqUSB_m = freq1;
        }
    }

    // add the FLOOG offset:
    if (freqLSB_m != 0.0)
        freqLSB_m += ((sbLock_m == 0) ? freqFLOOG_m : -freqFLOOG_m);
    if (freqUSB_m != 0.0)
        freqUSB_m += ((sbLock_m == 0) ? freqFLOOG_m : -freqFLOOG_m);
        
    // get the Ref2 frequency divide-by value depending on the band under test:
    double ref2DivideBy = getRef2DivideBy(band);   
    
    if (freqUSB_m == 0.0)
        freqRef2_m = 0.0;
    else
        // set the ref2 frequency based on the distance between the requested frequencies and the divide-by value: 
        freqRef2_m = ((freqUSB_m * coldMult) - (freqLSB_m * coldMult)) / ref2DivideBy;

    LOG(LM_INFO) << "LORTMImpl::PhaseLockTuningInit(band=" << band_m << " freqLO1=" << freqLO1 << " freqLO2=" << freqLO2 
                 << ") freqLSB=" << freqLSB_m << " freqUSB=" << freqUSB_m << " ref2DivideBy=" << ref2DivideBy << endl;
    
    //freqRefLaser_m = 192642.675;                  // GHz (from LORTM#4 documentation)
    const double freqRef3 = 0.125;                  // GHz
    
    double freqBeatNote = freqLSB_m + freqRef2_m;   // GHz
    freqSlaveLaser_m = freqRefLaser_m - freqBeatNote; // GHz
    unsigned long freqSlaveMHz = (unsigned long) (freqSlaveLaser_m * 1000.0);

    bool oddMOnly = false;
    
    LOG(LM_INFO) << "LORTMImpl::PhaseLockTuningInit "
                 << " freqRef2=" << freqRef2_m << ", oddMOnly=" << oddMOnly
                 << " freqRefLaser=" << setprecision(9) << freqRefLaser_m << setprecision(6)
                 << " freqRef3=" << freqRef3 << " freqBeatNote=" << freqBeatNote 
                 << " freqSlaveLaser=" << setprecision(9) << freqSlaveLaser_m << setprecision(6)
                 << " freqSlaveMHz=" << freqSlaveMHz << endl;
    
    bool success = false;
    if (!oddMOnly) {
        if (!success) {
            // try M=2 case:  M*Fref1-Fref3 <= 39 GHz
            factorM_m = 2;
            freqRef1_m = (freqBeatNote + freqRef3) / ((double) factorM_m);
            double freqTest = ((double) factorM_m) * freqRef1_m - freqRef3; 
            if (freqTest <= 39.0)
                success = true;
            LOG(LM_INFO) << "PhaseLockTuningInit try M=2 freqRef1=" << freqRef1_m
                         << " freqTest=" << freqTest << " success=" << success << endl;
        }
    }
    if (!success) {
        // try M=5 case: 64.5 GHz <= M*Fref1-Fref3 < 100 GHz
        factorM_m = 5;
        freqRef1_m = (freqBeatNote + freqRef3) / ((double) factorM_m);
        double freqTest = ((double) factorM_m) * freqRef1_m - freqRef3; 
        if (freqTest >= 64.5 && freqTest < 100.0)
            success = true;
        LOG(LM_INFO) << "PhaseLockTuningInit try M=5 freqRef1=" << freqRef1_m
                     << " freqTest=" << freqTest << " success=" << success << endl;
    }
    if (!success) {
        // try M=7 case: 100 GHz <= M*Fref1-Fref3 <= 150 GHz
        factorM_m = 7;
        freqRef1_m = (freqBeatNote + freqRef3) / ((double) factorM_m);
        double freqTest = ((double) factorM_m) * freqRef1_m - freqRef3; 
        if (freqTest >= 100.0 && freqTest <= 150.0)
            success = true;
        LOG(LM_INFO) << "PhaseLockTuningInit try M=7 freqRef1=" << freqRef1_m
                     << " freqTest=" << freqTest << " success=" << success << endl;
    }
    if (success)     
        LORTMImplBase::PhaseLockTuningInit(freqSlaveMHz);
    else
        resetTuning();
    return success;
}

bool LORTMImpl::PhaseLockGetTuning(double *freqLSB, double *freqUSB,
                                   double *freqRef1, double *freqRef2,
                                   double *freqSlaveLaser, short *factorM) const
{
    if (freqLSB)
        *freqLSB = freqLSB_m;
    if (freqUSB)
        *freqUSB = freqUSB_m;
    if (freqRef1)
        *freqRef1 = freqRef1_m;
    if (freqRef2)
        *freqRef2 = freqRef2_m;
    if (freqSlaveLaser)
        *freqSlaveLaser = freqSlaveLaser_m;
    if (factorM)
        *factorM = factorM_m;
    return true;
}

bool LORTMImpl::laserEnable(int laser, bool enable) {
    if (laser < 0 || laser > 3)
        return false;
    LOG(LM_INFO) << "LORTMImpl::laserEnable laser=" << laser << " enable=" << enable << endl;
    LaserISrcEnable(laser, enable);
    return true;
}

bool LORTMImpl::laserSetFrequency(int laser, double freq) {
    if (laser < 1 || laser > 3)
        return false;
    LOG(LM_INFO) << "LORTMImpl::setLaserFrequency laser=" << laser << " freq=" << freq << endl;
    unsigned long freqSlaveMHz = (unsigned long) (freq * 1000.0);
    LaserSetFrequency(laser, freqSlaveMHz);
    return true;
}

bool LORTMImpl::getMonitorLORTMStatus(LORTMStatus_t &target) {
    memset(&target, 0, sizeof(target));
    target.systemState = LORTMImplBase::systemState;
    target.systemWarning = LORTMImplBase::systemWarning;
    target.systemError = LORTMImplBase::systemError;
    target.operationPending = LORTMImplBase::operationPending;
    if (target.systemWarning) {
        target.interlockStatus = LORTMImplBase::SystemGetInterlockStatus_value;
        target.warningCode = LORTMImplBase::SystemGetWarning_value;
    }
    if (target.systemError) {
        target.errorCode = LORTMImplBase::SystemGetError_value;
    }
    target.phaselockLocked = LORTMImplBase::phaselockLocked;
    target.phaselockTuningReady = LORTMImplBase::phaselockTuningReady;
    target.phaselockRFInputReady = LORTMImplBase::phaselockRFInputReady;
    target.selectedLaser = LORTMImplBase::PhaselockGetSelectedLaser_value;
    target.selectedBand = LORTMImplBase::PhaselockGetSelectedBand_value;
    return true;
}

bool LORTMImpl::getMonitorLORTMLasers(LORTMLasers_t &target) {
    memset(&target, 0, sizeof(target));
    target.LaserGetStatus1_value = LaserGetStatus1_value; 
    target.LaserGetStatus2_value = LaserGetStatus2_value;
    target.LaserGetStatus3_value = LaserGetStatus3_value;
    target.LaserGetStatus4_value = LaserGetStatus4_value;
    target.LaserBiasMon1_value = LaserBiasMon1_value;
    target.LaserBiasMon2_value = LaserBiasMon2_value;
    target.LaserBiasMon3_value = LaserBiasMon3_value;
    target.LaserBiasMon4_value = LaserBiasMon4_value;
    target.LaserTempMon1_value = LaserTempMon1_value;
    target.LaserTempMon2_value = LaserTempMon2_value;
    target.LaserTempMon3_value = LaserTempMon3_value;
    target.LaserTempMon4_value = LaserTempMon4_value;
    target.LaserSlowCorrMon_value = LaserSlowCorrMon_value;
    return true;
}

bool LORTMImpl::getMonitorLORTMOther(LORTMOther_t &target) {
    memset(&target, 0, sizeof(target));
    target.LaserTecIMon1_value = LaserTecIMon1_value;
    target.LaserTecIMon2_value = LaserTecIMon2_value;
    target.LaserTecIMon3_value = LaserTecIMon3_value;
    target.LaserTecIMon4_value = LaserTecIMon4_value;
    target.LaserPowMon1_value = LaserPowMon1_value;
    target.LaserPowMon2_value = LaserPowMon2_value;
    target.LaserPowMon3_value = LaserPowMon3_value;
    target.LaserPowMon4_value = LaserPowMon4_value;
    target.PhMixBiasMon1_value = PhMixBiasMon1_value;
    target.PhMixBiasMon2_value = PhMixBiasMon2_value;
    target.PhMixBiasMon3_value = PhMixBiasMon3_value;
    target.PhMixBiasMon4_value = PhMixBiasMon4_value;
    target.RFAGCGainMon_value = RFAGCGainMon_value;
    target.RFPowMon34dB_value = RFPowMon34dB_value;
    target.ExternThermMon_value = ExternThermMon_value;
    target.InternThermMon_value = InternThermMon_value;
    return true;
};

//-----------------------------------------------------------------------------
// private:


double LORTMImpl::getRef2DivideBy(short band) const {
    // get the Ref2 frequency divide-by value depending on the band under test:
    // switch pos refers to the switches in the phase electronics to select the divider.
    //
    // This is set based on the cold multiplication factor, not the WCA.
    // The divide-by factor should be 2x the cold multiplier.
    //

    return 2.0 * ((double) ColdCartImpl::getMultiplier(band));

}

void LORTMImpl::resetTuning() {
    freqFLOOG_m = freqLSB_m = freqUSB_m = freqRef1_m = freqRef2_m = freqSlaveLaser_m = 0.0;
    band_m = factorM_m = 0;
}

