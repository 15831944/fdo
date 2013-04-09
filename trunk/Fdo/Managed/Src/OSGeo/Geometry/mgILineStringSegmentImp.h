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
#include "mgICurveSegmentAbstractImp.h"
#include "mgILineStringSegment.h"

class FdoILineStringSegment;
BEGIN_NAMESPACE_OSGEO_GEOMETRY
interface class IDirectPosition;
ref class DirectPositionCollection;
END_NAMESPACE_OSGEO_GEOMETRY

BEGIN_NAMESPACE_OSGEO_GEOMETRY

/// \ingroup (OSGeoFDOGeometry)
/// \brief
/// The ILineStringSegmentImp class is a LineString curve segment type.  
/// The shape of ILineStringSegmentImp is the set of positions defined by the contained collection, 
/// plus linear interpolation between consecutive points.
/// This is a helper type for Geometries in the Geometry package.
/// Note: It does not derive from IGeometry.
private ref class ILineStringSegmentImp : 
    public NAMESPACE_OSGEO_GEOMETRY::ICurveSegmentAbstractImp, public NAMESPACE_OSGEO_GEOMETRY::ILineStringSegment
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
	ILineStringSegmentImp(System::IntPtr unmanaged, System::Boolean autoDelete);

internal:
	FdoILineStringSegment* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;

public:
    /// \brief
    /// Gets the number of positions in this object.
    /// 
    /// \return
    /// Returns the number of positions
    /// 
    property System::Int32 Count
    {
        virtual System::Int32 get();
    }

    /// \brief
    /// Gets the position at the specified (zero-based) index.
    /// 
    /// \return
    /// Returns the position
    /// 
    property NAMESPACE_OSGEO_GEOMETRY::IDirectPosition^ default[System::Int32]
    {
        virtual NAMESPACE_OSGEO_GEOMETRY::IDirectPosition^ get(System::Int32 index);
    }

    /// \brief
    /// Gets a collection of all of the positions in this object.
    /// 
    /// \return
    /// Returns the positions
    /// 
    property NAMESPACE_OSGEO_GEOMETRY::DirectPositionCollection^ Positions
    {
        virtual NAMESPACE_OSGEO_GEOMETRY::DirectPositionCollection^ get();
    }
};

END_NAMESPACE_OSGEO_GEOMETRY


