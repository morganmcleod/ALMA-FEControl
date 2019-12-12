# ALMA - Atacama Large Millimiter Array
# (c) Associated Universities Inc., 2006
#
#This library is free software; you can redistribute it and/or
#modify it under the terms of the GNU Lesser General Public
#License as published by the Free Software Foundation; either
#version 2.1 of the License, or (at your option) any later version.
#
#This library is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#Lesser General Public License for more details.
#
#You should have received a copy of the GNU Lesser General Public
#License along with this library; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
#
# Morgan McLeod <mmcleod@nrao.edu> 30 August 2006
#
# makefile for ALMA FrontEnd low-level control library for Windows.
#
# uses:
# MinGW from http://www.mingw.org/ provides a POSIX build environment including GCC 3.4.2.
# Pthreads-win32 from http://sourceware.org/pthreads-win32/
# STLPort from http://www.stlport.org/ thought it can be built with GCC's STL as well.
# MySQL C API version 4.1 or later from http://www.mysql.com/
#

CONFIGCPP = CartConfig.cpp ConfigManager.cpp Configuration.cpp \
	ConfigProviderIniFile.cpp ConfigProviderMySQL.cpp ConfigProviderXML.cpp \
	FrontEndConfig.cpp FrontEndDatabase.cpp LookupTables.cpp SubsysConfig.cpp XMLParser.cpp

.PHONY: config
config: $(CONFIGCPP)
	g++ $(CPPFLAGS) $(DEBUGFLAGS) \
	$(PROJECTINC) $(MYSQLINC) $(EXPATMMINC) \
	-c $(CONFIGCPP)

clean:
	@rm -rf *.a
	@rm -rf *.o
	@rm -rf *.exe
	@rm -rf *.dll


