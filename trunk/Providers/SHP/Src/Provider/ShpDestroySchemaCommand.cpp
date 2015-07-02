/*
 * 
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
#include "ShpSchemaUtilities.h"

#include <FdoCommonStringUtil.h>

ShpDestroySchemaCommand::ShpDestroySchemaCommand (FdoIConnection *connection) :
    FdoCommonCommand<FdoIDestroySchema, ShpConnection> (connection)
{
}

/** Do not implement the copy constructor. **/
//ShpDestroySchemaCommand::ShpDestroySchemaCommand (const ShpDestroySchemaCommand &right) { }

ShpDestroySchemaCommand::~ShpDestroySchemaCommand (void)
{
}

/** Do not implement the assignment operator. **/
//ShpDestroySchemaCommand & operator= (const ShpDestroySchemaCommand &right);

///<summary>Gets the name of the schema to destroy as a string.</summary>
/// <returns>Returns the name of the schema to destroy</returns> 
FdoString* ShpDestroySchemaCommand::GetSchemaName ()
{
    return (mSchemaName);
}

///<summary>Sets the name of the schema to destroy as a string.</summary>
/// <param name="value">Input the name of the schema to destroy</param> 
/// <returns>Returns nothing</returns> 
void ShpDestroySchemaCommand::SetSchemaName (FdoString* value)
{
    mSchemaName = value;
}

///<summary>Executes the destroy schema command, which removes the schema, class
///definitions, relation definitions, and all instance data from the DataStore.
///If elements in other schemas refer to the schema to be destroyed
///an exception is thrown.</summary>
/// <returns>Returns nothing</returns> 
void ShpDestroySchemaCommand::Execute ()
{
    FdoPtr<FdoFeatureSchema> schema = FdoFeatureSchema::Create (mSchemaName, L"");
    schema->Delete ();
    FdoPtr<FdoIApplySchema> apply = (FdoIApplySchema*)mConnection->CreateCommand (FdoCommandType_ApplySchema);
    apply->SetFeatureSchema (schema);
    apply->Execute ();
}

