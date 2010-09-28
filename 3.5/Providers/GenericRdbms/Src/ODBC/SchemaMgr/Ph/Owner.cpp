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
#include "Owner.h"
#include "Table.h"
#include "View.h"
#include "Mgr.h"
#include "Rd/DbObjectReader.h"
#include "Rd/OraDbObjectReader.h"
#include "Rd/OraBaseObjectReader.h"
#include "Rd/OraPkeyReader.h"
#include "Rd/ConstraintReader.h"
#include "Inc/Rdbi/proto.h"

FdoSmPhOdbcOwner::FdoSmPhOdbcOwner(
    FdoStringP name,
    bool hasMetaSchema,
    const FdoSmPhDatabase* pDatabase,
    FdoSchemaElementState elementState
) :
    FdoSmPhGrdOwner(name, hasMetaSchema, pDatabase, elementState)
{
}

FdoSmPhOdbcOwner::~FdoSmPhOdbcOwner(void)
{
}

void FdoSmPhOdbcOwner::SetCurrent()
{
    FdoSmPhOdbcMgrP mgr = GetManager()->SmartCast<FdoSmPhOdbcMgr>();
    rdbi_context_def* mContext = mgr->GetRdbiContext();
    FdoStringP pName = GetName();
    int rdbi_status = RDBI_GENERIC_ERROR;
    if (mContext->dispatch.capabilities.supports_unicode == 1)
        rdbi_status = ::rdbi_set_schemaW(mContext, pName);
    else
        rdbi_status = ::rdbi_set_schema(mContext, pName);
    
    if ( RDBI_SUCCESS != rdbi_status )
    {
        rdbi_get_msg(mgr->GetRdbiContext()); 
        throw FdoSchemaException::Create( mgr->GetRdbiContext()->last_error_msg ); 
    }
}

FdoSmPhDbObjectP FdoSmPhOdbcOwner::NewTable(
    FdoStringP tableName,
    FdoSchemaElementState elementState,
    FdoSmPhRdDbObjectReader* reader
)
{
    return new FdoSmPhOdbcTable(tableName, this, elementState, L"", reader);
}

FdoSmPhDbObjectP FdoSmPhOdbcOwner::NewView(
    FdoStringP viewName,
    FdoStringP rootDatabase,
    FdoStringP rootOwner,
    FdoStringP rootObjectName,
    FdoSchemaElementState elementState,
    FdoSmPhRdDbObjectReader* reader
)
{
    return new FdoSmPhOdbcView(viewName, rootDatabase, rootOwner, rootObjectName, this, elementState, reader);
}

FdoPtr<FdoSmPhRdDbObjectReader> FdoSmPhOdbcOwner::CreateDbObjectReader( FdoStringP dbObject) const
{
    FdoSmPhOdbcOwner* pOwner = (FdoSmPhOdbcOwner*) this;

    FdoSmPhOdbcMgrP mgr = pOwner->GetManager()->SmartCast<FdoSmPhOdbcMgr>();

    rdbi_vndr_info_def info;
	rdbi_vndr_info( mgr->GetRdbiContext(), &info );

    if( info.dbversion == RDBI_DBVERSION_ODBC_ORACLE )
        return new FdoSmPhRdOraOdbcDbObjectReader( FDO_SAFE_ADDREF(pOwner), dbObject );
    else
       return new FdoSmPhRdOdbcDbObjectReader( FDO_SAFE_ADDREF(pOwner), dbObject );
}

FdoPtr<FdoSmPhRdBaseObjectReader> FdoSmPhOdbcOwner::CreateBaseObjectReader() const
{
    FdoSmPhOdbcOwner* pOwner = (FdoSmPhOdbcOwner*) this;
    FdoSmPhOdbcMgrP mgr = pOwner->GetManager()->SmartCast<FdoSmPhOdbcMgr>();
    rdbi_vndr_info_def info;
	rdbi_vndr_info( mgr->GetRdbiContext(), &info );
    
    // The Oracle primary key reader use bulk load to load all the primary keys.
    if( info.dbversion == RDBI_DBVERSION_ODBC_ORACLE )
        return new FdoSmPhRdOdbcOraBaseObjectReader( FDO_SAFE_ADDREF(pOwner) );
    else
        return (FdoSmPhRdBaseObjectReader *)NULL;
}

FdoPtr<FdoSmPhRdPkeyReader> FdoSmPhOdbcOwner::CreatePkeyReader() const
{
    FdoSmPhOdbcOwner* pOwner = (FdoSmPhOdbcOwner*) this;
    FdoSmPhOdbcMgrP mgr = pOwner->GetManager()->SmartCast<FdoSmPhOdbcMgr>();
    rdbi_vndr_info_def info;
	rdbi_vndr_info( mgr->GetRdbiContext(), &info );
    
    // The Oracle primary key reader use bulk load to load all the primary keys.
    if( info.dbversion == RDBI_DBVERSION_ODBC_ORACLE )
        return new FdoSmPhRdOraOdbcPkeyReader( pOwner->GetManager(), FDO_SAFE_ADDREF(pOwner) );
    else
        return (FdoSmPhRdPkeyReader*) NULL;
}

FdoPtr<FdoSmPhRdConstraintReader> FdoSmPhOdbcOwner::CreateConstraintReader( FdoStringP constraintName) const
{
    FdoSmPhOdbcOwner* pOwner = (FdoSmPhOdbcOwner*) this;

    return new FdoSmPhRdOdbcConstraintReader( FDO_SAFE_ADDREF(pOwner), constraintName );
}

FdoPtr<FdoSmPhRdConstraintReader> FdoSmPhOdbcOwner::CreateConstraintReader( FdoStringP tableName, FdoStringP constraintType) const
{
    FdoSmPhOdbcOwner* pOwner = (FdoSmPhOdbcOwner*) this;

    return new FdoSmPhRdOdbcConstraintReader( FDO_SAFE_ADDREF(pOwner), tableName, constraintType );
}

FdoStringP FdoSmPhOdbcOwner::GetBestSchemaName() const
{
    return ( wcslen(GetName()) > 0 ) ? 
        FdoStringP(GetName())
// TODO: It is currently not possible to bulk copy
// a schema named "Default" to the Shape Provider.
// In the meantime, default the schema name to "Fdo".
//        : L"Default";
        : FdoSmPhMgr::RdSchemaPrefix;
}

bool FdoSmPhOdbcOwner::Add()
{
    FdoSmPhOdbcMgrP mgr = GetManager()->SmartCast<FdoSmPhOdbcMgr>();
    GdbiConnection* gdbiConn = mgr->GetGdbiConnection();

    FdoStringP sqlStmt = FdoStringP::Format(
        L"create database %ls",
        GetName()
    );

    // Create the owner (datastore)
    gdbiConn->ExecuteNonQuery( (const char*) sqlStmt );

    if ( GetHasMetaSchema() ) {
        FdoStringsP keywords = FdoStringCollection::Create();
        keywords->Add( rdbi_vndr_name(mgr->GetRdbiContext()) );
        keywords->Add( L"SQLServer" );

        // The following keywords cause the long transaction and locking
        // system properties to be added and inherited by each class table.
        if ( GetLtMode() == FdoMode )
            keywords->Add( L"FdoLt" );

        if ( GetLckMode() == FdoMode )
            keywords->Add( L"FdoLock" );

        // Switch to newly created owner and add the MetaSchema

        SetCurrent();

        try {
            AddMetaSchema( keywords, GetIsSystem());
        }
        catch ( ... ) {
            try {
                FdoSmPhOwnerP prevOwner = mgr->FindOwner();
                
                if ( prevOwner )
                    prevOwner->SetCurrent();
            }
            catch (...) {
            }
            throw;
        }

        // Switch back to default owner
        FdoSmPhOwnerP prevOwner = mgr->FindOwner();
                
        if ( prevOwner && FdoStringP(prevOwner->GetName()).GetLength() != 0)
            prevOwner->SetCurrent();

        //TODO: spatial handling?
    }
    
    return true;
}

bool FdoSmPhOdbcOwner::Delete()
{
    FdoSmPhOdbcMgrP mgr = GetManager()->SmartCast<FdoSmPhOdbcMgr>();
    GdbiConnection* gdbiConn = mgr->GetGdbiConnection();

    FdoSmPhOwnerP currOwner = mgr->FindOwner();

    // Do use master when the datastore to delete is the current one. 
    if ( currOwner && FdoStringP(currOwner->GetName()).ICompare(GetName()) == 0 ) 
        gdbiConn->ExecuteNonQuery( "USE master", TRUE );

    FdoStringP sqlStmt = FdoStringP::Format(
        L"drop database %ls",
        GetName()
    );

    gdbiConn->ExecuteNonQuery( (const wchar_t*) sqlStmt, TRUE);

    return true;
}

void FdoSmPhOdbcOwner::SetOptions()
{
	bool validOption;

	// validate LT method
	validOption = (GetLtMode() == NoLtLock || GetLtMode() == FdoMode);
	if (validOption == false)	
		SetLtMode(FdoMode);
	FdoStringP tempValue = FdoStringP::Format(L"%d", (int)GetLtMode());
	FdoStringP sql_stmt = FdoStringP::Format(L"update F_OPTIONS set value = '%ls' where name = 'LT_MODE'",
							 (FdoString *) tempValue);

	FdoSmPhOdbcMgrP mgr = GetManager()->SmartCast<FdoSmPhOdbcMgr>();
    GdbiConnection* gdbiConn = mgr->GetGdbiConnection();
	gdbiConn->ExecuteNonQuery( (const char*) sql_stmt );

	// validate locking method
	validOption = (GetLckMode() == NoLtLock || GetLckMode() == FdoMode);
	if (validOption == false)	
		SetLckMode(NoLtLock);
	tempValue = FdoStringP::Format(L"%d", (int)GetLckMode());
	sql_stmt = FdoStringP::Format(L"update F_OPTIONS set value = '%ls' where name ='LOCKING_MODE'",
							(FdoString *) tempValue);

	gdbiConn->ExecuteNonQuery( (const char*) sql_stmt );

}

FdoString* FdoSmPhOdbcOwner::GetDbNameClause(bool isEqual)
{
	FdoStringP schemaCond = FdoStringP::Format(L"schemaname %ls '%ls'",
												isEqual ? L"=" : L"!=",
												GetName());
	return (FdoString*) schemaCond;
}

void FdoSmPhOdbcOwner::CreateMetaClass()
{
	FdoSmPhOdbcMgrP mgr = GetManager()->SmartCast<FdoSmPhOdbcMgr>();
    GdbiConnection* gdbiConn = mgr->GetGdbiConnection();
	FdoStringP sql_stmt;

	sql_stmt = FdoStringP::Format(L"insert into f_classdefinition "
			L"(classname,schemaname,tablename,classtype,description,isabstract,parentclassname,istablecreator,isfixedtable,hasversion,haslock) "
			L"values ('Class', 'F_MetaClass', ' ', 1, %ls, 0, 'ClassDefinition', 0, 1, 0, 0)",
			(FdoString *) GetManager()->FormatSQLVal(NlsMsgGet(FDORDBMS_504, "Non-feature metaclass"), FdoSmPhColType_String));
	gdbiConn->ExecuteNonQuery( (const char*) sql_stmt); 

	sql_stmt = FdoStringP::Format(L"insert into f_classdefinition "
			L"(classname, schemaname, tablename,classtype, description, isabstract, parentclassname, istablecreator, isfixedtable, hasversion, haslock) "
			L"values('ClassDefinition', 'F_MetaClass', 'f_classdefinition', 1, %ls, 1, null, 0, 1, 0, 0)",
			(FdoString *) GetManager()->FormatSQLVal(NlsMsgGet(FDORDBMS_496, "Base for all metaclasses"), FdoSmPhColType_String));
	gdbiConn->ExecuteNonQuery( (const char*) sql_stmt);

	sql_stmt = FdoStringP::Format(L"insert into f_attributedefinition "
			L"(tablename, classid, columnname, attributename, columntype, columnsize, columnscale, attributetype, isnullable,"
			L"isfeatid, issystem, isreadonly, isautogenerated, isrevisionnumber, owner, description, iscolumncreator, isfixedcolumn) "
			L"values ('f_classdefinition', IDENT_CURRENT('f_classdefinition'), 'classname', 'ClassName', 'CHAR', 30, 0, 'string', 0, 0, 1, 1, 0, 0,"
			L"SYSTEM_USER, %ls, 0, 1)",
			(FdoString *) GetManager()->FormatSQLVal(NlsMsgGet(FDORDBMS_497, "FDO base property: object class name"), FdoSmPhColType_String));
	gdbiConn->ExecuteNonQuery( (const char*) sql_stmt);

	sql_stmt = FdoStringP::Format(L"insert into f_attributedefinition "
			L"(tablename, classid, columnname, attributename, columntype, columnsize, columnscale, attributetype, isnullable,"
			L"isfeatid, issystem, isreadonly, isautogenerated, isrevisionnumber, owner, description, iscolumncreator, isfixedcolumn) "
			L"values ('f_classdefinition', IDENT_CURRENT('f_classdefinition'), 'schemaname', 'SchemaName', 'CHAR', 255, 0, 'string', 1, 0, 1, 1, 0, 0,"
			L"SYSTEM_USER, %ls, 0, 1)",
			(FdoString *) GetManager()->FormatSQLVal(NlsMsgGet(FDORDBMS_498, "FDO base property: object class schema name"), FdoSmPhColType_String));
	gdbiConn->ExecuteNonQuery( (const char*) sql_stmt);

	sql_stmt = FdoStringP::Format(L"insert into f_classdefinition "
			L"(classname,schemaname,tablename,classtype,description,isabstract,parentclassname,istablecreator,isfixedtable,hasversion,haslock) "
			L"values ('Feature','F_MetaClass','f_feature',1,%ls,0,'ClassDefinition',0,1,0,0)",
			(FdoString *) GetManager()->FormatSQLVal(NlsMsgGet(FDORDBMS_499, "Feature metaclass"), FdoSmPhColType_String));
	gdbiConn->ExecuteNonQuery( (const char*) sql_stmt);

	sql_stmt = FdoStringP::Format(L"insert into f_attributedefinition "
			L"(tablename,classid,columnname,attributename,idposition,columntype,columnsize,columnscale,attributetype, "
			L"isnullable,isfeatid,issystem,isreadonly,isautogenerated,isrevisionnumber,owner,description,iscolumncreator,isfixedcolumn) "
			L"values ('f_feature',IDENT_CURRENT('f_classdefinition'),'classid','ClassId',1,'NUMBER',20,0,'int64',0,0,1,1,1,0, "
			L"SYSTEM_USER,%ls,0,1)",
			(FdoString *) GetManager()->FormatSQLVal(NlsMsgGet(FDORDBMS_501, "Class id number (uniquely identifies a class)"), FdoSmPhColType_String));
	gdbiConn->ExecuteNonQuery( (const char*) sql_stmt);

	sql_stmt = FdoStringP::Format(L"insert into f_attributedefinition "
			L"(tablename,classid,columnname,attributename,columntype,columnsize,columnscale,attributetype, "
			L"isnullable,isfeatid,issystem,isreadonly,isautogenerated,isrevisionnumber,owner,description,iscolumncreator,isfixedcolumn) "
			L"values ('f_feature',IDENT_CURRENT('f_classdefinition'),'revisionnumber','RevisionNumber','NUMBER',0,0,'double',0,0,1,1,1,1, "
			L"SYSTEM_USER,%ls,0,1)",
			(FdoString *) GetManager()->FormatSQLVal(NlsMsgGet(FDORDBMS_502, "Edit count for supporting optimistic locking.  This value is incremented everytime the object is updated."), FdoSmPhColType_String));
	gdbiConn->ExecuteNonQuery( (const char*) sql_stmt);

	sql_stmt = FdoStringP::Format(L"insert into f_attributedefinition "
			L"(tablename,classid,columnname,attributename,columntype,columnsize,columnscale,attributetype,geometrytype,isnullable, "
			L"isfeatid,issystem,isreadonly,isautogenerated,isrevisionnumber,owner,description,iscolumncreator,isfixedcolumn) "
			L"values ('n/a',IDENT_CURRENT('f_classdefinition'),'n/a','Bounds','GEOMETRY',0,0,'4','84224',1,0,1,1,0,0, "
			L"SYSTEM_USER,%ls,0,0)",
			(FdoString *) GetManager()->FormatSQLVal(NlsMsgGet(FDORDBMS_503, "Bounding box for the feature"), FdoSmPhColType_String));
	gdbiConn->ExecuteNonQuery( (const char*) sql_stmt);
}
