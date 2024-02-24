#ifndef FEModeTestFixture_H
#define FEModeTestFixture_H

#include "AmbDeviceTestFixture.h"
#include <cppunit/extensions/HelperMacros.h>

class FEModeTestFixture : public AmbDeviceTestFixture {

    CPPUNIT_TEST_SUITE(FEModeTestFixture);
    CPPUNIT_TEST(testFE_MODE);
    CPPUNIT_TEST_SUITE_END();
    
public:

    FEModeTestFixture()
      : AmbDeviceTestFixture(std::string("FEModeTestFixture"))
        { expectStatusByte_m = false; }

    void setUp();
    void tearDown();
    
    void testFE_MODE();
};

#endif  // FEModeTestFixture_H
