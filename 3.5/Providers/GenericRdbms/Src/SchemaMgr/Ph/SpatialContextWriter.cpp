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
#include "SpatialContextWriter.h"

FdoSmPhGrdSpatialContextWriter::FdoSmPhGrdSpatialContextWriter(FdoSmPhMgrP mgr) : 
	FdoSmPhSpatialContextWriter( mgr )
{
}

FdoSmPhGrdSpatialContextWriter::~FdoSmPhGrdSpatialContextWriter(void)
{
}

void FdoSmPhGrdSpatialContextWriter::Add()
{
    FdoSmPhGrdMgrP mgr = GetManager().p->SmartCast<FdoSmPhGrdMgr>();
    GdbiConnection* conn = mgr->GetGdbiConnection();
    GdbiCommands* cmds = conn->GetCommands();
    FdoInt64      SpatialContextId;

	// Autogenerate the ID for the new SpatialContext...
    if ( cmds->IsFieldWriteable( L"f_spatialcontext", L"scid" ) )
    {
        // In advance for providers supporting sequences
        SpatialContextId = cmds->NextSequenceNumber( DBI_PLAN_SEQUENCEW );
        SetId( SpatialContextId );
        FdoSmPhSpatialContextWriter::Add();
    }
    else
    {
        // After insert for the autoincrement ones
        FdoSmPhSpatialContextWriter::Add();
        SpatialContextId = cmds->NextSequenceNumber( DBI_PLAN_SEQUENCEW );
        SetId( SpatialContextId );
    }
}
