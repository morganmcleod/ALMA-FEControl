/*
 * MeasureSISVoltageError.cpp
 *
 *  Created on: Feb 15, 2017
 *      Author: mmcleod
 */

#include "MeasureSISVoltageError.h"
#include "CartAssembly.h"
#include "stringConvert.h"
using namespace std;

bool MeasureSISVoltageError::start() {
	string msg("MeasureSISVoltageError band=");
	msg += to_string(ca_m.getBand());
	msg += ": process started.";
	setStatusMessage(true, msg);
	return OptimizeBase::startWorkerThread();
}

void MeasureSISVoltageError::optimizeAction() {
	bool success = true;

	// pause CCA monitoring while measuring voltage errors:
	ca_m.pauseMonitor(true, true, "measureSISVoltageError");

	// If band 5 or above, save the prior enabled state of the magnets:
	float iSet01(0.0), iSet02(0.0), iSet11(0.0), iSet12(0.0);
	bool magnetPrior(false);

	ColdCartImpl *cc = ca_m.useColdCart();

	if (cc -> getBand() <= 4)
		// delay to allow cartridge with no magnets to fully bias up:
		SLEEP(1500);

	else {
		magnetPrior = cc -> getSISMagnetEnableSetting();

		// save the prior current settings voltages, if enabled:
		if (magnetPrior) {
			iSet01 = cc -> getSISMagnetCurrentSetting(0, 1);
			iSet02 = cc -> getSISMagnetCurrentSetting(0, 2);
			iSet11 = cc -> getSISMagnetCurrentSetting(1, 1);
			iSet12 = cc -> getSISMagnetCurrentSetting(1, 2);
		}

		// enable the magnets normally, using the configuration database for settings:
		ca_m.setSISMagnet(true);
	}
	// measure the voltage offset errors:
	cc -> measureSISVoltageError(&result_m);

	// set the magnets back to their prior state:
	if (magnetPrior) {
		ca_m.setSISMagnet(true, 0, 1, &iSet01);
		ca_m.setSISMagnet(true, 0, 2, &iSet02);
		ca_m.setSISMagnet(true, 1, 1, &iSet11);
		ca_m.setSISMagnet(true, 1, 2, &iSet12);
	} else
		ca_m.setSISMagnet(false);

	// resume monitoring:
	ca_m.pauseMonitor(false, false);

	setFinished(success);
}

void MeasureSISVoltageError::exitAction(bool success) {
	string msg("MeasureSISVoltageError band=");
	msg += to_string(ca_m.getBand());

	if (success) {
		msg += " finished: ";
		msg += result_m;

	} else if (stopRequested())
		msg += ": process stopped.";
	else
		msg += ": failed.";

	setStatusMessage(success, msg);
}

