#ifndef AMBDEVICETESTFIXTURE_H
#define AMBDEVICETESTFIXTURE_H

#include <cppunit/TestFixture.h>
#include <FrontEndAMB/ambDeviceImpl.h>
#include <string>

class FEAmbDevice : public AmbDeviceImpl {
public:
    FEAmbDevice(const std::string &name, AmbChannel channel, AmbNodeAddr nodeAddress)
      : AmbDeviceImpl(name)
        { m_channel = channel;
          m_nodeAddress = nodeAddress; }
};

class AmbDeviceTestFixture : public CPPUNIT_NS::TestFixture {
public:
    AmbDeviceTestFixture(const std::string &name = std::string("AmbDeviceTestFixture"), 
                         AmbChannel channel = 0, 
                         AmbNodeAddr nodeAddress = 0x13,
                         bool expectStatusByte = true)
      : device_p(NULL),
        expectStatusByte_m(expectStatusByte),
        name_m(name),
        channel_m(channel),
        nodeAddress_m(nodeAddress)
        {}
    ///< Construct this base class with:
    ///< - the name of your derived test fixture (optional)
    ///< - the CAN channel to use.  0 is the usual value.
    ///< - the CAN node address to use.  0x13 is the front end.  Other values for other assemblies.
    ///< - expectStatusByte as true for the FE STANDARD assemblies and false for all other assemblies.
    
    void setUp();
    void tearDown();

protected:
    FEAmbDevice *device_p;          ///< the AmbDevice for talking to the FEMC module
    
    AmbRelativeAddr RCA_m;          ///< AMB relative adress for CAN request
    AmbDataLength_t dataLength_m;   ///< data length for CAN request & response
    AmbDataMem_t data_m[8];         ///< data for can request & response
    sem_t synchLock_m;              ///< semaphore for waiting on monitor response
    Time timestamp_m;               ///< timestamp for CAN request
    AmbErrorCode_t status_m;        ///< result of CAN request

    bool expectStatusByte_m;        ///< True for the front end subassemblies,
                                    ///< False for FE SPECIAL RCAs, AMBSII1, and other CAN nodes.

    unsigned short unpackU16(unsigned char *statusByte = NULL) const;
    ///< Unpack the contents of data_m into a 2-byte unsigned integer.
    ///< Also return the FE status byte if a pointer is provided.

    unsigned long unpackU32(unsigned char *statusByte = NULL) const;
    ///< Unpack the contents of data_m into a 4-byte unsigned integer.
    ///< Also return the FE status byte if a pointer is provided.

    float unpackSGL(unsigned char *statusByte = NULL) const;
    ///< Unpack the contents of data_m into a 4-byte floating point.
    ///< Also return the FE status byte if a pointer is provided.
    
    void packU16(unsigned short value, bool append = false);
    ///< Pack the given unsigned integer into data_m
    ///< If 'append' place it starting at dataLength_m and add 2 to dataLength_m
    ///< Else set dataLength_m = 2

    void packU32(unsigned long value, bool append = false);
    ///< Pack the given unsigned integer into data_m
    ///< If 'append' place it starting at dataLength_m and add 4 to dataLength_m
    ///< Else set dataLength_m = 4

    void packSGL(float value, bool append = false);
    ///< Pack the given floating point value into data_m
    ///< If 'append' place it starting at dataLength_m and add 4 to dataLength_m
    ///< Else set dataLength_m = 4

    void resetAmbVars();
    ///< Zero out all member variables.

    const std::string &getDataStr();
    ///< Get hexadecimal string representation of the contents of data_m.

    void monitor(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *monDetails = NULL);
    ///< Sends a monitor request and waits for the response.
    ///< Upon return, dataLength_m and data_m contain the resulting monitor data.
    ///< params:
    ///<    RCA: the monitor RCA to request
    ///<    callerDescription: Provide a string describing the context of the call.
    ///<    monDetails: optional pointer to string.
    ///<        This function appends callerDescription to monDetails, plus CAN-level details of the call.
    ///< Will raise an error if this object is not set up correctly or if there is a communication error.

    unsigned char monitorU8(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *monDetails = NULL, unsigned char *statusByte = NULL);
    ///< Specialized version of monitor() which returns a single byte.
    ///< Takes care of unpacking the data and checking the data length.
    ///< If statusByte is provided, the FEMC status byte will be placed there.

    unsigned short monitorU16(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *monDetails = NULL, unsigned char *statusByte = NULL);
    ///< Specialized version of monitor() which returns an unsigned short.
    ///< Takes care of unpacking the data and checking the data length.
    ///< If statusByte is provided, the FEMC status byte will be placed there.

    unsigned long monitorU32(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *monDetails = NULL, unsigned char *statusByte = NULL);
    ///< Specialized version of monitor() which returns an unsigned long.
    ///< Takes care of unpacking the data and checking the data length.
    ///< If statusByte is provided, the FEMC status byte will be placed there.

    float monitorSGL(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *monDetails = NULL, unsigned char *statusByte = NULL);
    ///< Specialized version of monitor() which returns an unsigned long.
    ///< Takes care of unpacking the data and checking the data length.
    ///< If statusByte is provided, the FEMC status byte will be placed there.

    void monitorImpl(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *monDetails = NULL);
    ///< Same as monitor() above but does no CPPUNIT error checking and therefore always returns, even if failed.

    void command(const AmbRelativeAddr &RCA,
                 const std::string &callerDescription,
                 std::string *cmdDetails = NULL,
                 bool checkSuccess = true,
                 int monOnCmdDelay_ms = 0);
    ///< Sends a command and then monitors on the command RCA.
    ///< Upon return, dataLength_m and data_m contain the results of the monitor-on-command RCA.
    ///< params:
    ///<    RCA: the monitor RCA to request
    ///<    dataLength_m and data_m:  Set these with the command data prior to calling.
    ///<    callerDescription: Provide a string describing the context of the call.
    ///<    cmdDetails: optional pointer to string.
    ///<        This function appends callerDescription to cmdDetails, plus CAN-level details of the call.
    ///<    checkSuccess: Pass true (the default) for commands which you expect to succeed,
    ///<        false for commands you expect to fail so that you may inspect the results in your calling test case.
    ///<    monOnCmdDelay_ms: wait this many milliseconds after the command before monitoring on the command RCA.
    ///< Will raise an error if this object is not set up correctly or if there is a communication error.
    ///< Will raise an error if checkSuccess is true and the monitor-on-command status byte is not FEMC_NO_ERROR.

    void commandU8(const AmbRelativeAddr &RCA, unsigned char value, const std::string &callerDescription, std::string *cmdDetails = NULL, bool checkSuccess = true);
    ///< Specialized version of command() which sends a single byte.
    ///< Takes care of packing the data and dataLength.

    void commandU16(const AmbRelativeAddr &RCA, unsigned short value, const std::string &callerDescription, std::string *cmdDetails = NULL, bool checkSuccess = true);
    ///< Specialized version of command() which sends an unsigned short.
    ///< Takes care of packing the data and dataLength.

    void commandU32(const AmbRelativeAddr &RCA, unsigned long value, const std::string &callerDescription, std::string *cmdDetails = NULL, bool checkSuccess = true);
    ///< Specialized version of command() which sends an unsigned long.
    ///< Takes care of packing the data and dataLength.

    void commandSGL(const AmbRelativeAddr &RCA, float value, const std::string &callerDescription, std::string *cmdDetails = NULL, bool checkSuccess = true);
    ///< Specialized version of command() which sends an unsigned long.
    ///< Takes care of packing the data and dataLength.

    void commandImpl(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *cmdDetails = NULL);
    ///< Same as command() above but does not monitor the command RCA and does no CPPUNIT error checking and therefore always returns, even if failed.

    enum FEMC_ERROR {
        FEMC_NO_ERROR           = 0,    //!< No error during the monitor operation.
        FEMC_GENERIC_ERROR      = 0xFF, //!< Error during the monitor operation.  FEMC failed to communicate with hardware.
        FEMC_HARDW_RANGE_ERR    = 0xFE, //!< The addressed hardware is not installed.
        FEMC_HARDW_BLOCKED_ERR  = 0xFD, //!< The hardware cannot be accessed because of operational state.
        FEMC_HARDW_CONVERT_ERR  = 0xFB, //!< Monitor data is out of range for conversion to proper units.
        FEMC_HARDW_ERROR        = 0xF0, //!< The selected hardware is in error state. Follow ICD instruction to proceed.
        FEMC_CON_ERROR_RNG      = 0xF6, //!< Value of last control message received is outside the allowed range
        FEMC_AMB_ERROR          = -44,  //!< The AMB subsystem reported an error.
        FEMC_UNPACK_ERROR       = -45,  //!< Unpacking monitor data failed
        FEMC_NOT_CONNECTED      = -46   //!< Cannot monitor because not connected to FE.
    };

    static bool statusByteEqual(AmbDataMem_t &data, FEMC_ERROR code)
      { signed char i(code);
        return data == *((unsigned char *) &i); }
    ///< helper to test data bytes for equality with FEMC_ERROR codes

private:
    std::string name_m;                 ///< The name of this test fixture, for logging.
    AmbChannel channel_m;               ///< The CAN channel to use.
    AmbNodeAddr nodeAddress_m;          ///< The CAN node address to talk to.
};    
    
#endif  // AMBDEVICETESTFIXTURE_H

