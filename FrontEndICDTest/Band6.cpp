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

#include "band6.h"

CPPUNIT_TEST_SUITE_REGISTRATION(Band6CCATestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(Band6WCATestFixture);

const std::vector<float> &Band6CCATestFixture::getTestValuesSISVoltage() const {
    static bool firstTime(true);
    static std::vector<float> values;
    if (firstTime) {
        firstTime = false;
        values.reserve(5);
        values.resize(0);
        values.push_back(-12.0);    // in-range value
        values.push_back(0.0);      // in-range value
        values.push_back(5.0);      // in-range value
        values.push_back(12.0);     // in-range value
        values.push_back(100.0);    // impossible value
    }
    return values;
}

void Band6CCATestFixture::getLimitsSisVoltage(float &min, float &max, float &tolerance) const {
    //const float ColdCartridgeTestFixture::ValidMinSISVoltage = -25.0;
    //const float ColdCartridgeTestFixture::ValidMaxSISVoltage = 25.0;
    //const float ColdCartridgeTestFixture::ToleranceSISVoltage = 0.5;
    min = -12.0;
    max = 12.0;
    tolerance = 5.0;
}

void Band6CCATestFixture::getLimitsSISCurrent(float &min, float &max) const {
    //SIS Current valid range in uA
    //const float ColdCartridgeTestFixture::ValidMinSISCurrent = -100.0;
    //const float ColdCartridgeTestFixture::ValidMaxSISCurrent = 100.0;
    min = -100.0;
    max = 100.0;
}

const std::vector<float> &Band6CCATestFixture::getTestValuesMagnetCurrent() const {
    static bool firstTime(true);
    static std::vector<float> values;
    if (firstTime) {
        firstTime = false;
        values.push_back(-100.0);   // in-range value (was -125 which is too big for band 6)
        values.push_back(0.0);      // in-range value
        values.push_back(20.0);     // in-range value
        values.push_back(100.0);    // in-range value (was 125 which is too big for band 6)
        values.push_back(200.0);    // impossible value
    }
    return values;
}

void Band6CCATestFixture::getLimitsMagnetCurrent(float &min, float &max, float &tolerance) const {
    //const float ColdCartridgeTestFixture::ValidMinSISMagnetCurrent = -125.0;
    //const float ColdCartridgeTestFixture::ValidMaxSISMagnetCurrent = 125.0;
    //const float ColdCartridgeTestFixture::ToleranceSISMagnetCurrent = 0.5;
    min = -100.0;   // (was -125 which is too big for band6)
    max = 100.0;    // (was 125 which is too big for band6)
    tolerance = 0.05 * max; // 5%
}

void Band6CCATestFixture::getLimitsMagnetVoltage(float &min, float &max) const {
    //SIS Magnet Voltage valid range in mV
    //const float ColdCartridgeTestFixture::ValidMinSISMagnetVoltage = -5.0;
    //const float ColdCartridgeTestFixture::ValidMaxSISMagnetVoltage = 5.0;
    min = -5.0;
    max = 5.0;
}

void Band6CCATestFixture::getLimitsHeaterCurrent(float &min, float &max) const {
    //SIS Heater Current valid range in mA
    //const float ColdCartridgeTestFixture::ValidMinSISHeaterCurrent = 0.0;
    //const float ColdCartridgeTestFixture::ValidMaxSISHeaterCurrent = 200.0;
    min = 0.0;
    max = 200.0;
}
