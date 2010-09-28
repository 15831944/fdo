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
#include <Fdo.h>

#include "mgIRasterCapabilitiesImp.h"

NAMESPACE_OSGEO_FDO_CONNECTIONS_CAPABILITIES::IRasterCapabilitiesImp::IRasterCapabilitiesImp(IntPtr unmanaged, Boolean autoDelete) 
	: NAMESPACE_OSGEO_RUNTIME::Disposable(unmanaged, autoDelete)
{

}

FdoIRasterCapabilities* NAMESPACE_OSGEO_FDO_CONNECTIONS_CAPABILITIES::IRasterCapabilitiesImp::GetImpObj()
{
    return static_cast<FdoIRasterCapabilities*>(UnmanagedObject.ToPointer());
}

IntPtr NAMESPACE_OSGEO_FDO_CONNECTIONS_CAPABILITIES::IRasterCapabilitiesImp::GetDisposableObject()
{
    return IntPtr(static_cast<FdoIDisposable*>(GetImpObj()));
}

System::Boolean NAMESPACE_OSGEO_FDO_CONNECTIONS_CAPABILITIES::IRasterCapabilitiesImp::SupportsRaster ()
{
	System::Boolean rv;
	EXCEPTION_HANDLER(rv = !!GetImpObj()->SupportsRaster())
	return rv;
}

System::Boolean NAMESPACE_OSGEO_FDO_CONNECTIONS_CAPABILITIES::IRasterCapabilitiesImp::SupportsStitching ()
{
	System::Boolean rv;
	EXCEPTION_HANDLER(rv = !!GetImpObj()->SupportsStitching())
	return rv;
}

System::Boolean NAMESPACE_OSGEO_FDO_CONNECTIONS_CAPABILITIES::IRasterCapabilitiesImp::SupportsSubsampling ()
{
	System::Boolean rv;
	EXCEPTION_HANDLER(rv = !!GetImpObj()->SupportsSubsampling())
	return rv;
}

System::Boolean NAMESPACE_OSGEO_FDO_CONNECTIONS_CAPABILITIES::IRasterCapabilitiesImp::SupportsDataModel (NAMESPACE_OSGEO_FDO_RASTER::RasterDataModel^ model)
{
	System::Boolean rv;
	EXCEPTION_HANDLER(rv = !!GetImpObj()->SupportsDataModel(model == nullptr ? nullptr : model->GetImpObj()))
	return rv;
}