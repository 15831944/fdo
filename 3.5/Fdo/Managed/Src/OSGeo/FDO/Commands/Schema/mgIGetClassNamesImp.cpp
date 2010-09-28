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
#include <Fdo\Commands\Schema\IGetClassNames.h>
#include <Fdo\Commands\CommandType.h>

#include "FDO\Commands\Schema\mgIGetClassNamesImp.h"
#include "FDO\mgObjectFactory.h"
#include "FDO\Schema\mgFeatureSchemaCollection.h"

FdoIGetClassNames* NAMESPACE_OSGEO_FDO_COMMANDS_SCHEMA::IGetClassNamesImp::GetImpObj()
{
    return static_cast<FdoIGetClassNames*>(UnmanagedObject.ToPointer());
}

IntPtr NAMESPACE_OSGEO_FDO_COMMANDS_SCHEMA::IGetClassNamesImp::GetDisposableObject()
{
    return IntPtr(static_cast<FdoIDisposable*>(GetImpObj()));
}

System::String^ NAMESPACE_OSGEO_FDO_COMMANDS_SCHEMA::IGetClassNamesImp::SchemaName::get()
{
	FdoString* result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetSchemaName())

	return CHECK_STRING(result);
}

System::Void NAMESPACE_OSGEO_FDO_COMMANDS_SCHEMA::IGetClassNamesImp::SchemaName::set(System::String^ value)
{
	EXCEPTION_HANDLER(GetImpObj()->SetSchemaName(StringToUni(value)))
}

NAMESPACE_OSGEO_COMMON::StringCollection^ NAMESPACE_OSGEO_FDO_COMMANDS_SCHEMA::IGetClassNamesImp::Execute()
{
	FdoStringCollection* result;

	EXCEPTION_HANDLER(result = GetImpObj()->Execute())

	return NAMESPACE_OSGEO_COMMON::ObjectFactory::CreateStringCollection(IntPtr(result), true);
}