# ALMA - Atacama Large Millimiter Array
# (c) Associated Universities Inc., 2006-2021
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
# Morgan McLeod <mmcleod@nrao.edu> 2021-10-15
#
# makefile for ALMA FrontEnd control library for Windows.
#
# uses:
#   MSYS2 from https://www.msys2.org/ with mingw32 for 32-bit build 
#     a POSIX build environment including GCC 10.3 or newer
#   NI-CAN 2.2 or newer from National Instruments via nicanmsc.lib which is the API for Microsoft VCPP.
#   MySQL C API version 4.1 or later from http://www.mysql.com/
#   ExcHndl from Dr.Mingw https://github.com/jrfonseca/drmingw embeddable exception handler 
#     for Windows post-mortem crash analysis 
#   ExpatMM XML Parser class for C++ based upon Expat https://github.com/ckane/expatmm
#     
BUILD = ..

EXCHNDLIB = -lexchndl
EXPATLIB = -lexpat
EXPATMMLIB = -lexpatmm

# includes and libs for the CppUtilityLib library:
UTILINC = -I$(BUILD)/CppUtilityLib/include
UTILLIB = -L$(BUILD)/CppUtilityLib/Debug -lCppUtilityLib

# includes and libs for the FEICDBLib library:
DBINC = -I$(BUILD)/FEICDBLib/include
DBLIB = -L$(BUILD)/FEICDBLib/Debug -lFEICDBLib

# includes and libs for the FrontEndAMB library:
AMBINC = -I$(BUILD)/FrontEndAMBLib/include
AMBLIB = -L$(BUILD)/FrontEndAMBLib/Debug -lFrontEndAMBLib

# includes and libs for MySQL library:
MYSQLINC = -I$(BUILD)/3rdparty/MySQL41/include
MYSQLLIB = -L$(BUILD)/3rdparty/MySQL41/lib/debug -Wl,-Bdynamic -llibmysql

# includes and libs for the NI CAN interface library:
NICANINC = -I$(BUILD)/3rdparty/Nican/include
NICANLIB = $(BUILD)/3rdparty/Nican/lib/nicanmsc.lib

# project includes plus this directory:
PROJECTINC = $(UTILINC) $(DBINC) $(AMBINC) -I././

# windows libs:
WINLIB = -lwinmm -lwsock32 -lWs2_32

# flags for compiling C++ code:
#  optimization: "Reduce compilation time and make debugging produce the expected results. This is the default."
#  don't wrap compiler error/warning messages
#  don't warn about else clauses without braces
#  don't warn about C++ member initializers out of order in constructor 
CPPFLAGS = -O0 -fmessage-length=0 -Wall -Wno-dangling-else -Wno-reorder
# flags for debug build:
#  debug level 2
DEBUGFLAGS = -g2

# list of output files in subdirectories:
DLLOUT = $(patsubst %.cpp,%.o,$(wildcard DLL/*.cpp)) DLL/resource.o
CONFIGOUT := $(patsubst %.cpp,%.o,$(wildcard CONFIG/*.cpp))
FEBASEOUT := $(patsubst %.cpp,%.o,$(wildcard FEBASE/*.cpp))
LOGGEROUT := $(patsubst %.cpp,%.o,$(wildcard LOGGER/*.cpp))
OPTIMIZEOUT := $(patsubst %.cpp,%.o,$(wildcard OPTIMIZE/*.cpp))

# rules to build library files in subdirectories:
$(DLLOUT):
	@echo "DLL"
	$(MAKE) -C DLL FLAGS="$(CPPFLAGS) $(DEBUGFLAGS)" INCLUDEDIR="$(PROJECTINC) $(MYSQLINC)"
$(CONFIGOUT):
	@echo "Include directories:"
	@echo "$(PROJECTINC) $(MYSQLINC)"
	$(MAKE) -C CONFIG FLAGS="$(CPPFLAGS) $(DEBUGFLAGS)" INCLUDEDIR=" $(PROJECTINC) $(MYSQLINC)"
$(FEBASEOUT):
	$(MAKE) -C FEBASE FLAGS="$(CPPFLAGS) $(DEBUGFLAGS)" INCLUDEDIR="$(PROJECTINC) $(MYSQLINC)"
$(LOGGEROUT):
	$(MAKE) -C LOGGER FLAGS="$(CPPFLAGS) $(DEBUGFLAGS)" INCLUDEDIR="$(PROJECTINC) $(MYSQLINC)"
$(OPTIMIZEOUT):
	$(MAKE) -C OPTIMIZE FLAGS="$(CPPFLAGS) $(DEBUGFLAGS)" INCLUDEDIR="$(PROJECTINC) $(MYSQLINC)"
	
# rules to build the top-level Front End files:
FRONTENDCPP = $(wildcard *.cpp)
FRONTENDOUT = $(patsubst %.cpp,%.o,$(wildcard $(FRONTENDCPP)))
	
.PHONY: frontend
frontend: $(FRONTENDOUT)
$(FRONTENDOUT): $(FRONTENDCPP)
	g++ $(CPPFLAGS) $(DEBUGFLAGS) \
	$(PROJECTINC) $(PORTABLEINC) $(MYSQLINC) \
	-c $(FRONTENDCPP)

# List of all output files:
ALLOUT = $(CONFIGOUT) $(FEBASEOUT) $(FRONTENDOUT) $(LOGGEROUT) $(OPTIMIZEOUT) $(DLLOUT)

.PHONY: allout
allout: $(ALLOUT)

.PHONY: dll
dll: FrontEndControl.dll DLL/libFrontEndControl.a DLL/FrontEndControl.def

FrontEndControl.dll DLL/FrontEndControl.def: $(DLLCPP) $(ALLOUT)
	g++ $(CPPFLAGS) $(DEBUGFLAGS) -DBUILD_FRONTENDCONTROL -shared -o FrontEndControl.dll \
	$(ALLOUT) \
	$(PROJECTINC) $(PORTABLEINC) $(MYSQLINC) \
	$(DBLIB) $(AMBLIB) $(UTILLIB) $(MYSQLLIB) $(WINLIB) $(NICANLIB) $(PORTABLELIB) $(EXCHNDLIB) $(EXPATLIB) $(EXPATMMLIB) \
	-Wl,--enable-auto-import,--output-def,DLL/FrontEndControl.def

# Build the import library for linking to the DLL:
DLL/libFrontEndControl.a: FrontEndControl.dll DLL/FrontEndControl.def
	dlltool --dllname FrontEndControl.dll --def DLL/FrontEndControl.def --output-lib DLL/libFrontEndControl.a -k

.PHONY: tests
tests: t_lv_wrapper.exe t_lv_wrapper_sigSrc.exe t_SocketClient.exe \
	t_LookupTables.exe t_semaphore_leaks.exe t_StreamLogger.exe t_FEICDataBase.exe

# This test uses the DLL:
t_lv_wrapper.exe : tests/t_lv_wrapper.cpp DLL/libFrontEndControl.a 
	g++ $(CPPFLAGS) $(DEBUGFLAGS) -o t_lv_wrapper.exe \
	tests/t_lv_wrapper.cpp DLL/libFrontEndControl.a \
	$(PROJECTINC) \
	$(EXCHNDLIB)
	
# This test uses the DLL:
t_lv_wrapper_sigSrc.exe : tests/t_lv_wrapper_sigSrc.cpp DLL/libFrontEndControl.a 
	g++ $(CPPFLAGS) $(DEBUGFLAGS) -o t_lv_wrapper_sigSrc.exe \
	tests/t_lv_wrapper_sigSrc.cpp DLL/libFrontEndControl.a \
	$(PROJECTINC)

t_SocketClient.exe : tests/t_SocketClient.cpp
	g++ $(CPPFLAGS) $(DEBUGFLAGS) -o t_SocketClient.exe \
	tests/t_SocketClient.cpp DLL/libFrontEndControl.a \
	$(PROJECTINC)
	
t_LookupTables.exe : tests/t_LookupTables.cpp
	g++ $(CPPFLAGS) $(DEBUGFLAGS) -o t_LookupTables.exe \
	tests/t_LookupTables.cpp CONFIG/LookupTables.o \
	$(PORTABLEINC) \
	$(PORTABLELIB)

t_semaphore_leaks.exe : tests/t_semaphore_leaks.cpp
	g++ $(CPPFLAGS) $(DEBUGFLAGS) -o t_semaphore_leaks.exe \
	tests/t_semaphore_leaks.cpp \
	$(PORTABLEINC) \
	$(PORTABLELIB)

t_StreamLogger.exe : tests/t_StreamLogger.cpp
	g++ $(CPPFLAGS) $(DEBUGFLAGS) -o t_StreamLogger.exe \
	tests/t_StreamLogger.cpp $(AMBOUT) $(BASICOUT) \
	$(PROJECTINC) \
	$(UTILLIB) $(PORTABLELIB) $(AMBLIB)

t_FEICDataBase.exe : tests/t_FEICDataBase.cpp CONFIG/FrontEndDatabase.cpp
	g++ $(CPPFLAGS) $(DEBUGFLAGS) -o t_FEICDataBase.exe \
	tests/t_FEICDataBase.cpp CONFIG/FrontEndDatabase.cpp CONFIG/LookupTables.cpp DLL/SWVersion.cpp \
	$(PROJECTINC) $(MYSQLINC) \
	$(DBLIB) $(UTILLIB) $(MYSQLLIB) $(PORTABLELIB)

.PHONY: all
all: allout dll tests

current: $(DLLOUT)

oneFile:
	$(MAKE) FEMCEventQueue.o
	
clean:
	@for dir in DLL CONFIG FEBASE LOGGER OPTIMIZE; do \
	           $(MAKE) -C $$dir clean; \
             done
	rm -rf *.a
	rm -rf *.o
	rm -rf *.exe
	rm -rf FrontEndControl.dll
	rm -rf DLL/*.a
	rm -rf DLL/FrontEndControl.def

