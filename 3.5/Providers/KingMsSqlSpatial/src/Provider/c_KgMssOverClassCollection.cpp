/*
 * Copyright (C) 2007  Haris Kurtagic
* 
* This library is free software; you can redistribute it and/or
* modify it under the terms of version 2.1 of the GNU Lesser
* General Public License as published by the Free Software Foundation.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include <c_KgMssOverClassCollection.h>


c_KgMssOverClassCollection* c_KgMssOverClassCollection::Create()
{
    return c_KgMssOverClassCollection::Create(NULL);
}

c_KgMssOverClassCollection* c_KgMssOverClassCollection::Create(FdoPhysicalElementMapping *Parent)
{
    c_KgMssOverClassCollection* pObject = new c_KgMssOverClassCollection(Parent);
    
    return pObject;
}

c_KgMssOverClassCollection::c_KgMssOverClassCollection()
{
}

c_KgMssOverClassCollection::c_KgMssOverClassCollection(FdoPhysicalElementMapping *Parent) :
    FdoPhysicalElementMappingCollection<c_KgMssOverClassDefinition>(Parent)
{
}

c_KgMssOverClassCollection::~c_KgMssOverClassCollection(void)
{
}

void c_KgMssOverClassCollection::Dispose()
{
    delete this;
}
