#ifndef FEMCTESTFIXTURE_H
#define FEMCTESTFIXTURE_H

#include "AmbDeviceTestFixture.h"
#include <cppunit/extensions/HelperMacros.h>

class FEMCTestFixture : public AmbDeviceTestFixture {
    CPPUNIT_TEST_SUITE(FEMCTestFixture);
    
    CPPUNIT_TEST(testAMBSI_SERIALNUM);
    CPPUNIT_TEST(testAMBSI_SLAVE_REV);
    CPPUNIT_TEST(testAMBSI_NUM_ERRORS);
    CPPUNIT_TEST(testAMBSI_NUM_TRANSACTIONS);
    CPPUNIT_TEST(testAMBSI_TEMPERATURE);
    CPPUNIT_TEST(testAMBSI_SOFTWARE_REV);
    CPPUNIT_TEST(testAMBSI_HARDWARE_REV);
    CPPUNIT_TEST(testAMBSI_HARDWARE_RESET);
    
    CPPUNIT_TEST(testGET_AMBSI1_VERSION);
    CPPUNIT_TEST(testGET_SETUP_INFO);
    CPPUNIT_TEST(testGET_VERSION_INFO);
    CPPUNIT_TEST(testGET_SPECIAL_MONITOR_RCA);
    CPPUNIT_TEST(testGET_SPECIAL_CONTROL_RCA);
    CPPUNIT_TEST(testGET_MONITOR_RCA);
    CPPUNIT_TEST(testGET_CONTROL_RCA);
    CPPUNIT_TEST(testGET_PPCOMM_TIME);
    CPPUNIT_TEST(testGET_FPGA_VERSION_INFO);
    CPPUNIT_TEST(testCONSOLE_ENABLE);
    CPPUNIT_TEST(testESNS);
    CPPUNIT_TEST(testERRORS);
    CPPUNIT_TEST(testFE_MODE);
    
    CPPUNIT_TEST(testSET_EXIT_PROGRAM);
    CPPUNIT_TEST(testSET_REBOOT);
    
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp();
    void tearDown();
    
    void testAMBSI_SERIALNUM();
    void testAMBSI_SLAVE_REV();
    void testAMBSI_NUM_ERRORS();
    void testAMBSI_NUM_TRANSACTIONS();
    void testAMBSI_TEMPERATURE();
    void testAMBSI_SOFTWARE_REV();
    void testAMBSI_HARDWARE_REV();
    void testAMBSI_HARDWARE_RESET();
    
    void testGET_AMBSI1_VERSION();
    void testGET_SETUP_INFO();
    void testGET_VERSION_INFO();
    void testGET_SPECIAL_MONITOR_RCA();
    void testGET_SPECIAL_CONTROL_RCA();
    void testGET_MONITOR_RCA();
    void testGET_CONTROL_RCA();
    void testGET_PPCOMM_TIME();
    void testGET_FPGA_VERSION_INFO();
    void testCONSOLE_ENABLE();
    void testESNS();
    void testERRORS();
    void testFE_MODE();
    
    void testSET_EXIT_PROGRAM();
    void testSET_REBOOT();
};

#endif  // FEMCTESTFIXTURE_H
