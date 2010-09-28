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
#include "Class.h"

FdoSmLpOdbcClass::FdoSmLpOdbcClass(FdoSmPhClassReaderP classReader, FdoSmLpSchemaElement* parent) :
    FdoSmLpClass(classReader, parent),
    FdoSmLpOdbcClassDefinition(classReader, parent),
    FdoSmLpClassBase(classReader, parent)
{
}

FdoSmLpOdbcClass::FdoSmLpOdbcClass(
    FdoClass* pFdoClass, 
    bool bIgnoreStates,
    FdoSmLpSchemaElement* parent
) :
    FdoSmLpClass(pFdoClass, bIgnoreStates, parent),
    FdoSmLpOdbcClassDefinition(pFdoClass, bIgnoreStates, parent),
    FdoSmLpClassBase(pFdoClass, bIgnoreStates, parent)
{
}

FdoSmLpOdbcClass::~FdoSmLpOdbcClass()
{
}

void FdoSmLpOdbcClass::Update(
    FdoClassDefinition* pFdoClass,
    FdoSchemaElementState elementState,
    FdoPhysicalClassMapping* pClassOverrides,
    bool bIgnoreStates
)
{
    // Apply the schema overrides first since these are needed to update properties.
    FdoSmLpOdbcClassDefinition::Update( pFdoClass, elementState, pClassOverrides, bIgnoreStates );
    // Do Generic feature class updates next
    FdoSmLpClass::Update( pFdoClass, elementState, pClassOverrides, bIgnoreStates );
}

FdoSmPhClassWriterP FdoSmLpOdbcClass::GetPhysicalAddWriter()
{
    FdoSmPhClassWriterP writer = FdoSmLpClassDefinition::GetPhysicalAddWriter();
    FdoSmLpOdbcClassDefinition::SetPhysicalAddWriter( writer );

    return writer;
}

void FdoSmLpOdbcClass::PostFinalize()
{
    FdoSmLpClassBase::PostFinalize();
    FdoSmLpOdbcClassDefinition::PostFinalize();
}