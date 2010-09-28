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

class FdoRdbmsOvPropertyMappingSingle;

BEGIN_NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE

///<summary>
///Abstract class defining the physical schema overrides for the Single 
///type object property mapping. Under the Single mapping type, the object 
///property is kept in the same table as its containing class.
///</summary>
public ref class OvPropertyMappingSingle : public NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE::OvPropertyMappingDefinition
{
internal:
	FdoRdbmsOvPropertyMappingSingle* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;
	
public protected:
	OvPropertyMappingSingle(System::IntPtr unmanaged, System::Boolean autoDelete);

public:
    ///<summary>
    ///Gets the prefix which is prepended to the name of each column
    ///</summary>
    /// <returns>Returns the prefix</returns>
    ///<summary>Sets the prefix</summary>
    /// <returns>Returns nothing</returns>
    virtual property System::String^ Prefix
    {
        System::String^ get();
        System::Void set(System::String^ value);
    }
};

END_NAMESPACE_OSGEO_FDO_PROVIDERS_RDBMS_OVERRIDE
