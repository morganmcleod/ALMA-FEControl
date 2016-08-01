/*******************************************************************************
* ALMA - Atacama Large Millimiter Array
* (c) Associated Universities Inc., 2016
*
*This library is free software; you can redistribute it and/or
*modify it under the terms of the GNU Lesser General Public
*License as published by the Free Software Foundation; either
*version 2.1 of the License, or (at your option) any later version.
*
*This library is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*Lesser General Public License for more details.
*
*You should have received a copy of the GNU Lesser General Public
*License along with this library; if not, write to the Free Software
*Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
*/


README file for FrontEndControl C++ source code distribution
Added for version 2.2.3, 25 August 2009
Updated for version 2.5.0, 1 August 2016
Morgan McLeod <mmcleod@nrao.edu>
National Radio Astronomy Observatory
Charlottesville, VA USA


ALMA Front End Control - Windows library


This project builds a Windows DLL for control of the ALMA Front End
and auxiliary devices via the CAN bus (as well as other communication links.)

The software development plan for this library is here:
FEND-40.00.00.00-075-A-PLA
http://edm.alma.cl/forums/alma/dispatch.cgi/iptfedocs/docProfile/102012

The algorithms embodied in this library are described here:
FEND-40.00.00.00-089-A-MAN
http://edm.alma.cl/forums/alma/dispatch.cgi/iptfedocs/showFolder/103707

The user manual for the LabVIEW software which uses this library is here:
FEND-40.09.03.00-053-A-MAN
http://edm.alma.cl/forums/alma/dispatch.cgi/iptfedocs/showFolder/104673


See historyAndTodos.txt for revision history, future enhancements, and known bugs.


This software library depends upon the following packages:
* MinGW - Minimalist Gnu for Windows - http://www.mingw.org/
* PthreadsWin32 - POSIX threads for Win32 - http://sourceware.org/pthreads-win32/
* MySQL client 4.1.22 or newer - http://www.mysql.com/
* MySQL ODBC connector 5.1.4 or newer - http://www.mysql.com/
* National Instruments NICAN 2.6.1 or newer

The following tools are very helpful for software development
* Eclipse 3.3.2 with CDT 4.0.3 - http://www.eclipse.org/
* Doxygen documentation generator - http://www.doxygen.org/
* Eclox, a Doxygen frontend plugin for Eclipse - http://home.gna.org/eclox/

Notes:

[version 2.5.0 - 2016-08-01] 

Having subdirectories:

* CppUtilityLib:   helper utilities used by all the other libraries.

* FEICDBLib:       database access library for the FETMS database.

* FrontEndAMBLib:  low-level CAN and Socket communications.

* FrontEndICDTest: a CPPUnit-based test deck to prove the library complies with the FE-Computing ICD

* FrontEndControl2:

The key procedures for operating receiver cartridges are found in CartAssembly.cpp.
CartAssembly calls classes in the OPTIMIZE folder for any time-consuming procedures or measurements.

The code was originally designed to mimic the architecture of the ALMA Computing Control software and it is very similar
to that software at the "AMB" layer of the software stack:

DLL/lv_wrapper etc. - DLL exported interface and adapters for use in LabVIEW
|
FrontEndImpl - Facade class encapsulating all front end functionality
|
CartAssembly, all other top-level directory objects
|
CONFIG - configuration database implementation for INI files and for MySQL
|
FEBASE - device drivers for FE hardware objects
|
LOGGER - customizations to the logger found in CppUtilityLib
|
OPTIMIZE - cartridge optimization measurements and other things which run on a worker thread.
|
TESTS - test programs to exercise this library
|
doc - Doxygen generated documentation for the source code













