# ALMA - Atacama Large Millimiter Array
# (c) Associated Universities Inc., 2010
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
# Morgan McLeod <mmcleod@nrao.edu> 19 October 2010
#
# makefile for test suites to exercise the the Front End, Compressor, and Power Supply ICDs.
#
# uses:
# MinGW from http://www.mingw.org/ provides a POSIX build environment including GCC 3.4.2.
# CppUnit from http://cppunit.sourceforge.net/ which is a unit testing framework for C++.
# FrontEndAMBLib for low-level CAN communications
# 
#
BUILD = L:/cpp

# includes and libs for "portable" code = POSIX threads:
PTHREADINC = -I$(BUILD)/3rdparty/include/pthreads_win32
PTHREADLIB = -L$(BUILD)/3rdparty/lib/pthreads_win32 -lpthreadGC2
STATICGCCLIB = -static-libgcc

PORTABLEINC = $(PTHREADINC)
PORTABLELIB = $(PTHREADLIB) $(STATICGCCLIB)

# includes and libs for the CppUtilityLib library:
export UTILINC = -I$(BUILD)/CppUtilityLib/include
UTILLIB = -L$(BUILD)/CppUtilityLib/Debug -lCppUtilityLib

# includes and libs for the FrontEndAMB library:
AMBINC = -I$(BUILD)/FrontEndAmbLib/include
AMBLIB = -L$(BUILD)/FrontEndAmbLib/Debug -lfrontEndAmb

# includes and libs for CppUnit:
CPPUNITINC = -I$(BUILD)/3rdparty/include/cppunit
CPPUNITLIB = -L$(BUILD)/3rdparty/lib/cppunit/Debug -lCppUnit 

# includes and libs for the NI CAN interface library: 
NICANINC = -I$(BUILD)/3rdparty/include/Nican
NICANLIB = $(BUILD)/3rdparty/lib/Nican/nicanmsc.lib

# include order for test apps:
APPINCLUDES = $(UTILINC) $(AMBINC) $(CPPUNITINC) $(PORTABLEINC)

# lib order for test apps:
APPLIBS = $(UTILLIB) $(AMBLIB) $(CPPUNITLIB) $(PORTABLELIB) $(NICANLIB)  

# C++ build options:
CPPFLAGS = -O0 -g3 -Wall -fmessage-length=0

# Link flags:
LDFLAGS = -Wl,--enable-auto-import

# target CompressorICDTest:
COMPRESSORSRC = AmbDeviceTestFixture.cpp CompressorTestFixture.cpp TestApp.cpp

CompressorICDTest.exe : $(COMPRESSORSRC)
	g++ $(CPPFLAGS) -o Debug/CompressorICDTest.exe \
	$(COMPRESSORSRC) $(APPINCLUDES) $(APPLIBS) $(LDFLAGS)

# target CryostatICDTest:
CRYOSTATSRC = AmbDeviceTestFixture.cpp CryostatTestFixture.cpp TestApp.cpp

CryostatICDTest.exe : $(CRYOSTATSRC)
	g++ $(CPPFLAGS) -o Debug/CryostatICDTest.exe \
	$(CRYOSTATSRC) $(APPINCLUDES) $(APPLIBS) $(LDFLAGS)

# target FrontEndICDTest:
FRONTENDSRC = AllBands.cpp AmbDeviceTestFixture.cpp Band3.cpp Band6.cpp Band7.cpp Band9.cpp \
    CartridgeTestFixture.cpp ColdCartridgeTestFixture.cpp \
	CPDSTestFixture_deepika.cpp FEMCTestFixture.cpp IFSwitchTestFixture.cpp TestApp.cpp WCATestFixture.cpp

FrontEndICDTest.exe : $(FRONTENDSRC)
	g++ $(CPPFLAGS) -o Debug/FrontEndICDTest.exe \
	$(FRONTENDSRC) $(APPINCLUDES) $(APPLIBS) $(LDFLAGS)

# target LPRICDTest:
LPRSRC = AmbDeviceTestFixture.cpp LPRTestFixture_deepika.cpp TestApp.cpp

LPRICDTest.exe : $(LPRSRC)
	g++ $(CPPFLAGS) -o Debug/LPRICDTest.exe \
	$(LPRSRC) $(APPINCLUDES) $(APPLIBS) $(LDFLAGS)

# target PowerSupplyICDTest:
POWERSUPPLYSRC = AmbDeviceTestFixture.cpp PowerSupplyTestFixture.cpp TestApp.cpp

PowerSupplyICDTest.exe : $(POWERSUPPLYSRC)
	g++ $(CPPFLAGS) -o Debug/PowerSupplyICDTest.exe \
	$(POWERSUPPLYSRC) $(APPINCLUDES) $(APPLIBS) $(LDFLAGS)

# target all:
all : CompressorICDTest.exe CryostatICDTest.exe FrontEndICDTest.exe LPRICDTest.exe PowerSupplyICDTest.exe

# target clean:
clean:
	rm -rf Debug/*.exe

