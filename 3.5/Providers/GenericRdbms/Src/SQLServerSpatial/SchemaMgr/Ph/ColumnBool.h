#ifndef FDOSMPHSQSCOLUMNBOOL_H
#define FDOSMPHSQSCOLUMNBOOL_H       1
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

#ifdef _WIN32
#pragma once
#endif

#include <Inc/rdbi.h>
#include <Sm/Ph/ColumnBool.h>

// Represents a boolean column, using the TINYINT SqlServer type. 
class FdoSmPhSqsColumnBool :
    public FdoSmPhColumnBool, public FdoSmPhSqsColumn
{
public:
    // Create a boolean columntype.
    FdoSmPhSqsColumnBool(
        FdoStringP columnName,
        FdoSchemaElementState elementState,
        FdoSmPhDbObject* parentObject,
        bool bNullable,
        FdoStringP rootColumnName = L"",
		FdoPtr<FdoDataValue> defaultValue = (FdoDataValue*) NULL,
        FdoSmPhRdColumnReader* reader = NULL
    ) :
        FdoSmPhSqsColumn (reader),
        FdoSmPhColumn    ( columnName, L"BIT", elementState, parentObject, bNullable, rootColumnName, defaultValue)
    {}

    virtual ~FdoSmPhSqsColumnBool(void) {}

    virtual int GetRdbType()
    {
        return RDBI_LONG;
    }
};

typedef FdoPtr<FdoSmPhSqsColumnBool> FdoSmPhSqsColumnBoolP;

#endif