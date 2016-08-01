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

#include "MixerHeating.h"
#include "XYPlotArray.h"
#include "logger.h"
#include "stringConvert.h"
#include "ColdCartImpl.h"
#include <fstream>
#include <iomanip>
using namespace std;

void MixerHeating::reset() {
    pol_m = -1;    
    targetTemp_m = 0.0;
    timeout_m = 0;
}

void MixerHeating::requestStop() {
    coldCart_m.stopMixerHeating();
    OptimizeBase::requestStop();
}

bool MixerHeating::start(int pol, float targetTemp, int timeout) {
    reset();
    pol_m = pol;
    targetTemp_m = targetTemp;
    timeout_m = timeout;
    return OptimizeBase::startWorkerThread();
}

void MixerHeating::optimizeAction() {
    bool ret = coldCart_m.sisMixerHeatingProcess(pol_m, targetTemp_m, timeout_m);
    setFinished(ret);
}



