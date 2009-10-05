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
#include "ArcSDEReader.h"
#include "ArcSDEUtils.h"


ArcSDEReader::ArcSDEReader (ArcSDEConnection *connection, FdoClassDefinition* fdoClassDef, FdoIdentifierCollection* propertiesToSelect) :
    mStream (NULL),
    mReady (false),
    mColumnCount (0),
    mColumnDefs (NULL),
    mConnection(connection),
    mClassDef (fdoClassDef),
    m_bBindVariablesInitialized(false),
    mDistinct (false),
    mSelectingAggregates (false),
    mOrderingOption (FdoOrderingOption_Ascending),
    mStreamStatsIndex (-1L),
    mRowNotValidated(true)
{
    FDO_SAFE_ADDREF(mConnection.p);
    FDO_SAFE_ADDREF(mClassDef.p);

    // Populate mSelectIds with the FdoIdentifier's the user is selecting:
    if ((propertiesToSelect == NULL) || (propertiesToSelect->GetCount() == 0))  // select *
    {
        mSelectIds = FdoIdentifierCollection::Create();
        if (mClassDef != NULL)
        {
            FdoPtr<FdoPropertyDefinitionCollection> properties = mClassDef->GetProperties ();
            for (FdoInt32 i=0; i<properties->GetCount (); i++)
            {
                FdoPtr<FdoPropertyDefinition> property = properties->GetItem(i);
                FdoPtr<FdoIdentifier> propId = FdoIdentifier::Create(property->GetName());
                mSelectIds->Add(propId);
            }
        }
    }
    else // not select *
        mSelectIds = FDO_SAFE_ADDREF(propertiesToSelect);
}

ArcSDEReader::~ArcSDEReader (void)
{
    LONG result;

    for (int i = 0; i < mColumnCount; i++)
        delete mColumnDefs[i];
    delete[] mColumnDefs;

    if (mConnection->GetConnectionState() == FdoConnectionState_Open) // connection close frees all it's streams
        if (NULL != mStream)
        {
            result = SE_stream_free (mStream);
            mStream = NULL;
            // we allow the possibility that the stream was closed when the SE_CONNECTION was closed
            if (SE_INVALID_STREAM != result)
                handle_sde_err<FdoCommandException>(mConnection->GetConnection (), result, __FILE__, __LINE__, ARCSDE_STREAM_FREE, "Stream free failed.");
        }
}

void ArcSDEReader::validate ()
{
    if (mConnection == NULL)
        throw FdoException::Create (NlsMsgGet (ARCSDE_CONNECTION_INVALID, "Connection is invalid."));
    if (!mReady)
        throw FdoException::Create(NlsMsgGet1(ARCSDE_READER_NOT_READY, "Must %1$ls prior to accessing reader.", L"ReadNext"));
    if (mStream == NULL)
        throw FdoException::Create (NlsMsgGet (ARCSDE_READER_CLOSED, "Reader is closed."));

    // Do additional validation for distinct option and if using aggregates:
    if (mDistinct || mSelectingAggregates)
    {
        if (mStreamStatsIndex < 0)
            throw FdoException::Create(NlsMsgGet1(ARCSDE_READER_NOT_READY, "Must %1$ls prior to accessing reader.", L"ReadNext"));

        for (int i=0; i<mColumnCount; i++)
            if (NULL == getColumnDef(i)->mStreamStats)
                throw FdoException::Create(NlsMsgGet(ARCSDE_STREAM_GENERAL_ERROR, "Error encountered while manipulating stream."));

        if ( ((mSelectingAggregates) && (mStreamStatsIndex >= 1))
           || (mDistinct && (mStreamStatsIndex >= getColumnDef(0)->mStreamStats->distinct->num_distinct)) )
            throw FdoException::Create(NlsMsgGet(ARCSDE_READER_EXHAUSTED, "Reader is exhausted."));
    }

    mRowNotValidated = false;
}

/** Do not implement the copy constructor. **/
//ArcSDEReader::ArcSDEReader (const ArcSDEReader &right) { }

void ArcSDEReader::Dispose ()
{
    delete this;
}

// index is 0-based
ArcSDEReader::ColumnDefinition* ArcSDEReader::getColumnDef (FdoInt32 index)
{
    ColumnDefinition* columnDef = NULL;

    if (0 == mColumnCount)
        getColumnDefs();

    // validate index:
    if ((index<0) || (index>=mColumnCount))
        throw FdoCommandException::Create(NlsMsgGet(ARCSDE_INDEX_OUT_OF_RANGE, "The index '%1$d' is out of range."));

    columnDef = mColumnDefs[index];

    return columnDef;
}


/// <summary>Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be FdoDataType_Boolean or an 
/// exception is thrown.</summary>
/// <param name="identifier">Input the property or column name.</param> 
/// <returns>Returns the Boolean value.</returns> 
bool ArcSDEReader::GetBoolean (FdoString* identifier)
{
    throw FdoException::Create (NlsMsgGet(ARCSDE_UNSUPPORTED_DATATYPE_BOOLEAN, "The Boolean data type is not supported by ArcSDE."));
}

/// <summary>Gets the byte value of the specified property. No conversion is 
/// performed, thus the property must be FdoDataType_Byte or an 
/// exception is thrown.</summary>
/// <param name="identifier">Input the property or column name.</param> 
/// <returns>Returns the byte value.</returns> 
FdoByte ArcSDEReader::GetByte (FdoString* identifier)
{
    throw FdoException::Create (NlsMsgGet(ARCSDE_UNSUPPORTED_DATATYPE_BYTE, "The Byte data type is not supported by ArcSDE."));
}

/// <summary> Gets the date and time value of the specified property. No conversion is 
/// performed, thus the property must be FdoDataType_DateTime or an 
/// exception is thrown.</summary>
/// <param name="identifier">Input the property or column name.</param> 
/// <returns>Returns the date and time value.</returns> 
FdoDateTime ArcSDEReader::GetDateTime (FdoString* identifier)
{
    if (mRowNotValidated)
    validate ();

    ColumnDefinition* columnDef = NULL;
    LONG result = 0L;
    struct tm timedate;
    FdoDateTime ret;

    columnDef = getColumnDef (identifier);
    if (columnDef->mPropertyType != FdoDataType_DateTime)
        throw FdoException::Create (NlsMsgGet2(ARCSDE_VALUE_TYPE_MISMATCH, "Value type to insert, update or retrieve doesn't match the type (%1$ls) of property '%2$ls'.", L"FdoDateTime", identifier));
    if (SE_IS_NULL_VALUE == columnDef->mBindIsNull)
        throw FdoException::Create (NlsMsgGet1(ARCSDE_NULL_VALUE, "The value of property '%1$ls' is null.", identifier));

    //if (mSelectingAggregates)
    //    ; // do nothing; an exception will have be thrown by now since the type doesn't match
    if (mDistinct)
        timedate = columnDef->mStreamStats->distinct->values[mStreamStatsIndex].uval.date_val;
    else
        timedate = columnDef->mBindVariable._timedate;

    // Convert struct tm to FdoDateTime:
    ret = SdeDateTime2FdoDateTime(timedate);

    return (ret);
}

/// <summary>Gets the double-precision floating point value of the specified property. No
/// conversion is performed, thus the property must be FdoDataType_Double
/// or an exception is thrown.</summary>
/// <param name="identifier">Input the property or column name.</param> 
/// <returns>Returns the double floating point value</returns> 
double ArcSDEReader::GetDouble (FdoString* identifier)
{
    if (mRowNotValidated)
    validate ();

    ColumnDefinition* columnDef = NULL;
    LONG result = 0L;
    double ret = 0.0;

    columnDef = getColumnDef (identifier);
    if (columnDef->mPropertyType != FdoDataType_Double)
        throw FdoException::Create (NlsMsgGet2(ARCSDE_VALUE_TYPE_MISMATCH, "Value type to insert, update or retrieve doesn't match the type (%1$ls) of property '%2$ls'.", L"FdoDouble", identifier));
    if (SE_IS_NULL_VALUE == columnDef->mBindIsNull)
        throw FdoException::Create (NlsMsgGet1(ARCSDE_NULL_VALUE, "The value of property '%1$ls' is null.", identifier));

    if (mSelectingAggregates)
        ret = GetAggregateValueDouble(columnDef->mStreamStats, GetAggregateFunctionName(columnDef->mIdentifier));
    else if (mDistinct)
        ret = columnDef->mStreamStats->distinct->values[mStreamStatsIndex].uval.double_val;
    else
        ret = columnDef->mBindVariable._double;

    return (ret);
}

/// <summary>Gets the 16-bit integer value of the specified property. No conversion is
/// performed, thus the property must be FdoDataType_Int16 or an exception
/// is thrown.</summary>
/// <param name="identifier">Input the property or column name.</param> 
/// <returns>Returns the FdoInt16 value.</returns> 
FdoInt16 ArcSDEReader::GetInt16 (FdoString* identifier)
{
    if (mRowNotValidated)
    validate ();

    ColumnDefinition* columnDef = NULL;
    LONG result = 0L;
    FdoInt16 ret = 0;

    columnDef = getColumnDef (identifier);
    if (columnDef->mPropertyType != FdoDataType_Int16)
        throw FdoException::Create (NlsMsgGet2(ARCSDE_VALUE_TYPE_MISMATCH, "Value type to insert, update or retrieve doesn't match the type (%1$ls) of property '%2$ls'.", L"FdoInt16", identifier));
    if (SE_IS_NULL_VALUE == columnDef->mBindIsNull)
        throw FdoException::Create (NlsMsgGet1(ARCSDE_NULL_VALUE, "The value of property '%1$ls' is null.", identifier));

    //if (mSelectingAggregates)
    //    ; // do nothing; an exception will have be thrown by now since the type doesn't match
    if (mDistinct)
        ret = (FdoInt16)columnDef->mStreamStats->distinct->values[mStreamStatsIndex].uval.int_val;
    else
        ret = (FdoInt16)columnDef->mBindVariable._short;

    return ret;
}

/// <summary>Gets the 32-bit integer value of the specified property. No conversion is
/// performed, thus the property must be FdoDataType_Int32 or an exception
/// is thrown.</summary>
/// <param name="identifier">Input the property or column name.</param> 
/// <returns>Returns the FdoInt32 value</returns> 
FdoInt32 ArcSDEReader::GetInt32 (FdoString* identifier)
{
    if (mRowNotValidated)
    validate ();

    ColumnDefinition* columnDef = NULL;
    LONG result = 0L;
    FdoInt32 ret = 0L;

    columnDef = getColumnDef (identifier);
    if (columnDef->mPropertyType != FdoDataType_Int32)
        throw FdoException::Create (NlsMsgGet2(ARCSDE_VALUE_TYPE_MISMATCH, "Value type to insert, update or retrieve doesn't match the type (%1$ls) of property '%2$ls'.", L"FdoInt32", identifier));
    if (SE_IS_NULL_VALUE == columnDef->mBindIsNull)
        throw FdoException::Create (NlsMsgGet1(ARCSDE_NULL_VALUE, "The value of property '%1$ls' is null.", identifier));

    //if (mSelectingAggregates)
    //    ; // do nothing; an exception will have be thrown by now since the type doesn't match
    if (mDistinct)
        ret = columnDef->mStreamStats->distinct->values[mStreamStatsIndex].uval.int_val;
    else
        ret = columnDef->mBindVariable._long;

    return ret;
}

/// <summary>Gets the 64-bit integer value of the specified property. No conversion is
/// performed, thus the property must be FdoDataType_Int64 or an exception
/// is thrown.</summary>
/// <param name="identifier">Input the property or column name.</param> 
/// <returns>Returns the FdoInt64 value.</returns> 
FdoInt64 ArcSDEReader::GetInt64 (FdoString* identifier)
{
    if (mRowNotValidated)
    validate ();

    ColumnDefinition* columnDef = NULL;
    LONG result = 0L;
    FdoInt32 ret = 0L;

    columnDef = getColumnDef (identifier);
    if (columnDef->mPropertyType != FdoDataType_Int64)
        throw FdoException::Create (NlsMsgGet2(ARCSDE_VALUE_TYPE_MISMATCH, "Value type to insert, update or retrieve doesn't match the type (%1$ls) of property '%2$ls'.", L"FdoInt64", identifier));
    if (SE_IS_NULL_VALUE == columnDef->mBindIsNull)
        throw FdoException::Create (NlsMsgGet1(ARCSDE_NULL_VALUE, "The value of property '%1$ls' is null.", identifier));

    // Int64 is currently only supported for aggregate functions of type int64:
    if (mSelectingAggregates)
        return GetAggregateValueInt64(columnDef->mStreamStats, GetAggregateFunctionName(columnDef->mIdentifier));
    else
        throw FdoException::Create (NlsMsgGet(ARCSDE_UNSUPPORTED_DATATYPE_INT64, "The Int64 data type is not supported by ArcSDE."));
}

/// <summary>Gets the Single floating point value of the specified property. No
/// conversion is performed, thus the property must be FdoDataType_Single
/// or an exception is thrown.</summary>
/// <param name="identifier">Input the property or column name.</param> 
/// <returns>Returns the single value</returns> 
float ArcSDEReader::GetSingle (FdoString* identifier)
{
    if (mRowNotValidated)
    validate ();

    ColumnDefinition* columnDef = NULL;
    LONG result = 0L;
    float ret = 0.0f;

    columnDef = getColumnDef (identifier);
    if (columnDef->mPropertyType != FdoDataType_Single)
        throw FdoException::Create (NlsMsgGet2(ARCSDE_VALUE_TYPE_MISMATCH, "Value type to insert, update or retrieve doesn't match the type (%1$ls) of property '%2$ls'.", L"FdoSingle", identifier));
    if (SE_IS_NULL_VALUE == columnDef->mBindIsNull)
        throw FdoException::Create (NlsMsgGet1(ARCSDE_NULL_VALUE, "The value of property '%1$ls' is null.", identifier));

    //if (mSelectingAggregates)
    //    ; // do nothing; an exception will have be thrown by now since the type doesn't match
    if (mDistinct)
        ret = (float)columnDef->mStreamStats->distinct->values[mStreamStatsIndex].uval.double_val;
    else
        ret = (float)columnDef->mBindVariable._float;

    return ret;
}

/// <summary>Gets the string value of the specified property. No conversion is
/// performed, thus the property must be FdoDataType_String or an exception
/// is thrown.</summary>
/// <param name="identifier">Input the property or column name.</param> 
/// <returns>Returns the string value</returns> 
FdoString* ArcSDEReader::GetString (FdoString* identifier)
{
    if (mRowNotValidated)
    validate ();

    ColumnDefinition* columnDef = NULL;

    columnDef = getColumnDef (identifier);
    if (columnDef->mPropertyType != FdoDataType_String)
        throw FdoException::Create (NlsMsgGet2(ARCSDE_VALUE_TYPE_MISMATCH, "Value type to insert, update or retrieve doesn't match the type (%1$ls) of property '%2$ls'.", L"FdoString", identifier));
    if (SE_IS_NULL_VALUE == columnDef->mBindIsNull)
        throw FdoException::Create (NlsMsgGet1(ARCSDE_NULL_VALUE, "The value of property '%1$ls' is null.", identifier));

        CHAR *mbValue = NULL;
        if (mSelectingAggregates)
            ; // do nothing; an exception will have be thrown by now since the type doesn't match
        else if (mDistinct)
            mbValue = columnDef->mStreamStats->distinct->values[mStreamStatsIndex].uval.str_val;
        else
            mbValue = columnDef->mBindVariable._string;

    if (NULL == columnDef->mValuePointer)
        {
        columnDef->mValuePointerSize = columnDef->mDataLength + 1;
            columnDef->mValuePointer = new wchar_t[columnDef->mValuePointerSize];
        }

#ifdef SDE_UNICODE
	wcscpy((wchar_t*)columnDef->mValuePointer, sde_pcus2wc(mbValue));
#else
	multibyte_to_wide_noalloc((wchar_t*)columnDef->mValuePointer, mbValue);
#endif

    return (wchar_t*)(columnDef->mValuePointer);
}


/// <summary>Gets a LOBValue reference. The LOB is fully read in and data available.
/// Because no conversion is performed, the property must be FdoDataType_BLOB or
/// FdoDataType_CLOB etc. (a LOB type)
/// <param name="propertyName">Input the property name.</param> 
/// <returns>Returns the reference to LOBValue</returns> 
FdoLOBValue* ArcSDEReader::GetLOB(FdoString* identifier)
{
    if (mRowNotValidated)
    validate ();

    ColumnDefinition* columnDef = NULL;
    LONG result = 0L;

    // Get appropriate ColumnDefinition:
    columnDef = getColumnDef (identifier);
    if ((columnDef->mPropertyType != FdoDataType_BLOB) && (columnDef->mPropertyType != FdoDataType_CLOB))
        throw FdoException::Create (NlsMsgGet2(ARCSDE_VALUE_TYPE_MISMATCH, "Value type to insert, update or retrieve doesn't match the type (%1$ls) of property '%2$ls'.", L"FdoBLOB", identifier));
    if (SE_IS_NULL_VALUE == columnDef->mBindIsNull)
        throw FdoException::Create (NlsMsgGet1(ARCSDE_NULL_VALUE, "The value of property '%1$ls' is null.", identifier));

    //if (mSelectingAggregates)
    //    ; // do nothing; an exception will have be thrown by now since the type doesn't match
    if (mDistinct)
        throw FdoCommandException::Create(NlsMsgGet(ARCSDE_SELECT_DISTINCT_INVALID_TYPE, "Select with 'distinct' option failed; ArcSDE doesn't support 'distinct' option with column types other than Int16, Int32, Single, Double, DateTime, and String."));

    // Create a FdoByteArray that contains the entire blob:
    FdoPtr<FdoByteArray> byteArray = FdoByteArray::Create ((FdoByte*) columnDef->mBindVariable._blob.blob_buffer,
        columnDef->mBindVariable._blob.blob_length);

#if !SDE_UNICODE
	if (columnDef->mColumnType == SE_CLOB_TYPE)
	{
		//Convert to UNICODE
		FdoPtr<FdoByteArray> unicodeByteArray = FdoByteArray::Create(2 * (strlen((char*)byteArray.p) + 1));
		multibyte_to_wide_noalloc((wchar_t*)unicodeByteArray.p, (char*)byteArray.p);
		return (FdoLOBValue*)FdoLOBValue::Create(unicodeByteArray, FdoDataType_BLOB);
	}
	else
#endif
	{
		// Return the LOB value:
		return (FdoLOBValue*)FdoLOBValue::Create(byteArray, FdoDataType_BLOB);
	}
}

/// <summary>Gets a reference of the specified LOB property as a FdoBLOBStreamReader or
/// FdoCLOBStreamReader etc. to allow reading in blocks of data.
/// Because no conversion is performed, the property must be FdoDataType_BLOB 
/// or FdoDataType_CLOB etc. (a LOB type)
/// Cast the FdoIStreamReader to the appropiate LOB Stream Reader.
/// <param name="propertyName">Input the property name.</param> 
/// <returns>Returns a reference to a LOB stream reader</returns> 
FdoIStreamReader* ArcSDEReader::GetLOBStreamReader(FdoString* identifier )
{
    if (mRowNotValidated)
    validate ();

    ColumnDefinition* columnDef = NULL;
    LONG result = 0L;
    SE_BLOB_INFO *blobInfo = new SE_BLOB_INFO;

    // Get appropriate ColumnDefinition:
    columnDef = getColumnDef (identifier);
    if ((columnDef->mPropertyType != FdoDataType_BLOB) && (columnDef->mPropertyType != FdoDataType_CLOB))
        throw FdoException::Create (NlsMsgGet2(ARCSDE_VALUE_TYPE_MISMATCH, "Value type to insert, update or retrieve doesn't match the type (%1$ls) of property '%2$ls'.", L"FdoBLOB", identifier));
#if !SDE_UNICODE
	if (columnDef->mColumnType == SE_CLOB_TYPE)
		throw FdoException::Create (NlsMsgGet2(ARCSDE_VALUE_TYPE_MISMATCH, "Cannot get LOB stream on property '%2$ls'.  Underlying DBMS is multibyte, stream requires unicode.  Use GetLOB instead.", L"FdoBLOB", identifier));
#endif
    if (SE_IS_NULL_VALUE == columnDef->mBindIsNull)
        throw FdoException::Create (NlsMsgGet1(ARCSDE_NULL_VALUE, "The value of property '%1$ls' is null.", identifier));

    //if (mSelectingAggregates)
    //    ; // do nothing; an exception will have be thrown by now since the type doesn't match
    if (mDistinct)
        throw FdoCommandException::Create(NlsMsgGet(ARCSDE_SELECT_DISTINCT_INVALID_TYPE, "Select with 'distinct' option failed; ArcSDE doesn't support 'distinct' option with column types other than Int16, Int32, Single, Double, DateTime, and String."));

    // Create & return blob stream reader:
    // NOTE: if someone calls GetLOBStreamReader() twice on the same row & column,
    //       an error will occur since we've exhausted the blob data; is this acceptable?
    SE_BLOB_INFO *blobInfoClone = new SE_BLOB_INFO;
    blobInfoClone->blob_length = columnDef->mBindVariable._blob.blob_length;
    blobInfoClone->blob_buffer = columnDef->mBindVariable._blob.blob_buffer;
    columnDef->mBindVariable._blob.blob_length = 0;
    columnDef->mBindVariable._blob.blob_buffer = NULL;
    return new ArcSDEBLOBStreamReader(blobInfoClone);
}


/// <summary>Returns true if the value of the specified property is null.</summary>
/// <param name="identifier">Input the property or column name.</param> 
/// <returns>Returns true if the value is null.</returns> 
bool ArcSDEReader::IsNull (FdoString* identifier)
{
    if (mRowNotValidated)
    validate ();

    // Get appropriate ColumnDefinition:
    ColumnDefinition* columnDef = NULL;
    columnDef = getColumnDef (identifier);

    if (mSelectingAggregates)
        return false;
    else if (mDistinct)
        return false; // TODO: can distinct return NULL values ??  I don't think so; need to check this
    else
    {
        bool bIsNull = (columnDef->mBindIsNull == SE_IS_NULL_VALUE);

        // If geometry value, we need to further check if the geometry value is a NIL shape.
        if (!bIsNull && (columnDef->mColumnType == SE_SHAPE_TYPE))
        {
            LONG shapeType;
            LONG lResult = SE_shape_get_type(columnDef->mBindVariable._shape, &shapeType);
            handle_sde_err<FdoCommandException>(mConnection->GetConnection (), lResult, __FILE__, __LINE__, ARCSDE_UNEXPECTED_ERROR, "Unexpected error encountered in ArcSDE Provider.");

            if (SG_NIL_SHAPE == shapeType)
                bIsNull = true;
        }

        return bIsNull;
    }
}


/// <summary>Gets the geometry value of the specified property as a byte array in 
/// FGF format. Because no conversion is performed, the property must be
/// of Geometric type; otherwise, an exception is thrown.</summary>
/// <param name="identifier">Input the property or column name.</param> 
/// <returns>Returns the byte array in FGF format.</returns> 
FdoByteArray* ArcSDEReader::GetGeometry (FdoString* propertyName)
{
    if (mRowNotValidated)
    validate ();

    ColumnDefinition* columnDef = NULL;
    LONG result = 0L;
    LONG size = 0L;
    FdoByteArray* ret = NULL;

    //if (mSelectingAggregates)
    //    ; // do nothing; an exception will have be thrown by now since the type doesn't match
    if (mDistinct)
        throw FdoCommandException::Create(NlsMsgGet(ARCSDE_SELECT_DISTINCT_INVALID_TYPE, "Select with 'distinct' option failed; ArcSDE doesn't support 'distinct' option with column types other than Int16, Int32, Single, Double, DateTime, and String."));

    columnDef = getColumnDef (propertyName);
    if (columnDef->mPropertyType != (FdoDataType)-1)
        throw FdoException::Create (NlsMsgGet2(ARCSDE_VALUE_TYPE_MISMATCH, "Value type to insert, update or retrieve doesn't match the type (%1$ls) of property '%2$ls'.", L"Geometry", propertyName));
    if (SE_IS_NULL_VALUE == columnDef->mBindIsNull)
        throw FdoException::Create (NlsMsgGet1(ARCSDE_NULL_VALUE, "The value of property '%1$ls' is null.", propertyName));

    if (NULL == columnDef->mValuePointer)
    {
        convert_sde_shape_to_fgf(mConnection, columnDef->mBindVariable._shape, (FdoByteArray*&)columnDef->mValuePointer);
        columnDef->mValuePointerSize = ((const FdoByteArray*)columnDef->mValuePointer)->GetCount();
    }

    // return FdoByteArray:
    return FDO_SAFE_ADDREF((FdoByteArray*)columnDef->mValuePointer);
}



/// <summary>Gets the geometry value of the specified property as a byte array in 
/// FGF format. Because no conversion is performed, the property must be
/// of Geometric type; otherwise, an exception is thrown. 
/// This method is a language-specific performance optimization that returns a
/// pointer to the array data, rather than to an object that encapsulates
/// the array.  The array's memory area is only guaranteed to be valid
/// until a call to ReadNext() or Close(), or the disposal of this reader
/// object.</summary>
/// <param name="propertyName">Input the property name.</param> 
/// <param name="count">Output the number of bytes in the array.</param> 
/// <returns>Returns a pointer to the byte array in FGF format.</returns> 
const FdoByte * ArcSDEReader::GetGeometry(FdoString* propertyName, FdoInt32 * count)
{
    FdoByteArray* fdoByteArray = GetGeometry(propertyName);
    fdoByteArray->Release();
    *count = fdoByteArray->GetCount();
    return fdoByteArray->GetData();
}

/// <summary>Gets the raster object of the specified property.
/// Because no conversion is performed, the property must be
/// of Raster type; otherwise, an exception is thrown.</summary>
/// <param name="propertyName">Input the property name.</param> 
/// <returns>Returns the raster object.</returns> 
FdoIRaster* ArcSDEReader::GetRaster(FdoString* propertyName)
{
    throw FdoException::Create (NlsMsgGet(ARCSDE_UNSUPPORTED_DATATYPE_RASTER, "Raster properties are not supported."));
}

// Clears the locally-cached values on each column:
void ArcSDEReader::ClearCachedColumnValues(bool bReaderClosing)
{
    if (mColumnDefs != NULL)  // If mColumnDefs not initialized, nothing to do
    {
        getColumnDefs();
        for (int iColumn=0; iColumn<mColumnCount; iColumn++)
        {
            ColumnDefinition *columnDef = getColumnDef(iColumn);
            columnDef->ClearCache(bReaderClosing);
        }
    }
}


/// <summary>Advances the reader to the next item and returns true if there is
/// another object to read or false if reading is complete. The default
/// position of the reader is prior to the first item. Thus you must
/// call ReadNext to begin accessing any data.</summary>
/// <returns>Returns true if there is a next item.</returns> 
bool ArcSDEReader::ReadNext ()
{
    bool ret = false;
    LONG result = 0L;
    mRowNotValidated = true;

    mReady = true;
    if (mDistinct || mSelectingAggregates)
    {
        // NOTE: don't need to initialize bind variables here, since we use the stream statistics mechanism instead

        ClearCachedColumnValues(false);

        mStreamStatsIndex ++;  // move to next distinct value
        if (mSelectingAggregates)
            ret = mStreamStatsIndex < 1;
        else // (mDistinct)
            ret = (mStreamStatsIndex < getColumnDef(0)->mStreamStats->distinct->num_distinct);
    }
    else
    {
        // Set up bind variables, if not yet done so:
        if (!m_bBindVariablesInitialized)
        {
            getColumnDefs();
            for (int iColumn=0; iColumn<mColumnCount; iColumn++)
            {
                ColumnDefinition *columnDef = getColumnDef(iColumn);
#ifdef SDE_UNICODE
                if (columnDef->mColumnType == SE_NSTRING_TYPE)
                {
                    columnDef->mBindVariable._string = (new CHAR[columnDef->mDataLength + 1]);
                    columnDef->mBindVariableInitialized = true;
                    result = SE_stream_bind_output_column(mStream, columnDef->mColumnNumber, (void*)(columnDef->mBindVariable._string), &columnDef->mBindIsNull);
                }
				else if (columnDef->mColumnType == SE_NCLOB_TYPE)
                {
                    columnDef->mBindVariable._blob.blob_length = 0;
                    columnDef->mBindVariable._blob.blob_buffer = NULL;
                    result = SE_stream_bind_output_column(mStream, columnDef->mColumnNumber, (void*)&(columnDef->mBindVariable._blob), &columnDef->mBindIsNull);
                }
				else
#endif
                if (columnDef->mColumnType == SE_STRING_TYPE)
                {
                    columnDef->mBindVariable._string = (new CHAR[columnDef->mDataLength + 1]);
                    columnDef->mBindVariableInitialized = true;
                    result = SE_stream_bind_output_column(mStream, columnDef->mColumnNumber, (void*)(columnDef->mBindVariable._string), &columnDef->mBindIsNull);
                }
				else if (columnDef->mColumnType == SE_CLOB_TYPE)
                {
                    columnDef->mBindVariable._blob.blob_length = 0;
                    columnDef->mBindVariable._blob.blob_buffer = NULL;
                    result = SE_stream_bind_output_column(mStream, columnDef->mColumnNumber, (void*)&(columnDef->mBindVariable._blob), &columnDef->mBindIsNull);
                }
                else if (columnDef->mColumnType == SE_UUID_TYPE)
                {
                    columnDef->mBindVariable._string = (new CHAR[columnDef->mDataLength + 1]);
                    columnDef->mBindVariableInitialized = true;
                    result = SE_stream_bind_output_column(mStream, columnDef->mColumnNumber, (void*)(columnDef->mBindVariable._string), &columnDef->mBindIsNull);
                }
                else if (columnDef->mColumnType == SE_SHAPE_TYPE)
                {
                    result = SE_shape_create (NULL, &columnDef->mBindVariable._shape);
                    handle_sde_err<FdoCommandException>(result, __FILE__, __LINE__, ARCSDE_UNEXPECTED_ERROR_ARG, "Unexpected error encountered in ArcSDE Provider (%1$ls).", L"creating an ArcSDE shape");
                    columnDef->mBindVariableInitialized = true;
                    result = SE_stream_bind_output_column(mStream, columnDef->mColumnNumber, (void*)(columnDef->mBindVariable._shape), &columnDef->mBindIsNull);
                }
                else if (columnDef->mColumnType == SE_BLOB_TYPE)
                {
                    columnDef->mBindVariable._blob.blob_length = 0;
                    columnDef->mBindVariable._blob.blob_buffer = NULL;
                    result = SE_stream_bind_output_column(mStream, columnDef->mColumnNumber, (void*)&(columnDef->mBindVariable._blob), &columnDef->mBindIsNull);
                }
                else
                    result = SE_stream_bind_output_column(mStream, columnDef->mColumnNumber, (void*)&(columnDef->mBindVariable), &columnDef->mBindIsNull);

                handle_sde_err<FdoCommandException>(mStream, result, __FILE__, __LINE__, ARCSDE_BIND_COLUMN_FAILED, "Failed to bind column '%1$ls'.", columnDef->mColumnName);
            }
            m_bBindVariablesInitialized = true;
        }

        ClearCachedColumnValues(false);


        // Fetch next row:
        result = SE_stream_fetch (mStream);
        if (SE_FINISHED == result)
        {
            Close (); // close immediately
            ret = false;
        }
        else
        {
            handle_sde_err<FdoCommandException>(mStream, result, __FILE__, __LINE__, ARCSDE_STREAM_FETCH, "Stream fetch failed.");
            ret = true;
        }
    }

    return (ret);
}

/// <summary>Closes the FdoIFeatureReader object, freeing any resources it may be holding.</summary>
/// <returns>Returns nothing</returns> 
void ArcSDEReader::Close ()
{
    LONG result;

    if (mConnection->GetConnectionState() == FdoConnectionState_Open) // connection close frees all it's streams
        if (NULL != mStream)
        {
            ClearCachedColumnValues (true);
            result = SE_stream_free (mStream);
            mStream = NULL;
            // we allow the possibility that the stream was closed when the SE_CONNECTION was closed
            if (SE_INVALID_STREAM != result)
                handle_sde_err<FdoCommandException>(mConnection->GetConnection (), result, __FILE__, __LINE__, ARCSDE_STREAM_FREE, "Stream free failed.");
        }
}

//
// ColumnDefinition stuff
// May need to be broken out later.
//

ArcSDEReader::ColumnDefinition::ColumnDefinition () :
    mColumnNumber(0),
    mPropertyType((FdoDataType)0L),
    mColumnType(0L),
    mDataLength(0),
    mBindIsNull(SE_IS_NULL_VALUE),
    mValuePointer(NULL),
    mValuePointerSize(0),
    mStreamStats(NULL)
{
    mColumnName[0] = L'\0';
    mPropertyName[0] = L'\0';
    mBindVariableInitialized = false;
}

ArcSDEReader::ColumnDefinition::~ColumnDefinition ()
{
    if (mBindVariableInitialized)
    {
#ifdef SDE_UNICODE
        if (mColumnType == SE_NSTRING_TYPE || mColumnType == SE_UUID_TYPE)
            delete[] mBindVariable._string;
		else
#endif
        if (mColumnType == SE_STRING_TYPE || mColumnType == SE_UUID_TYPE)
            delete[] mBindVariable._string;
        else if (mColumnType == SE_SHAPE_TYPE)
            SE_shape_free (mBindVariable._shape);
    }

    // Clean up stream stats;
    if (NULL != mStreamStats)
        SE_table_free_stats(mStreamStats);

    ClearCache(true);
}

void ArcSDEReader::ColumnDefinition::ClearCache(bool bReaderClosing)
{
    if (mPropertyType == FdoDataType_String)
    {
        if (bReaderClosing)
        {
	    delete[] (wchar_t*)mValuePointer;
            mValuePointer = NULL;
            mValuePointerSize = 0;
        }
    }
    else if (mPropertyType == (FdoDataType)-1 && NULL != mValuePointer)
    {
        (void) ((FdoByteArray*)mValuePointer)->Release();
    mValuePointer = NULL;
    mValuePointerSize = 0;
}
    else if (NULL != mValuePointer)
	    throw FdoException::Create(NlsMsgGet(ARCSDE_UNEXPECTED_ERROR, "Unexpected error encountered in ArcSDE Provider."));
}
