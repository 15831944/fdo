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
// std
#include <cassert>
#include <string>

namespace fdo { namespace postgis {

InsertCommand::InsertCommand(Connection* conn) :
    Base(conn),
    mClassIdentifier(NULL),
    mProperties(NULL),
    mBatchParameters(NULL)
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
    SchemaDescription::Ptr schemaDesc(SchemaDescription::Create());
    schemaDesc->DescribeSchema(mConn, NULL);

    FdoPtr<FdoClassDefinition> classDef(schemaDesc->FindClassDefinition(mClassIdentifier));
    if (!classDef) 
    {
        throw FdoCommandException::Create(L"[PostGIS] InsertCommand can not find class definition");
    }

    ov::ClassDefinition::Ptr phClass(schemaDesc->FindClassMapping(mClassIdentifier));

    if (NULL != mProperties)
    {
        std::string sep;
        std::string columns;
        std::string values;

        ExpressionProcessor::Ptr expProc(new ExpressionProcessor());

        FdoInt32 const propsSize = mProperties->GetCount();
        for (FdoInt32 i = 0; i < propsSize; i++)
        {
            FdoPtr<FdoPropertyValue> propVal(mProperties->GetItem(i));
            FdoPtr<FdoIdentifier> propId(propVal->GetName());

            columns += sep + static_cast<char const*>(FdoStringP(propId->GetName()));

            FdoPtr<FdoValueExpression> expr(propVal->GetValue());

            expr->Process(expProc);
            values += sep + expProc->ReleaseBuffer();

            sep = ",";
        }

        std::string tablePath(static_cast<char const*>(phClass->GetTablePath()));
        std::string sql("INSERT INTO " + tablePath + " (" + columns + ") VALUES (" + values + ")");

        FdoSize affected = 0;
        details::pgexec_params_t params;
        mConn->PgExecuteCommand(sql.c_str(), affected); //, params, affected);

    }

    return 0;
}

}} // namespace fdo::postgis

