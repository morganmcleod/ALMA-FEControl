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
#define ERR_unassigned01        0x01 //!< Unassigned
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
#define ERR_unassigned11        0x11 //!< Unassigned
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
#define ERR_FETIM_EXT_TEMP      0x3F //!< Error in the FETIM external temperature module
#define ERR_COMP_HE2_PRESS      0x40 //!< Error in the FETIM compressor He2 pressure module

/* Error codes - shared by all modules */
#define ERC_NO_MEMORY           0x01 //!< Not enough memory
#define ERC_02                  0x02 //!<
#define ERC_IRQ_DISABLED        0x03 //!< Parallel port: IRQ was disabled
#define ERC_IRQ_RANGE           0x04 //!< Parallel port: IRQ out of range
#define ERC_AMBSI_WAIT          0x05 //!< Parallel port: Waiting for AMBSI ready
#define ERC_AMBSI_EXPIRED       0x06 //!< Parallel port: Timed out waiting for AMBSI ready.  CAN disabled
#define ERC_MAINT_MODE          0x07 //!< CAN: message blocked because FE is in MAINTENANCE mode
#define ERC_HARDWARE_TIMEOUT    0x08 //!< Timeout waiting for hardware to become ready
#define ERC_HARDWARE_ERROR      0x09 //!< Hardware is in an error state
#define ERC_HARDWARE_WAIT       0x0A //!< Waiting for previous command to finish
#define ERC_FLASH_ERROR         0x0B //!< Error reading/writing flash disk
#define ERC_HARDWARE_BLOCKED    0x0C //!< Command blocked by safety check
#define ERC_DEBUG_ME            0x0D //!< Software entered an invalid or impossible state
#define ERC_FPGA_NOT_READY      0x0E //!< Serial mux: critical error - FPGA not ready
#define ERC_0F                  0x0F //!<
#define ERC_MODULE_RANGE        0x10 //!< Submodule is out of range
#define ERC_MODULE_ABSENT       0x11 //!< Submodule is not installed
#define ERC_MODULE_POWER        0x12 //!< Submodule is not powered
#define ERC_RCA_CLASS           0x13 //!< RCA class out of range
#define ERC_RCA_RANGE           0x14 //!< RCA out of range
#define ERC_COMMAND_VAL         0x15 //!< Command value out of range

bool FEMCGetErrorDescription(unsigned short module, unsigned short errNo, std::string &description) {
    switch (module) {
        case ERR_ERROR:
            description ="Error module: ";
            break;
        case ERR_unassigned01:
            description ="Err_unassigned01: ";
            break;
        case ERR_PP:
            description ="Parallel Port: ";
            break;
        case ERR_CAN:
            description ="CAN: ";
            break;
        case ERR_CARTRIDGE:
            description ="Cartridge: ";
            break;
        case ERR_CARTRIDGE_TEMP:
            description ="Cartridge Temp. Sensor: ";
            break;
        case ERR_LO:
            description ="LO: ";
            break;
        case ERR_PLL:
            description ="LO PLL: ";
            break;
        case ERR_YTO:
            description ="LO YTO: ";
            break;
        case ERR_PHOTOMIXER:
            description ="LO Photomixer: ";
            break;
        case ERR_AMC:
            description ="LO AMC: ";
            break;
        case ERR_PA:
            description ="LO PA: ";
            break;
        case ERR_PA_CHANNEL:
            description ="LO PA Channel: ";
            break;
        case ERR_POLARIZATION:
            description ="Cartridge Polarization: ";
            break;
        case ERR_SIDEBAND:
            description ="Cartridge Sideband: ";
            break;
        case ERR_LNA_LED:
            description ="LNA LED: ";
            break;
        case ERR_SIS_HEATER:
            description ="SIS Heater: ";
            break;
        case ERR_unassigned11:
            description ="Err_unassigned11: ";
            break;
        case ERR_SIS:
            description ="SIS: ";
            break;
        case ERR_SIS_MAGNET:
            description ="SIS Magnet: ";
            break;
        case ERR_LNA:
            description ="LNA: ";
            break;
        case ERR_LNA_STAGE:
            description ="LNA Stage: ";
            break;
        case ERR_POL_SPECIAL_MSGS:
            description ="Pol. Special Messages: ";
            break;
        case ERR_POL_DAC:
            description ="Pol. DAC: ";
            break;
        case ERR_SERIAL_INTERFACE:
            description ="Serial: ";
            break;
        case ERR_SERIAL_MUX:
            description ="Serial Mux: ";
            break;
        case ERR_TIMER:
            description ="Timer: ";
            break;
        case ERR_BIAS_SERIAL:
            description ="Bias Serial: ";
            break;
        case ERR_LO_SERIAL:
            description ="LO Serial: ";
            break;
        case ERR_POWER_DISTRIBUTION:
            description ="Power Dist.: ";
            break;
        case ERR_PD_MODULE:
            description ="Power Dist. Module: ";
            break;
        case ERR_PD_CHANNEL:
            description ="Power Dist. Channel: ";
            break;
        case ERR_PD_SERIAL:
            description ="Power Dist. Serial: ";
            break;
        case ERR_IF_CHANNEL:
            description ="IF Switch Channel: ";
            break;
        case ERR_IF_SWITCH:
            description ="IF Switch: ";
            break;
        case ERR_IF_SERIAL:
            description ="IF Switch Serial: ";
            break;
        case ERR_CRYOSTAT:
            description ="Cryostat: ";
            break;
        case ERR_TURBO_PUMP:
            description ="Cryo. Turbo Pump: ";
            break;
        case ERR_VACUUM_CONTROLLER:
            description ="Cryo. Vacuum Controller: ";
            break;
        case ERR_GATE_VALVE:
            description ="Cryo. Gate Valve: ";
            break;
        case ERR_SOLENOID_VALVE:
            description ="Cryo. Solenoid Valve: ";
            break;
        case ERR_VACUUM_SENSOR:
            description ="Cryo. Vacuum Sensor: ";
            break;
        case ERR_CRYOSTAT_TEMP:
            description ="Cryo. Temperature: ";
            break;
        case ERR_CRYO_SERIAL:
            description ="Cryo Serial: ";
            break;
        case ERR_MODULATION_INPUT:
            description ="LPR Modulation Input: ";
            break;
        case ERR_PHOTO_DETECTOR:
            description ="LPR Photo Detector: ";
            break;
        case ERR_LASER:
            description ="LPR Laser: ";
            break;
        case ERR_EDFA:
            description ="LPR EDFA: ";
            break;
        case ERR_OPTICAL_SWITCH:
            description ="LPR Optical Switch: ";
            break;
        case ERR_LPR:
            description ="LPR: ";
            break;
        case ERR_LPR_TEMP:
            description ="LPR Temperature: ";
            break;
        case ERR_LPR_SERIAL:
            description ="LPR Serial: ";
            break;
        case ERR_MI_DAC:
            description ="LPR Modulation Input DAC: ";
            break;
        case ERR_INI:
            description ="INI Files module: ";
            break;
        case ERR_OWB:
            description ="One Wire Bus: ";
            break;
        case ERR_FETIM:
            description ="FETIM: ";
            break;
        case ERR_INTERLOCK:
            description ="FETIM Interlock: ";
            break;
        case ERR_COMPRESSOR:
            description ="FETIM Compressor: ";
            break;
        case ERR_INTRLK_SENS:
            description ="FETIM Interlock Sensors: ";
            break;
        case ERR_INTRLK_STATE:
            description ="FETIM Interlock State: ";
            break;
        case ERR_INTRLK_TEMP:
            description ="FETIM Interlock Temperature: ";
            break;
        case ERR_INTRLK_FLOW:
            description ="FETIM Interlock Flow: ";
            break;
        case ERR_INTRLK_GLITCH:
            description ="FETIM Interlock Glitch: ";
            break;
        case ERR_FETIM_EXT_TEMP:
            description ="FETIM External Temperature: ";
            break;
        case ERR_COMP_HE2_PRESS:
            description ="FETIM Compressor HE2 Press.: ";
            break;
        default:
            description = "Unknown module: ";
            break;
    }
    switch (errNo) {
        case ERC_NO_MEMORY:
            description += "Not enough memory";
            break;
        case ERC_02:
            description += "Undefined error";
            break;
        case ERC_IRQ_DISABLED:
            description += "Parallel port: IRQ was disabled";
            break;
        case ERC_IRQ_RANGE:
            description += "Parallel port: IRQ out of range";
            break;
        case ERC_AMBSI_WAIT:
            description += "Parallel port: Waiting for AMBSI ready";
            break;
        case ERC_AMBSI_EXPIRED:
            description += "Parallel port: Timed out waiting for AMBSI ready.  CAN disabled";
            break;
        case ERC_MAINT_MODE:
            description += "Message blocked because FE is in MAINTENANCE mode";
            break;
        case ERC_HARDWARE_TIMEOUT:
            description += "Timeout waiting for hardware to become ready";
            break;
        case ERC_HARDWARE_ERROR:
            description += "Hardware is in an error state";
            break;
        case ERC_HARDWARE_WAIT:
            description += "Waiting for previous command to finish";
            break;
        case ERC_FLASH_ERROR:
            description += "Error reading/writing flash disk";
            break;
        case ERC_HARDWARE_BLOCKED:
            description += "Command blocked by safety check";
            break;
        case ERC_DEBUG_ME:
            description += "DEBUG ME: Software entered an invalid or impossible state";
            break;
        case ERC_FPGA_NOT_READY:
            description += "Serial mux: critical error - FPGA not ready";
            break;
        case ERC_0F:
            description += "Undefined error";
            break;
        case ERC_MODULE_RANGE:
            description += "Submodule is out of range";
            break;
        case ERC_MODULE_ABSENT:
            description += "Submodule is not installed";
            break;
        case ERC_MODULE_POWER:
            description += "Submodule is not powered";
            break;
        case ERC_RCA_CLASS:
            description += "RCA class out of range";
            break;
        case ERC_RCA_RANGE:
            description += "RCA out of range";
            break;
        case ERC_COMMAND_VAL:
            description += "Command value out of range";
            break;
        default:
            description += "Undefined error";
            break;
    }
    return true;
}
