/*
 * CartridgeTestFixture.h
 */

#ifndef CARTRIDGETESTFIXTURE_H_
#define CARTRIDGETESTFIXTURE_H_
#include "AmbDeviceTestFixture.h"
#include <vector>

class CartridgeTestFixture : public AmbDeviceTestFixture {
public:

    static const bool checkIllegalFloatCommands;
    // If true, an error will be raised for "out of range" settings in commands which take a float.
    // If false, these checks are skipped.
    // Since the ICD doesn't give ranges for these, the default is false.


    CartridgeTestFixture(int band,
                         const std::string &name = std::string("CartridgeTestFixture"),
                         AmbChannel channel = 0,
                         AmbNodeAddr nodeAddress = 0x13,
                         bool expectStatusByte = true);
    ///< Construct this base class with:
    ///< - the cartridge band number (1 - 10).
    ///< - the name of your derived test fixture (optional)
    ///< - the CAN channel to use.  0 is the usual value.
    ///< - the CAN node address to use.  0x13 is the front end.  Other values for other assemblies.
    ///< - expectStatusByte as true for the front end and false for all other assemblies.

    void setUp();
    void tearDown();

protected:
    void ENABLE_CARTRIDGE();
    void DISABLE_CARTRIDGE();

    void implGenericTest(AmbRelativeAddr monitor_RCA,AmbRelativeAddr control_RCA,
    					std::vector<float> &test_values,
    					const float validmin, const float validmax, const float tolerance,
    					const std::string &callerDescription);

    void implToggleEnable(AmbRelativeAddr monitor_RCA, AmbRelativeAddr control_RCA,
        		        int SetVal, const std::string &callerDescription);

    void implGetFloat(AmbRelativeAddr monitor_RCA,
                        const float validmin, const float validmax,
                        const std::string &callerDescription);

    int band_m;                         ///< Which cartridge band this class represents.
    bool isEnabled_m;                   ///< True if the cartridge is powered on
    AmbRelativeAddr cartPowerRCA_m;     ///< RCA for power off/on cartridge
    AmbRelativeAddr cartBaseRCA_m;      ///< RCA for all cartridge m&c points
};

#endif /* CARTRIDGETESTFIXTURE_H_ */
