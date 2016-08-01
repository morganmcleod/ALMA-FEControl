#ifndef _FRONTENDCONTROL_LV_WRAPPER_DLL_EXPORT_H_
#define _FRONTENDCONTROL_LV_WRAPPER_DLL_EXPORT_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2005
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
* "@(#) $Id: dllExport.h,v 1.3 2005/07/15 21:34:46 mmcleod Exp $"
*
*/

/************************************************************************
 * Macros for exporting functions from a Windows DLL.
 * 
 *----------------------------------------------------------------------
 */

#ifdef __cplusplus
     #define cppfudge "C"
#else
     #define cppfudge
#endif

#ifdef BUILD_FRONTENDCONTROL
     // the dll exports
     #define DLLEXPORT __declspec(dllexport) __cdecl
#else
     // the exe imports
     #define DLLEXPORT extern cppfudge __declspec(dllimport) __cdecl
#endif

#endif /*_FRONTENDCONTROL_LV_WRAPPER_DLL_EXPORT_H_*/
