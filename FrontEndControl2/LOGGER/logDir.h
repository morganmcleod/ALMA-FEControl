/*
 * logDir.h
 *
 *  Created on: Jan 31, 2011
 *      Author: mmcleod
 */

#ifndef FECONFIG_LOGDIR_H_
#define FECONFIG_LOGDIR_H_
#include <string>

namespace FEConfig {

extern std::string logDir;

const std::string &getLogDir();

void setLogDir(const std::string newValue);

};


#endif /* FECONFIG_LOGDIR_H_ */
