#include "AmbDeviceTestFixture.h"
#include <cppunit/TestAssert.h>
#include <logger.h>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;

void AmbDeviceTestFixture::setUp() {
    device_p = new FEAmbDevice(name_m, channel_m, nodeAddress_m);
    sem_init(&synchLock_m, 0, 0);
    resetAmbVars();
}

void AmbDeviceTestFixture::tearDown() {
    delete device_p;
    device_p = NULL;
    sem_destroy(&synchLock_m);
}

//TODO:  move unpack routines into FrontEndAMBLib:

unsigned short AmbDeviceTestFixture::unpackU16(unsigned char *statusByte) const {
    // get the status byte from after the data payload:
    char ret(FEMC_NO_ERROR);
    if (dataLength_m == 1 || dataLength_m == 3)
        ret = (data_m[dataLength_m - 1]);
    // unsigned short is big-endian:
    unsigned short result = ((data_m[0] & 0xFF) << 8) + data_m[1];
    if (statusByte)
        *statusByte = ret;
    return result;
}

unsigned long AmbDeviceTestFixture::unpackU32(unsigned char *statusByte) const {
    // get the status byte from after the data payload:
    char ret(FEMC_NO_ERROR);
    if (dataLength_m == 1 || dataLength_m == 5)
        ret = (data_m[dataLength_m - 1]);
    // unsigned long is big-endian:
    unsigned result = 0;
    for (unsigned index = 0; index < 4; ++index) {
        result *= 0xFF;
        result += data_m[index];
    }
    if (statusByte)
        *statusByte = ret;
    return result;
}

//TODO:  move unpack routines into FrontEndAMBLib:

typedef union { // A union for float/can conversion
    unsigned char chr_val[4];
    float flt_val;
} AmbFloatConv;

typedef union { // A union for unsigned long/can conversion
    unsigned char chr_val[4];
    unsigned long long_val;
} AmbULConv;

float AmbDeviceTestFixture::unpackSGL(unsigned char *statusByte) const {
    char ret(FEMC_NO_ERROR);
    float target(0.0);
    AmbDataLength_t dataLength(dataLength_m);
    
    if (dataLength == 1 || dataLength == 5)
        ret = (data_m[--dataLength]);
    if (dataLength < 4)
        ret = FEMC_UNPACK_ERROR;
    else {
        // Convert to float the incoming CAN message:
        AmbFloatConv conv;
        conv.chr_val[0] = data_m[3];
        conv.chr_val[1] = data_m[2];
        conv.chr_val[2] = data_m[1];
        conv.chr_val[3] = data_m[0];
        target = conv.flt_val;
    }
    if (statusByte)
        *statusByte = ret;
    return target;    
}

void AmbDeviceTestFixture::packU16(unsigned short value) {
    // unsigned short is big-endian:
    dataLength_m = 2;
    data_m[0] = (value >> 8) & 0xff;  //shift and set upper byte
    data_m[1] = value & 0xff;         //set lower byte
}


void AmbDeviceTestFixture::packU32(unsigned long value) {
    // unsigned long is big-endian:
    AmbULConv conv;
    conv.long_val = value;
    dataLength_m = 4;
    data_m[0] = conv.chr_val[3];
    data_m[1] = conv.chr_val[2];
    data_m[2] = conv.chr_val[1];
    data_m[3] = conv.chr_val[0];
}

void AmbDeviceTestFixture::packSGL(float value) {
    // float is big-endian:
    AmbFloatConv conv;
    conv.flt_val = value;
    dataLength_m = 4;
    data_m[0] = conv.chr_val[3];
    data_m[1] = conv.chr_val[2];
    data_m[2] = conv.chr_val[1];
    data_m[3] = conv.chr_val[0];
}

void AmbDeviceTestFixture::resetAmbVars() {
    RCA_m = 0;
    dataLength_m = 0;
    memset(data_m, 0, 8);
    timestamp_m = 0;
    status_m = AMBERR_NOERR;    
}

const std::string &AmbDeviceTestFixture::getDataStr() {
    static string dataStr;
    std::stringstream streamOut;
    streamOut << "[" << uppercase << hex << setfill('0') 
              << setw(2) << (unsigned) data_m[0] << " "
              << setw(2) << (unsigned) data_m[1] << " "
              << setw(2) << (unsigned) data_m[2] << " "
              << setw(2) << (unsigned) data_m[3] << " "
              << setw(2) << (unsigned) data_m[4] << " "
              << setw(2) << (unsigned) data_m[5] << " "
              << setw(2) << (unsigned) data_m[6] << " "
              << setw(2) << (unsigned) data_m[7] << "]";
    dataStr = streamOut.str();
    return dataStr;
}

void AmbDeviceTestFixture::monitor(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *monDetails) {
    CPPUNIT_ASSERT_MESSAGE(callerDescription + ": FEDevice pointer is NULL", device_p);
    monitorImpl(RCA, callerDescription, monDetails);
    CPPUNIT_ASSERT_MESSAGE((monDetails) ? (*monDetails) : (callerDescription), status_m == AMBERR_NOERR);
}

unsigned char AmbDeviceTestFixture::monitorU8(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *monDetails, unsigned char *statusByte) {
    monitor(RCA, callerDescription, monDetails);
    CPPUNIT_ASSERT_MESSAGE((monDetails) ? (*monDetails) : (callerDescription), dataLength_m == (expectStatusByte_m ? 2 : 1));
    if (statusByte)
        *statusByte = data_m[1];
    return data_m[0];
}

unsigned short AmbDeviceTestFixture::monitorU16(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *monDetails, unsigned char *statusByte) {
    monitor(RCA, callerDescription, monDetails);
    CPPUNIT_ASSERT_MESSAGE((monDetails) ? (*monDetails) : (callerDescription), dataLength_m == (expectStatusByte_m ? 3 : 2));
    return unpackU16(statusByte);
}

unsigned long AmbDeviceTestFixture::monitorU32(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *monDetails, unsigned char *statusByte) {
    monitor(RCA, callerDescription, monDetails);
    CPPUNIT_ASSERT_MESSAGE((monDetails) ? (*monDetails) : (callerDescription), dataLength_m == (expectStatusByte_m ? 5 : 4));
    return unpackU32(statusByte);
}

float AmbDeviceTestFixture::monitorSGL(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *monDetails, unsigned char *statusByte) {
    monitor(RCA, callerDescription, monDetails);
    CPPUNIT_ASSERT_MESSAGE((monDetails) ? (*monDetails) : (callerDescription), dataLength_m == (expectStatusByte_m ? 5 : 4));
    return unpackSGL(statusByte);
}

void AmbDeviceTestFixture::monitorImpl(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *monDetails) {
    resetAmbVars();
    RCA_m = RCA;
    device_p -> monitor(RCA_m, dataLength_m, data_m, &synchLock_m, &timestamp_m, &status_m);
    sem_wait(&synchLock_m);
    if (monDetails) {
        std::stringstream streamOut;
        streamOut << callerDescription << ": RCA=" << uppercase << hex << setfill('0') << setw(5) << RCA_m
                  << dec << " status=" << status_m << " TS=" << timestamp_m
                  << " dataLength=" << dataLength_m << " data=" << getDataStr();
        *monDetails = streamOut.str();
    }
}

void AmbDeviceTestFixture::command(const AmbRelativeAddr &RCA,
                                   const std::string &callerDescription,
                                   std::string *cmdDetails,
                                   bool checkSuccess,
                                   int monOnCmdDelay_ms)
{
    CPPUNIT_ASSERT_MESSAGE(callerDescription + ": FEDevice pointer is NULL", device_p);
    // send the command:
    commandImpl(RCA, callerDescription, cmdDetails);
    CPPUNIT_ASSERT_MESSAGE((cmdDetails) ? (*cmdDetails) : (callerDescription), status_m == AMBERR_NOERR);

    // From the ICD:  "Monitor requests on Control RCAs
    //   For every Control RCA, a monitor request may be sent which will return the last value set
    //   to the control point, plus a status byte indicating the outcome of the setting."

    // we expect the same amount of data as we sent plus a status byte if expectStatusByte_m is true:
    AmbDataLength_t expectDataLength(dataLength_m + (expectStatusByte_m ? 1 : 0));

    // copy the payload we sent so we can compare what comes back:
    AmbDataMem_t data0[8];
    for (int i = 0; i < 8; i++)
        data0[i] = (i < dataLength_m) ? data_m[i] : 0;

    // wait a bit
    SLEEP(monOnCmdDelay_ms);

    // Now monitor on the command RCA:
    resetAmbVars();
    string monDetails;
    string callerDesc2(callerDescription + " MON_ON_CMD");
    monitor(RCA, callerDesc2, &monDetails);
    {
        std::stringstream streamOut;
        streamOut << callerDesc2 << ": RCA=" << uppercase << hex << setfill('0') << setw(5) << RCA_m
                  << dec << " status=" << status_m << " TS=" << timestamp_m
                  << " dataLength=" << dataLength_m << " expectDataLength=" << expectDataLength
                  << " data=" << getDataStr();
        monDetails = streamOut.str();

        // save the monitor on command details to the caller's string:
        if (cmdDetails)
            *cmdDetails = monDetails;
    }
    // check for errors unless suppressed by caller:
    if (checkSuccess) {
        // catch data length error:
        CPPUNIT_ASSERT_MESSAGE(monDetails, dataLength_m == expectDataLength);
        // catch status byte error:
        if (expectStatusByte_m)
            CPPUNIT_ASSERT_MESSAGE(monDetails, data_m[dataLength_m - 1] == FEMC_NO_ERROR);
        // catch command not matching sent payload:
        for (int i = 0; i < (dataLength_m - 1); i++) {
            // note: not comparing the status byte, only the original payload bytes
            CPPUNIT_ASSERT_MESSAGE(monDetails, data_m[i] == data0[i]);
        }
    }
}

void AmbDeviceTestFixture::commandU8(const AmbRelativeAddr &RCA, unsigned char value, const std::string &callerDescription, std::string *cmdDetails, bool checkSuccess) {
    resetAmbVars();
    data_m[0] = value;
    dataLength_m = 1;
    command(RCA, callerDescription, cmdDetails, checkSuccess);
}

void AmbDeviceTestFixture::commandU16(const AmbRelativeAddr &RCA, unsigned short value, const std::string &callerDescription, std::string *cmdDetails, bool checkSuccess) {
    resetAmbVars();
    packU16(value);
    command(RCA, callerDescription, cmdDetails, checkSuccess);
}

void AmbDeviceTestFixture::commandU32(const AmbRelativeAddr &RCA, unsigned long value, const std::string &callerDescription, std::string *cmdDetails, bool checkSuccess) {
    resetAmbVars();
    packU32(value);
    command(RCA, callerDescription, cmdDetails, checkSuccess);
}

void AmbDeviceTestFixture::commandSGL(const AmbRelativeAddr &RCA, float value, const std::string &callerDescription, std::string *cmdDetails, bool checkSuccess) {
    resetAmbVars();
    packSGL(value);
    command(RCA, callerDescription, cmdDetails, checkSuccess);
}


void AmbDeviceTestFixture::commandImpl(const AmbRelativeAddr &RCA, const std::string &callerDescription, std::string *cmdDetails) {
    if (!device_p) {
        LOG(LM_ERROR) << "AmbDeviceTestFixture::commandImpl device_p is NULL!" << endl;
        return;
    }

    RCA_m = RCA;
    device_p -> command(RCA_m, dataLength_m, data_m, &synchLock_m, &timestamp_m, &status_m);
    sem_wait(&synchLock_m);
    if (cmdDetails) {
        std::stringstream streamOut;
        streamOut << callerDescription << ": RCA=" << uppercase << hex << setfill('0') << setw(5) << RCA_m
                  << dec << " status=" << status_m << " TS=" << timestamp_m
                  << " dataLength=" << dataLength_m << " data=" << getDataStr();
        *cmdDetails = streamOut.str();
    }
}


