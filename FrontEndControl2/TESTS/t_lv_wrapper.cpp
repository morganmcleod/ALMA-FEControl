#include "../DLL/lv_wrapper_FE.h"
#include "../DLL/lv_structs.h"
#include "portable.h"
#include <stdio.h>
#include <windows.h>
#include <math.h>

static const float MY_INF = INFINITY;
// the above line will cause a divide by zero warning.

int main(int, char*[]) {

    if (FEControlInit() < 0)
        printf(">>> Not connected to Front End.\n");
    else {
        Sleep(1000);
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

        short band1 = 1;
        float freq1 = 662;

        if (FESetCartridgeOn(band1) < 0)
            printf(">>> FESetCartridgeOn failed.\n");

        if (FESetCartridgeOff(band1) < 0)
            printf(">>> FESetCartridgeOn failed.\n");

        if (FESetCartridgeOn(band1) < 0)
            printf(">>> FESetCartridgeOn failed.\n");

        if (FESetCartridgeObserving(band1) < 0)
            printf(">>> FESetCartridgeObserving failed.\n");


        bool done = false;
        while (!done) {
            if (MessageBox(NULL, "OK to take monitor readings.  Cancel to exit", "t_lv_wrapper", MB_OKCANCEL) == IDOK) {

//                CryostatData_t cryoInfo;
//                if (cryostatGetMonitor(&cryoInfo) < 0)
//                    printf(">>> cartGetMonitorPhotomixer failed.\n");
                
                CartYTOData_t ytoInfo;
                if (cartGetMonitorYTO(band1, &ytoInfo) < 0)
                    printf(">>> cartGetMonitorPhotomixer failed.\n");
        
                CartPhotomixerData_t pmInfo;
                if (cartGetMonitorPhotomixer(band1, &pmInfo) < 0)
                    printf(">>> cartGetMonitorPhotomixer failed.\n");
        
                CartPLLData_t pllInfo;
                if (cartGetMonitorPLL(band1, &pllInfo) < 0)
                    printf(">>> cartGetMonitorPLL failed.\n");
        
                CartAMCData_t amcInfo;
                if (cartGetMonitorAMC(band1, &amcInfo) < 0)
                    printf(">>> cartGetMonitorAMC failed.\n");
        
                CartPAData_t paInfo;
                if (cartGetMonitorPA(band1, &paInfo) < 0)
                    printf(">>> cartGetMonitorPA failed.\n");
        
                CartTempData_t tempInfo;
                if (cartGetMonitorTemp(band1, &tempInfo) < 0)
                    printf(">>> cartGetMonitorTemp failed.\n");
        
                CartSISData_t sisInfo;
                if (cartGetMonitorSIS(band1, 0, 1, &sisInfo) < 0)
                    printf(">>> cartGetMonitorSIS failed.\n");
                if (cartGetMonitorSIS(band1, 0, 2, &sisInfo) < 0)
                    printf(">>> cartGetMonitorSIS failed.\n");
                if (cartGetMonitorSIS(band1, 1, 1, &sisInfo) < 0)
                    printf(">>> cartGetMonitorSIS failed.\n");
                if (cartGetMonitorSIS(band1, 1, 2, &sisInfo) < 0)
                    printf(">>> cartGetMonitorSIS failed.\n");
                
                CartLNAData_t lnaInfo;
                if (cartGetMonitorLNA(band1, 0, 1, &lnaInfo) < 0)
                    printf(">>> cartGetMonitorLNA failed.\n");
                if (cartGetMonitorLNA(band1, 0, 2, &lnaInfo) < 0)
                    printf(">>> cartGetMonitorLNA failed.\n");
                if (cartGetMonitorLNA(band1, 1, 1, &lnaInfo) < 0)
                    printf(">>> cartGetMonitorLNA failed.\n");
                if (cartGetMonitorLNA(band1, 1, 2, &lnaInfo) < 0)
                    printf(">>> cartGetMonitorLNA failed.\n");
        
                CartAuxData_t auxInfo;
                if (cartGetMonitorAux(band1, 0, &auxInfo) < 0)
                    printf(">>> cartGetMonitorAux failed.\n");
                if (cartGetMonitorAux(band1, 1, &auxInfo) < 0)
                    printf(">>> cartGetMonitorAux failed.\n");
                
//                PowerModuleData_t modInfo;
//                if (powerGetMonitorModule(band1, &modInfo) < 0)
//                    printf(">>> powerGetMonitorModule failed.\n");
                    
                if (MessageBox(NULL, "OK to set the LO frequency.  Cancel to skip", "t_lv_wrapper", MB_OKCANCEL) == IDOK) {
                    if (cartSetLOFrequency(band1, freq1, 31.5, 0) < 0)
                        printf(">>> setLOFrequency failed.\n");
                }
                if (MessageBox(NULL, "OK to bias the preamps.  Cancel to skip", "t_lv_wrapper", MB_OKCANCEL) == IDOK) {
                    if (cartSetLNABias(band1, -1, -1, MY_INF, MY_INF, MY_INF, MY_INF, MY_INF, MY_INF) < 0)
                        printf(">>> cartSetLNABias failed.\n");
                    if (cartSetEnableLNABias(band1, 1, -1, -1) < 0)
                        printf(">>> cartSetEnableLNABias failed.\n");
                }
                if (MessageBox(NULL, "OK to bias the junctions.  Cancel to skip", "t_lv_wrapper", MB_OKCANCEL) == IDOK) {
                    if (cartSetSISBias(band1, 1, -1, -1, MY_INF, -1) < 0)
                        printf(">>> cartSetSISBias failed.\n");
                }
                if (MessageBox(NULL, "OK to bias the magnets.  Cancel to skip", "t_lv_wrapper", MB_OKCANCEL) == IDOK) {
                    if (cartSetSISMagnet(band1, 1, -1, -1, MY_INF) < 0)
                        printf(">>> cartSetSISMagnet failed.\n");
                }
                if (MessageBox(NULL, "OK to enable the LO power amps.  Cancel to skip", "t_lv_wrapper", MB_OKCANCEL) == IDOK) {
                    if (cartSetLOPowerAmps(band1, 1, MY_INF, MY_INF, MY_INF, MY_INF) < 0)
                        printf(">>> cartSetLOPowerAmps failed.\n");
                }
            } else {
                done = true;
                if (cartSetLOPowerAmps(band1, 0, MY_INF, MY_INF, MY_INF, MY_INF) < 0)
                    printf(">>> cartSetLOPowerAmps(disable) failed.\n");
                if (cartSetSISMagnet(band1, 0, -1, -1, MY_INF) < 0)
                    printf(">>> cartSetSISMagnet(disable) failed.\n");
                if (cartSetSISBias(band1, 0, -1, -1, MY_INF, -1) < 0)
                    printf(">>> cartSetSISBias(disable) failed.\n");
                if (cartSetEnableLNABias(band1, 0, -1, -1) < 0)
                    printf(">>> cartSetEnableLNABias(disable) failed.\n");
            }
        }
    }
    FEControlShutdown();
    return 0;   
}
