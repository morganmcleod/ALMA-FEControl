/*
 * LOCK_ONLINE_2021JUL_B.h
 *
 *  Created on: Oct 19, 2021
 *      Author: mmcleod
 */

#ifndef ICT_19283_band1_lock_five_point_H_
#define ICT_19283_band1_lock_five_point_H_

#include "LockingStrategy.h"
class WCAImpl;

class ICT_19283_LOCK_CV_Points : public LockingStrategy {
public:
    ICT_19283_LOCK_CV_Points(int numPoints = 5, int interval = 6, bool allowEdfaAdjust = true)
      : LockingStrategy(allowEdfaAdjust),
        numPoints_m(numPoints),
        interval_m(interval)
        {}
    virtual ~ICT_19283_LOCK_CV_Points() {}

    virtual bool lockPLL(CartAssembly &CA) const;
    // main entry point for locking strategy

    void setLPRVoltageForPmxCurrent(WCAImpl *WCA_p,
                                    double targetPmxCurrent_mA,
                                    double lprStep_V) const;
    // Set the LPR EDFA to the 'knee' voltage and then adjust upward until the target photomixer current is reached

    void setLPRVoltageForIFTP(WCAImpl *WCA_p,
                              double targetIFTP_V,
                              double toleranceIFTP_V,
                              double lprStep_V,
                              double pdPowerLimit_mW) const;
    // When locked: adjust the LPR EDFA to attain the requested IFTP detector voltage and tolerance
    // Stopping if the LPR photodetector power limit is exceeded.

private:
    int numPoints_m;
    int interval_m;
};

#endif /* ICT_19283_band1_lock_five_point_H_ */
