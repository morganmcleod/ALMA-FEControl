/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2011
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
*
*/

#include "SWVersion.h"

const std::string FRONTENDAMB_SW_VERSION_STRING("FrontEndAMB.DLL 0.0.2");

//******* Be sure to update resource.rc!



// Version history:
// 0.0.2   Looks for environment variable FRONTENDAMBDLL.INI, full path to FrontEndAmbDLL.INI
//         Default is current working directory.   Added example FrontEndAmbDLL.INI
// 0.0.1   Initial version
