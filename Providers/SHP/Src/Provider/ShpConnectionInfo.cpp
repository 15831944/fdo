/*
 * 
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

#include "stdafx.h"

#include <FdoCommonStringUtil.h>

ShpConnectionInfo::ShpConnectionInfo (ShpConnection* connection) :
    mConnection (connection)
{
    // NOTE: we use a weak reference here to avoid a circular dependancy:
    // FDO_SAFE_ADDREF(mConnection.p);
}

ShpConnectionInfo::~ShpConnectionInfo (void)
{
}

void ShpConnectionInfo::Dispose()
{
    delete this;
}

void ShpConnectionInfo::validate ()
{
    if (mConnection == NULL)
        throw FdoException::Create (NlsMsgGet (SHP_CONNECTION_INVALID, "Connection is invalid."));
}

/// <summary>Gets the name of the feature provider.</summary>
/// <returns>Returns the provider name</returns>
FdoString* ShpConnectionInfo::GetProviderName ()
{
    validate ();
    return (SHP_PROVIDER_NAME);
}

/// <summary>Gets the display name of the feature provider.</summary>
/// <returns>Returns the provider's display name</returns>
FdoString* ShpConnectionInfo::GetProviderDisplayName()
{
    validate ();
    return NlsMsgGet(SHP_PROVIDER_DISPLAY_NAME, SHP_PROVIDER_DEFAULT_DISPLAY_NAME);
}

/// <summary>Gets the description of the feature provider.</summary>
/// <returns>Returns the provider description</returns>
FdoString* ShpConnectionInfo::GetProviderDescription ()
{
    validate ();
    return NlsMsgGet(SHP_PROVIDER_DESCRIPTION, SHP_PROVIDER_DEFAULT_DESCRIPTION);
}

/// <summary>Gets the version of the feature provider.</summary>
/// <returns>Returns provider version</returns>
FdoString* ShpConnectionInfo::GetProviderVersion ()
{
    validate ();
    return (SHP_PROVIDER_VERSION);
}

/// <summary>Gets the version of the Feature Data Objects specification to which this provider conforms.</summary>
/// <returns>Returns FDO version supported.</returns>
FdoString* ShpConnectionInfo::GetFeatureDataObjectsVersion ()
{
    validate ();
    return (SHP_FDO_VERSION);
}

/// <summary>Gets the FdoIConnectionPropertyDictionary interface that can be used to dynamically query and set the properties required to establish a connection.</summary>
/// <returns>Returns the property dictionary</returns>
FdoIConnectionPropertyDictionary* ShpConnectionInfo::GetConnectionProperties ()
{
    validate ();
    if (mPropertyDictionary == NULL)
    {
        mPropertyDictionary = new FdoCommonConnPropDictionary (mConnection);

        // Define all the connection properties:
        char* mbPropName = NULL;
        wide_to_multibyte(mbPropName, CONNECTIONPROPERTY_DEFAULT_FILE_LOCATION);
        FdoPtr<ConnectionProperty> property = new ConnectionProperty (CONNECTIONPROPERTY_DEFAULT_FILE_LOCATION,
                NlsMsgGet(SHP_CONNECTION_PROPERTY_DEFAULT_FILE_LOCATION, mbPropName),
                L"", false, false, false, true, true, false, false, 0, NULL);
        mPropertyDictionary->AddProperty(property);
        wide_to_multibyte(mbPropName, CONNECTIONPROPERTY_TEMPORARY_FILE_LOCATION);
        property = new ConnectionProperty (CONNECTIONPROPERTY_TEMPORARY_FILE_LOCATION,
                NlsMsgGet(SHP_CONNECTION_PROPERTY_TEMPORARY_FILE_LOCATION, mbPropName),
                L"", false, false, false, false, true, false, false, 0, NULL);
        mPropertyDictionary->AddProperty(property);
    }
    return (FDO_SAFE_ADDREF(mPropertyDictionary.p));
}

/// <summary>Returns the provider type. A provider can be a file-based, database-based or
/// web-based provider. The valid values the function may return are defined in
/// the enumeration FdoProviderDatastoreType. The enumeration includes the following
/// values: FdoProviderDatastoreType_Unknown, FdoProviderDatastoreType_File,
/// FdoProviderDatastoreType_DatabaseServer, FdoProviderDatastoreType_WebServer.</summary>
/// <returns>Returns the provider data store type.</returns>
FdoProviderDatastoreType ShpConnectionInfo::GetProviderDatastoreType()
{
    return FdoProviderDatastoreType_File;
}

/// <summary>File-based providers depend on a various files. This function returns a list
/// of fully qualified dependend file names. The return parameter will be NULL if
/// the provider is not a file-based provider.</summary>
/// <returns>Returns the list of fully-qualified dependend file names if the provider is a
/// file-based provider, NULL otherwise.</returns>
FdoStringCollection* ShpConnectionInfo::GetDependentFileNames()
{
    ShpFileSet      *shpFileSet          = NULL;
    ShapeFile       *shpFile             = NULL;
    ShapeDBF        *shpDbfFile          = NULL;
    ShapePRJ        *shpPrjFile          = NULL;
    ShapeCPG        *shpCpgFile          = NULL;
    ShapeIndex      *shpIndexFile        = NULL;
    ShpSpatialIndex *shpSpatialIndexFile = NULL;

    // If the connection is not yet open return NULL.
    if (mConnection->GetConnectionState() != FdoConnectionState_Open)
        return NULL;

    if (mDependentFiles == NULL)
    {
        mDependentFiles = FdoStringCollection::Create();

        // The connection can be either to a single file or a single non-recursive
        // directory. To get the list of all dependent files, it is necessary to
        // get the SHP physical schema first and use it to process the list of SHP
        // file sets. For each file set, it is required to test whether or not the
        // file represents a temporary file. If this is the case then the file is
        // not added to the resulting file list.
        FdoPtr<ShpPhysicalSchema> shpPhysicalSchema = mConnection->GetPhysicalSchema();
        FdoInt32 sphFileSetCount = shpPhysicalSchema->GetFileSetCount();
        for (FdoInt32 index = 0; index < sphFileSetCount; index++)
        {
            shpFileSet = shpPhysicalSchema->GetFileSet(index);

            shpFile = shpFileSet->GetShapeFile();
            if ((shpFile != NULL) && (!shpFile->IsTemporaryFile()))
                mDependentFiles->Add(shpFile->GetAbsolutePath(shpFile->FileName()));

            shpDbfFile = shpFileSet->GetDbfFile();
            if ((shpDbfFile != NULL) && (!shpDbfFile->IsTemporaryFile()))
                mDependentFiles->Add(shpDbfFile->GetAbsolutePath(shpDbfFile->FileName()));

            shpPrjFile = shpFileSet->GetPrjFile();
            if ((shpPrjFile != NULL) && (!shpPrjFile->IsTemporaryFile()))
                mDependentFiles->Add(shpPrjFile->GetAbsolutePath(shpPrjFile->FileName()));

            shpCpgFile = shpFileSet->GetCpgFile();
            if ((shpCpgFile != NULL) && (!shpCpgFile->IsTemporaryFile()))
                mDependentFiles->Add(shpCpgFile->GetAbsolutePath(shpCpgFile->FileName()));

            shpIndexFile = shpFileSet->GetShapeIndexFile();
            if ((shpIndexFile != NULL) && (!shpIndexFile->IsTemporaryFile()))
                mDependentFiles->Add(shpIndexFile->GetAbsolutePath(shpIndexFile->FileName()));

            shpSpatialIndexFile = shpFileSet->GetSpatialIndex();
            if ((shpSpatialIndexFile != NULL) && (!shpSpatialIndexFile->IsTemporaryFile()))
                mDependentFiles->Add(shpSpatialIndexFile->GetAbsolutePath(shpSpatialIndexFile->FileName()));
        }
    }
    return (FDO_SAFE_ADDREF(mDependentFiles.p));
}
