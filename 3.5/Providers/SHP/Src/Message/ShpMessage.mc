;//
;//Copyright (C) 2004-2006  Autodesk, Inc.
;//
;//This library is free software; you can redistribute it and/or
;//modify it under the terms of version 2.1 of the GNU Lesser
;//General Public License as published by the Free Software Foundation.
;//
;//This library is distributed in the hope that it will be useful,
;//but WITHOUT ANY WARRANTY; without even the implied warranty of
;//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;//Lesser General Public License for more details.
;//
;//You should have received a copy of the GNU Lesser General Public
;//License along with this library; if not, write to the Free Software
;//Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
;//


;//************************************************************************************
;//************************************************************************************
;//** GENERIC MESSAGES 
;//************************************************************************************
;//************************************************************************************

MessageId=1001
SymbolicName=SHP_UNEXPECTED_ERROR
Language=English
Unexpected error '%1$ls'.
.
MessageId=1002
SymbolicName=SHP_UNDEFINED_ERROR
Language=English
Error code '%1$d' returned.
.
MessageId=1003
SymbolicName=SHP_OUT_OF_MEMORY_ERROR
Language=English
Out of memory.
.
MessageId=1004
SymbolicName=SHP_NULL_PARAMETER_ERROR
Language=English
Null parameter.
.
MessageId=1005
SymbolicName=SHP_READ_FILE_ERROR
Language=English
Error occured reading file '%1$ls'.
.
MessageId=1006
SymbolicName=SHP_WRITE_FILE_ERROR
Language=English
Error occured writing file '%1$ls'.
.
MessageId=1007
SymbolicName=SHP_INVALID_RECORD_NUMBER_ERROR
Language=English
Invalid record number %1$ld for file '%2$ls'.
.
MessageId=1008
SymbolicName=SHP_END_OF_FILE_ERROR
Language=English
End of file occured reading shape at offset %1$ld for file '%2$ls'.
.
MessageId=1009
SymbolicName=SHP_UNSUPPORTED_FILE_FORMAT_ERROR
Language=English
The file '%1$ls' has an unsupported format (%2$ld).
.
MessageId=1010
SymbolicName=SHP_UNSUPPORTED_FUNCTION
Language=English
'%1$ls': The SHP Provider does not support this function.
.

;//************************************************************************************
;//************************************************************************************
;//** CONNECTION MESSAGES 
;//************************************************************************************
;//************************************************************************************

MessageId=2001
SymbolicName=SHP_CONNECTION_TIMEOUT_UNSUPPORTED
Language=English
Connection timeout is not supported.
.
MessageId=2002
SymbolicName=SHP_CONNECTION_COMMAND_NOT_SUPPORTED
Language=English
The command %1$d is not supported.
.
MessageId=2003
SymbolicName=SHP_CONNECTION_CONFIGURATION_FILE_NOT_SUPPORTED
Language=English
Shp Provider does not support configuration files.
.
MessageId=2004
SymbolicName=SHP_CONNECTION_INVALID
Language=English
Connection is invalid.
.
MessageId=2005
SymbolicName=SHP_CONNECTION_REQUIRED_PROPERTY_NULL
Language=English
The required property '%1$ls' cannot be set to NULL.
.
MessageId=2006
SymbolicName=SHP_CONNECTION_ENUM_PROPERTY_WRONG_VALUE
Language=English
The given value '%1$ls' for enumerable property '%2$ls' is not in the enumeration value collection.
.
MessageId=2007
SymbolicName=SHP_CONNECTION_PROPERTY_NOT_FOUND
Language=English
The connection property '%1$ls' was not found.
.
MessageId=2008
SymbolicName=SHP_CONNECTION_ALREADY_OPEN
Language=English
The connection is already open.
.
MessageId=2009
SymbolicName=SHP_CONNECTION_LOCATION_NOT_EXIST
Language=English
The directory '%1$ls' does not exist.
.
MessageId=2010
SymbolicName=SHP_CONNECTION_FILE_INVALID_VERSION
Language=English
The file '%1$ls' has an invalid version number (%2$d).
.
MessageId=2011
SymbolicName=SHP_CONNECTION_FILE_NOT_FOUND
Language=English
The file '%1$ls' was not found.
.
MessageId=2012
SymbolicName=SHP_CONNECTION_PATH_NOT_FOUND
Language=English
The path to file '%1$ls' was not found.
.
MessageId=2013
SymbolicName=SHP_CONNECTION_TOO_MANY_OPEN_FILES
Language=English
Too many open files.
.
MessageId=2014
SymbolicName=SHP_CONNECTION_ACCESS_DENIED
Language=English
Access to file '%1$ls' was denied.
.
MessageId=2015
SymbolicName=SHP_CONNECTION_FILE_INVALID_BOUNDING_BOX
Language=English
The file '%1$ls' has an invalid bounding box (%2$ls).
.
MessageId=2016
SymbolicName=SHP_CONNECTION_FILE_INVALID
Language=English
The file '%1$ls' cannot be opened.
.
MessageId=2017
SymbolicName=SHP_CONNECTION_FILE_MISMATCH
Language=English
The file '%1$ls' is corrupt or does not have the same number of objects as the file '%2$ls'.
.
MessageId=2018
SymbolicName=SHP_NO_DIRECTORY_SPECIFIED
Language=English
The '%1$ls' connection property was not specified and no shape file location override was specified.
.
MessageId=2019
SymbolicName=SHP_TEMPORARY_FILE_PROBLEM
Language=English
A temporary file could not be created in directory '%1$ls'.
.
MessageId=2020
SymbolicName=SHP_INVALID_CONNECTION_STRING
Language=English
Invalid connection string '%1$ls'
.
MessageId=2021
SymbolicName=SHP_INVALID_CONNECTION_PROPERTY_NAME
Language=English
Invalid connection property name '%1$ls'
.
MessageId=2022
SymbolicName=SHP_CONNECTION_FILE_INVALID_FILECODE
Language=English
The file '%1$ls' has an invalid File Code number (%2$d).
.

;//************************************************************************************
;//************************************************************************************
;//** CONNECTION PROPERTIES 
;//************************************************************************************
;//************************************************************************************

MessageId=3001
SymbolicName=SHP_CONNECTION_PROPERTY_DEFAULT_FILE_LOCATION
Language=English
DefaultFileLocation
.

MessageId=3002
SymbolicName=SHP_CONNECTION_PROPERTY_TEMPORARY_FILE_LOCATION
Language=English
TemporaryFileLocation
.

;//************************************************************************************
;//************************************************************************************
;//** SCHEMA-RELATED MESSAGES 
;//************************************************************************************
;//************************************************************************************

MessageId=4001
SymbolicName=SHP_OBJECT_PROPERTIES_NOT_SUPPORTED
Language=English
Object properties are not supported.
.
MessageId=4002
SymbolicName=SHP_UNSUPPORTED_DATATYPE
Language=English
The '%1$ls' data type is not supported by Shp.
.
MessageId=4003
SymbolicName=SHP_UNKNOWN_COLUMN_TYPE
Language=English
The '%1$d' column type is unknown.
.
MessageId=4004
SymbolicName=SHP_UNKNOWN_SHAPE_TYPE
Language=English
The shape type number '%1$d' is unknown.
.
MessageId=4005
SymbolicName=SHP_SCHEMA_FAIL_READ_FEATURE_SCHEMAS
Language=English
Failed to de-serialize the feature schemas from the configuration.
.
MessageId=4006
SymbolicName=SHP_SCHEMA_FAIL_READ_SCHEMA_MAPPINGS
Language=English
Failed to de-serialize the schema mappings from the configuration.
.
MessageId=4008
SymbolicName=SHP_SCHEMA_EXCESSIVE_GEOMETRY
Language=English
The class '%1$ls' contains more than one geometry property.
.
MessageId=4010
SymbolicName=SHP_SCHEMA_CREATION_FAILED
Language=English
Schema create failed.
.
MessageId=4012
SymbolicName=SHP_MISSING_SCHEMA_MAPPING
Language=English
Missing schema mapping for feature schema in configuration file.
.
MessageId=4013
SymbolicName=SHP_MISSING_PHYSICAL_SCHEMA
Language=English
Missing physical schema.
.
MessageId=4014
SymbolicName=SHP_SCHEMA_NOT_FOUND
Language=English
Schema '%1$ls' not found.
.
MessageId=4015
SymbolicName=SHP_CLASS_IDENTIFIER_SCOPES_NOT_SUPPORTED
Language=English
Fdo Class Identifiers that contain scopes are not supported.
.
MessageId=4016
SymbolicName=SHP_CLASS_NAME_NOT_UNIQUE
Language=English
The unqualified FDO class name '%1$ls' is not unique across all FDO schemas.
.
MessageId=4017
SymbolicName=SHP_FEATURE_CLASS_NOT_FOUND
Language=English
FDO class '%1$ls' not found in schema.
.
MessageId=4018
SymbolicName=SHP_TOO_MANY_IDENTITY_PROPERTIES
Language=English
FDO class '%1$ls' has more than one identity property; only one identity property is supported.
.
MessageId=4019
SymbolicName=SHP_IDENTITY_PROPERTY_INCORRECT_TYPE
Language=English
FDO class '%1$ls' has an identity property of type '%2$ls'; only type 'Int32' is supported.
.
MessageId=4020
SymbolicName=SHP_SCHEMA_NOT_SPECIFIED
Language=English
No schema specified for the apply schema command.
.
MessageId=4021
SymbolicName=SHP_SCHEMA_UNSUPPORTED_ELEMENT_STATE
Language=English
Schema element state '%1$d' is not supported.
.
MessageId=4022
SymbolicName=SHP_SCHEMA_DELETED_CLASS_IN_ADDED_SCHEMA
Language=English
Conflicting schema element state: a class marked for deletion was found in a schema to be added.
.
MessageId=4023
SymbolicName=SHP_SCHEMA_MODIFIED_CLASS_IN_ADDED_SCHEMA
Language=English
Conflicting schema element state: a class marked for modification was found in a schema to be added.
.
MessageId=4024
SymbolicName=SHP_SCHEMA_UNCHANGED_CLASS_IN_ADDED_SCHEMA
Language=English
Conflicting schema element state: a class marked unchanged was found in a schema to be added.
.
MessageId=4025
SymbolicName=SHP_OVERRIDE_FILENAME_NOT_QUALIFIED
Language=English
The SHP filename override '%1$ls' is not a fully-qualified path.
.
MessageId=4026
SymbolicName=SHP_UNSUPPORTED_CLASSTYPE
Language=English
The '%1$ls' class type is not supported by Shp.
.
MessageId=4027
SymbolicName=SHP_COLUMN_WIDTH_INVALID
Language=English
The value '%1$d' is invalid for the column width (must be between 0 and 255).
.
MessageId=4028
SymbolicName=SHP_COLUMN_SCALE_INVALID
Language=English
The value '%1$d' is invalid for the column scale (must be between 0 and 255).
.
MessageId=4029
SymbolicName=SHP_CLASS_NAME_INVALID
Language=English
The class name '%1$ls' is invalid.
.
MessageId=4030
SymbolicName=SHP_APPLY_CONFIGURED
Language=English
Apply schema after configuration or override is not allowed.
.
MessageId=4031
SymbolicName=SHP_NOT_FOUND
Language=English
Shp file '%1$ls' was not found.
.
MessageId=4032
SymbolicName=SHP_APPLY_SINGLE_SHP
Language=English
Apply schema on a single shp is not allowed.
.
MessageId=4033
SymbolicName=SHP_INVALID_PHYSICAL_SCHEMA_PROVIDER_NAME
Language=English
Invalid Physical Schema Mapping Provider Name.
.
MessageId=4034
SymbolicName=SHP_INVALID_PHYSICAL_SCHEMA_PROVIDER_VERSION
Language=English
Invalid Physical Schema Mapping Provider Version.
.
MessageId=4035
SymbolicName=SHP_DUPLICATE_COLUMN
Language=English
Duplicate field name '%1$ls' in '%2$ls'.
.
MessageId=4036
SymbolicName=SHP_CANT_MODIFY_NON_EMPTY_CLASS
Language=English
Can't modify class '%1$ls' because it contains data.
.
MessageId=4037
SymbolicName=SHP_CANT_DELETE_NON_EMPTY_CLASS
Language=English
Can't delete class '%1$ls' because it contains data.
.
MessageId=4038
SymbolicName=SHP_CANT_DELETE_NON_EMPTY_SCHEMA
Language=English
Can't delete schema '%1$ls' because class '%2$ls' contains data.
.
MessageId=4039
SymbolicName=SHP_DEFAULT_SCHEMA_NAME
Language=English
Default
.
MessageId=4040
SymbolicName=SHP_DEFAULT_SCHEMA_DESCRIPTION
Language=English
Default schema.
.
MessageId=4041
SymbolicName=SHP_INVALID_INDEXOF_PARAM
Language=English
Only ordering properties or the identity property are allowed as IndexOf() argument.
.
MessageId=4042
SymbolicName=SHP_FIELD_NAME_INVALID
Language=English
The field '%1$ls' width is invalid (must be between 1 and 12).
.
;//************************************************************************************
;//************************************************************************************
;//** VERSION-RELATED MESSAGES 
;//************************************************************************************
;//************************************************************************************


;//************************************************************************************
;//************************************************************************************
;//** FILTERS & EXPRESSIONS MESSAGES 
;//************************************************************************************
;//************************************************************************************

MessageId=6001
SymbolicName=SHP_EXPRESSION_NULL_PARAMETER
Language=English
Null parameter.
.
MessageId=6002
SymbolicName=SHP_EXPRESSION_INVALID_PARAMETER_TYPE
Language=English
Invalid parameter type.
.

;//************************************************************************************
;//************************************************************************************
;//** FEATURE COMMAND MESSAGES 
;//************************************************************************************
;//************************************************************************************

MessageId=7001
SymbolicName=SHP_COMMAND_PARAMETERS_NOT_SUPPORTED
Language=English
Command parameters are not supported.
.
MessageId=7002
SymbolicName=SHP_COMMAND_TIMEOUT_NOT_SUPPORTED
Language=English
Command timeout is not supported.
.
MessageId=7003
SymbolicName=SHP_COMMAND_ERROR_READING_SHP
Language=English
Error #%1$d occured reading shape file '%2$ls'.
.
MessageId=7004
SymbolicName=SHP_COMMAND_PROPERTY_NOT_FOUND
Language=English
Property '%1$ls' is not part of class '%2$ls'.
.
MessageId=7005
SymbolicName=SHP_VALUE_TYPE_MISMATCH
Language=English
Value type to insert, update or retrieve doesn't match the type (%1$ls) of property '%2$ls'.
.
MessageId=7006
SymbolicName=SHP_FEATID_READONLY
Language=English
The property '%1$ls' is read-only.
.
MessageId=7007
SymbolicName=SHP_INVALID_DATA_TYPE
Language=English
The value for property '%1$ls' is not '%2$ls'.
.
MessageId=7008
SymbolicName=SHP_INVALID_GEOMETRY_TYPE
Language=English
The geometry type ('%1$ls') of the feature conflicts with the shape file type ('%2$ls').
.
MessageId=7009
SymbolicName=SHP_LITERAL_GEOM_VALUES_ONLY
Language=English
Only literal geometry values supported.
.
MessageId=7010
SymbolicName=SHP_DELETE_SETUP_FAILED
Language=English
Failed to initialize the delete operation.
.
MessageId=7011
SymbolicName=SHP_DELETE_FAILED
Language=English
Delete failed.
.
MessageId=7012
SymbolicName=SHP_ILLEGAL_COLUMN_ASSIGNMENT
Language=English
Illegal column assignment.
.
MessageId=7013
SymbolicName=SHP_VALUE_TOO_WIDE_FOR_COLUMN
Language=English
The value '%1$ls' is too wide for column '%2$ls'.
.
MessageId=7014
SymbolicName=SHP_UPDATE_SETUP_FAILED
Language=English
Failed to initialize the update operation.
.
MessageId=7015
SymbolicName=SHP_READ_ONLY_UPDATE
Language=English
Cannot update read-only file '%1$ls'.
.
MessageId=7016
SymbolicName=SHP_READ_ONLY_INSERT
Language=English
Cannot insert into read-only file '%1$ls'.
.
MessageId=7017
SymbolicName=SHP_READ_ONLY_DELETE
Language=English
Cannot delete from read-only file '%1$ls'.
.
MessageId=7018
SymbolicName=SHP_INVALID_LITERAL_TYPE
Language=English
Invalid literal type '%1$d'.
.

;//************************************************************************************
;//************************************************************************************
;//** LOCK-RELATED MESSAGES
;//************************************************************************************
;//************************************************************************************

MessageId=8001
SymbolicName=SHP_LOCKING_NOT_SUPPORTED
Language=English
Locking not supported (%1$ls).
.

;//************************************************************************************
;//************************************************************************************
;//** SPATIAL CONTEXT MESSAGES 
;//************************************************************************************
;//************************************************************************************

MessageId=9001
SymbolicName=SHP_SPATIALCONTEXT_INVALIDCONFIGURATION
Language=English
Cannot have more than one spatial context in a configuration file.
.
MessageId=9002
SymbolicName=SHP_SPATIALCONTEXT_INVALIDOPTION
Language=English
Unsupported option.
.
MessageId=9003
SymbolicName=SHP_SPATIALCONTEXT_EMPTYWKT
Language=English
WKT cannot be empty.
.
MessageId=9004
SymbolicName=SHP_SPATIALCONTEXT_INVALIDWKT
Language=English
Invalid WKT. Cannot extract coordinate system name.
.
MessageId=9005
SymbolicName=SHP_SPATIALCONTEXT_CONFLICTINGWKT
Language=English
Spatial Context name conflicts with the coordinate system name in WKT.
.

;//************************************************************************************
;//************************************************************************************
;//** TRANSACTION MESSAGES 
;//************************************************************************************
;//************************************************************************************

MessageId=10001
SymbolicName=SHP_CONNECTION_TRANSACTIONS_NOT_SUPPORTED
Language=English
Shp Provider does not support transactions.
.

;//************************************************************************************
;//************************************************************************************
;//** READER MESSAGES 
;//************************************************************************************
;//************************************************************************************

MessageId=11001
SymbolicName=SHP_READER_NOT_READY
Language=English
'%1$ls' reader not ready.
.
MessageId=11002
SymbolicName=SHP_READER_EXHAUSTED
Language=English
'%1$ls' reader exhausted.
.
MessageId=11003
SymbolicName=SHP_READER_PROPERTY_NULL
Language=English
The property '%1$ls' is NULL.
.

;//************************************************************************************
;//************************************************************************************
;//** Spatial Index related messages 
;//************************************************************************************
;//************************************************************************************

MessageId=12001
SymbolicName=SHP_SI_UNABLE_TO_OPEN
Language=English
The file '%1$ls' cannot be opened.
.
MessageId=12002
SymbolicName=SHP_SI_NOT_AN_SSI
Language=English
Corrupted Spatial Index file '%1$ls'.
.
MessageId=12003
SymbolicName=SHP_SI_READ_ERROR
Language=English
Error reading the file '%1$ls'.
.
MessageId=12004
SymbolicName=SHP_SI_WRITE_ERROR
Language=English
Error writing the file '%1$ls'.
.
MessageId=12005
SymbolicName=SHP_SI_VERSION_MISMATCH
Language=English
Version mismatch '%1$ls', expected %2$d, found %3$d.
.
MessageId=12006
SymbolicName=SHP_SI_READ_ONLY
Language=English
File '%1$ls' is read-only.
.
MessageId=12007
SymbolicName=SHP_SI_SEARCH_NOT_INITIALIZED
Language=English
Spatial Index search has not been initialized.
.
MessageId=12008
SymbolicName=SHP_SI_INVALID_PRECISION
Language=English
Invalid floating point precision (%2$d).
.
MessageId=12009
SymbolicName=SHP_SI_OBJECT_NOT_FOUND
Language=English
Specified object was not found in Spatial Index.
.
MessageId=12010
SymbolicName=SHP_SI_CANT_CREATE_TEMP_FILE
Language=English
Cannot create temporary file while defragmenting the Spatial Index.
.
MessageId=12011
SymbolicName=SHP_SI_TEMP_FILE_WRITE_ERROR
Language=English
Cannot write temporary file while defragmenting the Spatial Index.
.
MessageId=12012
SymbolicName=SHP_SI_CANT_REPLACE_SSI
Language=English
Cannot replace the Spatial Index.
.
MessageId=12013
SymbolicName=SHP_SI_OPERATOR_NOT_SUPPORTED
Language=English
The spatial operation %1$ls is not supported.
.
MessageId=12014
SymbolicName=SHP_SI_INITIALIZATION_FAILED
Language=English
Error #%1$d occured initializating the RTree.
.
MessageId=12015
SymbolicName=SHP_SI_TRAVERSAL_NOT_INITIALIZED
Language=English
Spatial Index traversal has not been initialized.
.
MessageId=12016
SymbolicName=SHP_SI_INVALID_MINIMUM_NODES
Language=English
Invalid minimum entries per node (%2$d).
.
MessageId=12017
SymbolicName=SHP_SI_INVALID_MAXIMUM_NODES
Language=English
Invalid maximum entries per node (%2$d).
.

;//************************************************************************************
;//************************************************************************************
;//** GEOMETRY MESSAGES 
;//************************************************************************************
;//************************************************************************************

MessageId=13001
SymbolicName=SHP_CREATE_GEOMETRY_FAILED
Language=English
Geometry creation failed for '%1$ls'.
.

;//************************************************************************************
;//************************************************************************************
;//** CONNECTION INFORMATIONS
;//************************************************************************************
;//************************************************************************************

MessageId=14001
SymbolicName=SHP_PROVIDER_DISPLAY_NAME
Language=English
OSGeo FDO Provider for SHP
.
MessageId=14002
SymbolicName=SHP_PROVIDER_DESCRIPTION
Language=English
Read/write access to spatial and attribute data in an ESRI SHP file.
.