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

#pragma  once

/// \cond DOXYGEN-IGNORE
BEGIN_NAMESPACE_OSGEO_FDO_SCHEMA
/// \endcond

/// \ingroup (OSGeoFDOSchema)
/// \brief
/// The SchemaElementState enumeration defines the current state of the schema element
public enum class SchemaElementState
{
    /// Indicates the schema element has been added.
	SchemaElementState_Added = FdoSchemaElementState_Added,

    /// Indicates the schema element is marked for deletion.
	SchemaElementState_Deleted = FdoSchemaElementState_Deleted,

    /// Indicates the schema element is not associated with an FeatureSchema.
	SchemaElementState_Detached = FdoSchemaElementState_Detached,

    /// Indicates the schema element has been modified in some way.
	SchemaElementState_Modified = FdoSchemaElementState_Modified,

    /// Indicates the schema element has not been changed.
	SchemaElementState_Unchanged = FdoSchemaElementState_Unchanged
};

/// \cond DOXYGEN-IGNORE
END_NAMESPACE_OSGEO_FDO_SCHEMA
/// \endcond
