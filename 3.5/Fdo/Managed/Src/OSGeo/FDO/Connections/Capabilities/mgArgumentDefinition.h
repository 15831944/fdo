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

#include "FDO\Schema\mgDataType.h"
#include "FDO\Schema\mgPropertyType.h"
#include "FDO\Schema\mgPropertyValueConstraint.h"
#include "FDO\Schema\mgPropertyValueConstraintList.h"
#include "FDO\mgObjectFactory.h"

class FdoArgumentDefinition;

using namespace NAMESPACE_OSGEO_FDO_SCHEMA;
BEGIN_NAMESPACE_OSGEO_FDO_CONNECTIONS_CAPABILITIES

/// \ingroup (OSGeoFDOConnectionsCapabilities)
/// \brief
/// The ArgumentDefinition class contains metadata that describes an argument to a function.
public ref class ArgumentDefinition : public NAMESPACE_OSGEO_RUNTIME::Disposable
{

public:

    /// \brief
    /// Constructs an instance of an ArgumentDefinition using the specified arguments.
    /// 
    /// \param name 
    /// Input the name of the argument.
    /// \param description 
    /// Input a brief description of the argument.
    /// \param dataType 
    /// Input the type of argument.
    /// 
    /// \return
    /// Returns ArgumentDefinition
    /// 
	ArgumentDefinition(System::String^ name, System::String^ description, NAMESPACE_OSGEO_FDO_SCHEMA::DataType dataType);

    /// \brief
    /// Constructs an instance of an ArgumentDefinition using the specified arguments.
    /// 
    /// \param name 
    /// Input the name of the argument.
    /// \param description 
    /// Input a brief description of the argument.
    /// \param propertyType 
    /// Input the proeprty of argument.
    /// \param dataType 
    /// Input the type of argument.
    /// 
    /// \return
    /// Returns ArgumentDefinition
    /// 
	ArgumentDefinition(System::String^ name, System::String^ description, NAMESPACE_OSGEO_FDO_SCHEMA::PropertyType propertyType, NAMESPACE_OSGEO_FDO_SCHEMA::DataType dataType);


    /// \brief
    /// Gets the name of the argument.
    /// 
    /// \return
    /// Returns the argument name
    /// 
    property System::String^ Name
    {
        System::String^ get();
    }

    /// \brief
    /// Gets a brief description of the argument.
    /// 
    /// \return
    /// Returns the argument description
    /// 
    property System::String^ Description
    {
        System::String^ get();
    }

    /// \brief
    /// Gets the PropertyType of the argument.
    /// 
    /// \return
    /// Returns the property type of the argument
    /// 
    property NAMESPACE_OSGEO_FDO_SCHEMA::PropertyType PropertyType
    {
        NAMESPACE_OSGEO_FDO_SCHEMA::PropertyType get();
    }

    /// \brief
    /// Gets the DataType of the argument.
    /// 
    /// \return
    /// Returns the data type of the argument
    /// 
    property NAMESPACE_OSGEO_FDO_SCHEMA::DataType DataType
    {
        NAMESPACE_OSGEO_FDO_SCHEMA::DataType get();
    }

    /// \brief
    /// Returns the argument value list for the current argument
    /// 
    /// \return
    /// Returns the argument value list for the current argument
    /// \brief
    /// Sets the argument value list for the current argument
    property NAMESPACE_OSGEO_FDO_SCHEMA::PropertyValueConstraintList^ ArgumentValueList
    {
        NAMESPACE_OSGEO_FDO_SCHEMA::PropertyValueConstraintList^ get();
        System::Void set(NAMESPACE_OSGEO_FDO_SCHEMA::PropertyValueConstraintList^ argumentValueList);
    }

internal:

	ArgumentDefinition(System::IntPtr unmanaged, System::Boolean autoDelete);

	inline FdoArgumentDefinition* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;
};

END_NAMESPACE_OSGEO_FDO_CONNECTIONS_CAPABILITIES

