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

#pragma once

class FdoMySQLOvPhysicalSchemaMapping;

BEGIN_NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE_MYSQL

///<summary>Concrete class offering schema overrides for
///the MySQL Provider.</summary>
public ref class OvPhysicalSchemaMapping : public NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE::OvPhysicalSchemaMapping
{
internal:
    OvPhysicalSchemaMapping(System::IntPtr unmanaged, System::Boolean autoDelete);
    inline FdoMySQLOvPhysicalSchemaMapping* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;

public:
    ///<summary>Constructs a default of an OvPhysicalSchemaMapping</summary>
    /// <returns>Returns OvPhysicalSchemaMapping</returns>
    OvPhysicalSchemaMapping();

    /// <summary>Constructs a physical schema mapping instance using the 
    /// result returned from IConnection::CreateSchemaMapping OR using the result of 
    /// executing an IDescribeSchemaMapping command. This constructor is provided 
    /// in order that clients are provided a mechanism to downcast from the FDO 
    /// PhysicalSchemaMapping base class to an FDO Provider specific class 
    /// that derives from PhysicalSchemaMapping. This constructor is the only 
    /// mechanism provided by the FDO Managed API that will allow the client to correctly
    /// construct a provider specific PhysicalSchemaMapping class. 
    /// NOTE: If the Provider name associated to the input parameter schemaMapping does not
    /// correspond to the provider name of the expected FDO Provider, and InvalidArgumentException
    /// will be thrown.
    /// </summary>
    OvPhysicalSchemaMapping(NAMESPACE_OSGEO_FDO_COMMANDS_SCHEMA::PhysicalSchemaMapping^ schemaMapping, System::Boolean autoDelete);

    ///<summary>Constructs an instance of an OvPhysicalSchemaMapping using the specified arguments</summary>
    /// <param name="name">Input name</param>
    /// <returns>Returns OvPhysicalSchemaMapping</returns>
    OvPhysicalSchemaMapping(System::String^ name);

    ///<summary>Gets the classes</summary>
    /// <returns>Returns the classes</returns>
    property NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE_MYSQL::OvClassCollection^ Classes
    {
        NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE_MYSQL::OvClassCollection^ get();
    }

    ///<summary>Gets the directory where table data is stored</summary>
    /// <returns>Returns the directory where table data is stored</returns>
    ///<summary>Sets the directory where table data is stored</summary>
    /// <returns>Returns nothing</returns>
    property System::String^ DataDirectory
    {
        System::String^ get();
        System::Void set(System::String^ dataDirectory);
    }

    ///<summary>Gets the directory where indexes are stored</summary>
    /// <returns>Returns the directory where indexes are stored</returns>
    ///<summary>Sets the directory where indexes are stored</summary>
    /// <returns>Returns nothing</returns>
    property System::String^ IndexDirectory
    {
        System::String^ get();
        System::Void set(System::String^ indexDirectory);
    }

    ///<summary>Gets the storage engine type</summary>
    /// <returns>Returns the storage engine type</returns>
    ///<summary>Sets the storage engine type</summary>
    /// <returns>Returns nothing</returns>
    property NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE_MYSQL::OvStorageEngineType StorageEngine
    {
        NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE_MYSQL::OvStorageEngineType get();
        System::Void set(NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE_MYSQL::OvStorageEngineType storageEngine);
    }

    ///<summary>Gets the database</summary>
    /// <returns>Returns the database</returns>
    ///<summary>Sets the database</summary>
    /// <returns>Returns nothing</returns>
    property System::String^ Database
    {
        System::String^ get();
        System::Void set(System::String^ database);
    }

    ///<summary>Gets the provider name</summary>
    /// <returns>Returns the provider name</returns>
    property System::String^ Provider
    {
        System::String^ get();
    }
};

END_NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE_MYSQL
