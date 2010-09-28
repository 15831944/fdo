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
#include <Sm/Ph/SpatialIndex.h>

FdoSmPhSpatialIndex::FdoSmPhSpatialIndex(
        FdoStringP name, 
        FdoSmPhDbObject* pParent,
        bool isUnique,
		FdoSchemaElementState elementState
) : 
    FdoSmPhIndex( name, pParent, isUnique, elementState ),
    FdoSmPhDbObject( name, (const FdoSmPhOwner*)(pParent->GetParent()), elementState )
{
}

FdoSmPhSpatialIndex::~FdoSmPhSpatialIndex(void)
{
}

bool FdoSmPhSpatialIndex::GetIsPrimary()
{
    bool isPrimary = false;

    FdoStringP indexName = FdoStringP(GetName()).Lower();
    FdoSize nameLength = indexName.GetLength();

    // IsPrimary setting encoded in spatial context name; true if contains "_gsi_" 
    // or ends with "_gsi".
    if ( indexName.Contains(L"_gsi_") || (indexName.Mid(nameLength - 4, 4) == L"_gsi") ) 
        isPrimary = true;

    return isPrimary;
}

void FdoSmPhSpatialIndex::XMLSerialize( FILE* xmlFp, int ref ) const
{
	fprintf( xmlFp, "<SpatialIndex name=\"%s\" unique=\"%s\" table=\"%s\">\n",
			(const char*) FdoStringP(GetName()), 
            GetIsUnique() ? "true" : "false",
            GetDbObject() ? (const char*) FdoStringP(GetDbObject()->GetName()) : ""
	);

    FdoSmPhDbObject::XMLSerialize(xmlFp, ref);

    fprintf( xmlFp, "</SpatialIndex>\n" );
}

void FdoSmPhSpatialIndex::Discard()
{
    int idx;

    // Remove from table's cache
    FdoSmPhIndex::Discard();

    FdoSmPhColumnsP columns = GetColumns();

    // Unlink from its geometric column.
    for ( idx = 0; idx < columns->GetCount(); idx++ ) {
        FdoSmPhColumnGeomP geomCol = columns->GetItem(idx)->SmartCast<FdoSmPhColumnGeom>(true);

        if ( geomCol ) 
            geomCol->DiscardSpatialIndex(this);
    }
}
