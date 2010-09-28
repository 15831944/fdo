//
// Copyright (C) 2004-2006  Autodesk, Inc.
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
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "stdafx.h"
#include "ArcSDESpatialContextSQLReader.h"


ArcSDESpatialContextSQLReader::ArcSDESpatialContextSQLReader(ArcSDEConnection* connection, FdoISQLDataReader* sqlReader)
{
    mConnection = FDO_SAFE_ADDREF(connection);
    mSqlReader = FDO_SAFE_ADDREF(sqlReader);
    mOnRowOfData = false;
}

ArcSDESpatialContextSQLReader::~ArcSDESpatialContextSQLReader(void)
{
}

void ArcSDESpatialContextSQLReader::Validate(void)
{
    if (!mOnRowOfData)
        throw FdoException::Create (NlsMsgGet (ARCSDE_READER_EXHAUSTED, "Reader is exhausted."));
}

FdoString* ArcSDESpatialContextSQLReader::GetName()
{
    Validate();

    if (mSpatialContextName == L"")
    {
        FdoStringP authname;
        if (!mSqlReader->IsNull(AdjustColumnName(L"auth_name")))
            authname = mSqlReader->GetString(AdjustColumnName(L"auth_name"));
        long srid = 0;
        if (!mSqlReader->IsNull(AdjustColumnName(L"srid")))
            srid = mSqlReader->GetInt32(AdjustColumnName(L"srid"));

        mSpatialContextName = ArcSDESpatialContextUtility::GetSpatialContextName(authname, srid);
    }

    return mSpatialContextName;
}

FdoString* ArcSDESpatialContextSQLReader::GetDescription()
{
    Validate();

    if (mDescription == L"")
    {
        if (!mSqlReader->IsNull(AdjustColumnName(L"description")))
            mDescription = mSqlReader->GetString(AdjustColumnName(L"description"));

        // NOTE: for some unknown scary reason, we get unexpected garbage at the end of the description,
        //    even though the description appears valid in the RDBMS, so we need to trim it:
        //TODO: see if we can fix this issue!!
        // NOTE: if FdoStringP::Left() doesnt find the given suffix (e.g. in the case of a natively-created spatial reference system),
        //    it returns the entire string unchanged.
        FdoStringP temp = mDescription.Left(SPATIALCONTEXT_DESC_SUFFIX);
        mDescription = temp; // avoid problems copying from self
    }

    return mDescription;
}

FdoString* ArcSDESpatialContextSQLReader::GetCoordinateSystem()
{
    Validate();

    if (mCoordSysName == L"")
    {
        LONG id = -1;
        LONG         lResult = SE_SUCCESS;
        SE_COORDREF  coordref;

        FdoString* coordSysWkt = GetCoordinateSystemWkt();

        // Get coordref's id, then store it in mCoordSysName:
        // ECO 10353b "Specify consistent handling of unknown coordinate systems":
        if (0==wcscmp(coordSysWkt, SPATIALCONTEXT_CORDSYS_WKT_UNKNOWN_FDO))
        {
            mCoordSysName = SPATIALCONTEXT_CORDSYS_NAME_UNKNOWN_FDO;
        }
        else
        {
            // Get coordref:
            lResult = SE_coordref_create (&coordref);
            handle_sde_err<FdoException>(lResult, __FILE__, __LINE__, ARCSDE_UNEXPECTED_ERROR, "Unexpected error encountered in ArcSDE Provider.");
            CHAR* mbCoordSysWkt = NULL;
            sde_wide_to_multibyte(mbCoordSysWkt, coordSysWkt);
            lResult = SE_coordref_set_by_description (coordref, mbCoordSysWkt);
            handle_sde_err<FdoException>(lResult, __FILE__, __LINE__, ARCSDE_UNEXPECTED_ERROR, "Unexpected error encountered in ArcSDE Provider.");
            lResult = SE_coordref_get_id(coordref, &id);
            handle_sde_err<FdoException>(lResult, __FILE__, __LINE__, ARCSDE_FAILED_TO_READ_SRS, "Failed to get or set information for this ArcSDE Spatial Reference System.");
            wchar_t wBuffer[20];
            mCoordSysName = FdoCommonOSUtil::ltow(id, wBuffer, ELEMENTS(wBuffer));

            // Cleanup:
            SE_coordref_free(coordref);
        }

    }

    return mCoordSysName;
}


FdoString* ArcSDESpatialContextSQLReader::GetCoordinateSystemWkt()
{
    Validate();

    if (mCoordSysWkt == L"")
    {
        if (!mSqlReader->IsNull(AdjustColumnName(L"srtext")))
            mCoordSysWkt = mSqlReader->GetString(AdjustColumnName(L"srtext"));
        else
            mCoordSysWkt = L"";

        // ECO 10353b "Specify consistent handling of unknown coordinate systems":
        if (0==sde_strcmp(sde_cstwc(mCoordSysWkt), SPATIALCONTEXT_COORDYS_WKT_UNKNOWN_SDE))
        {
            mCoordSysWkt = SPATIALCONTEXT_CORDSYS_WKT_UNKNOWN_FDO;
        }
    }

    return mCoordSysWkt;
}

FdoSpatialContextExtentType ArcSDESpatialContextSQLReader::GetExtentType()
{
    Validate();

    return FdoSpatialContextExtentType_Static;  //ok
}

FdoByteArray* ArcSDESpatialContextSQLReader::GetExtent()
{
    Validate();

    LONG         lResult = SE_SUCCESS;
    SE_ENVELOPE  envelope;

    // calculate envelope manually:
    double dFalseX  = mSqlReader->GetDouble(AdjustColumnName(L"falsex"));
    double dFalseY  = mSqlReader->GetDouble(AdjustColumnName(L"falsey"));
    double dXYunits = mSqlReader->GetDouble(AdjustColumnName(L"xyunits"));
    envelope.minx = dFalseX;
    envelope.maxx = dFalseX + (2147483645.0 / dXYunits);
    envelope.miny = dFalseY;
    envelope.maxy = dFalseY + (2147483645.0 / dXYunits);

    // Convert envelope to FGF byte array:
    FdoByteArray* byteArray = ArcSDESpatialContextUtility::EnvelopeToFgf(envelope);

    // Return byte array:
    return byteArray;
}

const double ArcSDESpatialContextSQLReader::GetXYTolerance()
{
    Validate();
 
    double xyUnits = mSqlReader->GetDouble(AdjustColumnName(L"xyunits"));

    // Convert ArcSDE XY units to FDO xy tolerance:
    double fdoXYTolerance = 1.0 / xyUnits;

    return fdoXYTolerance;
}

const double ArcSDESpatialContextSQLReader::GetZTolerance()
{
    Validate();

    double zUnits = 1.0;
    if (!mSqlReader->IsNull(AdjustColumnName(L"zunits")))
        zUnits = mSqlReader->GetDouble(AdjustColumnName(L"zunits"));

    // Convert ArcSDE XY units to FDO xy tolerance:
    double fdoZTolerance = 1.0 / zUnits;

    return fdoZTolerance;
}

const bool ArcSDESpatialContextSQLReader::IsActive()
{
    Validate();

    FdoString *currentName = GetName();
    FdoString *activeName = mConnection->GetActiveSpatialContext();

    return (activeName==NULL) ? false : (0==wcscmp(currentName, activeName));
}

bool ArcSDESpatialContextSQLReader::ReadNext()
{
    // de-cache all cached data:
    mSpatialContextName = (FdoString*)NULL;
    mDescription = (FdoString*)NULL;
    mCoordSysWkt = (FdoString*)NULL;
    mCoordSysName = (FdoString*)NULL;

    mOnRowOfData = mSqlReader->ReadNext();
    return mOnRowOfData;
}

void ArcSDESpatialContextSQLReader::Dispose()
{
    delete this;
}

long ArcSDESpatialContextSQLReader::GetSRID()
{
    Validate();

    LONG lSRID = 0;
    if (!mSqlReader->IsNull(AdjustColumnName(L"srid")))
        lSRID = mSqlReader->GetInt32(AdjustColumnName(L"srid"));

    return lSRID;
}


FdoStringP ArcSDESpatialContextSQLReader::AdjustColumnName(FdoString *name)
{
    // Different RDBMS's have different default case for column names
    FdoStringP nameCorrected = name;
    if (mConnection->RdbmsId() == SE_DBMS_IS_SQLSERVER)
        return nameCorrected.Lower();
    else
        return nameCorrected.Upper();
}

