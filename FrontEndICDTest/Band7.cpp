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

#include "Band7.h"

CPPUNIT_TEST_SUITE_REGISTRATION(Band7CCATestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(Band7WCATestFixture);

const std::vector<float> &Band7CCATestFixture::getTestValuesSISVoltage() const {
    static bool firstTime(true);
    static std::vector<float> values;
    if (firstTime) {
        firstTime = false;
        values.reserve(5);
        values.resize(0);
        values.push_back(-3.0);     // below-range value
        values.push_back(1.0);      // in-range value
        values.push_back(2.0);      // in-range value
        values.push_back(3.0);      // in-range value
        values.push_back(100.0);    // impossible value
    }
    return values;
}

void Band7CCATestFixture::getLimitsSisVoltage(float &min, float &max, float &tolerance) const {
    min = 0.0;
    max = 3.5;
    tolerance = 0.5;
}

void Band7CCATestFixture::getLimitsSISCurrent(float &min, float &max) const {
    min = -100.0;
    max = 100.0;
}

const std::vector<float> &Band7CCATestFixture::getTestValuesMagnetCurrent() const {
    static bool firstTime(true);
    static std::vector<float> values;
    if (firstTime) {
        firstTime = false;
        values.push_back(-60.0);    // in-range value
        values.push_back(0.0);      // in-range value
        values.push_back(20.0);     // in-range value
        values.push_back(60.0);     // in-range value
        values.push_back(200.0);    // impossible value
    }
    return values;
}

void Band7CCATestFixture::getLimitsMagnetCurrent(float &min, float &max, float &tolerance) const {
    min = -100.0;
    max = 100.0;
    tolerance = 0.05 * max; // 5%
}

void Band7CCATestFixture::getLimitsMagnetVoltage(float &min, float &max) const {
    min = -3.0;
    max = 3.0;
}

void Band7CCATestFixture::getLimitsHeaterCurrent(float &min, float &max) const {
    min = 0.0;
    max = 200.0;
}
