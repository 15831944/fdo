#ifndef FDOSMPHRDSQSSPATIALCONTEXTREADER_H
#define FDOSMPHRDSQSSPATIALCONTEXTREADER_H		1
/*
 * Copyright (C) 2004-2008  Autodesk, Inc.
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

#include <Sm/Ph/Rd/SpatialContextReader.h>
#include <Sm/Ph/Mgr.h>
#include <Sm/Ph/RowCollection.h>
#include "../Owner.h"

// This reader reads all of the spatial contexts for a datastore, when no MetaSchema
// information is present. 

class FdoSmPhRdSqsSpatialContextReader : public FdoSmPhRdSpatialContextReader
{
public:
	FdoSmPhRdSqsSpatialContextReader();

    /// Create and execute the reader
	FdoSmPhRdSqsSpatialContextReader(
        FdoSmPhOwnerP owner             // Get Spatial Contexts for this owner.
    );

	FdoSmPhRdSqsSpatialContextReader(
        FdoSmPhOwnerP owner,             // Get Spatial Contexts for this database object.
        FdoStringP dbObjectName
    );

    /// Deactivates the query.
	~FdoSmPhRdSqsSpatialContextReader(void);

    /// Gets the name of the spatial context currently being read.
    virtual FdoString* GetName();

    /// Gets the description of the spatial context currently being read.
    virtual FdoString* GetDescription();

    /// Geometry table name
	virtual FdoStringP GetGeomTableName();

    /// Geometry column name
	virtual FdoStringP GetGeomColumnName();

    /// Ordinate dimensionality of geometries.
    virtual bool GetHasElevation();

    virtual bool GetHasMeasure();

    /// Gets the name of the coordinate system of the spatial context currently
    /// being read. If the result is a null or empty string, the coordinate system is
    /// unknown to the spatial context.
    virtual FdoString* GetCoordinateSystem();

    /// Gets the coordinate system in OpenGIS SRS WKT format of the spatial context currently
    /// being read. If the result is an empty string, the coordinate system WKT description is
    /// not available.
    virtual FdoString* GetCoordinateSystemWkt();

    /// Spatial Reference ID (if not zero, must agree with CrsName).
	virtual FdoInt64 GetSrid();

     /// Gets the extent type of the spatial context currently being read.
    virtual FdoSpatialContextExtentType GetExtentType();

    /// Gets the extent of the spatial context currently being read as a byte
    /// array in AGF format.
    virtual FdoByteArray* GetExtent();

    /// Gets the tolerance value for XY ordinates of the spatial context
    /// currently being read. 
    virtual const double GetXYTolerance();

     /// Gets the tolerance value for Z ordinates of the spatial context
    /// currently being read. 
    virtual const double GetZTolerance();

    /// Returns true if the spatial context currently being read is the active
    /// spatial context.
    virtual const bool IsActive();

    /// Returns true if the geometry column is derived and the spatial 
    /// context is the same as that of the base column. For example, a 
    /// geometry column in a view is derived from the corresponding column
    /// in the base table.
    /// 
    virtual const bool IsDerived();

    /// Get the next row and make it current.
	virtual bool ReadNext();

protected:

    /// Creates a set of rows describing the fields for this reader. 
    virtual FdoSmPhRowsP MakeRows( FdoSmPhMgrP mgr );

	FdoSmPhReaderP MakeQueryReader( FdoSmPhOwnerP owner, FdoStringsP objectNames );

private:
    FdoSmPhSqsOwnerP mOwner;
};

// convenient smart pointer
typedef FdoPtr<FdoSmPhRdSqsSpatialContextReader> FdoSmPhRdSqsSpatialContextReaderP;

#endif

