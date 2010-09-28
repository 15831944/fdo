#ifndef FDOSMPHBASEOBJECTCOLLECTION_H
#define FDOSMPHBASEOBJECTCOLLECTION_H		1
//
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

#ifdef _WIN32
#pragma once
#endif

#include <Sm/Ph/../NamedCollection.h>
#include <Sm/Ph/BaseObject.h>

// Represents a collection of Table Objects.

class FdoSmPhBaseObjectCollection : public FdoSmNamedCollection<FdoSmPhBaseObject>
{
public:
	FdoSmPhBaseObjectCollection(const FdoSmSchemaElement* pParent) :
		FdoSmNamedCollection<FdoSmPhBaseObject>(pParent)
	{}
	~FdoSmPhBaseObjectCollection(void) {}

protected:
    /// unused constructor needed only to build on Linux
    FdoSmPhBaseObjectCollection() {}
};

typedef FdoPtr<FdoSmPhBaseObjectCollection> FdoSmPhBaseObjectsP;

#endif

