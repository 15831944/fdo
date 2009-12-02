#ifndef FDOSMPHPROPERTYREADER_H
#define FDOSMPHPROPERTYREADER_H		1
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

//#include <Sm/Ph/DependencyReader.h>
#include <Sm/Ph/Reader.h>

// This class retrieves all class properties for a given 
// Feature schema. 

class FdoSmPhPropertyReader :
	public FdoSmPhReader
{
public:
    /// Create a property reader.
    /// Properties are ordered by class name and 
    /// property name.
	//
    /// Parameters:
    /// 	schemaName: get the properties for this feature schema.
    /// 	mgr: holds the connection for a database.
    /// 		All Feature Schemas in this database are retrieved.
	FdoSmPhPropertyReader(FdoStringP schemaName, FdoSmPhMgrP mgr);

	FdoSmPhPropertyReader(FdoSmPhReaderP subReader, FdoSmPhMgrP mgr);

    /// Deactivate the query
	~FdoSmPhPropertyReader(void);

    /// Various getters for property attributes

    /// Property name
	FdoStringP GetName();

    /// Physical column name for the property
	FdoStringP GetColumnName();

    /// Physical column name for the property
	FdoStringP GetColumnType();

    /// true if property column has fixed name specified through 
    /// schema overrides
    bool GetIsFixedColumn();

    /// true if this class owns the column (column was created when class was created)
    /// false if this class attached itself to pre-existing created when it was created. 
    /// for object properties this setting can apply to multiple columns.
    bool GetIsColumnCreator();

    /// Property Description
	FdoStringP GetDescription();

    /// Owning class name
	FdoStringP GetFeatureClassName();

    /// Data type ( string equivalent of FDO type )
	FdoStringP GetDataType();

    /// Gets the current geometry type.
    FdoStringP GetGeometryType();

    /// If the property is an identity property, this function returns its position
    /// within the class's identity property collection.
    /// Otherwise, it returns 0.
	int GetIdPosition();

    /// Nullable
	bool GetIsNullable();

    /// Read only
	bool GetIsReadOnly();

    /// Is FeatId column 
	bool GetIsFeatId();

    /// Is FDO System column
	bool GetIsSystem();

    /// Total length for string type.
    /// Total number of digits for number type.
	int GetLength();

    /// Number of digits to right of decimal point
    /// for number type.
	int GetScale();

    /// Default Value
	FdoPtr<FdoDataValue> GetDefaultValue();

    /// Table name
	FdoStringP GetTableName();

    /// Gets the root object name for this property.
    //
    /// For an Object Property whose table is foreign, GetTableName() actually
    /// returns the view around the foreign table. This function returns the name
    /// of the foreign table.
    //
    /// For Data or Geometric Properties whose column is in a foreign table, GetColumnName() 
    /// returns the corresponding column in the view around the foreign table. This
    /// function returns the name of the column in the foreign table.
    //
    /// Returns L"" if the table for this property is not foreign.
	FdoStringP GetRootObjectName();

    /// Table where property is stored.
    /// NULL if the property has no table.
    /// TODO: think about removing this function.
//	const FdoSmPhTable* GetTable();

    /// Has elevation dimension( Geometric properties only )
	bool GetHasElevation();

    /// has measure dimension( Geometric properties only )
	bool GetHasMeasure();

    /// are values autogenerated? ( Data properties only )
	bool GetIsAutoGenerated();

    /// Is column Revision Number column
    bool GetIsRevisionNumber();

    /// Sequence name for allocating property values
    FdoStringP GetSequenceName();

    /// Returns a reader for retrieving all the attribute 
    /// dependencies for the current property.
    /// TODO: implement dependency reading
//	FdoSmPhDependencyReaderP GetDependencyReader();

    static FdoSmPhRowsP MakeRows( FdoSmPhMgrP mgr );

protected:
    /// Unused constructor needed only to build on Linux
    FdoSmPhPropertyReader() {}

    /// Top level function for creating inner reader
    FdoSmPhReaderP MakeReader( FdoSmPhMgrP mgr, FdoStringP schemaName );

    /// Create inner reader for MetaSchema
    FdoSmPhReaderP MakeMtReader( FdoSmPhRowsP rows, FdoStringP schemaName, FdoSmPhMgrP mgr );

//TODO: reader from native physical schema is created by 
// FdoSmPhClassReader. However we need to create one here for the 
// case where F_CLASSDEFINITION exists but F_ATTRIBUTEDEFINITION does not.
//    FdoSmPhReaderP MakeRdReader( FdoSmPhRowsP pTables, FdoStringP schemaName,FdoSmPhMgrP mgr );

private:

    bool mbAutogeneratedDefined;
    bool mbColumnCreatorDefined;
    bool mbRevisionNumberDefined;
    bool mbRootObjectNameDefined;

    /// Cache the associated spatial context ID and name (geometric properties only)
    FdoInt64    mScId;
    FdoStringP  mScName;
   
};

typedef FdoPtr<FdoSmPhPropertyReader> FdoSmPhPropertyReaderP;

#endif


