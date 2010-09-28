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
#include "Common\mgException.h"
#include "Common\Io\mgIoStream.h"
#include "Common\Io\mgIoCharStreamReader.h"


NAMESPACE_OSGEO_COMMON_IO::IoCharStreamReader::IoCharStreamReader(System::IntPtr unmanaged, System::Boolean autoDelete) : IStreamReaderImp(unmanaged, autoDelete)
{

}

FdoIStreamReaderTmpl<FdoCharacter>* NAMESPACE_OSGEO_COMMON_IO::IoCharStreamReader::GetImpObj()
{
	return static_cast<FdoIStreamReaderTmpl<FdoCharacter>*>(UnmanagedObject.ToPointer());
}

IntPtr NAMESPACE_OSGEO_COMMON_IO::IoCharStreamReader::GetDisposableObject()
{
    return IntPtr(static_cast<FdoIDisposable*>(GetImpObj()));
}

System::Void NAMESPACE_OSGEO_COMMON_IO::IoCharStreamReader::Skip(System::Int32 offest)
{
	EXCEPTION_HANDLER(GetImpObj()->Skip(offest))
}

System::Void NAMESPACE_OSGEO_COMMON_IO::IoCharStreamReader::Reset()
{
	EXCEPTION_HANDLER(GetImpObj()->Reset())
}

System::Int32 NAMESPACE_OSGEO_COMMON_IO::IoCharStreamReader::ReadNext(array<System::Char>^ buffer)
{
	pin_ptr<FdoCharacter> upByte = &buffer[0];
	FdoInt32 rCount;

	EXCEPTION_HANDLER(rCount = GetImpObj()->ReadNext(upByte))

	return rCount;	
}

System::Int32 NAMESPACE_OSGEO_COMMON_IO::IoCharStreamReader::ReadNext(array<System::Char>^ buffer, System::Int32 offest)
{
	pin_ptr<FdoCharacter> upByte = &buffer[0];
	FdoInt32 rCount;

	EXCEPTION_HANDLER(rCount = GetImpObj()->ReadNext(upByte, offest))

	return rCount;	
}

System::Int32 NAMESPACE_OSGEO_COMMON_IO::IoCharStreamReader::ReadNext(array<System::Char>^ buffer, System::Int32 offest, System::Int32 count)
{
	pin_ptr<FdoCharacter> upByte = &buffer[0];
	FdoInt32 rCount;

	EXCEPTION_HANDLER(rCount = GetImpObj()->ReadNext(upByte, offest, count))

	return rCount;
}

System::Int64 NAMESPACE_OSGEO_COMMON_IO::IoCharStreamReader::Length::get()
{
	FdoInt64 length;

	EXCEPTION_HANDLER(length = GetImpObj()->GetLength())

	return length;
}

System::Int64 NAMESPACE_OSGEO_COMMON_IO::IoCharStreamReader::Index::get()
{
	FdoInt64 index;

	EXCEPTION_HANDLER(index = GetImpObj()->GetIndex())

	return index;
}