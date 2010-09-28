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

#include "FDO\Schema\mgPropertyValueConstraintType.h"

class FdoPropertyValueConstraintList;

BEGIN_NAMESPACE_OSGEO_FDO_EXPRESSION
ref class DataValueCollection;
END_NAMESPACE_OSGEO_FDO_EXPRESSION

BEGIN_NAMESPACE_OSGEO_FDO_SCHEMA

/// \ingroup (OSGeoFDOSchema)
/// \brief
/// PropertyValueConstraintList is used to specify a list of allowed values for a particular property. 
/// It can be used for all data property types except for Boolean, BLOB, or CLOB.
/// This constraint is represented by a list of values of the same type as the corresponding data property. 
/// The list cannot be empty. The list could be a single value. While that may not seem very useful it could serve as a 
/// temporary way to enforce a single value for a property, e.g. for a particular customer, Parcel.State is always �CA? 
/// In this case, the definer of the property definition probably should also set the default value to be the same, unless it can be null.
/// The list does not need to be ordered. However, a provider may choose to reorder the list so the list that is returned when 
/// a user gets the definition of this constraint for a property may be different than the order that was given when defined.
/// E.g. defined as (12, 8, 24), but returned as (8, 12, 24). If the list has duplicate values, the provider may choose to eliminate 
/// duplicates from the list.
/// If the data property definition allows nulls, a null value is considered as being valid regardless of this list of valid values. 
/// The list of valid values should not include the null value itself, but leave the specification of whether null is allowed to 
/// the null value constraint. If the data property definition includes a non-null default value, then that value should be one 
/// of the values in the list.
/// If both a valid values list constraint and a unique constraint on the same property are included, then the side effect is that 
/// there will be an upper limit of the number of objects of that class that have non-null values for the property.
public ref class PropertyValueConstraintList : public NAMESPACE_OSGEO_FDO_SCHEMA::PropertyValueConstraint
{
internal:
	PropertyValueConstraintList(System::IntPtr unmanaged, System::Boolean autoDelete);

	inline FdoPropertyValueConstraintList* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;

public:    
    /// \brief
    /// Constructs an empty instance of an PropertyValueConstraintList.
    /// 
    /// \return
    /// Returns an PropertyValueConstraintList
    /// 
    PropertyValueConstraintList();

    /// \brief
    /// Returns the list of allowed values for a particular property.
    /// 
    /// \return
    /// Returns an instance of DataValueCollection.
    /// 
    property NAMESPACE_OSGEO_FDO_EXPRESSION::DataValueCollection^ ConstraintList
    {
        NAMESPACE_OSGEO_FDO_EXPRESSION::DataValueCollection^ get();
    }	
};

END_NAMESPACE_OSGEO_FDO_SCHEMA

