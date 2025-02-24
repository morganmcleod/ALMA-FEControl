#include "portable.h"
#include "../DLL/lv_wrapper_sigSrc.h"
#include "../DLL/lv_wrapper_FE.h"
#include "../DLL/lv_structs.h"
#include <stdio.h>
#include <math.h>

int main(int, char*[]) {

    if (sigSrcControlInit() < 0)
        printf(">>> Not connected to Signal Source.\n");
    else {
        Sleep(1000);
        printf(">>> Connected to Signal Source.\n");

        bool done = false;
        bool reset = true;
        short id(0);
        char description[100];
        while (!done) {
            if (sigSrcGetNextConfiguration(reset, &id, description) < 0)
                done = true;
            else
                printf("Configuration(%d): %s\n", (int) id, description);
            reset = false;
        }

        sigSrcLoadConfiguration(1);
        cartSetLOFrequency(-1, 100.0, 0.0315, 0);

        float temp;
        if (sigSrcGetAMBSITemperature(&temp) >= 0)
            printf(">>> AMBSITemperature=%f\n", temp);
        else
            printf(">>> sigSrcGetAMBSITemperature failed.\n");
        
        char serialNum[20];
        char firmwareRev[20];
        unsigned long numErrors, numTrans;
        if (sigSrcGetAMBSIInfo(serialNum, firmwareRev, &numErrors, &numTrans) >= 0)
            printf(">>> serialNum=%s firmwareRev=%s numErrors=%lu numTrans=%lu\n", 
                   serialNum, firmwareRev, numErrors, numTrans);
        else
            printf(">>> sigSrcGetAMBSIInfo failed.\n");

        char AMBSILibraryVersion[20];
        char FEMCFirmwareVersion[20];
        char FEMCFPGAVersion[20];
        if (sigSrcGetFirmwareInfo(AMBSILibraryVersion, FEMCFirmwareVersion, FEMCFPGAVersion) >= 0)
            printf(">>> AMBSILibraryVersion=%s\n"
                   ">>> FEMCFirmwareVersion=%s\n"
                   ">>> FEMCFPGAVersion=%s\n", 
                   AMBSILibraryVersion, FEMCFirmwareVersion, FEMCFPGAVersion);
        else
            printf(">>> sigSrcGetFirmwareInfo failed.\n");

        if (FESetCartridgeOn(-1) < 0)
            printf(">>> sigSrcSetCartridgeOn failed.\n");

        Sleep(3000);

        CartYTOData_t ytoInfo;
        if (cartGetMonitorYTO(-1, &ytoInfo) < 0)
            printf(">>> sigSrcGetMonitorYTO failed.\n");

        CartPhotomixerData_t pmInfo;
        if (cartGetMonitorPhotomixer(-1, &pmInfo) < 0)
            printf(">>> sigSrcGetMonitorPhotomixer failed.\n");

        CartPLLData_t pllInfo;
        if (cartGetMonitorPLL(-1, &pllInfo) < 0)
            printf(">>> sigSrcGetMonitorPLL failed.\n");

        CartAMCData_t amcInfo;
        if (cartGetMonitorAMC(-1, &amcInfo) < 0)
            printf(">>> sigSrcGetMonitorAMC failed.\n");

        CartPAData_t paInfo;
        if (cartGetMonitorPA(-1, &paInfo) < 0)
            printf(">>> sigSrcGetMonitorPA failed.\n");
        
        if (FESetCartridgeOff(-1) < 0)
            printf(">>> sigSrcSetCartridgeOff failed.\n");
    }
    sigSrcControlShutdown();
    return 0;   
}
