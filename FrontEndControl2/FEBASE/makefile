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
# Morgan McLeod <mmcleod@nrao.edu> 2021-10-15

FEBASECPP = $(wildcard *.cpp)

INCLUDEDIR_MOD=$(patsubst -I%, -I../%, $(INCLUDEDIR))

.PHONY: febase
febase: $(FEBASECPP)
	g++ $(FLAGS) $(INCLUDEDIR_MOD) -c $(FEBASECPP)

clean:
	@rm -rf *.a
	@rm -rf *.o
