//
// Copyright (C) 2007 Refractions Research, Inc. 
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of version 2.1 of the GNU Lesser
// General Public License as published by the Free Software Foundation.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
#include "stdafx.h"

#include "PostGisProvider.h"
#include "ListDataStores.h"
#include "DataStoreReader.h"
#include "Connection.h"
#include "PgCursor.h"
// std
#include <cassert>

namespace fdo { namespace postgis {

ListDataStores::ListDataStores(Connection* conn) : Base(conn)
{
    FDOLOG_WRITE("ListDataStores command created");
}

ListDataStores::~ListDataStores()
{
}

bool ListDataStores::GetIncludeNonFdoEnabledDatastores()
{
    return mIncludeNonFdo;
}

void ListDataStores::SetIncludeNonFdoEnabledDatastores(bool includeNonFdo)
{
    FDOLOG_WRITE("Fetch non-FDO datastores = %d", includeNonFdo);
    
    mIncludeNonFdo = includeNonFdo;
}

FdoIDataStoreReader* ListDataStores::Execute()
{
    FDOLOG_MARKER("ListDataStores::+Execute");

    std::string cursorName("crsListDSCmd");
    std::string sql("SELECT ns.nspname AS schemaname, r.rolname AS ownername, "
                    "pg_catalog.obj_description(ns.oid, 'pg_namespace') AS description "
                    "FROM pg_catalog.pg_namespace ns "
                    "JOIN pg_catalog.pg_roles r ON ns.nspowner = r.oid "
                    "WHERE ns.nspname !~ \'^pg_\' "
                    "AND ns.nspname != \'information_schema\' "
                    "ORDER BY 1");
    
    // NOTE: Following may throw

    // Create a cursor associated with query fetching data stores 
    PgCursor::Ptr cursor = mConn->PgCreateCursor(cursorName.c_str());
    cursor->Declare(sql.c_str());

    // NOTE: The DataStoreReader constructor doesn't throw.

    // Cursor ownership is transfered to the reader
    return (new DataStoreReader(cursor));
}

}} // namespace fdo::postgis