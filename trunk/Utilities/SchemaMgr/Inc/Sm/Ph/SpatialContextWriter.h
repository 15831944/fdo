#ifndef FDOSMPHSPATIALCONTEXTWRITER_H
#define FDOSMPHSPATIALCONTEXTWRITER_H		1
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

#ifdef _WIN32
#pragma once
#endif

#include <Sm/Ph/Writer.h>
#include <Sm/Ph/Mgr.h>
#include <Sm/Ph/Row.h>

// This class writes spatial context definitions 
// to the f_spatialcontext table.
class FdoSmPhSpatialContextWriter : public FdoSmPhWriter
{
public:

    /// Create a spatial context writer.
	//
    /// Parameters:
    /// 	mgr: holds the connection for the datastore to query.
	FdoSmPhSpatialContextWriter(FdoSmPhMgrP mgr);

	~FdoSmPhSpatialContextWriter(void);
    
    /// Various field value getters

    /// spatial context id
    FdoInt64 GetId();

    /// spatial context name
	FdoStringP GetName();

    /// spatial context description
	FdoStringP GetDescription();

    /// spatial context group id
    FdoInt64 GetGroupId();

    /// Corresponding setter functions. There's none for scid 
    /// since it is autogenerated when a new spatial context is written.

	void SetName( FdoStringP sValue );

	void SetDescription( FdoStringP sValue );

    /// spatial context group id
    void SetGroupId( FdoInt64 iValue);

    /// Add the current row as a new spatial context
	virtual void Add();

    /// Modify a spatial context using the current field values.
    //
    /// Parameters
    ///      scId: primary key of the spatial context to modify
    virtual void Modify( FdoInt64 scId );

    /// Delete a spatial context using the current field values.
    //
    /// Parameters
    ///      scId: primary key of the spatial context to delete
    virtual void Delete( FdoInt64 scId );

    /// Get a field list for reading from or writing to 
    /// f_spatialcontext.
	//
    /// Parameters:
    /// 	physicalSchema: holds the connection for the datastore to query.
    static FdoSmPhRowP MakeRow( FdoSmPhMgrP mgr );

protected:
    /// Unused constructor needed only to build on Linux
    FdoSmPhSpatialContextWriter() {}

    FdoSmPhWriterP MakeWriter( FdoSmPhMgrP mgr );
    
    void SetId( FdoInt64 lValue);
};

typedef FdoPtr<FdoSmPhSpatialContextWriter> FdoSmPhSpatialContextWriterP;

#endif


