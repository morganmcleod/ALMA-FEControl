/*
 * LockingStrategy.h
 *
 *  Created on: Oct 19, 2021
 *      Author: mmcleod
 */

#ifndef LOCKINGSTRATEGY_H_
#define LOCKINGSTRATEGY_H_
#include <cstddef>
class CartAssembly;
class LPRImpl;
class WCAImpl;

class LockingStrategy {
public:
    LockingStrategy(bool allowEdfaAdjust = true)
      : allowEdfaAdjust_m(allowEdfaAdjust)
        {}
    virtual ~LockingStrategy() {}

    static void setLPR(LPRImpl *LPR)
      { LPR_p = LPR; }

    virtual bool lockPLL(CartAssembly &CA) const = 0;
protected:
    static LPRImpl *LPR_p;
    bool allowEdfaAdjust_m;

private:
    // disallow copy and assignment:
    LockingStrategy(const LockingStrategy &other);
    LockingStrategy &operator =(const LockingStrategy &other);
};

class LOCK_Normal : public LockingStrategy {
///< Original FETMS locking strategy.
///< Does not adjust LPR EDFA and ignores allowEdfaAdjust_m.
public:
    LOCK_Normal(bool allowEdfaAdjust = false)
      : LockingStrategy(false)  ///< we don't adjust the EDFA in this strategy
        {}
    virtual ~LOCK_Normal() {}
    virtual bool lockPLL(CartAssembly &CA) const;
};

#endif /* LOCKINGSTRATEGY_H_ */
