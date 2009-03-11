/*
* 
* Copyright (C) 2004-2006  Autodesk, Inc.
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
*
*/

#include "stdafx.h"

#include <Fdo\Commands\DataStore\ICreateDataStore.h>
#include <Fdo\Commands\CommandType.h>

#include "FDO\Commands\DataStore\mgICreateDataStoreImp.h"
#include "FDO\mgObjectFactory.h"
#include "FDO\Commands\DataStore\mgIDataStorePropertyDictionaryImp.h"

FdoICreateDataStore* NAMESPACE_OSGEO_FDO_COMMANDS_DATASTORE::ICreateDataStoreImp::GetImpObj()
{
    return static_cast<FdoICreateDataStore*>(__super::UnmanagedObject.ToPointer());
}

NAMESPACE_OSGEO_FDO_COMMANDS_DATASTORE::IDataStorePropertyDictionary^ NAMESPACE_OSGEO_FDO_COMMANDS_DATASTORE::ICreateDataStoreImp::DataStoreProperties::get()
{
	FdoIDataStorePropertyDictionary* result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetDataStoreProperties())

    return NAMESPACE_OSGEO_FDO::ObjectFactory::CreateIDataStorePropertyDictionary(IntPtr(result), true);
}

System::Void NAMESPACE_OSGEO_FDO_COMMANDS_DATASTORE::ICreateDataStoreImp::Execute()
{
	EXCEPTION_HANDLER(GetImpObj()->Execute())
}
