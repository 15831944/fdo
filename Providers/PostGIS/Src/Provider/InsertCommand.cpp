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

#include "InsertCommand.h"
#include "Connection.h"
#include "ExpressionProcessor.h"
#include "PgUtility.h"
// std
#include <cassert>
#include <string>

namespace fdo { namespace postgis {

InsertCommand::InsertCommand(Connection* conn) :
    Base(conn)
{
    // idle
}

InsertCommand::~InsertCommand()
{
    // idle
}

///////////////////////////////////////////////////////////////////////////////
// FdoIInsert interface
///////////////////////////////////////////////////////////////////////////////

FdoIdentifier* InsertCommand::GetFeatureClassName()
{
    FDO_SAFE_ADDREF(mClassIdentifier.p);
    return mClassIdentifier.p;
}
    
void InsertCommand::SetFeatureClassName(FdoIdentifier* classIdentifier)
{
    // TODO: Currently, when we use common schema for all feature classes - FdoPostGIS
    // it's safe to make FdoPostGIS lower-case because the PostGIS provider
    // does not use it in comparison, searching for classes, etc. operations.

    FdoPtr<FdoIdentifier> newIdentifier;
    if (NULL != classIdentifier)
    {
        FdoStringP schema(classIdentifier->GetSchemaName());
        FdoStringP name(classIdentifier->GetName());
        FdoStringP tmp(schema);
        if (tmp.GetLength() > 0)
            tmp += L":";
        tmp += name.Lower();

        newIdentifier = FdoIdentifier::Create(tmp);
    }

    mClassIdentifier = newIdentifier;
    FDO_SAFE_ADDREF(mClassIdentifier.p);
}
    
void InsertCommand::SetFeatureClassName(FdoString* className)
{
    FdoPtr<FdoIdentifier> cid;
    if (NULL != className)
        cid = FdoIdentifier::Create(className);
    else
        cid = NULL;

    SetFeatureClassName(cid);
}
    
FdoPropertyValueCollection* InsertCommand::GetPropertyValues()
{
    if (NULL == mProperties)
    {
        mProperties = FdoPropertyValueCollection::Create();
        assert(NULL != mProperties);
    }

    FDO_SAFE_ADDREF(mProperties.p);
    return mProperties.p;
}
    
FdoBatchParameterValueCollection* InsertCommand::GetBatchParameterValues()
{
    return GetCollection(mBatchParameters);
}
    
FdoIFeatureReader* InsertCommand::Execute()
{
    FDOLOG_MARKER("InsertCommand::+Execute");
    assert(NULL != mConn);

    if (NULL == mProperties)
    {
        return NULL;
    }

    //
    // Collect schema details required to build SQL command
    //
    
    // NOTE - Eric Barby: faster with mConn->DescribeSchema() , uses cache
    // XXX - mloskot: Why not wrapped with FdoPtr, SchemaDescription::Ptr?
    SchemaDescription*  schemaDesc = mConn->DescribeSchema();
    if (!schemaDesc || !schemaDesc->IsDescribed()) 
    {
        throw FdoCommandException::Create(L"[PostGIS] InsertCommand can not find schema definition");
    }

    FdoPtr<FdoClassDefinition> classDef(schemaDesc->FindClassDefinition(mClassIdentifier));
    ov::ClassDefinition::Ptr phClass(schemaDesc->FindClassMapping(mClassIdentifier));
    if (!classDef || !phClass) 
    {
        throw FdoCommandException::Create(L"[PostGIS] InsertCommand can not find class definition or class mapping!");
    }


    std::string sep;
    std::string columns;
    std::string values;
    std::string sequence;

    ExpressionProcessor::Ptr expProc(new ExpressionProcessor());

    //
    // Check auto-generated PRIMARY KEY and configure sequence
    //
    FdoStringP pkColumn;
    FdoPtr<FdoPropertyDefinitionCollection>     propsDefinition(classDef->GetProperties());
    FdoPtr<FdoDataPropertyDefinitionCollection> propsIdentity(classDef->GetIdentityProperties());
    FdoInt32 currentSrid = GetSRID(propsDefinition);
    if (1 == propsIdentity->GetCount())
    {
        FdoPtr<FdoDataPropertyDefinition> prop(propsIdentity->GetItem(0));
        if (prop->GetIsAutoGenerated()
            && (FdoDataType_Int16 == prop->GetDataType() 
            || FdoDataType_Int32 == prop->GetDataType()
            || FdoDataType_Int64 == prop->GetDataType()))
        {
            pkColumn = prop->GetName();

            std::string table(static_cast<char const*>(phClass->GetTableName()));
            std::string column(static_cast<char const*>(pkColumn));
            sequence = details::MakeSequenceName(table, column);
        }
    }

    //
    // Collect column & value pairs
    //
    FdoInt32 const propsSize = mProperties->GetCount();
    for (FdoInt32 i = 0; i < propsSize; i++)
    {
        FdoPtr<FdoPropertyValue> propVal(mProperties->GetItem(i));
        FdoStringP   pName(propVal->GetName()->GetName());
        FdoPtr<FdoPropertyDefinition> propDef(GetPropDefinition(propsDefinition, pName));
        if (propDef)
        {
            pName = propDef->GetName();
        }
        else
        {
            FDOLOG_WRITE(L"can not find porpertyDefinition '%s'",
                static_cast<FdoString*>(propVal->GetName()->GetName()));        
        }

        columns += sep + static_cast<char const*>(pName);

        if (0 == pkColumn.ICompare(pName))
        {
            values += sep + "nextval(\'" + sequence + "\')";
        }
        else
        {
            FdoValueExpression *expr=propVal->GetValue();
            expr->Process(expProc);
            std::string value;
            if (propDef)
            {
                if (FdoPropertyType_DataProperty == propDef->GetPropertyType())
                {
                    FdoDataPropertyDefinition* dataDef = static_cast<FdoDataPropertyDefinition*>(propDef.p);


                    // NOTE - Eric Barby: Need to parse date properly
                    // XXX - mloskot: Is the code below the solution?
                    if (FdoDataType_DateTime == dataDef->GetDataType())
                    {
                        FdoDateTimeValue *dateValuePtr=dynamic_cast<FdoDateTimeValue*>(propVal->GetValue());
                        if (!dateValuePtr->IsNull())
                        {
                            FDOLOG_WRITE(L"convert Date:", dateValuePtr->ToString());
                            value = static_cast<char const*>(FdoStringP(dateValuePtr->ToString()));
                        }
                        expProc->ReleaseBuffer();
                    }
                    else
                    {
                        value = expProc->ReleaseBuffer();
                    }
                } 
                else
                {
                    if (FdoPropertyType_GeometricProperty == propDef->GetPropertyType())
                    {
                        if (currentSrid != -1) 
                        {
                            value = str(boost::format("setsrid(Geometry(%s),%d)")
                                % expProc->ReleaseBuffer() % currentSrid);
                        }
                        else
                        {
                            value = expProc->ReleaseBuffer();
                        }
                    }
                }
            }
            values += sep + value;            
        }
        sep = ",";
    }

    //
    // Build and execute SQL command
    //
    std::string tablePath(static_cast<char const*>(phClass->GetTablePath()));
    std::string pkValue("nextval(\'" + sequence + "\'");
    std::string sql("INSERT INTO " + tablePath + " (" + columns + ") VALUES (" + values + ")");

    FdoSize affected = 0;
    mConn->PgExecuteCommand(sql.c_str(), affected);

    // TODO: What are we supposed to send out from here?
    return NULL;
}

}} // namespace fdo::postgis

