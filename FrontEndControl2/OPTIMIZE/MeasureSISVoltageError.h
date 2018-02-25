/*
 * MeasureSISVoltageError.h
 *
 *  Created on: Feb 15, 2017
 *      Author: mmcleod
 */

#ifndef OPTIMIZE_MEASURESISVOLTAGEERROR_H_
#define OPTIMIZE_MEASURESISVOLTAGEERROR_H_

#include "OptimizeBase.h"

class CartAssembly;

class MeasureSISVoltageError: public OptimizeBase {
public:
	MeasureSISVoltageError(CartAssembly &ca)
	  : ca_m(ca),
	    measureOnMainThread_m(false)
		{}

	virtual ~MeasureSISVoltageError()
	  {}

	bool start();

protected:
    virtual void optimizeAction();

    virtual void exitAction(bool success);

private:
    CartAssembly &ca_m;
    std::string result_m;
    bool measureOnMainThread_m;
};
#endif /* OPTIMIZE_MEASURESISVOLTAGEERROR_H_ */
