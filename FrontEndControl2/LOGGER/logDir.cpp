/*
 * logDir.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: mmcleod
 */

#include "logDir.h"

namespace FEConfig {

std::string logDir;

const std::string &getLogDir() {
    return logDir;
}


void setLogDir(const std::string newValue) {
    logDir = newValue;
}

};
