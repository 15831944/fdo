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
// The following header files sequence is very important
#include <Fdo\Raster\RasterDataModel.h>
#include <Fdo\Expression\DataValue.h>
#include <Fdo\Commands\Feature\IReader.h>
#include <Fdo\Commands\CommandType.h>

#include "FDO\Commands\Feature\mgIReaderImp.h"
#include "FDO\mgObjectFactory.h"
#include "FDO\Expression\mgLOBValue.h"
#include "FDO\Raster\mgIRasterImp.h"

FdoIReader* NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetImpObj()
{
    return static_cast<FdoIReader*>(UnmanagedObject.ToPointer());
}

IntPtr NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetDisposableObject()
{
    return IntPtr(static_cast<FdoIDisposable*>(GetImpObj()));
}

System::String^ NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetPropertyName(System::Int32 index)
{
    FdoString* result;

    EXCEPTION_HANDLER(result = GetImpObj()->GetPropertyName(index))

    return CHECK_STRING(result);
}

System::Int32 NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetPropertyIndex(System::String^ name)
{
    System::Int32 result;

    EXCEPTION_HANDLER(result = GetImpObj()->GetPropertyIndex(StringToUni(name)))

    return result;
}

System::Boolean NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetBoolean(System::String^ name)
{
	System::Boolean result;

	EXCEPTION_HANDLER(result = !!GetImpObj()->GetBoolean(StringToUni(name)))

	return result;
}

System::Byte NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetByte(System::String^ name)
{
	System::Byte result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetByte(StringToUni(name)))

	return result;
}

System::DateTime NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetDateTime(System::String^ name)
{
	FdoDateTime result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetDateTime(StringToUni(name)))

	return FdoDateTimeToDateTime(result);
}

System::Double NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetDouble(System::String^ name)
{
	System::Double result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetDouble(StringToUni(name)))

	return result;
}

System::Int16 NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetInt16(System::String^ name)
{
	System::Int16 result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetInt16(StringToUni(name)))

	return result;
}

System::Int32 NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetInt32(System::String^ name)
{
	System::Int32 result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetInt32(StringToUni(name)))

	return result;
}

System::Int64 NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetInt64(System::String^ name)
{
	System::Int64 result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetInt64(StringToUni(name)))

	return result;
}

System::Single NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetSingle(System::String^ name)
{
	System::Single result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetSingle(StringToUni(name)))

	return result;
}

System::String^ NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetString(System::String^ name)
{
	FdoString* result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetString(StringToUni(name)))

	return CHECK_STRING(result);
}

NAMESPACE_OSGEO_FDO_EXPRESSION::LOBValue^ NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetLOB(System::String^ name)
{
	FdoLOBValue* result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetLOB(StringToUni(name)))

	return NAMESPACE_OSGEO_FDO::ObjectFactory::CreateLOBValue(IntPtr(result), true);
}

NAMESPACE_OSGEO_COMMON::IStreamReader^ NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetLOBStreamReader(System::String^ name)
{
	FdoIStreamReader* result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetLOBStreamReader(StringToUni(name)))

    return NAMESPACE_OSGEO_COMMON::ObjectFactory::CreateIStreamReader(IntPtr(result), true);
}

NAMESPACE_OSGEO_FDO_RASTER::IRaster^ NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetRaster(System::String^ name)
{
	FdoIRaster* result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetRaster(StringToUni(name)))

    return NAMESPACE_OSGEO_FDO::ObjectFactory::CreateIRaster(IntPtr(result), true);
}

System::Boolean NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::IsNull(System::String^ name)
{
	System::Boolean result;

	EXCEPTION_HANDLER(result = !!GetImpObj()->IsNull(StringToUni(name)))

	return result;
}

array<System::Byte>^ NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetGeometry(System::String^ name)
{
    array<System::Byte>^ result;
	FdoByteArray* arr;
    try
    {
	    EXCEPTION_HANDLER(arr = GetImpObj()->GetGeometry(StringToUni(name)))
        if (arr != NULL)
        {
            result = FdoByteArrayToByteArray(arr->GetData(), arr->GetCount());
        }
    }
    finally
    {
	    arr->Release();
    }
	return result;
}

System::Boolean NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetBoolean(System::Int32 index)
{
    System::Boolean result;

    EXCEPTION_HANDLER(result = !!GetImpObj()->GetBoolean(index))

    return result;
}

System::Byte NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetByte(System::Int32 index)
{
    System::Byte result;

    EXCEPTION_HANDLER(result = GetImpObj()->GetByte(index))

    return result;
}

System::DateTime NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetDateTime(System::Int32 index)
{
    FdoDateTime result;

    EXCEPTION_HANDLER(result = GetImpObj()->GetDateTime(index))

    return FdoDateTimeToDateTime(result);
}

System::Double NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetDouble(System::Int32 index)
{
    System::Double result;

    EXCEPTION_HANDLER(result = GetImpObj()->GetDouble(index))

    return result;
}

System::Int16 NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetInt16(System::Int32 index)
{
    System::Int16 result;

    EXCEPTION_HANDLER(result = GetImpObj()->GetInt16(index))

    return result;
}

System::Int32 NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetInt32(System::Int32 index)
{
    System::Int32 result;

    EXCEPTION_HANDLER(result = GetImpObj()->GetInt32(index))

    return result;
}

System::Int64 NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetInt64(System::Int32 index)
{
    System::Int64 result;

    EXCEPTION_HANDLER(result = GetImpObj()->GetInt64(index))

    return result;
}

System::Single NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetSingle(System::Int32 index)
{
    System::Single result;

    EXCEPTION_HANDLER(result = GetImpObj()->GetSingle(index))

    return result;
}

System::String^ NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetString(System::Int32 index)
{
    FdoString* result;

    EXCEPTION_HANDLER(result = GetImpObj()->GetString(index))

    return CHECK_STRING(result);
}

NAMESPACE_OSGEO_FDO_EXPRESSION::LOBValue^ NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetLOB(System::Int32 index)
{
    FdoLOBValue* result;

    EXCEPTION_HANDLER(result = GetImpObj()->GetLOB(index))

    return NAMESPACE_OSGEO_FDO::ObjectFactory::CreateLOBValue(IntPtr(result), true);
}

NAMESPACE_OSGEO_COMMON::IStreamReader^ NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetLOBStreamReader(System::Int32 index)
{
    FdoIStreamReader* result;

    EXCEPTION_HANDLER(result = GetImpObj()->GetLOBStreamReader(index))

    return NAMESPACE_OSGEO_COMMON::ObjectFactory::CreateIStreamReader(IntPtr(result), true);
}

NAMESPACE_OSGEO_FDO_RASTER::IRaster^ NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetRaster(System::Int32 index)
{
    FdoIRaster* result;

    EXCEPTION_HANDLER(result = GetImpObj()->GetRaster(index))

    return NAMESPACE_OSGEO_FDO::ObjectFactory::CreateIRaster(IntPtr(result), true);
}

System::Boolean NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::IsNull(System::Int32 index)
{
    System::Boolean result;

    EXCEPTION_HANDLER(result = !!GetImpObj()->IsNull(index))

    return result;
}

array<System::Byte>^ NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::GetGeometry(System::Int32 index)
{
    array<System::Byte>^ result;
    FdoByteArray* arr;
    try
    {
        EXCEPTION_HANDLER(arr = GetImpObj()->GetGeometry(index))
        if (arr != NULL)
        {
            result = FdoByteArrayToByteArray(arr->GetData(), arr->GetCount());
        }
    }
    finally
    {
        arr->Release();
    }
    return result;
}

System::Boolean NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::ReadNext()
{
	System::Boolean result;

	EXCEPTION_HANDLER(result = !!GetImpObj()->ReadNext())

	return result;
}

System::Void NAMESPACE_OSGEO_FDO_COMMANDS_FEATURE::IReaderImp::Close()
{
	EXCEPTION_HANDLER(GetImpObj()->Close())
}
