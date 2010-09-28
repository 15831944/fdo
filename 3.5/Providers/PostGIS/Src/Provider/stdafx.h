//
// Copyright (C) 2007 Refractions Research, Inc. 
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of version 2.1 of the GNU Lesser
// General Public License as published by the Free Software Foundation.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
#ifndef POSTGISPROVIDER_STDAFX_H_INCLUDED
#define POSTGISPROVIDER_STDAFX_H_INCLUDED

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER          // Allow use of features specific to Windows XP or later.
#define WINVER 0x0501   // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501 // Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS          // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410   // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600    // Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

// Build-in support of Visual Leak Detector
#ifdef FDOPOSTGIS_USE_VLD
//#include <vld.h>
#endif // FDOPOSTGIS_USE_VLD

// Windows Header Files
#ifdef _WIN32
#include <windows.h>
#endif

#include <cstring>

// FDO PostGIS Logger
#include "Logger.h"

#endif // POSTGISPROVIDER_STDAFX_H_INCLUDED