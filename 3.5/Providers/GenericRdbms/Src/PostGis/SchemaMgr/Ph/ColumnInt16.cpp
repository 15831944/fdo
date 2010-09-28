//
// Copyright (C) 2006 Refractions Research, Inc.
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
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
#include "stdafx.h"
#include "ColumnInt16.h"

FdoSmPhPostGisColumnInt16::FdoSmPhPostGisColumnInt16(
    FdoStringP columnName,
    FdoSchemaElementState state,
    FdoSmPhDbObject* parentObject,
    bool isNullable,
    FdoStringP rootColumnName,
    FdoPtr<FdoDataValue> defaultValue,
    FdoSmPhRdColumnReader* reader)
    : FdoSmPhPostGisColumn(reader),
      FdoSmPhColumn(columnName, L"SMALLINT", state, parentObject,
        isNullable, rootColumnName, defaultValue)
{
    // idle
}

FdoSmPhPostGisColumnInt16::~FdoSmPhPostGisColumnInt16()
{
    // idle
}

int FdoSmPhPostGisColumnInt16::GetRdbType()
{
    return RDBI_SHORT;
}