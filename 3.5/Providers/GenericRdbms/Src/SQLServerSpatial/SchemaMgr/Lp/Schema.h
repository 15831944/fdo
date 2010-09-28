#ifndef FDOSMLPSQSSCHEMA_H
#define FDOSMLPSQSSCHEMA_H      1
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

#include "../../../SchemaMgr/Lp/Schema.h"
#include <Rdbms/Override/SQLServerSpatial/SqlServerOvTextInRowOption.h>


// SqlServer Provider's LogicalPhysical schema definition
class FdoSmLpSqsSchema : public FdoSmLpGrdSchema
{
public:

    // Creates a schema from a schema reader.
    //
    // Parameters:
    //      rdr: get the schema attributes from this schema reader.
    //      physicalSchema: Physical Schema Manager
    //      schemas: containing schema collection for this schema.
    FdoSmLpSqsSchema(
        FdoSmPhSchemaReaderP rdr,
        FdoSmPhMgrP physicalSchema,
        FdoSmLpSchemaCollection* schemas
    );

    // Creates an instance of a LogicalPhysicalSchema from an FDO Feature Schema.
	//
	// Parameters:
	//		FeatSchema: the FDO schema.
    //      bIgnoreStates: true if the element state on the FDO schema is 
    //          to be ignored.
	//		physicalSchema: Physical Schema Manager. Used to retrieve the schema's 
	//          classes
    FdoSmLpSqsSchema(
		FdoFeatureSchema* pFeatSchema,
        bool bIgnoreStates,
		FdoSmPhMgrP physicalSchema, 
		FdoSmLpSchemaCollection* schemas
	);

    // Generates the physical mappings for this feature schema
    virtual FdoPhysicalSchemaMappingP GetSchemaMappings( bool bIncludeDefaults ) const;
	
    // Copy updates from corresponding FDO feature schema. 
    //
    // Parameters
    //      pFeatSchema: the FDO feature schema.
    //      elementState: the modification state for this feature schema
    //      pPropOverrides: Fdo to physical mapping overrides.
    //      bIgnoreStates: true if the element state on the FDO feature schema is 
    //          to be ignored.
	virtual void Update(
        FdoFeatureSchema* pFeatSchema, 
        FdoSchemaElementState elementState,
        FdoPhysicalSchemaMapping* pOverrides,
        bool bIgnoreStates
    ); 

    // Get the default overrides for tables in this feature schema:
    FdoString* GetTableFilegroup() const       { return mTableFilegroup; }
    FdoString* GetIndexFilegroup() const       { return mIndexFilegroup; }
    FdoString* GetTextFilegroup() const        { return mTextFilegroup;  }
    FdoString* GetTextInRow() const          { return mTextInRow;    }

    // Set the default overrides for tables in this feature schema:
    void SetTableFilegroup(FdoString* tableFilegroup)    { mTableFilegroup = tableFilegroup; }
    void SetIndexFilegroup(FdoString* indexFilegroup)    { mIndexFilegroup = indexFilegroup; }
    void SetTextFilegroup(FdoString* textFilegroup)      { mTextFilegroup = textFilegroup; }
    void SetTextInRow(FdoString* textInRow)          { mTextInRow = textInRow; }


    // Various property definition creator implementors.
    virtual FdoSmLpPropertyP CreateDataProperty( FdoSmPhClassPropertyReaderP propReader, FdoSmLpClassDefinition* parent  );
    virtual FdoSmLpPropertyP CreateGeometricProperty( FdoSmPhClassPropertyReaderP propReader, FdoSmLpClassDefinition* parent  );
    virtual FdoSmLpPropertyP CreateObjectProperty( FdoSmPhClassPropertyReaderP propReader, FdoSmLpClassDefinition* parent  );
    virtual FdoSmLpPropertyP CreateAssociationProperty( FdoSmPhClassPropertyReaderP propReader, FdoSmLpClassDefinition* parent  );

    virtual FdoSmLpPropertyP CreateDataProperty(
        FdoDataPropertyDefinition* pFdoProp, 
        bool bIgnoreStates,
        FdoSmLpClassDefinition* parent
    );
    virtual FdoSmLpPropertyP CreateGeometricProperty(
        FdoGeometricPropertyDefinition* pFdoProp, 
        bool bIgnoreStates,
        FdoSmLpClassDefinition* parent
    );
    virtual FdoSmLpPropertyP CreateGeometricProperty(
        FdoGeometricPropertyDefinition* pFdoProp, 
        FdoString * columnNameX,
        FdoString * columnNameY,
        FdoString * columnNameZ,
        bool bIgnoreStates,
        FdoSmLpClassDefinition* parent
    );
    virtual FdoSmLpPropertyP CreateObjectProperty(
        FdoObjectPropertyDefinition* pFdoProp, 
        bool bIgnoreStates,
        FdoSmLpClassDefinition* parent
    );
    virtual FdoSmLpPropertyP CreateAssociationProperty(
        FdoAssociationPropertyDefinition* pFdoProp, 
        bool bIgnoreStates,
        FdoSmLpClassDefinition* parent
    );

    virtual FdoSchemaExceptionP Errors2Exception(FdoSchemaException* pFirstException ) const;

protected:
    virtual ~FdoSmLpSqsSchema(void);

    // Get a writer for adding this feature schema to the MetaSchema.
    virtual FdoSmPhSchemaWriterP GetPhysicalAddWriter();

    // Various class definition creator implementors.
    virtual FdoSmLpClassDefinitionP CreateFeatureClass( FdoSmPhClassReaderP classReader );
    virtual FdoSmLpClassDefinitionP CreateClass( FdoSmPhClassReaderP classReader );

    virtual FdoSmLpClassDefinitionP CreateFeatureClass(
        FdoFeatureClass* pFdoClass, 
        bool bIgnoreStates
    );
    virtual FdoSmLpClassDefinitionP CreateClass(
        FdoClass* pFdoClass, 
        bool bIgnoreStates
    );

    // Creates a corresponding SQLServer schema for this 
    // Feature Schema
    virtual void CreatePhysicalSchema(
        FdoSmPhOwnerP owner
    );

    // Deletes the corresponding SQLServer schema for this 
    // Feature Schema. Silent if the schema does not exist.
    virtual void DeletePhysicalSchema(
        FdoSmPhOwnerP owner
    );

private:
    FdoStringP                      mTableFilegroup;
    FdoStringP                      mIndexFilegroup;
    FdoStringP                      mTextFilegroup;
    FdoStringP                      mTextInRow;
};

typedef FdoPtr<FdoSmLpSqsSchema> FdoSmLpSqsSchemaP;

#endif
