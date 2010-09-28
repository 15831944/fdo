#ifndef FDOSMMYSQLSCHEMAMANAGER_H
#define FDOSMMYSQLSCHEMAMANAGER_H     1
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

#ifdef _WIN32
#pragma once
#endif
#include "../../SchemaMgr/SchemaManager.h"

// Schema Manager for MySql Provider
class FdoMySqlSchemaManager : public FdoGrdSchemaManager
{
public:
    FdoMySqlSchemaManager(
        GdbiConnection* connection,     // allows access to the MySql RDBMS
        FdoStringP oraSchemaName        // default physical schema for current MySql connection.
    );

    FdoMySqlSchemaManager() {}

protected:
    virtual ~FdoMySqlSchemaManager(void);

    // Create MySql Provider's LogicalPhysical Schema collection.
    virtual FdoSmLpSchemasP CreateLogicalPhysicalSchemas(FdoSmPhMgrP physMgr, FdoSmLpSpatialContextMgrP scMgr);

    // Create MySql Provider's Physical Schema Manager.
    virtual FdoSmPhMgrP CreatePhysicalSchema();

	// Apply updates to the given schema to the FDO DataStore.
    //
    // Parameters
    //      pFeatSchema: the Feature schema.
    //      pOverrides: Fdo to physical mapping overrides for the feature schema.
	virtual void ApplySchema( 
        FdoFeatureSchemaP pFeatSchema, 
        FdoPhysicalSchemaMappingP pOverrides,
        bool bIgnoreStates = false
    );

private:
    FdoStringP mMySqlSchemaName;
};

#endif
