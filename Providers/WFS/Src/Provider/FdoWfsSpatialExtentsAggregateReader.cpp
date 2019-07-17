// 
//  
//  Copyright (C) 2004-2006  Autodesk, Inc.
//  
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//  
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//  
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//  

#include "stdafx.h"
#include "FdoWfsConnection.h"
#include "FdoWfsSpatialExtentsAggregateReader.h"
#include "FdoWfsFeatureTypeList.h"
#include "FdoWfsServiceMetadata.h"
#include "FdoWfsFeatureType.h"

FdoWfsSpatialExtentsAggregateReader::FdoWfsSpatialExtentsAggregateReader(FdoWfsConnection* conn, FdoIdentifier* className, FdoString* aliasName) :
    m_ReaderIndex(-1),
    m_AliasName(aliasName)
{
    // Get the cached metadata and feature types 
    FdoPtr<FdoWfsServiceMetadata> serviceMetadata = conn->GetServiceMetadata ();
    FdoPtr<FdoWfsFeatureType> featureType = serviceMetadata->GetFeatureType(className);
    if (featureType == NULL)
    {
        // handle the case which 'real' class name ends with "_" or "_Feature"
        FdoStringP cName = className->GetText();
        FdoStringP realName = cName + L"_";
        FdoPtr<FdoIdentifier> realNameIdentifier = FdoIdentifier::Create(realName);
        featureType = serviceMetadata->GetFeatureType(realNameIdentifier);
        if (featureType == NULL)
        {
            FdoInt32 len = cName.GetLength();
            FdoStringP suffix = cName.Mid(len - 8, 8);
            if (wcsicmp(L"_Feature", suffix) == 0)
                realName = cName.Mid(0, len - 7);
            realNameIdentifier = FdoIdentifier::Create(realName);
            featureType = serviceMetadata->GetFeatureType(realNameIdentifier);
        }
    }
    if (featureType == NULL)
        throw FdoCommandException::Create (
            NlsMsgGet(FDO_NLSID(WFS_NAMED_FEATURETYPE_NOT_FOUND), (FdoString*)className->GetText()));

    // Get the total extent of the feature type
    FdoOwsGeographicBoundingBoxCollectionP extents = featureType->GetSRSExtents();

    FdoString* srsName = featureType->GetSRS();
    // Build an extents geometry out of the spatial extents only in case CS of the layer is in default CS (EPSG:4326 or CRS:4326);
    if (extents->GetCount() != 0 && (FdoCommonOSUtil::wcsicmp(srsName, L"EPSG:4326") == 0 || FdoCommonOSUtil::wcsicmp(srsName, L"CRS:4326") == 0)) 
    {
        // Get the extents from the bounding box collection
        FdoOwsGeographicBoundingBoxP bbox = extents->GetExtents();
        
        // Copy the extent values to an array of doubles
        double ordinates[10];
        ordinates[0] = bbox->GetWestBoundLongitude ();
        ordinates[1] = bbox->GetSouthBoundLatitude ();
        ordinates[2] = bbox->GetEastBoundLongitude ();
        ordinates[3] = bbox->GetSouthBoundLatitude ();
        ordinates[4] = bbox->GetEastBoundLongitude ();
        ordinates[5] = bbox->GetNorthBoundLatitude ();
        ordinates[6] = bbox->GetWestBoundLongitude ();
        ordinates[7] = bbox->GetNorthBoundLatitude ();
        ordinates[8] = bbox->GetWestBoundLongitude ();
        ordinates[9] = bbox->GetSouthBoundLatitude ();

        // Create a linear ring using the bounding box ordinates 
        FdoPtr<FdoFgfGeometryFactory> gf = FdoFgfGeometryFactory::GetInstance();
        FdoPtr<FdoILinearRing> linearRing = gf->CreateLinearRing(FdoDimensionality_XY, 10, ordinates);

        // Create a polygon geometry representing the extents from the linear ring
        m_Extents = gf->CreatePolygon(linearRing, NULL);
    }
}

FdoWfsSpatialExtentsAggregateReader::~FdoWfsSpatialExtentsAggregateReader()
{
}

void FdoWfsSpatialExtentsAggregateReader::Dispose()
{
    delete this;
}

bool FdoWfsSpatialExtentsAggregateReader::ReadNext()
{
    m_ReaderIndex++;

    if (m_ReaderIndex==0) {
        PopulatePropertyValueCache();
    }

    return (m_ReaderIndex==0);
}

void FdoWfsSpatialExtentsAggregateReader::Close()
{
}

FdoInt32 FdoWfsSpatialExtentsAggregateReader::GetItemCount(void)
{
    return 1;  // always one property in result set
}

FdoString* FdoWfsSpatialExtentsAggregateReader::GetItemName(FdoInt32 i)
{
    if (i!=0) {
        throw FdoException::NLSGetMessage(FDO_NLSID(FDO_5_INDEXOUTOFBOUNDS));
    }

    return (FdoString*)m_AliasName;
}

FdoInt32 FdoWfsSpatialExtentsAggregateReader::GetItemIndex(FdoString* itemName)  
{
    if (0!=wcscmp(itemName, (FdoString*)m_AliasName)) {
        throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_1_INVALID_INPUT_ON_CLASS_FUNCTION),
                                                               L"FdoWfsSpatialExtentsAggregateReader::GetItemIndex",
                                                               L"itemName"));
    }

    return 0;
}

// We only support geometry values (spatialextents to be precise)
FdoPropertyType FdoWfsSpatialExtentsAggregateReader::GetItemType(FdoInt32 i)
{
    if (i!=0) {
        throw FdoException::NLSGetMessage(FDO_NLSID(FDO_5_INDEXOUTOFBOUNDS));
    }

    return FdoPropertyType_GeometricProperty;
}

// We only support geometry values (spatialextents to be precise), so any call to this function will fail
FdoDataType FdoWfsSpatialExtentsAggregateReader::GetItemDataType(FdoInt32 i)
{
    throw FdoException::NLSGetMessage(FDO_NLSID(WFS_UNSUPPORTED_FUNCTION), L"FdoWfsSpatialExtentsAggregateReader::GetItemDataType");
}

// Retrieve data values; these are guaranteed to only be called once per row of data:
void FdoWfsSpatialExtentsAggregateReader::GetGeometryForCache(FdoIdentifier *itemName, FdoByteArray **byteArray, bool *bIsNull)
{
    FdoPtr<FdoFgfGeometryFactory> gf = FdoFgfGeometryFactory::GetInstance();
    FdoByteArray* geomFGF = !m_Extents ? NULL : gf->GetFgf(m_Extents);

    if (bIsNull) {
        *bIsNull = !m_Extents;
    }

    if (byteArray) {
        *byteArray = geomFGF;
    }
}

// Retrieve information about the row of items:
bool FdoWfsSpatialExtentsAggregateReader::ReaderHasData(void)
{
    // return false IF before first row or after last row
    return (m_ReaderIndex==0);  
}
