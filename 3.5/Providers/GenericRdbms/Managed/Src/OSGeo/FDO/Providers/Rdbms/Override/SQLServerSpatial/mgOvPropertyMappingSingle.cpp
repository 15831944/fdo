//
// Copyright (C) 2004-2008  Autodesk, Inc.
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
//

#include "stdafx.h"
#include "Rdbms\Override\RdbmsOv.h"
#include "SQLServerSpatial\SqlServerOvPropertyMappingSingle.h"

#include "FDO\Providers\Rdbms\Override\SQLServerSpatial\mgOvPropertyMappingSingle.h"
#include "FDO\Providers\Rdbms\Override\SQLServerSpatial\mgObjectFactory.h"
#include "FDO\Providers\Rdbms\Override\SQLServerSpatial\mgOvClassDefinition.h"

NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE_SQLSERVERSPATIAL::OvPropertyMappingSingle::OvPropertyMappingSingle() : NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE::OvPropertyMappingSingle(System::IntPtr::Zero, false)
{
	EXCEPTION_HANDLER(Attach(IntPtr((FdoIDisposable*)FdoSqlServerOvPropertyMappingSingle::Create()), true))
}

NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE_SQLSERVERSPATIAL::OvPropertyMappingSingle::OvPropertyMappingSingle(System::IntPtr unmanaged, System::Boolean autoDelete) : NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE::OvPropertyMappingSingle(unmanaged, autoDelete)
{

}

FdoSqlServerOvPropertyMappingSingle* NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE_SQLSERVERSPATIAL::OvPropertyMappingSingle::GetImpObj()
{
	return dynamic_cast<FdoSqlServerOvPropertyMappingSingle*>((FdoIDisposable*)UnmanagedObject.ToPointer());
}

IntPtr NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE_SQLSERVERSPATIAL::OvPropertyMappingSingle::GetDisposableObject()
{
    return IntPtr(static_cast<FdoIDisposable*>(GetImpObj()));
}