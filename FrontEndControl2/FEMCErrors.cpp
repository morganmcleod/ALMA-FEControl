/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2014
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

#include "FEMCErrors.h"
#include <cstdio>

#define ERR_ERROR               0x00 //!< Error in the Error Module
//    #define ERR_CPU                 0x01 //!< Error in the Pegasus Module
#define ERR_PP                  0x02 //!< Error in the Parallel Port Module
#define ERR_CAN                 0x03 //!< Error in the CAN Module
#define ERR_CARTRIDGE           0x04 //!< Error in the Cartridge Module
#define ERR_CARTRIDGE_TEMP      0x05 //!< Error in the Cartridge Temperature Sensor Module
#define ERR_LO                  0x06 //!< Error in the LO Module
#define ERR_PLL                 0x07 //!< Error in the PLL Module
#define ERR_YTO                 0x08 //!< Error in the YTO Module
#define ERR_PHOTOMIXER          0x09 //!< Error in the Photomixer Module
#define ERR_AMC                 0x0A //!< Error in the AMC Module
#define ERR_PA                  0x0B //!< Error in the PA Module
#define ERR_PA_CHANNEL          0x0C //!< Error in the PA Channel Module
#define ERR_POLARIZATION        0x0D //!< Error in the Polarization Module
#define ERR_SIDEBAND            0x0E //!< Error in the Sideband Module
#define ERR_LNA_LED             0x0F //!< Error in the LNA led Module
#define ERR_SIS_HEATER          0x10 //!< Error in the SIS heater Module
#define ERR_SCHOTTKY_MIXER      0x11 //!< Error in the Schottky mixer Module
#define ERR_SIS                 0x12 //!< Error in the SIS Module
#define ERR_SIS_MAGNET          0x13 //!< Error in the SIS Magnet Module
#define ERR_LNA                 0x14 //!< Error in the LNA Module
#define ERR_LNA_STAGE           0x15 //!< Error in the LNA stage Module
#define ERR_POL_SPECIAL_MSGS    0x16 //!< Error in the Polarization Special Messages Module
#define ERR_POL_DAC             0x17 //!< Error in the Polarization DAC
#define ERR_SERIAL_INTERFACE    0x18 //!< Error in the Serial Interface Module
#define ERR_SERIAL_MUX          0x19 //!< Error in the Serial Mux Board
#define ERR_TIMER               0x1A //!< Error in the Timer Module
#define ERR_BIAS_SERIAL         0x1B //!< Error in the Bias Serial Interface Module
#define ERR_LO_SERIAL           0x1C //!< Error in the LO Serial Interface Module
#define ERR_POWER_DISTRIBUTION  0x1D //!< Error in the Power Distribution Module
#define ERR_PD_MODULE           0x1E //!< Error in the PD Module Module
#define ERR_PD_CHANNEL          0x1F //!< Error in the PD Channel Module
#define ERR_PD_SERIAL           0x20 //!< Error in the PD serial interface module
#define ERR_IF_CHANNEL          0x21 //!< Error in the IF Channel Module
#define ERR_IF_SWITCH           0x22 //!< Error in the IF Switch Module
#define ERR_IF_SERIAL           0x23 //!< Error in the IF Switch serial interface module
#define ERR_CRYOSTAT            0x24 //!< Error in the Cryostat Module
#define ERR_TURBO_PUMP          0x25 //!< Error in the Turbo Pump Module
#define ERR_VACUUM_CONTROLLER   0x26 //!< Error in the Vacuum Controller Module
#define ERR_GATE_VALVE          0x27 //!< Error in the Gate Valve Module
#define ERR_SOLENOID_VALVE      0x28 //!< Error in the Solenoid Valve Module
#define ERR_VACUUM_SENSOR       0x29 //!< Error in the Vacuum Sensor Module
#define ERR_CRYOSTAT_TEMP       0x2A //!< Error in the Cryostat Temperature Module
#define ERR_CRYO_SERIAL         0x2B //!< Error in the Cryostat serial interface module
#define ERR_MODULATION_INPUT    0x2C //!< Error in the EDFA modulation input module
#define ERR_PHOTO_DETECTOR      0x2D //!< Error in the EDFA photo detector module
#define ERR_LASER               0x2E //!< Error in the EDFA laser detector module
#define ERR_EDFA                0x2F //!< Error in the EDFA module
#define ERR_OPTICAL_SWITCH      0x30 //!< Error in the Optical Switch module
#define ERR_LPR                 0x31 //!< Error in the LPR module
#define ERR_LPR_TEMP            0x32 //!< Error in the LPR temperature module
#define ERR_LPR_SERIAL          0x33 //!< Error in the LPR serial interface module
#define ERR_MI_DAC              0x34 //!< Error in the Modulation input DAC module
#define ERR_INI                 0x35 //!< Error in the INI file access module
#define ERR_OWB                 0x36 //!< Error in the OWB module
#define ERR_FETIM               0x37 //!< Error in the FETIM module
#define ERR_INTERLOCK           0x38 //!< Error in the FETIM interlock module
#define ERR_COMPRESSOR          0x39 //!< Error in the FETIM compressor module
#define ERR_INTRLK_SENS         0x3A //!< Error in the FETIM interlock sensors module
#define ERR_INTRLK_STATE        0x3B //!< Error in the FETIM interlock state module
#define ERR_INTRLK_TEMP         0x3C //!< Error in the FETIM interlock temperature module
#define ERR_INTRLK_FLOW         0x3D //!< Error in the FETIM interlock flow module
#define ERR_INTRLK_GLITCH       0x3E //!< Error in the FETIM interlock glitch module
#define ERR_COMP_TEMP           0x3F //!< Error in the FETIM compressor temperature module
#define ERR_COMP_HE2_PRESS      0x40 //!< Error in the FETIM compressor He2 pressure module

#define PP_DEFAULT_IRQ_NO       0x07

bool FEMCGetErrorDescription(unsigned short module, unsigned short errNo, std::string &description) {

    description = "";
    static char buf[200];

    switch (module) {
        /* Error library */
        case ERR_ERROR: // Error library
            switch(errNo){
                case 0x01: // Not enough memory for the error array
                    sprintf(buf,
                            "%s",
                            "Error library: Warning: enough memory for error reporting. Error reporting disabled");
                    break;
                case 0x02: // Error redirecting stderr
                    sprintf(buf,
                            "%s",
                            "Error library: Warning: error redirecting stderr.");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Error library: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Parallel Port */
        case ERR_PP: // Parallel Port
            switch(errNo){
                case 0x01: // IRQ not enable
                    sprintf(buf,
                            "%s%d",
                            "Parallel Port: Warning: The IRQ for the parallel port was disable.\nIt was enable and assigned the value:",
                            PP_DEFAULT_IRQ_NO);
                    break;
                case 0x02: // IRQ out ot range
                    sprintf(buf,
                            "Parallel Port: Error: The IRQ number for the parallel port is out of range");
                    break;
                case 0x03: // AMBSI not ready
                    sprintf(buf,
                            "Parallel Port: Warning: Waiting for AMBSI to get ready for parallel communication");
                    break;
                case 0x04: // AMBSI ready timer expired
                    sprintf(buf,
                            "Parallel Port: Error: Timer expired while waiting for AMBSI to get ready. CAN disabled! Only console operations available");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Parallel Port: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* CAN */
        case ERR_CAN: // CAN
            switch(errNo){
                case 0x01: // Required Class outside allowed range
                    sprintf(buf,
                            "%s",
                            "CAN: Error: The required RCA class is outside the allowed range");
                    break;
                case 0x02: // Required module outside allowed range
                    sprintf(buf,
                            "%s",
                            "CAN: Error: The addressed Front End module is outside the allowed range");
                    break;
                case 0x03: // Monitor RCA out of range
                    sprintf(buf,
                            "%s",
                            "CAN: Warning: The RCA of the received monitor message is out of the defined range");
                    break;
                case 0x04: // Control RCA out of range
                    sprintf(buf,
                            "%s",
                            "CAN: Warning: The RCA of the received control message is out of the defined range");
                    break;
                case 0x05: // Special Monitor RCA out of range
                    sprintf(buf,
                            "%s",
                            "CAN: Warning: The RCA of the received special monitor message is out of the defined range");
                    break;
                case 0x06: // Special Control RCA out of range
                    sprintf(buf,
                            "%s",
                            "CAN: Warning: The RCA of the received special control message is out of the defined range");
                    break;
                case 0x07: // Illegal Front End Mode
                    sprintf(buf,
                            "%s",
                            "CAN: The selected Front End mode is not allowed");
                    break;
                case 0x08: // Front End in maintenance mode: standard RCAs blocked
                    sprintf(buf,
                            "CAN: The Front End is in Maintenance mode: only the special RCAs are available");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CAN: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Cartridge */
        case ERR_CARTRIDGE: // Cartridge
            switch(errNo){
                case 0x01: // Cartridge not installed
                    sprintf(buf,
                            "Cartridge: Error: The addressed cartridge is not intalled in the dewar");
                    break;
                case 0x02: // Cartridge submodule out of Range
                    sprintf(buf,
                            "%s",
                            "Cartridge: Error: The addressed subsystem is out of the defined range");
                    break;
                case 0x03: // LO and cartridge temperature submodule out of Range
                    sprintf(buf,
                            "%s",
                            "Cartridge: Error: The addressed LO and cartrdige temperature module is out of the defined range");
                    break;
                case 0x04: // cartridge temperature submodule out of Range
                    sprintf(buf,
                            "%s",
                            "Cartridge: Error: The addressed Cartrdige temperature module is out of the defined range");
                    break;
                case 0x05: // BIAS submodule out of Range
                    sprintf(buf,
                            "%s",
                            "Cartridge: Error: The addressed BIAS module is out of the defined range");
                    break;
                case 0x06: // Cartridge not POWERED
                    sprintf(buf,
                            "%s",
                            "Cartridge: Error: The addressed cartridge is not powered");
                    break;
                case 0x07: // Cartridge in ERROR state
                    sprintf(buf,
                            "%s",
                            "Cartridge: Error: The addressed cartridge is in error state");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Cartridge: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Cartridge Temperature Sensor */
        case ERR_CARTRIDGE_TEMP: // Cartridge Temperature Sensor
            switch(errNo){
                case 0x01: // Temperature sensor not installed
                    sprintf(buf,
                            "%s",
                            "CCA.Temp.Sensor: Error: The addressed cartridge temperature sensor is not installed");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "CCA.Temp.Sensor: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "CCA.Temp.Sensor: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored cartridge temperature in error range
                    sprintf(buf,
                            "%s",
                            "CCA.Temp.Sensor: Error: The monitored cartridge temperature is in the error range");
                    break;
                case 0x05: // Monitored cartrdige temperature in warning range
                    sprintf(buf,
                            "%s",
                            "CCA.Temp.Sensor: Warning: The monitored cartrdige temperature is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CCA.Temp.Sensor: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* LO */
        case ERR_LO: // LO
            switch(errNo){
/*                  case 0x01: // LO module not installed
                    sprintf(buf,
                            "%s",
                            "WCA.LO: Error: The LO in the cartridge is not installed");
                    break; */
                case 0x02: // LO submodule out of range
                    sprintf(buf,
                            "%s",
                            "WCA.LO: Error: The addressed LO submodule is out of range");
                    break;
                case 0x03: // Warning: The addressed hardware is not properly defined yet
                    sprintf(buf,
                            "WCA.LO: Warning: The addressed hardware is not properly defined yet. Firmware needs updating.");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s",
                            "WCA.LO: The specified error is not defined for this module");
                    break;
            }
            break;

        /* PLL */
        case ERR_PLL: // PLL
            switch(errNo){
                case 0x01: // PLL submodule out of range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Error: The addressed PLL submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "WCA.PLL: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "WCA.PLL: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored PLL lock detect voltage in error range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Error: The monitored PLL lock detect voltage is in the error range");
                    break;
                case 0x05: // Monitored PLL lock detect voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Warning: The monitored PLL lock detect voltage is in the warning range");
                    break;
                case 0x06: // Monitored PLL correction voltage in error range
                    sprintf(buf,
                            "%s",
                            "Error: The monitored PLL correction voltage is in the error range");
                    break;
                case 0x07: // Monitored PLL correction voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Warning: The monitored PLL correction voltage is in the warning range");
                    break;
                case 0x08: // Monitored PLL assembly temperature in error range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Error: The monitored PLL assembly temperature is in the error range");
                    break;
                case 0x09: // Monitored PLL assembly temperature in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Warning: The monitored PLL assembly temperature is in the warning range");
                    break;
                case 0x0A: // Monitored YIG heater current in error range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Error: The monitored YIG heater current is in the error range");
                    break;
                case 0x0B: // Monitored YIG heater current in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Warning: The monitored YIG heater current is in the warning range");
                    break;
                case 0x0C: // Monitored PLL reference total power in error range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Error: The monitored PLL reference total power is in the error range");
                    break;
                case 0x0D: // Monitored PLL reference total power in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Warning: The monitored PLL reference total power is in the warning range");
                    break;
                case 0x0E: // Monitored PLL if total power in error range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Error: The monitored PLL if total power is in the error range");
                    break;
                case 0x0F: // Monitored PLL if total power in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Warning: The monitored PLL if total power is in the warning range");
                    break;
                case 0x10: // Monitored unlock detect latch bit in error range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Error: The monitored unlock detect latch bit is in the error range");
                    break;
                case 0x11: // Monitored unlock detect latch bit in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.PLL: Warning: The monitored unlock detect latch bit is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "WCA.PLL: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* YTO */
        case ERR_YTO: // YTO
            switch(errNo){
                case 0x01: // YTO coarse tune out of range
                    sprintf(buf,
                            "%s%u%s",
                            "WCA.YTO: Error: The commanded YTO coarse tune set point is out of the allowed range");
                    break;
                case 0x02: // Monitored YTO coarse tune in error range
                    sprintf(buf,
                            "%s%u%s",
                            "WCA.YTO: Error: The monitored YTO coarse tune is in the error range");
                    break;
                case 0x03: // Monitored YTO coarse tune in warning range
                    sprintf(buf,
                            "%s%u%s",
                            "WCA.YTO: Warning: The monitored YTO coarse tune is in the warning range");
                    break;
                case 0x04: // LO PA drain voltages were limited before YTO tuning
                    sprintf(buf,
                            "%s%u%s",
                            "WCA.YTO: Warning: The commanded YTO coarse tune set point required one or both LO PA drain voltages to be reduced prior to tuning");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "WCA.YTO: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Photomixer */
        case ERR_PHOTOMIXER: // Photomixer
            switch(errNo){
                case 0x01: // Photomixer submodule out of range
                    sprintf(buf,
                            "%s",
                            "WCA.Photomixer: Error: The addressed Photomixer submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "WCA.Photomixer: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "WCA.Photomixer: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored photomixer voltage in error range
                    sprintf(buf,
                            "%s",
                            "WCA.Photomixer: Error: The monitored photomixer voltage is in the error range");
                    break;
                case 0x05: // Monitored photomixer voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.Photomixer: Warning: The monitored photomixer voltage is in the warning range");
                    break;
                case 0x06: // Monitored photomixer current in error range
                    sprintf(buf,
                            "%s",
                            "WCA.Photomixer: Error: The monitored photomixer current is in the error range");
                    break;
                case 0x07: // Monitored photomixer current in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.Photomixer: Warning: The monitored photomixer current is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "WCA.Photomixer: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* AMC */
        case ERR_AMC: // AMC
            switch(errNo){
                case 0x01: // AMC submodule out of range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The addressed AMC submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "WCA.AMC: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "WCA.AMC: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored AMC gate A voltage in error range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The monitored AMC gate A voltage is in the error range");
                    break;
                case 0x05: // Monitored AMC gate A voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Warning: The monitored AMC gate A voltage is in the warning range");
                    break;
                case 0x06: // Monitored AMC drain A voltage in error range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The monitored AMC drain A voltage is in the error range");
                    break;
                case 0x07: // Monitored AMC drain A voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Warning: The monitored AMC drain A voltage is in the warning range");
                    break;
                case 0x08: // Monitored AMC drain A current in error range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The monitored AMC drain A current is in the error range");
                    break;
                case 0x09: // Monitored AMC drain A current in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Warning: The monitored AMC drain A current is in the warning range");
                    break;
                case 0x0A: // Monitored AMC gate B voltage in error range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The monitored AMC gate B voltage is in the error range");
                    break;
                case 0x0B: // Monitored AMC gate B voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Warning: The monitored AMC gate B voltage is in the warning range");
                    break;
                case 0x0C: // Set AMC drain B voltage out of range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The commanded AMC drain B voltage set point is out of the allowed range");
                    break;
                case 0x0D: // Monitored AMC drain B voltage in error range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The monitored AMC drain B voltage is in the error range");
                    break;
                case 0x0E: // Monitored AMC drain B voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Warning: The monitored AMC drain B voltage is in the warning range");
                    break;
                case 0x0F: // Monitored AMC drain B current in error range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The monitored AMC drain B current is in the error range");
                    break;
                case 0x10: // Monitored AMC drain B current in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Warning: The monitored AMC drain B current is in the warning range");
                    break;
                case 0x11: // Set AMC multiplier D voltage out of range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The commanded AMC multiplier D voltage set point is out of the allowed range");
                    break;
                case 0x12: // Monitored AMC multiplier D current in error range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The monitored AMC multiplier D current is in the error range");
                    break;
                case 0x13: // Monitored AMC multiplier D current in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Warning: The monitored AMC multiplier D current is in the warning range");
                    break;
                case 0x14: // Set AMC gate E voltage out of range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The commanded AMC gate E voltage set point is out of the allowed range");
                    break;
                case 0x15: // Monitored AMC gate E voltage in error range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The monitored AMC gate E voltage is in the error range");
                    break;
                case 0x16: // Monitored AMC gate E voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Warning: The monitored AMC gate E voltage is in the warning range");
                    break;
                case 0x17: // Set AMC drain E voltage out of range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The commanded AMC drain E voltage set point is out of the allowed range");
                    break;
                case 0x18: // Monitored AMC drain E voltage in error range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The monitored AMC drain E voltage is in the error range");
                    break;
                case 0x19: // Monitored AMC drain E voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Warning: The monitored AMC drain E voltage is in the warning range");
                    break;
                case 0x1A: // Monitored AMC drain E current in error range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The monitored AMC drain E current is in the error range");
                    break;
                case 0x1B: // Monitored AMC drain E current in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Warning: The monitored AMC drain E current is in the warning range");
                    break;
                case 0x1C: // Monitored AMC 3V supply voltage in error range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The monitored AMC 3V supply voltage is in the error range");
                    break;
                case 0x1D: // Monitored AMC 3V supply voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Warning: The monitored AMC 3V supply voltage is in the warning range");
                    break;
                case 0x1E: // Monitored AMC 5V supply voltage in error range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Error: The monitored AMC 5V supply voltage is in the error range");
                    break;
                case 0x1F: // Monitored AMC 5V supply voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.AMC: Warning: The monitored AMC 5V supply voltage is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Cart.AMC: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* PA */
        case ERR_PA: // PA
            switch(errNo){
                case 0x01: // PA submodule out of range
                    sprintf(buf,
                            "%s",
                            "WCA.PA: Error: The addressed PA submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "WCA.PA: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "WCA.PA: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored PA 3V power supply voltage in error range
                    sprintf(buf,
                            "%s",
                            "WCA.PA: Error: The monitored PA 3V power supply voltage is in the error range");
                    break;
                case 0x05: // Monitored PA 5V power supply voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.PA: Warning: The monitored PA 5V power supply voltage is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "WCA.PA: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* PA Channel */
        case ERR_PA_CHANNEL: // PA Channel
            switch(errNo){
                case 0x01: // PA Channel submodule out of range
                    sprintf(buf,
                            "%s",
                            "WCA.PA Channel: Error: The addressed PA channel submodule is out of range");
                    break;
                case 0x02: // Set PA channel gate voltage out of range
                    sprintf(buf,
                            "%s",
                            "WCA.PA Channel: Error: The commanded PA gate voltage set point is out of the allowed range");
                    break;
                case 0x03: // Monitored PA channel gate voltage in error range
                    sprintf(buf,
                            "%s",
                            "WCA.PA Channel: Error: The monitored PA channel gate voltage is in the error range");
                    break;
                case 0x04: // Monitored PA channel gate voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.PA Channel: Warning: The monitored PA channel gate voltage is in the warning range");
                    break;
                case 0x05: // Set PA channel drain voltage out of range
                    sprintf(buf,
                            "%s",
                            "WCA.PA Channel: Error: The commanded PA drain voltage set point is out of the allowed range");
                    break;
                case 0x06: // Monitored PA channel drain voltage in error range
                    sprintf(buf,
                            "%s",
                            "WCA.PA Channel: Error: The monitored PA channel drain voltage is in the error range");
                    break;
                case 0x07: // Monitored PA channel drain voltage in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.PA Channel: Warning: The monitored PA channel drain voltage is in the warning range");
                    break;
                case 0x08: // Control message out of range
                    sprintf(buf,
                            "WCA.PA Channel: Error: The control message is out of range");
                    break;
                case 0x09: // Monitor message out of range
                    sprintf(buf,
                            "WCA.PA Channel: Error: The monitor message is out of range");
                    break;
                case 0x0A: // Monitored PA channel drain current in error range
                    sprintf(buf,
                            "%s",
                            "WCA.PA Channel: Error: The monitored PA channel drain current is in the error range");
                    break;
                case 0x0B: // Monitored PA channel drain current in warning range
                    sprintf(buf,
                            "%s",
                            "WCA.PA Channel: Warning: The monitored PA channel drain current is in the warning range");
                    break;
                case 0x0C: // Warning: The addressed hardware is not properly defined yet
                    sprintf(buf,
                            "WCA.PA Channel: Warning: The addressed hardware is not properly defined yet. Firmware needs updating.");
                    break;
                case 0x0D: // Warning: The cryostat temperature is outside the allowed range
                    sprintf(buf,
                            "WCA.PA Channel: Warning: The cryostat temperature is outside the allowed range. PA blocked.");
                    break;
                case 0x0E: // Warning: Attempted to set LO PA above max safe power level.
                    sprintf(buf,
                            "WCA.PA Channel: Warning: Attempted to set PA channel drain voltage above max safe limit.");
                    break;

                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "WCA.PA Channel: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Polarization */
        case ERR_POLARIZATION: // Polarization
            switch(errNo){
                case 0x01: // Polarization module not installed
                    sprintf(buf,
                            "%s",
                            "CCA.Pol: Error: The required polarization is not installed");
                    break;
                case 0x02: // Polarization submodule out of range
                    sprintf(buf,
                            "%s",
                            "CCA.Pol: Error: The addressed polarization submodule is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CCA.Pol: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Sideband */
        case ERR_SIDEBAND: // Sideband
            switch(errNo){
                case 0x01: // Sideband module not installed
                    sprintf(buf,
                            "%s",
                            "CCA.SB: Error: The required sideband is not installed");
                    break;
                case 0x02: // Sideband submodule out of range
                    sprintf(buf,
                            "%s",
                            "CCA.SB: Error: The addressed sideband submodule is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CCA.SB: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* LNA led */
        case ERR_LNA_LED: // LNA led
            switch(errNo){
                case 0x01: // LNA led module not installed
                    sprintf(buf,
                            "%s",
                            "CCA.LNA LED: Error: The required LNA led is not installed");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CCA.LNA LED: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* SIS Heater */
        case ERR_SIS_HEATER: // SIS Heater
            switch(errNo){
                case 0x01: // SIS Heater module not installed
                    sprintf(buf,
                            "%s",
                            "CCA.SIS Heater: Error: The required SIS Heater is not installed");
                    break;
                case 0x02: // SIS Heater submodule out of range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS Heater: Error: The addressed SIS heater submodule is out of range");
                    break;
                case 0x03: // Monitored SIS heater current in error range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS Heater: Error: The monitored SIS heater current is in the error range");
                    break;
                case 0x04: // Monitored SIS heater current in warning range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS Heater: Warning: The monitored SIS heater current is in the warning range");
                    break;
                case 0x05: // Control message out of range
                    sprintf(buf,
                            "CCA.SIS Heater: Error: The control message is out of range");
                    break;
                case 0x06: // Monitor message out of range
                    sprintf(buf,
                            "CCA.SIS Heater: Error: The monitor message is out of range");
                    break;
                case 0x07: // Band9 heater on blocked
                    sprintf(buf,
                            "CCA.SIS Heater: Error: The heater on band9 cannot be turned on until the timer has expired");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CCA.SIS Heater: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Schottky Mixer */
        case ERR_SCHOTTKY_MIXER: // Schottky Mixer
            switch(errNo){
                case 0x01: // Schottky Mixer module not installed
                    sprintf(buf,
                            "%s",
                            "CCA.Schottky Mixer: Error: The required Schottky Mixer is not installed");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CCA.Schottky Mixer: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* SIS */
        case ERR_SIS: // SIS
            switch(errNo){
                case 0x01: // SIS module not installed
                    sprintf(buf,
                            "%s",
                            "CCA.SIS: Error: The required SIS is not installed");
                    break;
                case 0x02: // SIS submodule out of range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS: Error: The addressed SIS submodule is out of range");
                    break;
                case 0x03: // Set SIS voltage out of range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS: Error: The commanded SIS voltage set point is out of the allowed range");
                    break;
                case 0x04: // Monitored SIS voltage in error range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS: Error: The monitored SIS voltage is in the error range");
                    break;
                case 0x05: // Monitored SIS voltage in warning range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS: Warning: The monitored SIS voltage is in the warning range");
                    break;
                case 0x06: // Monitored SIS current in error range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS: Error: The monitored SIS current is in the error range");
                    break;
                case 0x07: // Monitored SIS current in warning range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS: Warning: The monitored SIS current is in the warning range");
                    break;
                case 0x08: // Control message out of range
                    sprintf(buf,
                            "CCA.SIS: Error: The control message is out of range");
                    break;
                case 0x09: // Monitor message out of range
                    sprintf(buf,
                            "CCA.SIS: Error: The monitor message is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CCA.SIS: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* SIS Magnet */
        case ERR_SIS_MAGNET: // SIS magnet
            switch(errNo){
                case 0x01: // SIS magnet module not installed
                    sprintf(buf,
                            "%s",
                            "CCA.SIS Magnet: Error: The required SIS magnet is not installed");
                    break;
                case 0x02: // SIS magnet submodule out of range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS Magnet: Error: The addressed SIS magnet submodule is out of range");
                    break;
                case 0x03: // Monitored SIS Magnet voltage in error range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS Magnet: Error: The monitored SIS Magnet voltage is in the error range");
                    break;
                case 0x04: // Monitored SIS Magnet voltage in warning range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS Magnet: Warning: The monitored SIS Magnet voltage is in the warning range");
                    break;
                case 0x05: // Monitored SIS Magnet current in error range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS Magnet: Error: The monitored SIS Magnet current is in the error range");
                    break;
                case 0x06: // Monitored SIS Magnet current in warning range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS Magnet: Warning: The monitored SIS Magnet current is in the warning range");
                    break;
                case 0x07: // Set SIS Magnet current out of range
                    sprintf(buf,
                            "%s",
                            "CCA.SIS Magnet: Error: The commanded SIS Magnet current set point is out of the allowed range");
                    break;
                case 0x08: // Control message out of range
                    sprintf(buf,
                            "CCA.SIS Magnet: Error: The control message is out of range");
                    break;
                case 0x09: // Monitor message out of range
                    sprintf(buf,
                            "CCA.SIS Magnet: Error: The monitor message is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CCA.SIS Magnet: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* LNA */
        case ERR_LNA: // LNA
            switch(errNo){
                case 0x01: // LNA module not installed
                    sprintf(buf,
                            "%s",
                            "CCA.LNA: Error: The required LNA is not installed");
                    break;
                case 0x02: // LNA submodule out of range
                    sprintf(buf,
                            "%s%",
                            "CCA.LNA: Error: The addressed LNA submodule is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CCA.LNA: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* LNA Stage */
        case ERR_LNA_STAGE: // LNA stage
            switch(errNo){
                case 0x01: // LNA stage module not installed
                    sprintf(buf,
                            "%s",
                            "CCA.LNA Stage: Error: The required LNA stage is not installed");
                    break;
                case 0x02: // LNA stage submodule out of range
                    sprintf(buf,
                            "%s",
                            "CCA.LNA Stage: Error: The addressed LNA stage submodule is out of range");
                    break;
                case 0x03: // Monitored LNA stage gate voltage in error range
                    sprintf(buf,
                            "%s",
                            "CCA.LNA Stage: Error: The monitored LNA stage gate voltage is in the error range");
                    break;
                case 0x04: // Monitored LNA stage gate voltage in warning range
                    sprintf(buf,
                            "%s",
                            "CCA.LNA Stage: Warning: The monitored LNA stage gate voltage is in the warning range");
                    break;
                case 0x05: // Monitored LNA stage drain voltage in error range
                    sprintf(buf,
                            "%s",
                            "CCA.LNA Stage: Error: The monitored LNA stage drain voltage is in the error range");
                    break;
                case 0x06: // Monitored LNA stage drain voltage in warning range
                    sprintf(buf,
                            "%s",
                            "CCA.LNA Stage: Warning: The monitored LNA stage drain voltage is in the warning range");
                    break;
                case 0x07: // Monitored LNA stage drain current in error range
                    sprintf(buf,
                            "%s",
                            "CCA.LNA Stage: Error: The monitored LNA stage drain current is in the error range");
                    break;
                case 0x08: // Monitored LNA stage drain current in warning range
                    sprintf(buf,
                            "%s",
                            "CCA.LNA Stage: Warning: The monitored LNA stage drain current is in the warning range");
                    break;
                case 0x09: // Set LNA stage drain voltage out of range
                    sprintf(buf,
                            "%s",
                            "CCA.LNA Stage: Error: The commanded LNA stage drain voltage set point is out of the allowed range");
                    break;
                case 0x0A: // Set LNA stage drain current out of range
                    sprintf(buf,
                            "%s",
                            "CCA.LNA Stage: Error: The commanded LNA stage drain current set point is out of the allowed range");
                    break;
                case 0x10: // Control message out of range
                    sprintf(buf,
                            "CCA.LNA Stage: Error: The control message is out of range");
                    break;
                case 0x11: // Monitor message out of range
                    sprintf(buf,
                            "CCA.LNA Stage: Error: The monitor message is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CCA.LNA Stage: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Polarization Special Messages */
        case ERR_POL_SPECIAL_MSGS: // Polarization Special Messages
            switch(errNo){
                case 0x01: // Polarization special messages submodule out of range
                    sprintf(buf,
                            "%s",
                            "CCA.Pol Special: Error: The addressed polarization special messages submodule is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CCA.Pol Special: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Polarization DAC */
        case ERR_POL_DAC: // Polarization DAC
            switch(errNo){
                case 0x01: // Polarization DAC submodule out of range
                    sprintf(buf,
                            "%s",
                            "CCA.Pol DAC: Error: The addressed polarization DAC submodule is out of range");
                    break;
                case 0x02: // Polarization DAC submodule doesn't accept clear strobes
                    sprintf(buf,
                            "%s",
                            "CCA.Pol DAC: Error: The addressed polarization DAC submodule doesn't accept clear strobes");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "CCA.Pol DAC: Error: The monitor message is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CCA.Pol DAC: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Serial interface */
        case ERR_SERIAL_INTERFACE: // Serial Interface
            switch(errNo){
                case 0x01: // Selected command out of range
                    sprintf(buf,
                            "Serial Interface: Error: The selected command word is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Serial Interface: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Serial Mux Board */
        case ERR_SERIAL_MUX: // Serial Mux Board
            switch(errNo){
                case 0x01: // Data lenght out of range
                    sprintf(buf,
                            "Serial Mux: Error: The selected data length is out of range");
                    break;
                case 0x02: // Timeout while waiting for the mux board to become ready
                    sprintf(buf,
                            "Serial Mux: Error: Timeout while waiting for the board to become ready");
                    break;
                case 0x03: // FPGA is not ready
                    sprintf(buf,
                            "Serial Mux: Error: The FPGA is not ready for communication");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Serial Mux: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Timer Module */
        case ERR_TIMER: // Timer Module
            switch(errNo){
                case 0x01: // Async timer already running
                    sprintf(buf,
                            "Timer Module: Error: The asynchronous timer is already running");
                    break;
                case 0x02: // Required async timer out of range
                    sprintf(buf,
                            "Timer Module: Error: The required asynchronous timer is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Timer Module: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Bias Serial Interface Module */
        case ERR_BIAS_SERIAL: // Bias Serial Interface Module
            switch(errNo){
                case 0x01: // Timeout while waiting for the BIAS ADC to become ready
                    sprintf(buf,
                            "CCA.Bias: Error: Timeout while waiting for the BIAS ADC to become ready");
                    break;
                case 0x02: // Timeout while waiting for the BIAS DAC1 to become ready
                    sprintf(buf,
                            "CCA.Bias: Error: Timeout while waiting for the BIAS DAC1 to become ready");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CCA.Bias: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* LO Serial Interface Module */
        case ERR_LO_SERIAL: // LO Serial Interface Module
            switch(errNo){
                case 0x01: // Timeout while waiting for the LO ADC to become ready
                    sprintf(buf,
                            "WCA.Serial: Error: Timeout while waiting for the LO ADC to become ready");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "WCA.Serial: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Power Distribution */
        case ERR_POWER_DISTRIBUTION: // Power distribution
            switch(errNo){
                case 0x01: // Power distribution submodule out of range
                    sprintf(buf,
                            "%s",
                            "CPDS: Error: The addressed power distribution submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "CPDS: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "CPDS: Error: The monitor message is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CPDS: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Power Distribution Module */
        case ERR_PD_MODULE: // Power distribution module
            switch(errNo){
                case 0x01: // Power distribution module submodule out of range
                    sprintf(buf,
                            "%s",
                            "CPDS Module: Error: The addressed power distribution module submodule is out of range");
                    break;
                case 0x02: // Corresponding cartridge not installed
                    sprintf(buf,
                            "%s",
                            "CPDS Module: Error: The cartridge corresponding to the addressed power distribution module is not intalled in the dewar");
                    break;
                case 0x03: // Max number of cartrdiges already on
                    sprintf(buf,
                            "%s",
                            "CPDS Module: Error: The maximum allowed number of powered cartrdiges is already turned on");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CPDS Module: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Power Distribution Channel */
        case ERR_PD_CHANNEL: // Power distribution channel
            switch(errNo){
                case 0x01: // Power distribution channel submodule out of range
                    sprintf(buf,
                            "%s",
                            "CPDS Channel: Error: The addressed power distribution channel submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "CPDS Channel: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "CPDS Channel: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored power distribution channel voltage in error range
                    sprintf(buf,
                            "%s",
                            "CPDS Channel: Error: The monitored power distribution channel voltage is in the error range");
                    break;
                case 0x05: // Monitored power distribution channel voltage in warning range
                    sprintf(buf,
                            "%s",
                            "CPDS Channel: Warning: The monitored power distribution channel voltage is in the warning range");
                    break;
                case 0x06: // Monitored power distribution channel current in error range
                    sprintf(buf,
                            "%s",
                            "CPDS Channel: Error: The monitored power distribution channel current is in the error range");
                    break;
                case 0x07: // Monitored power distribution channel current in warning range
                    sprintf(buf,
                            "%s",
                            "CPDS Channel: Warning: The monitored power distribution current is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CPDS Channel: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Power distribution Serial Interface Module */
        case ERR_PD_SERIAL: // PD Serial Interface Module
            switch(errNo){
                case 0x01: // Timeout while waiting for the PD ADC to become ready
                    sprintf(buf,
                            "CPDS Serial: Error: Timeout while waiting for the PD ADC to become ready");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "CPDS Serial: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* IF Channel */
        case ERR_IF_CHANNEL: // IF Channel
            switch(errNo){
                case 0x01: // IF Channel submodule out of range
                    sprintf(buf,
                            "%s",
                            "IF Switch Channel: Error: The addressed IF channel submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "IF Switch Channel: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "IF Switch Channel: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored IF channel assembly temperature in error range
                    sprintf(buf,
                            "%s",
                            "IF Switch Channel: Error: The monitored IF channel assembly temperaure is in the error range");
                    break;
                case 0x05: // Monitored IF channel assembly temperature in warning range
                    sprintf(buf,
                            "%s",
                            "IF Switch Channel: Warning: The monitored IF channel assembly temperature is in the warning range");
                    break;
                case 0x06: // Set IF channel attenuation out of range
                    sprintf(buf,
                            "%s",
                            "IF Switch Channel: Error: The commanded IF channel attenuation set point is out of the allowed range");
                    break;
                case 0x07: // Temperature servo OFF
                    sprintf(buf,
                            "IF Switch Channel: Error: The temperature servo is OFF: Monitoring of temperature disabled.");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "IF Switch Channel: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* IF Switch */
        case ERR_IF_SWITCH: // IF Switch
            switch(errNo){
                case 0x01: // IF Switch submodule out of range
                    sprintf(buf,
                            "%s",
                            "IF Switch: Error: The addressed IF Switch submodule is out of range");
                    break;
                case 0x02: // Set IF switch select band out of range
                    sprintf(buf,
                            "%s",
                            "IF Switch: Error: The commanded IF switch band select is out of the allowed range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "IF Switch: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* IF Switch Serial Interface Module */
        case ERR_IF_SERIAL: // IF Switch Serial Interface Module
            switch(errNo){
                case 0x01: // Timeout while waiting for the IF Switch ADC to become ready
                    sprintf(buf,
                            "IF Switch Serial: Error: Timeout while waiting for the IF Switch ADC to become ready");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "IF Switch Serial: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        case ERR_CRYOSTAT: // Cryostat
            switch(errNo){
                case 0x01: // Cryostat submodule out of range
                    sprintf(buf,
                            "%s",
                            "Cryostat: Error: The addressed Cryostat submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "Cryostat: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "Cryostat: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored 230V supply current in error range
                    sprintf(buf,
                            "%s",
                            "Cryostat: Error: The monitored 230V supply current is in the error range");
                    break;
                case 0x05: // Monitored 230V supply current in warning range
                    sprintf(buf,
                            "%s",
                            "Cryostat: Warning: The monitored 230V supply current is in the warning range");
                    break;
                case 0x06: // Warning: Backing pump OFF
                    sprintf(buf,
                            "Warning: The backing pump is OFF. Supply current monitoring disabled.");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Cryostat: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        case ERR_TURBO_PUMP: // Turbo Pump
            switch(errNo){
                case 0x01: // Turbo pump submodule out of range
                    sprintf(buf,
                            "%s",
                            "Turbo Pump: Error: The addressed Turbo Pump submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "Turbo Pump: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "Turbo Pump: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored turbo pump state in error range
                    sprintf(buf,
                            "%s",
                            "Turbo Pump: Error: The monitored turbo pump state is in the error range");
                    break;
                case 0x05: // Monitored turbo pump state in warning range
                    sprintf(buf,
                            "%s",
                            "Turbo Pump: Warning: The monitored turbo pump state is in the warning range");
                    break;
                case 0x06: // Monitored turbo pump speed in error range
                    sprintf(buf,
                            "%s",
                            "Turbo Pump: Error: The monitored turbo pump speed is in the error range");
                    break;
                case 0x07: // Monitored turbo pump speed in warning range
                    sprintf(buf,
                            "%s",
                            "Turbo Pump: Warning: The monitored turbo pump speed is in the warning range");
                    break;
                case 0x08: // Warning: Backing pump OFF
                    sprintf(buf,
                            "Turbo Pump: Warning: The backing pump is OFF. Turbo pump disabled.");
                    break;
                case 0x09: // Warning: Temperature out of range
                    sprintf(buf,
                            "Turbo Pump: Warning: The turbo pump temperature is outside the allowed range. Turbo pump disabled.");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Turbo Pump: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        case ERR_VACUUM_CONTROLLER: // Vacuum Controller
            switch(errNo){
                case 0x01: // Vacuum Controller submodule out of range
                    sprintf(buf,
                            "%s",
                            "Vacuum Controller: Error: The addressed Vacuum Controller submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "Vacuum Controller: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "Vacuum Controller: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored vacuum controller state in error range
                    sprintf(buf,
                            "%s",
                            "Vacuum Controller: Error: The monitored vacuum controller state is in the error range");
                    break;
                case 0x05: // Monitored vacuum controller state in warning range
                    sprintf(buf,
                            "%s",
                            "Vacuum Controller: Warning: The monitored vacuum controller state is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Vacuum Controller: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        case ERR_GATE_VALVE: // Gate Valve
            switch(errNo){
                case 0x01: // Monitored gate valve state in error range
                    sprintf(buf,
                            "%s",
                            "Gate Valve: Error: The monitored gate valve state is in the error range");
                    break;
                case 0x02: // Monitored gate valve state in warning range
                    sprintf(buf,
                            "%s",
                            "Gate Valve: Warning: The monitored gate valve state is in the warning range");
                    break;
                case 0x03: // Warning: Backing pump OFF
                    sprintf(buf,
                            "Gate Valve: Warning: The backing pump is OFF. Gate valve disabled.");
                    break;
                case 0x04: // Warning: Valve still moving
                    sprintf(buf,
                            "Gate Valve: Warning: The gate valve is still moving. Wait until stopped.");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Gate Valve: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        case ERR_SOLENOID_VALVE: // Solenoid Valve
            switch(errNo){
                case 0x01: // Monitored solenoid valve state in error range
                    sprintf(buf,
                            "%s",
                            "Solenoid Valve: Error: The monitored solenoid valve state is in the error range");
                    break;
                case 0x02: // Monitored solenoid valve state in warning range
                    sprintf(buf,
                            "%s",
                            "Solenoid Valve: Warning: The monitored solenoid valve state is in the warning range");
                    break;
                case 0x03: // Warning: Backing pump OFF
                    sprintf(buf,
                            "Solenoid Valve: Warning: The backing pump is OFF. Solenoid valve disabled.");
                    break;
                case 0x04: // Solenoid valve controlled by interlock
                    sprintf(buf,
                            "Solenoid Valve: Warning: The solenoid valve is controlled by the interlock system and closed.");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Solenoid Valve: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        case ERR_VACUUM_SENSOR: // Vacuum Sensor
            switch(errNo){
                case 0x01: // Control message out of range
                    sprintf(buf,
                            "Vacuum Sensor: Error: The control message is out of range");
                    break;
                case 0x02: // Monitor message out of range
                    sprintf(buf,
                            "Vacuum Sensor: Error: The monitor message is out of range");
                    break;
                case 0x03: // Monitored vacuum sensor pressure in error range
                    sprintf(buf,
                            "%s",
                            "Vacuum Sensor: Error: The monitored vacuum sensor pressure is in the error range");
                    break;
                case 0x04: // Monitored vacuum sensor pressure in warning range
                    sprintf(buf,
                            "%s",
                            "Vacuum Sensor: Warning: The monitored vacuum sensor pressure is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Vacuum Sensor: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        case ERR_CRYOSTAT_TEMP: // Cryostat temperature
            switch(errNo){
                case 0x01: // Control message out of range
                    sprintf(buf,
                            "Cryostat Temp: Error: The control message is out of range");
                    break;
                case 0x02: // Monitor message out of range
                    sprintf(buf,
                            "Cryostat Temp: Error: The monitor message is out of range");
                    break;
                case 0x03: // Monitored cryostat temperature in error range
                    sprintf(buf,
                            "%s",
                            "Cryostat Temp: Error: The monitored cryostat temperature is in the error range");
                    break;
                case 0x04: // Monitored cryostat temperature pressure
                    sprintf(buf,
                            "%s",
                            "Cryostat Temp: Warning: The monitored cryostat temperature is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Cryostat Temp: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Cryostat Serial Interface Module */
        case ERR_CRYO_SERIAL: // Cryostat Serial Interface Module
            switch(errNo){
                case 0x01: // Timeout while waiting for the CRYO ADC to become ready
                    sprintf(buf,
                            "Cryostat Serial: Error: Timeout while waiting for the CRYO ADC to become ready");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "Cryostat Serial: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* EDFA Modulation Input */
        case ERR_MODULATION_INPUT: // EDFA Modulation Input
            switch(errNo){
                case 0x01: // Set EDFA modulation input value out of range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Modulation Error: The commanded EDFA modulation input value is out of the allowed range");
                    break;
                case 0x02: // Modulation input submodule out of range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Modulation Error: The addressed modulation input submodule is out of range");
                    break;

                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "LPR.EDFA Modulation The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* EDFA Photo Detector */
        case ERR_PHOTO_DETECTOR: // EDFA Photo detector
            switch(errNo){
                case 0x01: // EDFA photo detector submodule out of range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Photodetector: Error: The addressed EDFA photo detector submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "LPR.EDFA Photodetector: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "LPR.EDFA Photodetector: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored EDFA photodetector current in error range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Photodetector: Error: The monitored EDFA photodetector current is in the error range");
                    break;
                case 0x05: // Monitored EDFA photodetector current in warning range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Photodetector: Warning: The monitored EDFA photodetector current is in the warning range");
                    break;
                case 0x06: // Monitored EDFA photodetector power in error range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Photodetector: Error: The monitored EDFA photodetector power is in the error range");
                    break;
                case 0x07: // Monitored EDFA photodetector power in warning range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Photodetector: Warning: The monitored EDFA photodetector power is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "LPR.EDFA Photodetector: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* EDFA laser */
        case ERR_LASER: // EDFA Laser
            switch(errNo){
                case 0x01: // EDFA laser submodule out of range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Laser: Error: The addressed EDFA laser submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "LPR.EDFA Laser: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "LPR.EDFA Laser: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored laser pump temperature in error range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Laser: Error: The monitored pump temperature is in the error range");
                    break;
                case 0x05: // Monitored laser pump temperature in warning range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Laser: Warning: The monitored pump temperature is in the warning range");
                    break;
                case 0x06: // Set laser drive current out of range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Laser: Error: The commanded laser drive current set point is out of the allowed range");
                    break;
                case 0x07: // Monitored laser drive current in error range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Laser: Error: The monitored laser drive current is in the error range");
                    break;
                case 0x08: // Monitored laser drive current in warning range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Laser: Warning: The monitored laser drive current is in the warning range");
                    break;
                case 0x09: // Monitored laser photo detector current in error range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Laser: Error: The monitored laser photo detector current is in the error range");
                    break;
                case 0x0A: // Monitored laser photo detector current in warning range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA Laser: Warning: The monitored laser photo detector current is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "LPR.EDFA Laser: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* EDFA */
        case ERR_EDFA: // EDFA
            switch(errNo){
                case 0x01: // EDFA submodule out of range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA: Error: The addressed EDFA submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "LPR.EDFA: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "LPR.EDFA: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored EDFA driver status in error range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA: Error: The monitored EDFA driver status is in the error range");
                    break;
                case 0x05: // Monitored EDFA driver status in warning range
                    sprintf(buf,
                            "%s",
                            "LPR.EDFA: Warning: The monitored EDFA driver status is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "LPR.EDFA: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Optical Switch */
        case ERR_OPTICAL_SWITCH: // Optical switch
            switch(errNo){
                case 0x01: // Optical Switch submodule out of range
                    sprintf(buf,
                            "%s",
                            "LPR.Optical Switch: Error: The addressed Optical Switch submodule is out of range");
                    break;
                case 0x02: // Set Optical Switch port out of range
                    sprintf(buf,
                            "%s",
                            "LPR.Optical Switch: Error: The commanded Optical Switch port is out of the allowed range");
                    break;
                case 0x03: // Control message out of range
                    sprintf(buf,
                            "LPR.Optical Switch: Error: The control message is out of range");
                    break;
                case 0x04: // Monitor message out of range
                    sprintf(buf,
                            "LPR.Optical Switch: Error: The monitor message is out of range");
                    break;
                case 0x05: // Monitored optical switch state in error range
                    sprintf(buf,
                            "%s",
                            "LPR.Optical Switch: Error: The monitored optical switch state is in the error range");
                    break;
                case 0x06: // Monitored optical switch state in warning range
                    sprintf(buf,
                            "%s",
                            "LPR.Optical Switch: Warning: The monitored optical switch state is in the warning range");
                    break;
                case 0x07: // Monitored optical switch busy state in error range
                    sprintf(buf,
                            "%s",
                            "LPR.Optical Switch: Error: The monitored optical switch busy state is in the error range");
                    break;
                case 0x08: // Monitored optical switch busy state in warning range
                    sprintf(buf,
                            "%s",
                            "LPR.Optical Switch: Warning: The monitored optical switch busy state is in the warning range");
                    break;
                case 0x09: // Optical switch timed out while waiting for busy state during initialization
                    sprintf(buf,
                            "LPR.Optical Switch: Timed out while waiting for ready signal during initialization");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "LPR.Optical Switch: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* LPR */
        case ERR_LPR: // LPR
            switch(errNo){
                case 0x01: // LPR submodule out of range
                    sprintf(buf,
                            "%s",
                            "LPR: Error: The addressed LPR submodule is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "LPR: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* LPR temperature */
        case ERR_LPR_TEMP: // LPR temperature
            switch(errNo){
                case 0x01: // Control message out of range
                    sprintf(buf,
                            "LPR Temp: Error: The control message is out of range");
                    break;
                case 0x02: // Monitor message out of range
                    sprintf(buf,
                            "LPR Temp: Error: The monitor message is out of range");
                    break;
                case 0x03: // Monitored LPR temperature in error range
                    sprintf(buf,
                            "%s",
                            "LPR Temp: Error: The monitored LPR temperature is in the error range");
                    break;
                case 0x04: // Monitored LPR temperature pressure
                    sprintf(buf,
                            "%s",
                            "LPR Temp: Warning: The monitored LPR temperature is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "LPR Temp: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* LPR Serial Interface Module */
        case ERR_LPR_SERIAL: // LPR Serial Interface Module
            switch(errNo){
                case 0x01: // Timeout while waiting for the LPR ADC to become ready
                    sprintf(buf,
                            "LPR Serial: Error: Timeout while waiting for the LPR ADC to become ready");
                    break;
                case 0x02: // Optical switch busy
                    sprintf(buf,
                            "LPR Serial: Error: Optical switch busy");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "LPR Serial: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Modulation Input DAC */
        case ERR_MI_DAC: // Modulation Input DAC
            switch(errNo){
                case 0x01: // Monitor message out of range
                    sprintf(buf,
                            "LPR.MI DAC: Error: The monitor message is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "LPR.MI DAC: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* INI file access */
        case ERR_INI: // INI file access
            switch(errNo){
                case 0x01: // Data not found
                    sprintf(buf,
                            "INI File Access: Error: The required data was not found in the configuration file.");
                    break;
                case 0x02: // Error opening file
                    sprintf(buf,
                            "INI File Access: Error: The was an error while opening the required configuration file.");
                    break;
                case 0x03: // Number of items returned is wrong
                    sprintf(buf,
                            "INI File Access: Error: The number of returned items doesn't match the required ones.");
                    break;
                case 0x04: // Error handling file
                    sprintf(buf,
                            "INI File Access: Error: The was an error while handling the required configuration file.");
                    break;
                case 0x05: // Error closing file
                    sprintf(buf,
                            "INI File Access: Error: The was an error while closing the required configuration file.");
                    break;
                case 0x06: // Error updating the file
                    sprintf(buf,
                            "INI File Access: Error: The was an error while updating the required configuration file.");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "INI File Access: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* One wire bus */
        case ERR_OWB: // One wire bus
            switch(errNo){
                case 0x01: // Time out on IRQ
                    sprintf(buf,
                            "OWB: Error: Time out waiting for IRQ");
                    break;
                case 0x02: // Presence pulse not detected
                    sprintf(buf,
                            "OWB: Error: Presence pulse not detected");
                    break;
                case 0x03: // Maximum number of devices reached
                    sprintf(buf,
                            "OWB: Error: Maximum number of devices reached. Check the bus for problems!");
                    break;
                case 0x04: // Bus reset timed out
                    sprintf(buf,
                            "OWB: Error: Time out while resetting the bus.");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "OWB: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Front End Thermal Interlock Module */
        case ERR_FETIM: // Front End Interlock Module
            switch(errNo){
                case 0x01: // Submodule out of range
                    sprintf(buf,
                            "%s",
                            "FETIM: Error: The addressed FETIM submodule is out of range");
                    break;
                case 0x02: // FETIM not installed
                    sprintf(buf,
                            "FETIM: Error: The FETIM is not intalled in the Front End");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "FETIM: he specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* FETIM Interlock Module */
        case ERR_INTERLOCK: // FETIM Interlock
            switch(errNo){
                case 0x01: // Submodule out of range
                    sprintf(buf,
                            "%s",
                            "FETIM Interlock: Error: The addressed FETIM interlock submodule is out of range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "FETIM Interlock: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* FETIM Compressor Module */
        case ERR_COMPRESSOR: // FETIM Compressor
            switch(errNo){
                case 0x01: // Submodule out of range
                    sprintf(buf,
                            "%s",
                            "FETIM Compressor: Error: The addressed FETIM compressor submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "FETIM Compressor: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "FETIM Compressor: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored interlock status digital value in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Compressor: Error: The monitored interlock status digital value is in the error range");
                    break;
                case 0x05: // Monitored interlock status digital value in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Compressor: Warning: The monitored interlock status digital value is in the warning range");
                    break;
                case 0x06: // Monitored compressor cable digital value in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Compressor: Error: The monitored compressor cable digital value is in the error range");
                    break;
                case 0x07: // Monitored compressor cable digital value in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Compressor: Warning: The monitored compresor cable digital value is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "FETIM Compressor: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* FETIM Interlock Sensors Module */
        case ERR_INTRLK_SENS: // FETIM Interlock sensors
            switch(errNo){
                case 0x01: // Submodule out of range
                    sprintf(buf,
                            "%s",
                            "FETIM Sensors: Error: The addressed FETIM interlock sensors submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "FETIM Sensors: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "FETIM Sensors: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored single fail digital value in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Sensors: Error: The monitored single fail digital value is in the error range");
                    break;
                case 0x05: // Monitored single fail digital value in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Sensors: Warning: The monitored single fail digital value is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "FETIM Sensors: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* FETIM Interlock State Module */
        case ERR_INTRLK_STATE: // FETIM Interlock state
            switch(errNo){
                case 0x01: // Submodule out of range
                    sprintf(buf,
                            "%s",
                            "FETIM Interlock: Error: The addressed FETIM interlock state submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "FETIM Interlock: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "FETIM Interlock: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored multi fail digital value in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Interlock: Error: The monitored multi fail digital value is in the error range");
                    break;
                case 0x05: // Monitored multi fail digital value in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Interlock: Warning: The monitored multi fail digital value is in the warning range");
                    break;
                case 0x06: // Monitored temperature out of range digital value in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Interlock: Error: The monitored temperature out of range digital value is in the error range");
                    break;
                case 0x07: // Monitored temperature out of range digital value in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Interlock: Warning: The monitored temperature out of range digital value is in the warning range");
                    break;
                case 0x08: // Monitored airflow out of range digital value in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Interlock: Error: The monitored airflow out of range digital value is in the error range");
                    break;
                case 0x09: // Monitored airflow out of range digital value in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Interlock: Warning: The monitored airflow out of range digital value is in the warning range");
                    break;
                case 0x0A: // Monitored shutdwon delay triggered digital value in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Interlock: Error: The monitored shutdown delay triggered digital value is in the error range");
                    break;
                case 0x0B: // Monitored shutdown delay triggered digital value in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Interlock: Warning: The monitored shutdown delay triggered digital value is in the warning range");
                    break;
                case 0x0C: // Monitored shutdwon triggered digital value in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Interlock: Error: The monitored shutdown triggered digital value is in the error range");
                    break;
                case 0x0D: // Monitored shutdown triggered digital value in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Interlock: Warning: The monitored shutdown triggered digital value is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "FETIM Interlock: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* FETIM Interlock Temperature Module */
        case ERR_INTRLK_TEMP: // FETIM Interlock temeprature
            switch(errNo){
                case 0x01: // Submodule out of range
                    sprintf(buf,
                            "%s",
                            "FETIM Temp: Error: The addressed FETIM interlock temperature submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "FETIM Temp: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "FETIM Temp: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored interlock temperature in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Temp: Error: The monitored interlock internal temperature is in the error range");
                    break;
                case 0x05: // Monitored interlock temperature in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Temp: Warning: The monitored interlock internal temperature is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "FETIM Temp: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* FETIM Interlock Flow Module */
        case ERR_INTRLK_FLOW: // FETIM Interlock flow
            switch(errNo){
                case 0x01: // Submodule out of range
                    sprintf(buf,
                            "%s",
                            "FETIM Flow: Error: The addressed FETIM interlock flow submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "FETIM Flow: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "FETIM Flow: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored airflow in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Flow: Error: The monitored airflow is in the error range");
                    break;
                case 0x05: // Monitored airflow in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Flow: Warning: The monitored airflow is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "FETIM Flow: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* FETIM Interlock Glitch Module */
        case ERR_INTRLK_GLITCH: // FETIM Interlock GLITCH
            switch(errNo){
                case 0x01: // Submodule out of range
                    sprintf(buf,
                            "%s",
                            "FETIM Glitch: Error: The addressed FETIM interlock glitch submodule is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "FETIM Glitch: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "FETIM Glitch: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored interlock analog glitch counter value in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Glitch: Error: The monitored interlock analog glitch counter value is in the error range");
                    break;
                case 0x05: // Monitored interlock analog glitch counter value in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Glitch: Warning: The monitored interlock analog glitch counter value is in the warning range");
                    break;
                case 0x06: // Monitored interlock glitch counter tigger in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Glitch: Error: The monitored interlock glitch counter trigger is in the error range");
                    break;
                case 0x07: // Monitored interlock glitch counter trigger in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Glitch: Warning: The monitored interlock glitch counter trigger is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "FETIM Glitch: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* FETIM Compressor Temperature Module */
        case ERR_COMP_TEMP: // FETIM Compressor Temeprature
            switch(errNo){
                case 0x01: // Submodule out of range
                    sprintf(buf,
                            "%s",
                            "FETIM Compress Temp: Error: The addressed FETIM compressor temperature is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "FETIM Compress Temp: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "FETIM Compress Temp: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored compressor temperature in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Compress Temp: Error: The monitored compressor external temperature is in the error range");
                    break;
                case 0x05: // Monitored compressor temperature in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Compress Temp: Warning: The monitored compressor external temperature is in the warning range");
                    break;
                case 0x06: // Monitored temperature out of range digital value in error range
                    sprintf(buf,
                            "%s",
                            "FETIM Compress Temp: Error: The monitored external temperature out of range digital value is in the error range");
                    break;
                case 0x07: // Monitored temperature out of range digital value in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM Compress Temp: Warning: The monitored external temperature out of range digital value is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "FETIM Compress Temp: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* FETIM Compressor He2 pressure Module */
        case ERR_COMP_HE2_PRESS: // FETIM Compressor He2 pressure
            switch(errNo){
                case 0x01: // Submodule out of range
                    sprintf(buf,
                            "%s",
                            "FETIM He2 Press: Error: The addressed FETIM compressor He2 pressure is out of range");
                    break;
                case 0x02: // Control message out of range
                    sprintf(buf,
                            "FETIM He2 Press: Error: The control message is out of range");
                    break;
                case 0x03: // Monitor message out of range
                    sprintf(buf,
                            "FETIM He2 Press: Error: The monitor message is out of range");
                    break;
                case 0x04: // Monitored compressor He2 pressure in error range
                    sprintf(buf,
                            "%s",
                            "FETIM He2 Press: Error: The monitored compressor He2 pressure is in the error range");
                    break;
                case 0x05: // Monitored compressor He2 pressure in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM He2 Press: Warning: The monitored compressor He2 pressure is in the warning range");
                    break;
                case 0x06: // Monitored He2 pressure out of range digital value in error range
                    sprintf(buf,
                            "%s",
                            "FETIM He2 Press: Error: The monitored He2 pressure out of range digital value is in the error range");
                    break;
                case 0x07: // Monitored He2 pressure out of range digital value in warning range
                    sprintf(buf,
                            "%s",
                            "FETIM He2 Press: Warning: The monitored He2 pressure out of range digital value is in the warning range");
                    break;
                default: // Undefined error
                    sprintf(buf,
                            "%s%d%s",
                            "FETIM He2 Press: The specified error (",
                            errNo,
                            ") is not defined for this module");
                    break;
            }
            break;

        /* Default */
        default: // Undefined module
            sprintf(buf,
                    "%s%d%s",
                    "The specified error (",
                    errNo,
                    ") is not defined");
            break;
    }
    description = buf;
    return true;
}
