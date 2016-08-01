#ifndef CARTHEALTHCHECK_H_
#define CARTHEALTHCHECK_H_
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

/// \file
/// \brief Worker thread to perform CCA/WCA health check procedure.

#include "OptimizeBase.h"
#include "CONFIG/FrontEndDataBase.h"
class CartAssembly;
class XYPlotArray;

class CartHealthCheck : public OptimizeBase {
public:
    CartHealthCheck(FrontEndDatabase &dbObject, ///< database interface for saving results
                    CartAssembly &ca,           ///< CartAssembly to healthCheck
                    XYPlotArray &ivData)        ///< output data array for I-V curves
      : OptimizeBase("CartHealthCheck"),
        dbObject_m(dbObject),
        ca_m(ca),
        ivData_m(ivData)
        { reset(); }

    virtual ~CartHealthCheck()
      {}

    void reset();
    ///< reset all state to initial/just constructed.

    bool prepare(const FEICDataBase::ID_T &feConfig,        ///< front end configuration ID for saving results
                 FEICDataBase::DATASTATUS_TYPES dataStatus, ///< data status for saving results
                 double &freqLOret,                         ///< LO frequency for the cartridge is returned here.
                 bool receiverIsCold,                       ///< true if the receiver is cold
                 int warmUpTimeSeconds);                    ///< how long to wait before taking health check data
    ///< set up the cartridge for performing the health check.   Returns the cartridge LO frequency in reference parameter freqLOret.

    bool start();
    ///< wait the specified warmUpTimeSeconds start the health check measurements.
    ///< executes on worker thread.

protected:
    virtual void optimizeAction();
    ///< worker thread method to take the measurements.

    virtual void exitAction(bool success);
    ///< gets called just prior to the thread exiting.

private:
    FrontEndDatabase &dbObject_m;   ///< database interface for saving results.
    CartAssembly &ca_m;             ///< CartAssembly to healthCheck
    XYPlotArray &ivData_m;          ///< output data array for I-V curves

    FEICDataBase::ID_T feConfig_m;  ///< configuration ID for the front end
    FEICDataBase::DATASTATUS_TYPES dataStatus_m;    ///< data status for saving results
    bool receiverIsCold_m;          ///< true if receiver is cold
    int warmUpTimeSeconds_m;        ///< how long to wait before taking health check data
    double freqLO_m;                ///< LO frequency for cartridge operation
};


#endif /* CARTHEALTHCHECK_H_ */
