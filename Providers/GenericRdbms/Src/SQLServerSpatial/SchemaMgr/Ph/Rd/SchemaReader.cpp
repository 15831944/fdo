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

#include "stdafx.h"
#include "SchemaReader.h"
#include "../Mgr.h"
#include "../../../../SchemaMgr/Ph/Rd/QueryReader.h"

FdoSmPhRdSqsSchemaReader::FdoSmPhRdSqsSchemaReader(FdoSmPhRowsP rows, FdoSmPhOwnerP owner, bool dsInfo) :
	FdoSmPhRdSchemaReader(rows, owner, dsInfo),
    mRows(rows),
    mOwner(owner)
{
    FdoStringP           sqlString;
    FdoStringP           ownerName = owner->GetName();
    FdoSmPhMgrP          mgr = owner->GetManager();
    FdoSmPhSqsMgr*       pMgr = (FdoSmPhSqsMgr*)(FdoSmPhMgr*)mgr;

    FdoSmPhReaderP reader;

//TODO: cache the queries for performance
    if ( !reader ) {

        // Single row, no joins
        FdoSmPhRowP userRow = new FdoSmPhRow(mgr, L"UserFields"); 

        // Each field adds itself to the row.
        FdoSmPhFieldP pField = new FdoSmPhField(
            userRow, 
            L"schemaname",
            userRow->CreateColumnDbObject(L"schemaname",false)
        );

        if ( owner->GetElementState() == FdoSchemaElementState_Added ) {
            // The DbObject query selects from owner.INFORMATION_SCHEMA tables so
            // it will fail when the owner does not yet exist. In this case create
            // an empty reader that returns no rows.
            reader = new FdoSmPhReader( mgr, rows );
        }
        else {
            // Generate sql statement if not already done
            //SqlServer> describe INFORMATION_SCHEMA.TABLES;
            //+-----------------+--------------+------+-----+---------+-------+
            //| Field           | Type         | Null | Key | Default | Extra |
            //+-----------------+--------------+------+-----+---------+-------+
            //| TABLE_CATALOG   | varchar(512) | YES  |     | NULL    |       |
            //| TABLE_SCHEMA    | varchar(64)  | NO   |     |         |       |
            //| TABLE_NAME      | varchar(64)  | NO   |     |         |       |
            //| TABLE_TYPE      | varchar(64)  | NO   |     |         |       |
            //+-----------------+--------------+------+-----+---------+-------+

            // Retrieve each distinct user that owns a table.
            sqlString = FdoStringP::Format(
                L"select distinct a.TABLE_SCHEMA collate latin1_general_bin as schemaname"
                L"  from %ls.INFORMATION_SCHEMA.TABLES a \n"
                L" where a.TABLE_CATALOG = %ls \n"
                L"  and a.TABLE_TYPE in ('BASE TABLE', 'VIEW') "
                L" order by a.TABLE_SCHEMA collate latin1_general_bin asc",
                (FdoString*)ownerName,
                (FdoString*)mgr->FormatSQLVal(ownerName,FdoSmPhColType_String)
            );

            reader = new FdoSmPhRdGrdQueryReader( userRow, sqlString, mgr );
        }
    }

    SetSubReader(reader);
}


FdoSmPhRdSqsSchemaReader::~FdoSmPhRdSqsSchemaReader(void)
{
}

bool FdoSmPhRdSqsSchemaReader::ReadNext()
{
    return FdoSmPhReader::ReadNext();
}

FdoStringP FdoSmPhRdSqsSchemaReader::GetString( FdoStringP tableName, FdoStringP fieldName )
{
    if ( fieldName == L"schemaname" )
        return FdoSmPhRdSchemaReader::GetString( tableName, fieldName );

    FdoSmPhFieldP field = mRows->GetField(tableName, fieldName);

    if ( !field ) 
        return FdoSmPhRdSchemaReader::GetString( tableName, fieldName );

    return field->GetFieldValue();
}

