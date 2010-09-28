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
#include "mgIGeometricAggregateAbstractImp.h"
#include "mgIMultiGeometry.h"

class FdoIMultiGeometry;
BEGIN_NAMESPACE_OSGEO_GEOMETRY
interface class IGeometry;
END_NAMESPACE_OSGEO_GEOMETRY

BEGIN_NAMESPACE_OSGEO_GEOMETRY

/// \ingroup (OSGeoFDOGeometry)
/// \brief
/// The IMultiGeometryImp class is a heterogeneous MultiGeometry type.
/// One MultiGeometry containing another MultiGeometry
/// is not supported.
private ref class IMultiGeometryImp : 
    public NAMESPACE_OSGEO_GEOMETRY::IGeometricAggregateAbstractImp, public NAMESPACE_OSGEO_GEOMETRY::IMultiGeometry
{
public:
    /// \brief
    /// Constructs a managed object based on an unmanaged instance of the object
    /// 
    /// \param unmanaged 
    /// Input A Pointer to the unmanaged object.
    /// 
    /// \param autoDelete 
    /// Input Indicates if the constructed element should be automatically deleted 
    /// once it no longer referenced.
    /// 
	IMultiGeometryImp(System::IntPtr unmanaged, System::Boolean autoDelete);

internal:
	FdoIMultiGeometry* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;

public:
    /// \brief
    /// Gets a Geometry at the given zero-based index.
    /// 
    /// \param index 
    /// Input Zero-based index in the object's list of geometries.
    /// 
    /// \return
    /// Returns a Geometry
    /// 
    property NAMESPACE_OSGEO_GEOMETRY::IGeometry^ default[System::Int32]
    {
        virtual NAMESPACE_OSGEO_GEOMETRY::IGeometry^ get(System::Int32 index);
    }
};

END_NAMESPACE_OSGEO_GEOMETRY

