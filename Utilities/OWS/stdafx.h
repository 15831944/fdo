// Copyright (C) 2004-2006  Autodesk, Inc.
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
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
    #include <windows.h>
#else
    //defines that helps us use hash_map (and similar STL classes) in the same way 
    //under Linux and Windows.
    #include <ext/hash_map>
    #include <ext/functional>
    namespace stdext = ::__gnu_cxx;
    using namespace std;
#endif

#include <cstring>
#include <OWS/FdoOWS.h>
#include <FdoStd.h>
#include <Fdo.h>
#include <FdoCommonMiscUtil.h>
#include <FdoCommonOSUtil.h>
