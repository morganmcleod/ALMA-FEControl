#ifndef IFSWITCHIMPLBASE_H_
#define IFSWITCHIMPLBASE_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2007
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
 * base class for the FrontEnd IF Switch module
 * This will be replaced by auto-generated code in the Control environment.
 *----------------------------------------------------------------------
 */
 
#include "FEHardwareDevice.h"

class IFSwitchImplBase : public FEHardwareDevice {
public:
    IFSwitchImplBase(const std::string &name);
    virtual ~IFSwitchImplBase()
      { shutdown(); }

    virtual void initialize(unsigned long channel, unsigned long nodeAddress);
    virtual void shutdown();

// monitor properties:
    virtual unsigned char switchCartridge();
    virtual unsigned char pol0Sb1Attenuation();
    virtual unsigned char pol0Sb2Attenuation();
    virtual unsigned char pol1Sb1Attenuation();
    virtual unsigned char pol1Sb2Attenuation();
    virtual float pol0Sb1AssemblyTemp();
    virtual float pol0Sb2AssemblyTemp();
    virtual float pol1Sb1AssemblyTemp();
    virtual float pol1Sb2AssemblyTemp();
    virtual bool pol0Sb1TempServoEnable();
    virtual bool pol0Sb2TempServoEnable();
    virtual bool pol1Sb1TempServoEnable();
    virtual bool pol1Sb2TempServoEnable();

// control points:
    virtual void switchCartridge(unsigned char val);
    virtual void pol0Sb1Attenuation(unsigned char val);
    virtual void pol0Sb2Attenuation(unsigned char val);
    virtual void pol1Sb1Attenuation(unsigned char val);
    virtual void pol1Sb2Attenuation(unsigned char val);
    virtual void pol0Sb1TempServoEnable(bool val);
    virtual void pol0Sb2TempServoEnable(bool val);
    virtual void pol1Sb1TempServoEnable(bool val);
    virtual void pol1Sb2TempServoEnable(bool val);

protected:
    virtual void monitorAction(Time *timestamp_p);
    
    enum MonitorControlOffset {
        TEMP_SERVO_ENABLE   = 0x0000,
        ATTENUATION         = 0x0001,
        ASSEMBLY_TEMP       = 0x0002,
        SWITCH_CARTRIDGE    = 0x0010
    };

    unsigned char switchCartridge_value;
    unsigned char pol0Sb1Attenuation_value;
    unsigned char pol0Sb2Attenuation_value;
    unsigned char pol1Sb1Attenuation_value;
    unsigned char pol1Sb2Attenuation_value;
    float pol0Sb1AssemblyTemp_value;
    float pol0Sb2AssemblyTemp_value;
    float pol1Sb1AssemblyTemp_value;
    float pol1Sb2AssemblyTemp_value;
    bool pol0Sb1TempServoEnable_value;
    bool pol0Sb2TempServoEnable_value;
    bool pol1Sb1TempServoEnable_value;
    bool pol1Sb2TempServoEnable_value;

    int switchCartridge_status;
    int pol0Sb1Attenuation_status;
    int pol0Sb2Attenuation_status;
    int pol1Sb1Attenuation_status;
    int pol1Sb2Attenuation_status;
    int pol0Sb1AssemblyTemp_status;
    int pol0Sb2AssemblyTemp_status;
    int pol1Sb1AssemblyTemp_status;
    int pol1Sb2AssemblyTemp_status;
    int pol0Sb1TempServoEnable_status;
    int pol0Sb2TempServoEnable_status;
    int pol1Sb1TempServoEnable_status;
    int pol1Sb2TempServoEnable_status;

    AmbRelativeAddr baseRCA;
    AmbRelativeAddr switchCartridge_RCA;
    AmbRelativeAddr pol0Sb1Attenuation_RCA;
    AmbRelativeAddr pol0Sb2Attenuation_RCA;
    AmbRelativeAddr pol1Sb1Attenuation_RCA;
    AmbRelativeAddr pol1Sb2Attenuation_RCA;
    AmbRelativeAddr pol0Sb1AssemblyTemp_RCA;
    AmbRelativeAddr pol0Sb2AssemblyTemp_RCA;
    AmbRelativeAddr pol1Sb1AssemblyTemp_RCA;
    AmbRelativeAddr pol1Sb2AssemblyTemp_RCA;
    AmbRelativeAddr pol0Sb1TempServoEnable_RCA;
    AmbRelativeAddr pol0Sb2TempServoEnable_RCA;
    AmbRelativeAddr pol1Sb1TempServoEnable_RCA;
    AmbRelativeAddr pol1Sb2TempServoEnable_RCA;

private:
    // forbid copy constructor:
    IFSwitchImplBase(const IFSwitchImplBase &other);
    
    Time lastMonitorTime;
    int monitorPhase;
    
    DECLARE_MONITORS_REGISTRY(IFSwitchImplBase)
    void logMon(bool printHeader = false) const;
};      


#endif /*IFSWITCHIMPLBASE_H_*/
