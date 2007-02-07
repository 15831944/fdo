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
#ifndef FDOPOSTGIS_CONNECTION_H_INCLUDED
#define FDOPOSTGIS_CONNECTION_H_INCLUDED

#include <PostGIS/FdoPostGisProvider.h>
#include <Fdo/Connections/IConnection.h>
// boost
#include <boost/tuple/tuple.hpp>
// PostgreSQL client library
#include <libpq-fe.h>

//
// Forward declarations
//
//class FdoIConnectionPropertyDictionary;

namespace fdo { namespace postgis {

/// Implementation of connection interface for PostGIS provider.
/// It represents a unique session with FDO datastore.
///
/// \todo Make it noncopyable
///
class Connection : public FdoIConnection
{
public:

    /// Default constructor.
    Connection();

    //
    // FdoIConnection interface
    //

    /// Get pointer to FdoIConnectionCapabilities interface describing
    /// capabilities of the connection.
    FDOPOSTGIS_API FdoIConnectionCapabilities* GetConnectionCapabilities();

    /// Get pointer to FdoISchemaCapabilities interface describing
    /// support for the feature schema provided by PostGIS Provider.
    FDOPOSTGIS_API FdoISchemaCapabilities* GetSchemaCapabilities();
 	
    /// Get pointer to FdoICommandCapabilities interface describing supported commands.
    FDOPOSTGIS_API FdoICommandCapabilities* GetCommandCapabilities();

    /// Get pointer to FdoIFilterCapabilities interface describing 
    /// support for filters.
    FDOPOSTGIS_API FdoIFilterCapabilities* GetFilterCapabilities();

    /// Get pointer to FdoIExpressionCapabilities interface describing
    /// support for expressions.
    FDOPOSTGIS_API FdoIExpressionCapabilities* GetExpressionCapabilities();

    /// Get pointer to FdoIRasterCapabilities interface describing
    /// support for raster images.
    FDOPOSTGIS_API FdoIRasterCapabilities* GetRasterCapabilities();

    /// Get pointer to FdoITopologyCapabilities interface describing support for topology.
    FDOPOSTGIS_API FdoITopologyCapabilities* GetTopologyCapabilities();

    /// Get pointer to FdoIGeometryCapabilities interface describing support for geometry.
    FDOPOSTGIS_API FdoIGeometryCapabilities* GetGeometryCapabilities();

    /// Get connection string used to establish connection with a datastore.
    FDOPOSTGIS_API FdoString* GetConnectionString();

    /// Set connection string used to establish connection with a datastore.
    FDOPOSTGIS_API void SetConnectionString(FdoString* value);

    /// Get pointer to FdoIConnectionInfo interface that can be used to interrogate
    /// and set connection properties.
    FDOPOSTGIS_API FdoIConnectionInfo* GetConnectionInfo();

    /// Get state of current connection.
    FDOPOSTGIS_API FdoConnectionState GetConnectionState();

    /// Get number of milliseconds to wait while trying to establish a connection
    /// before terminating the attempt and generating an error.
    FDOPOSTGIS_API FdoInt32 GetConnectionTimeout();

    /// Set number of milliseconds to wait while trying to establish a connection
    /// before terminating the attempt and generating an error.
    FDOPOSTGIS_API void SetConnectionTimeout(FdoInt32 value);

    /// Establish connection with a datastore using settings
    /// specified by ConnectionString attribute.
    FDOPOSTGIS_API FdoConnectionState Open();

    /// Close current connection.
    FDOPOSTGIS_API void Close();

    /// Begin transaction and return pointer to FdoITransaction interface realizing
    /// transaction operations support.
    FDOPOSTGIS_API FdoITransaction* BeginTransaction();

    /// Create command object of specified type and return pointer to interface realizing
    /// command capabilities associated with the connection.
    FdoICommand* CreateCommand(FdoInt32 type);

    /// Factory function that creates an empty Schema Override mapping specific
    /// for PostGIS Provider.
    FdoPhysicalSchemaMapping* CreateSchemaMapping();

    /// Set XML configuration stream used to configure connected datastore.
    void SetConfiguration(FdoIoStream* configStream);

    /// Force to write any cached data to currently connected datastore
    void Flush();

protected:

    /// Destructor.
    virtual ~Connection();

    //
    // FdoIDisposable interface
    //

    /// Dispose this object.
    void Dispose();

private:

    //
    // Data members
    //

    // Connection information object.
    // It's of lazy-creation object, delayed until first request,
    // on GetConnectionInfo() call.
    FdoPtr<FdoIConnectionInfo> mConnInfo;

    // Connection string.
    FdoStringP mConnString;

    // Connection state flag (closed, pending, open).
    FdoConnectionState mConnState;

    // Pointer to PostgreSQL connection object of current session.
    PGconn* mPgConn;

    //
    // Private operations
    //

    // Tuple with PostgreSQL connection string tokens.
    typedef boost::tuples::tuple
        <
        std::string, std::string, std::string, std::string, std::string, std::string, std::string
        >
        pgconn_params_t;

    // Get PostgreSQL connection parameters from FDO connection properties.
    pgconn_params_t GetPgConnectionParams(FdoPtr<FdoIConnectionPropertyDictionary> dict);

};

}} // namespace fdo::postgis

#endif // FDOPOSTGIS_CONNECTION_H_INCLUDED