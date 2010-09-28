// 
//  
//  Copyright (C) 2004-2006  Autodesk, Inc.
//  
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//  
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//  
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//  
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif

#include <Fdo.h>
#include <Fdo/Xml/FeatureContext.h>

///<summary>
/// This is only a dummy implementation to facilitate linkage of WFS provider,
/// it will be updated later.
///</summary>


FdoXmlFeatureContext::FdoXmlFeatureContext(FdoXmlFeaturePropertyReader* reader)
:FdoXmlSaxContext(NULL)
//no default constructor defined in FdoXmlSaxContext,
//must force to call FdoXmlSaxContext(FdoXmlReader)
//otherwise linking error
{
}

FdoXmlFeatureContext::~FdoXmlFeatureContext()
{
}

FdoXmlFeatureContext* FdoXmlFeatureContext::Create(FdoXmlFeaturePropertyReader* reader)
{
	return new FdoXmlFeatureContext(reader);
}

FdoXmlFeaturePropertyReader* FdoXmlFeatureContext::GetFeaturePropertyReader()
{
	return NULL;
}

