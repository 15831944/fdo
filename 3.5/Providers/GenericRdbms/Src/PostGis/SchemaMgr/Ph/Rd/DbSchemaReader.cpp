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
#include "DbSchemaReader.h"
#include "../Mgr.h"
#include "../../../../SchemaMgr/Ph/Rd/QueryReader.h"
#include <Sm/Ph/Rd/DbObjectBinds.h>

FdoSmPhRdPostGisDbSchemaReader::FdoSmPhRdPostGisDbSchemaReader(
    FdoSmPhOwnerP owner,
    FdoStringP schemaName
    ) :
	FdoSmPhReader()
{
    SetSubReader(MakeQueryReader(owner,schemaName));
}


FdoSmPhRdPostGisDbSchemaReader::~FdoSmPhRdPostGisDbSchemaReader(void)
{
}

FdoSmPhRowsP FdoSmPhRdPostGisDbSchemaReader::MakeRows( FdoSmPhMgrP mgr )
{
    // Call superclass to populate generic row/columns:
    FdoSmPhRowsP rows = new FdoSmPhRowCollection();
        
    FdoSmPhRowP row = new FdoSmPhRow( mgr, L"DbSchemaFields" );
    rows->Add( row );

    // Each field adds itself to the row.
    FdoSmPhFieldP pField;
    pField = new FdoSmPhField(row, L"schema_name", row->CreateColumnDbObject(L"schema_name", false));

    return( rows);
}


FdoSmPhReaderP FdoSmPhRdPostGisDbSchemaReader::MakeQueryReader(
    FdoSmPhOwnerP owner,
    FdoStringP schemaName
)
{
    FdoStringP           sqlString;
    FdoStringP           ownerName = owner->GetName();
    FdoSmPhMgrP          mgr = owner->GetManager();
    FdoSmPhPostGisMgr*       pMgr = (FdoSmPhPostGisMgr*)(FdoSmPhMgr*)mgr;

    FdoSmPhReaderP reader;

//TODO: cache the queries for performance
/*
    if ( object_set )
        reader = pMgr->GetDbObjectReader(dblink_set);
    else
        reader = pMgr->GetDbObjectsReader(dblink_set);
*/
    if ( !reader ) {
        FdoSmPhRowsP rows = MakeRows( mgr );
        FdoSmPhRowP row = rows->GetItem(0);

        if ( owner->GetElementState() == FdoSchemaElementState_Added ) {
            // Owner is new so there is nothing to retrieve
            reader = new FdoSmPhReader( mgr, rows );
        }
        else {
            // Generate sql statement if not already done
            sqlString = FdoStringP(L"SELECT DISTINCT t.nspname as schema_name FROM  pg_catalog.pg_namespace t WHERE substr(t.nspname,1,3) <> 'pg_' AND t.nspname <>'information_schema' ORDER BY 1");

            reader = new FdoSmPhRdGrdQueryReader(row, sqlString, mgr, NULL );
        }
    }
    else {
        // Re-executing so update bind variables first.
        FdoSmPhRdGrdQueryReader* pReader = dynamic_cast<FdoSmPhRdGrdQueryReader*>((FdoSmPhReader*) reader);

        // pReader is NULL when it is an empty reader. No need to re-execute in this case.
        if ( pReader ) {
		    FdoSmPhRowP binds = pReader->GetBinds();

		    if ( schemaName != L"" ) {
			    FdoSmPhFieldsP	fields = binds->GetFields();
			    FdoSmPhFieldP(fields->GetItem(L"schema_name"))->SetFieldValue(schemaName);
		    }

            pReader->Execute();
        }
    }

    return reader;
}

FdoSmPhRowP FdoSmPhRdPostGisDbSchemaReader::MakeBinds( FdoSmPhMgrP mgr, FdoStringP schemaName )
{
    FdoSmPhRowP row = new FdoSmPhRow( mgr, L"Binds" );
    FdoSmPhDbObjectP rowObj = row->GetDbObject();

    if ( schemaName.GetLength() > 0 ) {
        FdoSmPhFieldP field = new FdoSmPhField(
            row, 
            L"schema_name",
            rowObj->CreateColumnDbObject(L"schema_name",false)
        );

        field->SetFieldValue(schemaName); 
    }

    return( row );
}
