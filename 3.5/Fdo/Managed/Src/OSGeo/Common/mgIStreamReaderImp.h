/*
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

#pragma once

#include "Runtime\Disposable.h"
#include "Common\mgIStreamReader.h"

class FdoIStreamReader;

BEGIN_NAMESPACE_OSGEO_COMMON

/// \ingroup (OSGeoFDOCommon)
/// \brief
///     The IStreamReader class is a top level interface for stream reader classes. 
///     The API it provides allows the user to read large streams of data in blocks of items. 
public ref class IStreamReaderImp : 
    public NAMESPACE_OSGEO_RUNTIME::Disposable, public NAMESPACE_OSGEO_COMMON::IStreamReader
{
public:
    /// \brief
    ///     Constructs a IStreamReader object based on an unmanaged instance of the stream
    /// 
    /// \param unmanaged 
    ///     Input A Pointer to the unmanaged Stream Reader.
    /// 
    /// \param autoDelete 
    ///     Input Indicates if the constructed dictionary should be automatically deleted 
    ///     once it no longer referenced.
    /// 
	IStreamReaderImp(System::IntPtr unmanaged, System::Boolean autoDelete);

    /// \brief
    /// Gets the type of the Stream Reader
    /// 
    /// \return
    /// Returns the Stream Reader type
    /// 
    virtual property StreamReaderType Type
    {
        StreamReaderType get();
    }

/// \cond DOXYGEN-IGNORE

internal:
	inline  FdoIStreamReader* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;
/// \endcond
};

END_NAMESPACE_OSGEO_COMMON

