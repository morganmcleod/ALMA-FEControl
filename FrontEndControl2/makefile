# ALMA - Atacama Large Millimiter Array
# (c) Associated Universities Inc., 2006
#
# This library is free software; you can redistribute it and/or modify it 
# under the terms of the GNU Library General Public License as published by 
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful but WITHOUT 
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public 
# License for more details.
#
# You should have received a copy of the GNU Library General Public License 
# along with this library; if not, write to the Free Software Foundation, 
# Inc., 675 Massachusetts Ave, Cambridge, MA, 02139, USA.
#
# Morgan McLeod <mmcleod@nrao.edu> 30 August 2006
#
# makefile for ALMA FrontEnd low-level control library for Windows.
#
# uses:
# MinGW from http://www.mingw.org/ provides a POSIX build environment including GCC 3.4.2.
# Pthreads-win32 from http://sourceware.org/pthreads-win32/
# NI-CAN 2.2 from National Instruments via nicanmsc.lib which is the API for Microsoft VCPP.
# MySQL C API version 4.1 or later from http://www.mysql.com/
#
export BUILD = L:/ALMA-FEControl

# includes and libs for "portable" code = POSIX threads:
PTHREADINC = -I$(BUILD)/3rdparty/include/pthreads_win32
PTHREADLIB = -L$(BUILD)/3rdparty/lib/pthreads_win32 -lpthreadGC2
STATICGCCLIB = -static-libgcc

export PORTABLEINC = $(PTHREADINC)
PORTABLELIB = $(PTHREADLIB) $(STATICGCCLIB)

# includes and libs for the CppUtilityLib library:
export UTILINC = -I$(BUILD)/CppUtilityLib/include
UTILLIB = -L$(BUILD)/CppUtilityLib/Debug -lCppUtilityLib

# includes and libs for the FEICDBLib library:
export DBINC = -I$(BUILD)/FEICDBLib/include
DBLIB = -L$(BUILD)/FEICDBLib/Debug -lFEICDBLib

# includes and libs for the FrontEndAMB library:
export AMBINC = -I$(BUILD)/FrontEndAmbLib/include
AMBLIB = -L$(BUILD)/FrontEndAmbLib/Debug -lFrontEndAmb

# includes and libs for MySQL library:
export MYSQLINC = -I$(BUILD)/3rdparty/include/MySQL41
MYSQLLIB = -L$(BUILD)/3rdparty/lib/MySQL41/debug -llibmysql

# includes and libs for the NI CAN interface library: 
export NICANINC = -I$(BUILD)/3rdparty/include/Nican
NICANLIB = $(BUILD)/3rdparty/lib/Nican/nicanmsc.lib

# project includes are CppUtilityLib, FEICDBLib, FrontEndAMBLib, and this directory:
export PROJECTINC = $(UTILINC) $(DBINC) $(AMBINC) -I$(CURDIR)

# windows libs:
WINLIBS = -lwinmm -lwsock32

# flags for compiling C++ code:
export CPPFLAGS = -g 
#-std=c++0x 
#-Wall ALL warnings.

# list of output files in subdirectories:
CONFIGOUT = CONFIG/CartConfig.o CONFIG/ConfigManager.o CONFIG/Configuration.o CONFIG/ConfigProviderIniFile.o CONFIG/ConfigProviderMySQL.o \
	CONFIG/FrontEndConfig.o CONFIG/FrontEndDatabase.o CONFIG/LookupTables.o CONFIG/SubsysConfig.o
FEBASEOUT = FEBASE/AMBSIHardwareDevice.o \
	FEBASE/ColdCartImplBase.o FEBASE/CompressorImplBase.o FEBASE/CryostatImplBase.o \
	FEBASE/FETIMImplBase.o FEBASE/FEHardwareDevice.o FEBASE/FrontEndImplBase.o \
	FEBASE/IFSwitchImplBase.o FEBASE/LORTMImplBase.o FEBASE/LPRImplBase.o \
	FEBASE/PowerModuleImplBase.o FEBASE/WCAImplBase.o
LOGGEROUT = LOGGER/ambTransactionLogger.o LOGGER/feAddressMeta.o LOGGER/logDir.o
OPTIMIZEOUT = OPTIMIZE/CartHealthCheck.o OPTIMIZE/CryostatPumping.o \
    OPTIMIZE/Maximizer.o OPTIMIZE/MaximizeIFPower.o OPTIMIZE/MeasureFineLOSweep.o \
	OPTIMIZE/MeasureIVCurve.o OPTIMIZE/MeasureSISCurrent.o OPTIMIZE/MeasureIFPower.o OPTIMIZE/MixerDeflux.o OPTIMIZE/MixerHeating.o \
    OPTIMIZE/OptimizeBase.o OPTIMIZE/OptimizeLOPowerAmp.o OPTIMIZE/ThermalLogger.o

# rules to build all library files in subdirectories:
$(CONFIGOUT):
	$(MAKE) -C CONFIG
$(FEBASEOUT):
	$(MAKE) -C FEBASE
$(LOGGEROUT):
	$(MAKE) -C LOGGER
$(OPTIMIZEOUT):
	$(MAKE) -C OPTIMIZE
	
# rule to build the top-level Front End files:
FRONTENDCPP = CartAssembly.cpp CartridgesContainer.cpp ColdCartImpl.cpp CompressorImpl.cpp CryostatImpl.cpp \
	FEMCErrors.cpp FEMCEventQueue.cpp FETIMImpl.cpp FrontEndImpl.cpp IFSwitchImpl.cpp LORTMImpl.cpp LPRImpl.cpp \
	PowerModuleImpl.cpp PowerModulesContainer.cpp SignalSourceImpl.cpp DLL/SWVersion.cpp WCAImpl.cpp
FRONTENDOUT = CartAssembly.o CartridgesContainer.o ColdCartImpl.o CompressorImpl.o CryostatImpl.o \
	FEMCErrors.o FEMCEventQueue.o FETIMImpl.o FrontEndImpl.o IFSwitchImpl.o LORTMImpl.o LPRImpl.o \
	PowerModuleImpl.o PowerModulesContainer.o SignalSourceImpl.o SWVersion.o WCAImpl.o
	
.PHONY: frontend
frontend: $(FRONTENDOUT)
$(FRONTENDOUT): $(FRONTENDCPP)
	g++ $(CPPFLAGS) \
	 $(PROJECTINC) $(PORTABLEINC) $(MYSQLINC) \
	-c $(FRONTENDCPP)

# List of all output files:
ALLOUT = $(CONFIGOUT) $(FEBASEOUT) $(FRONTENDOUT) $(LOGGEROUT) $(OPTIMIZEOUT)

.PHONY: allout
allout: $(ALLOUT)

# Rule to make the DLL:
.PHONY: dll
dll: FrontEndControl.dll DLL/libFrontEndControl.a DLL/FrontEndControl.def

DLLCPP = DLL/DllMain.c DLL/lv_structs.cpp DLL/lv_wrapper.cpp DLL/lv_wrapper_Compressor.cpp DLL/lv_wrapper_FE.cpp DLL/lv_wrapper_LORTM.cpp DLL/lv_wrapper_sigSrc.cpp

FrontEndControl.dll DLL/FrontEndControl.def: $(DLLCPP) $(ALLOUT)
	g++ $(CPPFLAGS) -DBUILD_FRONTENDCONTROL -shared -o FrontEndControl.dll \
	$(DLLCPP) $(ALLOUT) \
	$(PROJECTINC) $(PORTABLEINC) $(MYSQLINC) \
	$(DBLIB) $(AMBLIB) $(UTILLIB) $(MYSQLLIB) $(WINLIBS) $(NICANLIB) $(PORTABLELIB) \
	-Wl,--enable-auto-import,--output-def,DLL/FrontEndControl.def
	- cp FrontEndControl.dll "../../labview2013/drivers/FE Control/"

# Build the import library for linking to the DLL:
DLL/libFrontEndControl.a: FrontEndControl.dll DLL/FrontEndControl.def
	dlltool --dllname FrontEndControl.dll --def DLL/FrontEndControl.def --output-lib DLL/libFrontEndControl.a -k

.PHONY: tests
tests: t_lv_wrapper.exe t_lv_wrapper_sigSrc.exe t_SocketClient.exe \
	t_LookupTables.exe t_semaphore_leaks.exe t_StreamLogger.exe t_FEICDataBase.exe

# This test uses the DLL:
t_lv_wrapper.exe : tests/t_lv_wrapper.cpp DLL/libFrontEndControl.a 
	g++ $(CPPFLAGS) -o t_lv_wrapper.exe \
	tests/t_lv_wrapper.cpp DLL/libFrontEndControl.a \
	$(PORTABLEINC) $(PROJECTINC)

# This test uses the DLL:
t_lv_wrapper_sigSrc.exe : tests/t_lv_wrapper_sigSrc.cpp DLL/libFrontEndControl.a 
	g++ $(CPPFLAGS) -o t_lv_wrapper_sigSrc.exe \
	tests/t_lv_wrapper_sigSrc.cpp DLL/libFrontEndControl.a \
	$(PORTABLEINC) $(PROJECTINC)

t_SocketClient.exe : tests/t_SocketClient.cpp
	g++ $(CPPFLAGS) -o t_SocketClient.exe \
	tests/t_SocketClient.cpp DLL/libFrontEndControl.a \
	$(PORTABLEINC) $(PROJECTINC)
	
t_LookupTables.exe : tests/t_LookupTables.cpp
	g++ $(CPPFLAGS) -o t_LookupTables.exe \
	tests/t_LookupTables.cpp CONFIG/LookupTables.o \
	$(PORTABLEINC) \
	$(PORTABLELIB)

t_semaphore_leaks.exe : tests/t_semaphore_leaks.cpp
	g++ $(CPPFLAGS) -o t_semaphore_leaks.exe \
	tests/t_semaphore_leaks.cpp \
	$(PORTABLEINC) \
	$(PORTABLELIB)

t_StreamLogger.exe : tests/t_StreamLogger.cpp
	g++ $(CPPFLAGS) -o t_StreamLogger.exe \
	tests/t_StreamLogger.cpp $(AMBOUT) $(BASICOUT) \
	$(UTILINC) $(PORTABLEINC) $(PROJECTINC) \
	$(UTILLIB) $(PORTABLELIB) $(AMBLIB)

t_FEICDataBase.exe : tests/t_FEICDataBase.cpp CONFIG/FrontEndDatabase.cpp
	g++ $(CPPFLAGS) -o t_FEICDataBase.exe \
	tests/t_FEICDataBase.cpp CONFIG/FrontEndDatabase.cpp CONFIG/LookupTables.cpp DLL/SWVersion.cpp \
	$(DBINC) $(UTILINC) $(MYSQLINC) $(PORTABLEINC) $(PROJECTINC) \
	$(DBLIB) $(UTILLIB) $(MYSQLLIB) $(PORTABLELIB)

.PHONY: all
all: allout dll tests

current:
	$(MAKE) -C OPTIMIZE
	
oneFile:
	$(MAKE) FEMCEventQueue.o
	
clean:
	@for dir in CONFIG FEBASE LOGGER OPTIMIZE; do \
	           $(MAKE) -C $$dir clean; \
             done
	rm -rf *.a
	rm -rf *.o
	rm -rf *.exe
	rm -rf *.dll
	rm -rf DLL/*.a
	rm -rf DLL/FrontEndControl.def

