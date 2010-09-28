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

#include "mgIGeometricAggregateAbstract.h"

BEGIN_NAMESPACE_OSGEO_GEOMETRY
interface class ICurvePolygon;
END_NAMESPACE_OSGEO_GEOMETRY

BEGIN_NAMESPACE_OSGEO_GEOMETRY

/// \ingroup (OSGeoFDOGeometry)
/// \interface OSGeo::FDO::Geometry::IMultiCurvePolygon
/// \brief
/// The IMultiCurvePolygon class is a multi-CurvePolygon aggregate Geometry type.
public interface class IMultiCurvePolygon : public IGeometricAggregateAbstract
{
public:
    /// \brief
    /// Gets a CurvePolygon at the given zero-based index.
    /// 
    /// \param index 
    /// Input Zero-based index in the object's list of polygons.
    /// 
    /// \return
    /// Returns a CurvePolygon
    /// 
    property NAMESPACE_OSGEO_GEOMETRY::ICurvePolygon^ default[System::Int32]
    {
        NAMESPACE_OSGEO_GEOMETRY::ICurvePolygon^ get(System::Int32 index);
    }
};

END_NAMESPACE_OSGEO_GEOMETRY

