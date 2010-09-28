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

#include "PostGisProvider.h"
#include "SelectAggregatesCommand.h"
#include "DataReader.h"
#include "FilterProcessor.h"
#include "Connection.h"
#include "PgCursor.h"
#include "PgUtility.h"
#include "PostGIS/FdoPostGisOverrides.h"
// std
#include <cassert>
#include <string>

namespace fdo { namespace postgis {

SelectAggregatesCommand::SelectAggregatesCommand(Connection* conn) :
    Base(conn),
    mOrderingOption(FdoOrderingOption_Ascending),
    mDistinct(false)
{
    // idle
}

SelectAggregatesCommand::~SelectAggregatesCommand()
{
}

///////////////////////////////////////////////////////////////////////////////
// FdoIDisposable interface
///////////////////////////////////////////////////////////////////////////////

void SelectAggregatesCommand::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
// FdoIBaseSelect interface
///////////////////////////////////////////////////////////////////////////////

FdoIdentifierCollection* SelectAggregatesCommand::GetPropertyNames()
{
    if (NULL == mProperties)
    {
        mProperties = FdoIdentifierCollection::Create();
    }

    FDO_SAFE_ADDREF(mProperties.p);
    return mProperties.p;
}
 	
FdoIdentifierCollection* SelectAggregatesCommand::GetOrdering()
{
    if (NULL == mOrderingProperties)
    {
        mOrderingProperties = FdoIdentifierCollection::Create();
    }

    FDO_SAFE_ADDREF(mOrderingProperties.p);
    return mOrderingProperties.p;
}

void SelectAggregatesCommand::SetOrderingOption(FdoOrderingOption option)
{
    FDOLOG_MARKER("SelectAggregatesCommand::+SetOrderingOption");
    FDOLOG_WRITE("Ordering: %d", option);

    mOrderingOption = option;
}

FdoOrderingOption SelectAggregatesCommand::GetOrderingOption()
{
    return mOrderingOption;
}

///////////////////////////////////////////////////////////////////////////////
// FdoISelectAggregates interface
///////////////////////////////////////////////////////////////////////////////

void SelectAggregatesCommand::SetDistinct(bool value)
{
    mDistinct = value;
}

bool SelectAggregatesCommand::GetDistinct()
{
    return mDistinct;
}

FdoIdentifierCollection* SelectAggregatesCommand::GetGrouping()
{
    if (NULL == mGroupingProperties)
    {
        mGroupingProperties = FdoIdentifierCollection::Create();
    }

    FDO_SAFE_ADDREF(mGroupingProperties.p);
    return mGroupingProperties.p;
}

void SelectAggregatesCommand::SetGroupingFilter(FdoFilter* filter)
{
    mGroupingFilter = filter;
    FDO_SAFE_ADDREF(mGroupingFilter.p);
}

FdoFilter* SelectAggregatesCommand::GetGroupingFilter()
{
    FDO_SAFE_ADDREF(mGroupingFilter.p);
    return mGroupingFilter.p;
}

FdoIDataReader* SelectAggregatesCommand::Execute()
{
    FDOLOG_MARKER("SelectAggregatesCommand::+Execute");

    try
    {
        //////////////////////////////////////////////////////////////
        // TODO: Refactor this big function into smaller procedures
        //////////////////////////////////////////////////////////////

        //
        // Collect schema details
        //

        // XXX - Eric Barby: Why not use mConn->DescribeSchema() / mConn->GetLogicalSchema()
        // and use mConn->GetPhysicalSchemaMapping() ?
        FdoFeatureSchemaCollection *logicalSchemas = mConn->GetLogicalSchema();
        if (!logicalSchemas) 
        {
            throw FdoCommandException::Create(L"[PostGIS] SelectCommand can not find schema definition");
        }

        FDOLOG_WRITE(L"Number of logical schemas: %d", logicalSchemas->GetCount());

        //
        // Get Feature Class definition
        //
        FdoPtr<FdoClassDefinition> classDef;
        FdoPtr<FdoIdentifier> classIdentifier = GetFeatureClassName();
        assert(NULL != classIdentifier);
        FdoStringP classId = classIdentifier->GetText();

        FDOLOG_WRITE(L"Logical schema name: %ls",
            static_cast<FdoString*>(classIdentifier->GetSchemaName()));
        FDOLOG_WRITE(L"Class name: %ls",
            static_cast<FdoString*>(classIdentifier->GetName()));

        // Find definition of the feature class
        FdoPtr<FdoIDisposableCollection> featureClasses;
        featureClasses = logicalSchemas->FindClass(classId);
        if (NULL == featureClasses)
        {
            FdoStringP msg =
                FdoStringP::Format(L"Logical schema for '%ls' not found", 
                static_cast<FdoString*>(classId));
            FDOLOG_WRITE("ERROR: %s", static_cast<char const*>(msg));
            throw FdoCommandException::Create(msg);
        }

        FDOLOG_WRITE(L"Number of feature schemas: %d", featureClasses->GetCount());

        if (featureClasses->GetCount() <= 0)
        {
            FdoStringP msg =
                FdoStringP::Format(L"No class definition found for schema '%ls'", 
                static_cast<FdoString*>(classId));
            FDOLOG_WRITE("ERROR: %s", static_cast<char const*>(msg));
            throw FdoCommandException::Create(msg);
        }

        // We have a single-element collection here
        classDef = static_cast<FdoClassDefinition*>(featureClasses->GetItem(0));
        assert(NULL != classDef);
        FdoStringP className = classDef->GetName();

        FDOLOG_WRITE(L"Class definition for: %ls", static_cast<FdoString*>(className));

        //
        // Get Physical Schema details
        //
        ov::PhysicalSchemaMapping::Ptr schemaMapping;
        schemaMapping = mConn->GetPhysicalSchemaMapping();

        FDOLOG_WRITE(L"Schema mapping for: '%ls'", schemaMapping->GetName());

        ov::ClassDefinition::Ptr phClassDef;
        phClassDef = schemaMapping->FindByClassName(className);
        if (NULL == phClassDef)
        {
            FdoStringP msg =
                FdoStringP::Format(L"Physical schema for '%ls' not found", 
                static_cast<FdoString*>(className));
            FDOLOG_WRITE("ERROR: %s", static_cast<char const*>(msg));
            throw FdoCommandException::Create(msg);
        }

        // NOTE: Schema and table names are wrapped with double quotes
        std::string tablePath(static_cast<char const*>(phClassDef->GetTablePath()));

        //
        // Build SELECT clause
        //
        std::string sep;
        std::string sqlSelect("");
        FdoPtr<FdoPropertyDefinitionCollection> props = classDef->GetProperties();
        FdoInt32 currentSrid = GetSRID(props);

        FilterProcessor::Ptr filterProc(new FilterProcessor());
        ExpressionProcessor::Ptr exprProc(new ExpressionProcessor());

        FDOLOG_WRITE("Number of properties: %d", mProperties->GetCount());

        // TODO: What should we do if mProperties is NULL?
        FdoInt32 const propsSize = mProperties->GetCount();

        for (FdoInt32 propIdx = 0; propIdx < propsSize; propIdx++)
        {
            FdoPtr<FdoIdentifier> id(mProperties->GetItem(propIdx));

            if (NULL != dynamic_cast<FdoComputedIdentifier*>(id.p))
            {
                // Parse complex computed expression and identifier

                FdoComputedIdentifier* compId = NULL;
                compId = dynamic_cast<FdoComputedIdentifier*>(id.p);

                FdoStringP name(compId->GetName());

                FdoPtr<FdoExpression> expr(compId->GetExpression());
                if (NULL != dynamic_cast<FdoFunction*>(expr.p))
                {
                    // Parse function: FOO( argument ) AS alias

                    FdoFunction* func = dynamic_cast<FdoFunction*>(expr.p);               
                    func->Process(exprProc);
                    sqlSelect.append(sep + exprProc->ReleaseBuffer() + " AS ");
                    sqlSelect.append(static_cast<char const*>(name));
                }
                else
                {
                    // Parse other types like arithmetic expressions, etc.

                    expr->Process(exprProc);
                    sqlSelect.append(sep + exprProc->ReleaseBuffer() + " AS ");
                    sqlSelect.append(static_cast<char const*>(name));
                }
            }
            else
            {
                // Parse simple name identifier

                FdoStringP name(id->GetName());
                sqlSelect.append(sep + static_cast<char const*>(name));
            }

            sep = ",";
        }

        //
        // Build GROUP BY clause
        //
        sep.clear();
        std::string sqlGroupBy;

        if (NULL != mGroupingProperties)
        {
            FdoInt32 const groupingSize = mGroupingProperties->GetCount();

            for (FdoInt32 i = 0; i < groupingSize; i++)
            {
                FdoPtr<FdoIdentifier> id(mGroupingProperties->GetItem(i));

                FdoStringP name = id->GetName();
                sqlGroupBy.append(sep + static_cast<char const*>(name));

                sep = ",";
            }
        }

        //
        // Build WHERE or HAVING clause
        //
        std::string sqlWhere;
        std::string sqlHaving;

        //
        // Process filter conditions
        //
        if (NULL != mFilter)
        {
            FDOLOG_WRITE("Configuring filter");

            FilterProcessor::Ptr proc(new FilterProcessor(currentSrid));
            mFilter->Process(proc);
            sqlWhere = proc->GetFilterStatement();
        }

        //
        // Compile final SQL query
        //
        assert(!sqlSelect.empty());

        std::string sql("SELECT ");

        if(mDistinct)
        {
          sql.append(" DISTINCT ");
        }

        sql.append(sqlSelect);

        sql.append(" FROM " + tablePath);

        if (!sqlWhere.empty())
        {
            sql.append(" WHERE " + sqlWhere);
        }

        if (!sqlGroupBy.empty())
        {
            sql.append(" GROUP BY " + sqlGroupBy);
        }

        if (!sqlHaving.empty())
        {
            sql.append(" HAVING " + sqlHaving);
        }    

        // 3 - ORDER BY clause
        FdoPtr<FdoIdentifierCollection> orderingColumns = GetOrdering();
        FdoInt32 const orderingCount = orderingColumns->GetCount();
        if (orderingCount > 0)
        {
            std::string sep("");
            sql.append(" ORDER BY ");

            for (FdoInt32 i = 0; i < orderingCount; ++i)
            {
                FdoPtr<FdoIdentifier> id(orderingColumns->GetItem(i));
                FdoStringP name(id->GetName());

                sql.append(sep + static_cast<char const*>(name));

                if (FdoOrderingOption_Ascending == GetOrderingOption())
                    sql.append(" ASC ");
                else
                    sql.append(" DESC ");

                sep = ",";
            }
        }

        //
        // Declare cursor and create feature reader
        //
        FDOLOG_WRITE("Creating cursor: crsSelAggCmd");

        PgCursor::Ptr cursor;
        cursor = mConn->PgCreateCursor("crsSelAggCmd");
        assert(NULL != cursor);

        // Collect bind parameters
        details::pgexec_params_t params;
        Base::PgGenerateExecParams(params);

        // Open new cursor
        cursor->Declare(sql.c_str(), params);

        DataReader::Ptr reader(new DataReader(mConn, cursor));

        FDO_SAFE_ADDREF(reader.p);
        return reader.p;
    }
    catch (FdoException* e)
    {
        FDOLOG_WRITE("ERROR: The execution of Select Aggregates command failed.");

        FdoCommandException* ne = NULL;
        ne = FdoCommandException::Create(NlsMsgGet(MSG_POSTGIS_COMMAND_SELECT_FAILED,
            "The execution of Select Aggregates command failed."), e);
        e->Release();
        throw ne;
    }
}

}} // namespace fdo::postgis