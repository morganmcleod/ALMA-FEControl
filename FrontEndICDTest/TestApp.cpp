/*******************************************************************************
* ALMA - Atacama Large Millimiter Array
* (c) Associated Universities Inc., 2010
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


#include "TestApp.h"
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <logger.h>
#include <FrontEndAMB/NICANBusInterface.h>
#include <setTimeStamp.h>
#include <string>

namespace FrontEndTestApp {
    const AmbInterface *ambItf_p(NULL);
    CANBusInterface *canBus_p(NULL);
    FILE *logStream = NULL;
    const char *versionString = "1.4.1: 2019-12-19  Verify changes for FEMC 2.9.x";
//                              "1.3.3: 2017-01-06  Used to verify FEMC 2.8.4.  No delay after STANDBY2 command.";
//                              "1.3.2: 2016-12-31  Used to verify FEMC 2.8.2.  No CRYOSTAT_COLD_HEAD_HOURS.  CAN timeout=2ms";
//                              "1.3.1: 2016-12-05  Used to verify FEMC 2.8.0";
//                              "1.3.0: 2016-11-30  Added CRYOSTAT_COLD_HEAD_HOURS, strict ICD for the cryostat";
//                              "1.2.0: 2016-11-15  Added band6 STANDBY2 support.";
//                              "1.1.0: 2014-09-02  Modified timing of CPDSTestFixture::implPwrDistributionModule";
//                              "       Fix incorrect RCA enum for band 9 CPDS enable/monitor.";
//                              "       Loop to turn off bands 1-10 before CPDS test.";
//                              "1.0.1: 2013-04-24  Recompiled in 3.4.5 (mingw-vista special r3)";
//                              "1.0.0: 2010-10-22  Initial release, was used for report FEND-40.00.00.00-0416-A-REP";
};
using namespace FrontEndTestApp;


int main(int argc, char* argv[]) {
    printf("FrontEndICDTest:\n"
           "Regression test for the FEMC CAN interface and FE-Computing ICD\n%s\n", versionString);

    bool wasSucessful(false);

    // Set the logger output to a unique file:
    Time ts;
    std::string tmp, logFile;
    setTimeStamp(&ts);
    timestampToText(&ts, tmp, true);
    logFile = "ICDLOG-" + tmp + ".txt";
    logStream = fopen(logFile.c_str(), "w");
    StreamOutput::setStream(logStream);

    // Create the AMB/CAN connection:
    ambItf_p = AmbInterface::getInstance();
    canBus_p = new NICANBusInterface();
    
    if (ambItf_p && canBus_p) {
        // Set the CAN bus to be used by the AmbInterface:
        ambItf_p -> setBus(canBus_p);
    
        // Set the CAN timeout to the minimum:
        canBus_p -> monitorTimeout_m = 2;   //ms
        
        // Get the top level suite from the registry
        CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();
    
        // Adds the test to the list of test to run
        CPPUNIT_NS::TextUi::TestRunner runner;
        runner.addTest( suite );
    
        // Run the test.
        wasSucessful = runner.run();

        // Destroy AMB/CAN connection:
        AmbInterface::deleteInstance();
        ambItf_p = NULL;
        delete canBus_p;
        canBus_p = NULL;
    }

    // Close the output log stream:
    StreamOutput::setStream(NULL);
    if (logStream) {
        fflush(logStream);
        fclose(logStream);
        delete logStream;
    }

    // Return error code 1 if the one of test failed or could not construct AMB/CAN connection:
    return wasSucessful ? 0 : 1;
}

