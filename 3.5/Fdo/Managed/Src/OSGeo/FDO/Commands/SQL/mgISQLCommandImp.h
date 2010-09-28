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

#include "FDO\Commands\mgICommandImp.h"
#include "FDO\Commands\SQL\mgISQLCommand.h"

class FdoISQLCommand;

BEGIN_NAMESPACE_OSGEO_FDO_COMMANDS_SQL
interface class ISQLDataReader;

/// \ingroup (OSGeoFDOCommandsSQL)
/// \brief
/// The ISQLCommandImp class is a concrete implementation of ISQLCommand.
/// The ISQLCommand interface defines the SQL command, which supports the
/// execution of a SQL statement against an underlying RDBMS. Two execute
/// methods are provided to distinguish between statements that return table data
/// versus those that execute non query type operations.
private ref class ISQLCommandImp : public NAMESPACE_OSGEO_FDO_COMMANDS::ICommandImp, 
                                    public NAMESPACE_OSGEO_FDO_COMMANDS_SQL::ISQLCommand
{
public:
    /// \brief
    /// Gets the SQL statement to be executed as a string.
    /// 
    /// \return
    /// Returns the SQL statement.
    /// 
    /// \brief
    /// Sets the SQL statement to be executed as a string.
    /// 
    /// \param value 
    /// Input the SQL statement.
    /// 
    /// \return
    /// Returns nothing
    /// 
    virtual property System::String^ SQLStatement
    {
        System::String^ get();
        System::Void set(System::String^ value);
    }

    /// \brief
    /// Sets the fetch size of the data set when executing query against 
    /// the underlying data store.
    /// 
    /// \param value
    /// Input the fetch size.
    /// 
    /// \return  
    /// Returns nothing.
    /// 
    /// \brief 
    /// Gets the fetch size of the data set when executing query against 
    /// the underlying data store.
    /// 
    /// \return  
    /// Returns the fetch size.
    /// 
    virtual property System::Int32 FetchSize
    {
        System::Int32 get();
        System::Void set(System::Int32 value);
    }

    /// \brief
    /// Executes the SQL statement against the connection object and returns
    /// the number of rows affected.
    /// 
    /// \return
    /// Returns the number of rows affected.
    /// 
	virtual System::Int32 ExecuteNonQuery();

    /// \brief
    /// Executes the SQL statement against the connection object and returns
    /// an ISQLDataReader.
    /// 
    /// \return
    /// Returns the data reader.
    /// 
	virtual NAMESPACE_OSGEO_FDO_COMMANDS_SQL::ISQLDataReader^ ExecuteReader();

internal:
	ISQLCommandImp(System::IntPtr unmanaged, System::Boolean autoDelete) : NAMESPACE_OSGEO_FDO_COMMANDS::ICommandImp(unmanaged, autoDelete)
	{

	}

	inline FdoISQLCommand* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;
};

END_NAMESPACE_OSGEO_FDO_COMMANDS_SQL

