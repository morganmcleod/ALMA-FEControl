#ifndef MeasureFineLOSweep_H_
#define MeasureFineLOSweep_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2009
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

/// \file
/// \brief Worker thread object to measure the SIS current, IF output power, and PA drain voltage while sweeping the LO frequency.

#include "OptimizeBase.h"
#include "CONFIG/FrontEndDataBase.h"
#include "XYPlotArray.h"
#include <iostream>
class CartAssembly;

class MeasureFineLOSweep : public OptimizeBase {
public:    
    MeasureFineLOSweep(FrontEndDatabase &dbObject,  ///< database interface for saving results
                       CartAssembly &CA,            ///< CartAssembly for fine LO sweep
                       XYPlotArray &target)         ///< output data array for plot results
      : OptimizeBase("MeasureFineLOSweep"),
        dbObject_m(dbObject),
        cartAssembly_m(CA),
        sisCurrents_m(),
        loPaVoltages_m(target),
        dataFile_mp(NULL)
        { reset(); }

    virtual ~MeasureFineLOSweep()
      { delete dataFile_mp; }
    
    void reset();
    ///< reset all state to initial/just constructed.
    
    bool start(const FEICDataBase::ID_T &feConfig,          ///< front end configuration ID for saving results
               FEICDataBase::DATASTATUS_TYPES dataStatus,   ///< data status for saving results
               float tiltAngle, int pol, float VJ, float IJ,
               const float *fixedVD,
               double LOStart, double LOStop, double LOStep, int repeatCount = 1);
    ///< start a measurement of SIS current, IF power, and PA drain voltage vs. LO frequency

protected:    
    virtual void optimizeAction();
    ///< worker thread calls this function repeatedly until done.

    virtual void exitAction(bool success);
    ///< called by OptimizeBase prior to worker thread exit.

private:
    void saveData(int pol);
    
    FrontEndDatabase &dbObject_m;   ///< database interface for saving results.
    CartAssembly &cartAssembly_m;   ///< the cold cartridge assembly to measure.

    FEICDataBase::ID_T feConfig_m;  ///< configuration ID for the front end
    FEICDataBase::DATASTATUS_TYPES dataStatus_m;    ///< data status for saving results
    FrontEndDatabase::ID_T testDataHeaderId_m;      ///< TestDataHeader key for saving results

    int pol_m;                  ///< Polarization to measure
    float tiltAngle_m;          ///< Tilt elevation of this measurement
    float VJNom01_m;            ///< Pol0 SIS1 voltage prior to starting measurement.
    float VJNom02_m;            ///< Pol0 SIS2 voltage prior to starting measurement.
    float VJNom11_m;            ///< Pol1 SIS1 voltage prior to starting measurement.
    float VJNom12_m;            ///< Pol1 SIS2 voltage prior to starting measurement.
    float VDNom0_m;             ///< Pol0 LO PA drain voltage prior to starting measurement.
    float VDNom1_m;             ///< Pol1 LO PA drain voltage prior to starting measurement.

    double freqLONom_m;         ///< LO frequency prior to starting measurement.
    double freqFLOOG_m;         ///< FLOOG frequency in use by the cart assembly.
    int sbLock_m;               ///< lock below/above ref setting in use by cart assembly.

    float VJ_m;                 ///< junction voltage setting to use for entire sweep.
    float IJ_m;                 ///< junction current to maintain at each LO step.
    float fixedVD_m;            ///< if >=0 we are measuring at a fixed LO PA drain voltage instead of optimizing it.
    double LOStart_m;           ///< One end of range for LO frequency to sweep.  
    double LOStop_m;            ///< Other end of range for LO frequency to sweep.
    double LOStep_m;            ///< Step size for LO frequency to sweep.
    int steps_m;                ///< number of LO steps to measure.
    
    double FLO_m;               ///< Current value of LO frequency.
    int repeatCount_m;          ///< how many times to repeat the measurement.
    float progress_m;           ///< progress bar value.
    float progressIncrement_m;  ///< how much to increment the progress bar at each step.
    XYPlotArray sisCurrents_m;  ///< output data array for SIS current vs FreqLO
    XYPlotArray &loPaVoltages_m;///< output data array for LO PA drain voltage vs FreqLO.
    std::ostream *dataFile_mp;  ///< the output log file.
};

#endif /*MeasureFineLOSweep_H_*/
