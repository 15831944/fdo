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

#include <ShapeFile.h>
#include <ColumnInfo.h>

ShpDescribeSchemaCommand::ShpDescribeSchemaCommand (FdoIConnection *connection) :
    FdoCommonCommand<FdoIDescribeSchema, ShpConnection> (connection)
{
    mClassNames = NULL;
}

/** Do not implement the copy constructor. **/
//ShpDescribeSchemaCommand::ShpDescribeSchemaCommand (const ShpDescribeSchemaCommand &right) { }

ShpDescribeSchemaCommand::~ShpDescribeSchemaCommand (void)
{
    FDO_SAFE_RELEASE(mClassNames);
}

/** Do not implement the assignment operator. **/
//ShpDescribeSchemaCommand & operator= (const ShpDescribeSchemaCommand &right);

/// <summary>Gets the name of the schema to describe. This function is optional;
/// if not specified, execution of the command will describe all schemas.</summary>
/// <returns>Returns the schema name</returns> 
const wchar_t* ShpDescribeSchemaCommand::GetSchemaName ()
{
    return (mSchemaName);
}


/// <summary>Sets the name of the schema to describe. This function is optional; if not
/// specified, execution of the command will describe all schemas.</summary>
/// <param name="value">Input the schema name</param> 
/// <returns>Returns nothing</returns> 
void ShpDescribeSchemaCommand::SetSchemaName (const wchar_t* value)
{
    mSchemaName = value;
}


/// <summary>Gets the names of the classes to retrieve. This is optional,
/// if not specified execution of the command will describe all classes.
/// If the class name is not qualified, and the schema name is not specified,
/// the requested class from all schemas will be described.
/// The class names specified serve only as a hint.  Use of the hint
/// during command execution is provider dependent.  Providers that 
/// will not use the hint will describe the schema for all classes.</summary>
/// <returns>Returns the collection of class names</returns>
FdoStringCollection* ShpDescribeSchemaCommand::GetClassNames()
{
    return FDO_SAFE_ADDREF(mClassNames);
}

/// <summary>Sets the name of the classes to retrieve. This is optional, if not
/// specified execution of the command will describe all classes.
/// If the class name is not qualified, and the schema name is not specified,
/// the requested class from all schemas will be described.
/// The class names specified serve only as a hint.  Use of the hint
/// during command execution is provider dependent.  Providers that 
/// will not use the hint will describe the schema for all classes.</summary>
/// <param name="value">Input the collection of class names</parm>
/// <returns>Returns nothing</returns>
void ShpDescribeSchemaCommand::SetClassNames(FdoStringCollection* value)
{
    FDO_SAFE_RELEASE(mClassNames);
    mClassNames = FDO_SAFE_ADDREF(value);
}

/// <summary>Executes the DescribeSchema command and returns a 
/// FdoFeatureSchemaCollection. If a schema name is given that has 
/// references to another schema, the dependent schemas will 
/// be returned as well. If the specified schema name does not exist,
/// the Execute method throws an exception.</summary>
/// <returns>Returns the schema collection representing the schema created.</returns> 
FdoFeatureSchemaCollection* ShpDescribeSchemaCommand::Execute ()
{
    FdoPtr<FdoFeatureSchemaCollection> ret;

    // Get the Logical schema collection:
    FdoPtr<ShpConnection> shpConn = (ShpConnection*)GetConnection ();

    FdoPtr<FdoStringCollection> clsNames = NULL;
    if (NULL != mClassNames)
    {
        int cnt = mClassNames->GetCount();
        clsNames = FdoStringCollection::Create();
        for (int i = 0; i < cnt; i++)
        {
            FdoString* clsName = mClassNames->GetString(i);
            FdoStringP name = clsName;
            if (name.Contains(L":"))
            {
                name = name.Right(L":");
            }
            clsNames->Add(name);
        }
    }

    FdoPtr<ShpLpFeatureSchemaCollection> lpSchemas = shpConn->GetLpSchemas(clsNames);
    if (lpSchemas != NULL)
    {
        FdoPtr<FdoFeatureSchemaCollection> logicalSchemas = lpSchemas->GetLogicalSchemas();
        VALIDATE_POINTER(logicalSchemas);

        // handle missing schema better than the FdoCommonSchemaUtil message of FDO_4_UNREADY
        if (0 != mSchemaName.GetLength ())
        {
            FdoPtr<FdoFeatureSchema> schema = logicalSchemas->FindItem (mSchemaName);
            if (schema == NULL)
                throw FdoException::Create(NlsMsgGet(SHP_SCHEMA_NOT_FOUND, "Schema '%1$ls' not found.", (FdoString*)mSchemaName));
        }

        // Make a copy of the requested schema (or all schemas if no schemaname is provided), and return it:
        ret = FdoCommonSchemaUtil::DeepCopyFdoFeatureSchemas(logicalSchemas, mSchemaName);
        VALIDATE_POINTER(ret);
    }
    else
        if (0 != mSchemaName.GetLength ())
            throw FdoException::Create(NlsMsgGet(SHP_SCHEMA_NOT_FOUND, "Schema '%1$ls' not found.", (FdoString*)mSchemaName));
        else
            ret = FdoFeatureSchemaCollection::Create (NULL);

    //shpConn->GetLpSchemas() may return a cached copy. If the previous request was for a full schema and this one is partial
    //we need to whittle down the list of class definitions to match. If we're whittling down, it's a clone we're whittling
    //down, so no side-effects here.
    if (ret->GetCount() > 0 && NULL != clsNames && clsNames->GetCount() > 0)
    {
        FdoPtr<FdoFeatureSchema> schema = ret->GetItem(0);
        FdoPtr<FdoClassCollection> classes = schema->GetClasses();

        for (FdoInt32 i = classes->GetCount() - 1; i >= 0; i--)
        {
            FdoPtr<FdoClassDefinition> clsDef = classes->GetItem(i);
            FdoString* clsName = clsDef->GetName();
            if (clsNames->IndexOf(clsName) < 0)
            {
                //This class definition is not in the list of requested classes. Remove it.
                classes->RemoveAt(i);
            }
        }
    }

    return FDO_SAFE_ADDREF(ret.p);
}


