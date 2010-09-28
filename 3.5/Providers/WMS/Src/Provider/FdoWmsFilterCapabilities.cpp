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

#include "stdafx.h"
#include "FdoWmsFilterCapabilities.h"

FdoWmsFilterCapabilities::FdoWmsFilterCapabilities ()
{
}

FdoWmsFilterCapabilities::~FdoWmsFilterCapabilities ()
{
}

void FdoWmsFilterCapabilities::Dispose ()
{
    delete this;
}

/// <summary>Returns an array of FdoConditionType objects the feature provider supports.</summary>
/// <param name="length">Output the number of condition types</param> 
/// <returns>Returns the list of condition types</returns> 
FdoConditionType* FdoWmsFilterCapabilities::GetConditionTypes (FdoInt32& length)
{
    length = 0;
    return ((FdoConditionType*)NULL);
}

/// <summary>Returns an array of FdoSpatialOperation objects the feature provider supports.</summary>
/// <param name="length">Output the number of spatial operations</param> 
/// <returns>Returns the list of spartial operations</returns> 
FdoSpatialOperations* FdoWmsFilterCapabilities::GetSpatialOperations (FdoInt32& length)
{
    length = 0;
    return ((FdoSpatialOperations*)NULL);
}

/// <summary>Returns an array of FdoDistanceOperation objects the feature provider supports.</summary>
/// <param name="length">Output the number of distance operations</param> 
/// <returns>Returns the list of distance operations</returns> 
FdoDistanceOperations* FdoWmsFilterCapabilities::GetDistanceOperations (FdoInt32& length)
{
    length = 0;
    return ((FdoDistanceOperations*)NULL);
}

/// <summary>Determines if the feature provider supports geodesic distance measurement. Returns false if the feature provider supports only linear distance measurement.</summary>
/// <returns>Returns true if the feature provider supports geodesic distance measurement</returns> 
bool FdoWmsFilterCapabilities::SupportsGeodesicDistance ()
{
    return (false);
}

/// <summary>Determines if spatial and distance operations can be applied between two 
/// geometric properties. Returns false if spatial and distance operations can be applied only between 
/// a geometric property and a literal geometry.</summary>
/// <returns>Returns true if spatial and distance operations can be applied</returns> 
bool FdoWmsFilterCapabilities::SupportsNonLiteralGeometricOperations ()
{
    return (false);
}