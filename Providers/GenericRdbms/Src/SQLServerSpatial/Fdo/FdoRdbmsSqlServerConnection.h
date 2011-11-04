#ifndef FDORDBMSSQLSERVERCONNECTION_H
#define FDORDBMSSQLSERVERCONNECTION_H	1
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


#include "FdoRdbms.h"
#include <Rdbms/Override/SQLServerSpatial/SqlServerOvPhysicalSchemaMapping.h>

// ODBC date/time format:
#define FDORDBMSODBCFILTER_DATE_PREFIX                 L"{d '"
#define FDORDBMSODBCFILTER_TIME_PREFIX                 L"{t '"
#define FDORDBMSODBCFILTER_TIMESTAMP_PREFIX            L"{ts '"
#define FDORDBMSODBCFILTER_DATE_FORMAT                 L"%.4d-%.2d-%.2d"
#define FDORDBMSODBCFILTER_TIME_FORMAT                 L"%.2d:%.2d:%6.3f"
#define FDORDBMSODBCFILTER_DATETIME_SEPARATOR          L" "
#define FDORDBMSODBCFILTER_DATETIME_SUFFIX             L"'}"

class FdoRdbmsSqlServerFilterProcessor;

class FdoRdbmsSqlServerConnection: public FdoRdbmsConnection
{
private:
    FdoRdbmsSqlServerFilterProcessor *mFilterProcessor;

    FdoIConnectionInfo          *mConnectionInfo;

    bool                        mIsGeogLatLongSet;
    bool                        mIsGeogLatLong;
    long                        mGeomVersion;

protected:
    virtual ~FdoRdbmsSqlServerConnection ();
    FdoRdbmsSqlServerConnection ();

    // Checks if SQL Server version is 8 or earlier and 
    // throws an exception if it is. The SQLServerSpatial provider
    // relies on some sys views introduced in version 9.
    void CheckForUnsupportedVersion();

    // Checks for FDO-style geometries. If any are present, an
    // exception is thrown. These geometries are created by the SqlServer
    // provider, meaning they cannot be handled by the SQLServerSpatial
    // provider.
    void CheckForFdoGeometries();

    // Generates a native ODBC connection string, from other connection parms,
    //if given string is empty.
    virtual FdoStringP GenConnectionStringParm( FdoStringP connectionString );

public:
	static FdoRdbmsSqlServerConnection* Create();
    
    //	Creates and returns the specified type of command object
    //	associated with the connection.
    virtual FdoICommand *CreateCommand (FdoInt32 commandType);

    virtual FdoRdbmsFilterProcessor* GetFilterProcessor();

    // Creates a Schema Manager for the Oracle Provider.
    virtual FdoSchemaManagerP NewSchemaManager(
        GdbiConnection* gdbiConnection, 
        FdoStringP currUser,
        FdoStringP currSchema
    );

    // Creates an empty Schema Mapping set appropriate for the SqlServer Provider 
    FDORDBMS_API virtual FdoPhysicalSchemaMapping* CreateSchemaMapping()
    {
        return FdoSqlServerOvPhysicalSchemaMapping::Create();
    }

    virtual FdoIConnectionInfo* GetConnectionInfo();

    /// <summary>Gets an FdoIGeometryCapabilities interface describing the provider's support for geometry.</summary>
    /// <returns>Returns the geometry capabilities</returns>
    virtual FdoIGeometryCapabilities* GetGeometryCapabilities();

    /// <summary>Gets an FdoISchemaCapabilities interface describing the provider's support for the feature schema.</summary>
    /// <returns>Returns schema capabilities</returns> 
	virtual FdoISchemaCapabilities* GetSchemaCapabilities();

    /// <summary>Gets an FdoIConnectionCapabilities interface describing the provider's support for misc connection-related items.</summary>
    /// <returns>Returns connection capabilities</returns> 
    virtual FdoIConnectionCapabilities* GetConnectionCapabilities();

    //
    // Converts a SqlServer string date of a specific format to a FdoDateTime (time_t) format.
    virtual FdoDateTime  DbiToFdoTime( const char* time );

    //
    // Converts a SqlServer string date of a specific format to a FdoDateTime (time_t) format.
    virtual FdoDateTime  DbiToFdoTime( const wchar_t* time );

    virtual long GetSpatialGeometryVersion() { return mGeomVersion; }

    virtual void Flush();

    virtual const wchar_t* FdoToDbiTime( FdoDateTime  time, wchar_t* dest, size_t size );
    //
    // Convert time_t( FdoDateTime ) to a SqlServer string date of the form.
    // It return a statically allocated storage that can be overwritten by subsequent call to this or other methods.
    virtual const char* FdoToDbiTime( FdoDateTime  time );
    virtual const char* FdoToDbiTimeFilter( FdoDateTime  when );
	virtual FdoConnectionState Open ();
	virtual void Close();

    // Creates a Long Transaction Manager and its corresponding Long Transaction
    // Manager Service.
    FdoRdbmsLongTransactionManager *CreateLongTransactionManager();

	// Returns a DataStorePropertyDictionary object for this provider.
	virtual FdoIDataStorePropertyDictionary* CreateDataStoreProperties( int action );

	virtual FdoIFilterCapabilities* GetFilterCapabilities();

    virtual FdoIExpressionCapabilities* GetExpressionCapabilities();

	//Lock Manager
	FdoRdbmsLockManager *CreateLockManager();

	virtual FdoICommandCapabilities* GetCommandCapabilities();

    // Returns the bind string. The geometries need a conversion wrapper
    virtual FdoStringP GetBindString( int n, const FdoSmLpPropertyDefinition* prop = NULL );

    // Workaround for SqlServer spatial bug: on Insert the geometries need to be bound last.
    virtual bool  BindGeometriesLast();

    //Count() and SpatialExtents()
    virtual FdoRdbmsFeatureReader *GetOptimizedAggregateReader(const FdoSmLpClassDefinition* classDef, aggr_list *selAggrList, FdoFilter* filter = NULL);

    // Returns true if SQL Server handles geographic column coordinates in Latitude first order.
    // True only for beta versions of SQL Server 2008
    bool IsGeogLatLong();
    
    virtual FdoInt32 ExecuteDdlNonQuery(FdoString* sql);

    virtual void StartStoredProcedure();
    virtual void EndStoredProcedure();

    virtual FdoRdbmsSqlBuilder* GetSqlBuilder();

    bool GetServerSideFunctionCollection (FdoFunctionDefinitionCollection* coll);
};


#endif // FDORDBMSSqlServerCONNECTION_H
