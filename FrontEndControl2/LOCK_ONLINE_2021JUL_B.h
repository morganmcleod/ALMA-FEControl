/*
 * LOCK_ONLINE_2021JUL_B.h
 *
 *  Created on: Oct 19, 2021
 *      Author: mmcleod
 */

#ifndef LOCK_ONLINE_2021JUL_B_H_
#define LOCK_ONLINE_2021JUL_B_H_

#include "LockingStrategy.h"
class WCAImpl;

class LOCK_ONLINE_2021JUL_B : public LockingStrategy {
public:
    LOCK_ONLINE_2021JUL_B(bool allowEdfaAdjust = true)
      : LockingStrategy(allowEdfaAdjust)
        {}
    virtual ~LOCK_ONLINE_2021JUL_B() {}
    virtual bool lockPLL(CartAssembly &CA) const;

    void optimizeLPRVoltage(WCAImpl *WCA_p, double target_pll_if_pmW,
                            double lpr_step, double pplimit,
                            bool set_mod_input = false) const;
    void lowerLPRVoltage(WCAImpl *WCA_p,
                         double target_pll_if_pmW,
                         double lpr_step) const;
private:
    void maximizeIFTP(WCAImpl *WCA_p, const int yigSpan) const;
};

#endif /* LOCK_ONLINE_2021JUL_B_H_ */
