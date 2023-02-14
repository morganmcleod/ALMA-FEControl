# FrontEndAMBDLL
## Implements NI-CAN methods to be used by the Python package https://gitlab.nrao.edu/mmcleod/almafe-ambdevicelibrary
Morgan McLeod <mmcleod@nrao.edu>, February 2023

### Background
While implementing [ALMAFE-AMBDeviceLibrary](https://gitlab.nrao.edu/mmcleod/almafe-ambdevicelibrary) I noticed that the performance was poor when measuring SIS I-V curves.  Typically this would consist of about 800 repetitions of CAN messages with the sequence:  set Vj, read Vj, read Ij.  Since I already had [a C++ DLL](https://github.com/morganmcleod/ALMA-FEControl/tree/master/FrontEndControl2) which implements FrontEndControl for the FETMS, I took parts of that to make this DLL.

Unlike FrontEndControl, this DLL is mostly just a simple pass-through of CAN messages.  However it adds a runSequence() method which dispatches a prepared list of CAN commands and requests and then returns the results back to the caller when finished.

### Installation
Place the following files in a folder anywhere on your Windows system:
- FrontEndAMB.dll
- exchndl.dll
- libstdc++-6.dll
- mgwhelp.dll
- FrontEndAmbDLL.ini

You can customize FrontEndAmbDLL.INI if you wish:
```ini
[connection]
channel=0                   ; corresponds to CAN0
nodeAddress=0x13            ; this value is standard for the FEMC module

[logger]
logDir=C:\Temp              ; where a CAN transaction log will be written

[debug]
CAN_debugStdout=0           ; advanced debugging options
CAN_noTransmit=0            ;
CAN_monitorTimeout=4        ; milliseconds.  Set to longer if you are getting timeout errors
```

Add an environment variable (system or user) with the full path to FrontEndAmbDLL.ini:
```
set FRONTENDAMBDLL.INI=C:\example\FrontEndAmbDLL.ini
```

You can either add the install folder to your PATH (system or user) or you can pass the full path to the constructor for AMBConnectionDLL. You will need to fix up some of unit tests in ALMAFE-AMBDeviceLibrary to get them to work with your installation.
