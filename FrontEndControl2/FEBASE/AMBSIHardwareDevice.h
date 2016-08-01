#ifndef AMBSIHARDWAREDEVICE_H_
#define AMBSIHARDWAREDEVICE_H_

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
 * intermediate class derived from FEHardwareDevice which declares
 *   and implements the mandatory AMBSI interface monitor points.
 *----------------------------------------------------------------------
 */

#include "FEHardwareDevice.h"

class AMBSIHardwareDevice : public FEHardwareDevice {
public:
    AMBSIHardwareDevice(const std::string &name);
    virtual ~AMBSIHardwareDevice()
      {}

// AMBSI monitor points:
    virtual std::string AMBSISerialNum();
    virtual std::string AMBSIFirmwareRev();
    virtual unsigned long AMBSINumErrors();
    virtual unsigned long AMBSINumTransactions();
    virtual float AMBSITemperature();

protected:
    enum MonitorControlOffset {
        AMBSI_BASE              = 0x30000UL,
        // AMBSI monitor points:
        AMBSI_SERIAL_NUMBER     = 0x0000,   // AMBSI serial number is not offset by AMBSI_BASE
        AMBSI_FIRMWARE_REV      = 0x0000,   // AMBSI firmware library revision
        AMBSI_NUM_ERRORS        = 0x0001,   // AMBSI number of errors
        AMBSI_NUM_TRANSACTIONS  = 0x0002,   // AMBSI number of transactions
        AMBSI_TEMPERATURE       = 0x0003,   // AMBSI temperature
    };

    std::string AMBSISerialNum_value;
    std::string AMBSIFirmwareRev_value;
    unsigned long AMBSINumErrors_value;
    unsigned long AMBSINumTransactions_value;
    float AMBSITemperature_value;

private:
    AMBSIHardwareDevice(const AMBSIHardwareDevice &other);
    ///< forbid copy constructor
};

#endif /* AMBSIHARDWAREDEVICE_H_ */
