#ifndef INCLUDE_FRONTENDAMB_FEMCDEFS_H_
#define INCLUDE_FRONTENDAMB_FEMCDEFS_H_

/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2022
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
*/

/// Error codes defined by the FEMC firmware and by this module
enum FEMC_ERROR {
    FEMC_NO_ERROR           = 0,    //!< No error during the monitor operation.
    FEMC_GENERIC_ERROR      = -1,   //!< Error during the monitor operation.  FEMC failed to communicate with hardware.
    FEMC_HARDW_RANGE_ERR    = -2,   //!< The addressed hardware is not installed.
    FEMC_HARDW_BLOCKED_ERR  = -3,   //!< The hardware cannot be accessed because of operational state.
    FEMC_HARDW_UPDATE_WARN  = -4,   //!< Properties of the addressed hardware need to be updated in firmware.  Result may be suspect.
    FEMC_HARDW_CONVERT_ERR  = -5,   //!< Monitor data is out of range for conversion to proper units.
    FEMC_HARDW_RETRY_WARN   = -6,   //!< The monitor point takes time to stabilize.  Retry the request immediately.
    FEMC_MON_ERROR_RANGE    = -10,  //!< The monitor value is in the error range, according to the configuration database.
    FEMC_MON_WARN_RANGE     = -11,  //!< The monitor value is in the warning range, according to the configuration database.
    FEMC_UNDEFINED_RCA      = -12,  //!< The monitor request is to an undefined RCA.
    FEMC_MON_ERROR_ACTION   = -13,  //!< FEMC took action because the monitor value is in the error range.
    FEMC_MON_WARN_ACTION    = -14,  //!< FEMC took action because the monitor value is in the warning range.
    FEMC_MON_NOTIMPL        = -15,  //!< The monitor request is not yet implemented in hardware.  Result may be suspect.

    FEMC_LAST_ERROR_CODE    = -31,  //!< We assume that this is the largest negative number the FEMC will return as a status byte.
                                    // the following codes are generated by this library, not the FEMC module:

    FEMC_AMB_ERROR          = -44,  //!< The AMB subsystem reported an error.
    FEMC_UNPACK_ERROR       = -45,  //!< Unpacking monitor data failed
    FEMC_NOT_CONNECTED      = -46   //!< Cannot monitor because not connected to FE.
};

#endif /* INCLUDE_FRONTENDAMB_FEMCDEFS_H_ */
