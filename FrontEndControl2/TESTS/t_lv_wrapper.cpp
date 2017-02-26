#include "../DLL/lv_wrapper.h"
#include "../DLL/lv_wrapper_FE.h"
#include "../DLL/lv_wrapper_sigSrc.h"
#include "../DLL/lv_structs.h"
#include "portable.h"
#include <stdio.h>
#include <windows.h>
#include <math.h>

static const float MY_INF = INFINITY;
// the above line will cause a divide by zero warning.

int main(int, char*[]) {
    char DLLVersion[20];
    if (getSoftwareVersion(DLLVersion) < 0) {
        printf (">>> Could not get FrontEndControl.DLL version string. Exiting.\n");
        return -1;
    }
    printf(">>> FrontEndControl.DLL version %s\n", DLLVersion);

    if (FEControlInit() < 0) {
        printf(">>> Not connected to Front End. Exiting.\n");
        return -1;
    }
    printf(">>> Connected to Front End.\n");

    float temp;
    if (FEMCGetAMBSITemperature(&temp) >= 0)
        printf(">>> AMBSITemperature=%f\n", temp);
    else
        printf(">>> FEMCGetAMBSITemperature failed.\n");

    char serialNum[20];
    char firmwareRev[20];
    unsigned long numErrors, numTrans;
    if (FEMCGetAMBSIInfo(serialNum, firmwareRev, &numErrors, &numTrans) >= 0)
        printf(">>> serialNum=%s firmwareRev=%s numErrors=%lu numTrans=%lu\n",
               serialNum, firmwareRev, numErrors, numTrans);
    else
        printf(">>> FEMCGetAMBSIInfo failed.\n");

    char AMBSILibraryVersion[20];
    char FEMCFirmwareVersion[20];
    char FEMCFPGAVersion[20];
    if (FEMCGetFirmwareInfo(AMBSILibraryVersion, FEMCFirmwareVersion, FEMCFPGAVersion) >= 0)
        printf(">>> AMBSILibraryVersion=%s\n"
               ">>> FEMCFirmwareVersion=%s\n"
               ">>> FEMCFPGAVersion=%s\n",
               AMBSILibraryVersion, FEMCFirmwareVersion, FEMCFPGAVersion);
    else
        printf(">>> FEMCGetFirmwareInfo failed.\n");

    short moduleNum, errorNum;
    char description[200];

    while (FEMCGetNextError(&moduleNum, &errorNum, description) == 0) {
        SLEEP(10);
    }

    int sigSrcStatus = sigSrcControlInit();
    if (sigSrcStatus < 0)
        printf (">>> Not connected to SignalSource.\n");
    else {
        short reset = 1;
        short configId = 0;
        while (sigSrcGetNextConfiguration(reset, &configId, description) >= 0) {
            reset = false;
            printf(">>> sigSrc Config %d: %s\n", configId, description);
        }
    }

    bool done = false;
    short configId = 0;
    while (!done) {
        fflush(stdout);
        if (MessageBox(NULL, "OK to proceed with test.  Cancel to exit", "t_lv_wrapper", MB_OKCANCEL) != IDOK)
            done = true;
        else {
            configId++;
            if (sigSrcLoadConfiguration(configId) < 0)
                printf(">>> sigSrcLoadConfiguration %d error.\n", configId);
            configId++;
            if (sigSrcLoadConfiguration(configId) < 0)
                printf(">>> sigSrcLoadConfiguration %d error.\n", configId);
//            SLEEP(5000);
        }
    }
    if (sigSrcStatus >= 0)
        sigSrcControlShutdown();
    FEControlShutdown();
    return 0;   
}
