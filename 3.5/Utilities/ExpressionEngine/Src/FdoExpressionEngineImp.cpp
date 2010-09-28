/*
 * 
* Copyright (C) 2004-2007  Autodesk, Inc.
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

#include <stdafx.h>
#include <FdoExpressionEngineImp.h>
#include <FdoCommonOSUtil.h>
#include <FdoCommonMiscUtil.h>
#include <FdoCommonThreadMutex.h>
#include <FdoCommonStringUtil.h>
#include <FdoCommonSchemaUtil.h>

#include <Spatial/SpatialStd.h>
#include <Spatial/SpatialUtility.h>

#include <wctype.h>
#include <malloc.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>

#include <FdoExpressionEngineIAggregateFunction.h>
#include <Util/FdoExpressionEngineUtilDataReader.h>
#include "ExpressionEngineInitializeClass.h"

enum OptFilterType
{
    OptFilterType_ComCond,
    OptFilterType_DistCond,
    OptFilterType_InCond,
    OptFilterType_NullCond,
    OptFilterType_SpaCond,
    OptFilterType_UnarCond,
    OptFilterType_OrCond,
    OptFilterType_AndCond
};
typedef std::vector< std::pair< OptFilterType, FdoFilter* > > FilterList;
typedef std::vector< FilterList* > StackFilter;

FdoCommonThreadMutex mutex;

ExpressionEngineInitializeClass initFunction;

FdoExpressionEngineImp* FdoExpressionEngineImp::Create(FdoIReader* reader, FdoClassDefinition* classDef, FdoIdentifierCollection* identifiers,
		FdoExpressionEngineFunctionCollection *userDefinedFunctions)
{
	return new FdoExpressionEngineImp(reader, classDef, identifiers, userDefinedFunctions);
}


FdoExpressionEngineImp::FdoExpressionEngineImp(FdoIReader* reader, FdoClassDefinition* classDef, FdoIdentifierCollection* compIdents,
		FdoExpressionEngineFunctionCollection *userDefinedFunctions)
{

	m_reader = reader;  // NOTE: weak reference
	m_classDefinition = FDO_SAFE_ADDREF(classDef);

    m_compIdents = FDO_SAFE_ADDREF(compIdents);

	m_propIndex = new FdoCommonPropertyIndex(classDef, 0);

    m_AllFunctions = FdoFunctionDefinitionCollection::Create();

    m_FunctionsPopulated = false;
	m_UserDefinedFunctions = FDO_SAFE_ADDREF(userDefinedFunctions);

	m_Size = 10;
	m_Current = 0;
	m_CacheFunc = (FunctionCache *) malloc(m_Size*sizeof(FunctionCache));
    m_CurrentIndex = 0;

	m_ExpressionCacheSize = 10;
	m_ExpressionCacheCurrent = 0;
	m_ExpressionCache = new ExpressionCache[m_ExpressionCacheSize];

    m_processingAggregate = false;
}

FdoExpressionEngineImp::FdoExpressionEngineImp()
{
}

bool FdoExpressionEngineImp::IsAggregateFunction(FdoFunctionDefinitionCollection *funcDefs, FdoString *name)
{

	bool ret = false;
	for(int i=0; i<funcDefs->GetCount(); i++)
	{
		FdoPtr<FdoFunctionDefinition> funcDef = funcDefs->GetItem(i);
		if (FdoCommonStringUtil::StringCompareNoCase(name, funcDef->GetName()) == 0)
		{
			ret = funcDef->IsAggregate();
			break;
		}
	}
	return ret;
}



FdoExpressionEngineImp::~FdoExpressionEngineImp()
{
	m_propIndex->Release();
	for (int i=0; i<m_Current; i++)
	{
		FDO_SAFE_RELEASE(m_CacheFunc[i].function);
	}

    size_t count = m_AggregateFunctions.size();
    for (size_t i=0; i<count; i++)
    {
        FdoExpressionEngineIAggregateFunction *func = (FdoExpressionEngineIAggregateFunction*)m_AggregateFunctions.back();
        FDO_SAFE_RELEASE(func);
        m_AggregateFunctions.pop_back();
    }
	free(m_CacheFunc);

    delete [] m_ExpressionCache;

    for (retval_stack::iterator iter = m_retvals.begin (); iter != m_retvals.end (); iter++)
        delete *iter;
    for (std::vector<FdoBooleanValue*>::iterator i = mBooleanPool.begin (); i != mBooleanPool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoByteValue*>::iterator i = mBytePool.begin (); i != mBytePool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoDateTimeValue*>::iterator i = mDateTimePool.begin (); i != mDateTimePool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoDecimalValue*>::iterator i = mDecimalPool.begin (); i != mDecimalPool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoDoubleValue*>::iterator i = mDoublePool.begin (); i != mDoublePool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoInt16Value*>::iterator i = mInt16Pool.begin (); i != mInt16Pool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoInt32Value*>::iterator i = mInt32Pool.begin (); i != mInt32Pool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoInt64Value*>::iterator i = mInt64Pool.begin (); i != mInt64Pool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoSingleValue*>::iterator i = mSinglePool.begin (); i != mSinglePool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoStringValue*>::iterator i = mStringPool.begin (); i != mStringPool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoBLOBValue*>::iterator i = mBLOBPool.begin (); i != mBLOBPool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoCLOBValue*>::iterator i = mCLOBPool.begin (); i != mCLOBPool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoLiteralValueCollection*>::iterator i = mLiteralValueCollectionPool.begin (); i != mLiteralValueCollectionPool.end (); i++)
        (*i)->Release ();


    for (std::vector<FdoBooleanValue*>::iterator i = mPotentialBooleanPool.begin (); i != mPotentialBooleanPool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoByteValue*>::iterator i = mPotentialBytePool.begin (); i != mPotentialBytePool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoDateTimeValue*>::iterator i = mPotentialDateTimePool.begin (); i != mPotentialDateTimePool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoDecimalValue*>::iterator i = mPotentialDecimalPool.begin (); i != mPotentialDecimalPool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoDoubleValue*>::iterator i = mPotentialDoublePool.begin (); i != mPotentialDoublePool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoInt16Value*>::iterator i = mPotentialInt16Pool.begin (); i != mPotentialInt16Pool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoInt32Value*>::iterator i = mPotentialInt32Pool.begin (); i != mPotentialInt32Pool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoInt64Value*>::iterator i = mPotentialInt64Pool.begin (); i != mPotentialInt64Pool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoSingleValue*>::iterator i = mPotentialSinglePool.begin (); i != mPotentialSinglePool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoStringValue*>::iterator i = mPotentialStringPool.begin (); i != mPotentialStringPool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoBLOBValue*>::iterator i = mPotentialBLOBPool.begin (); i != mPotentialBLOBPool.end (); i++)
        (*i)->Release ();
    for (std::vector<FdoCLOBValue*>::iterator i = mPotentialCLOBPool.begin (); i != mPotentialCLOBPool.end (); i++)
        (*i)->Release ();
}

void FdoExpressionEngineImp::Dispose ()
{
    delete this;
}


FdoCommonPropertyStub* FdoExpressionEngineImp::GetPropInfo(FdoString* name)
{
	FdoCommonPropertyStub* stub = m_propIndex->GetPropInfo(name);
    return (stub);
}



void FdoExpressionEngineImp::Reset ()
{
    for (retval_stack::iterator iter = m_retvals.begin (); iter != m_retvals.end (); iter++)
        RelinquishDataValue (*iter);

    m_retvals.clear ();
}

//returns the data type of the result
//of the expression/filter evaluation.
//For filters it is always boolean
FdoDataType FdoExpressionEngineImp::GetResultDataType ()
{
    if (GetResultPropertyType() != FdoPropertyType_DataProperty)
        throw FdoException::Create(FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));

    return (static_cast<FdoDataValue*>(m_retvals.back ())->GetDataType ());
}

FdoPropertyType FdoExpressionEngineImp::GetResultPropertyType ()
{
    if ( dynamic_cast<FdoDataValue*>(m_retvals.back ()) != NULL )
        return FdoPropertyType_DataProperty;
    else if ( dynamic_cast<FdoGeometryValue*>(m_retvals.back ()) != NULL )
        return FdoPropertyType_GeometricProperty;
    else
        throw FdoException::Create(FdoException::NLSGetMessage (FDO_NLSID (FDO_57_UNEXPECTEDERROR)));
}

bool FdoExpressionEngineImp::IsResultNull ()
{
    FdoPropertyType propType = GetResultPropertyType();
    if (propType == FdoPropertyType_DataProperty)
        return ((FdoDataValue*)m_retvals.back())->IsNull();
    else if (propType == FdoPropertyType_GeometricProperty)
        return ((FdoGeometryValue*)m_retvals.back())->IsNull();
    else
        throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_57_UNEXPECTEDERROR)));
}


bool FdoExpressionEngineImp::GetBooleanResult (bool &bIsNull)
{
    bool ret = false;

    if (FdoPropertyType_DataProperty != GetResultPropertyType())
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));

    FdoDataValue* dv = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    if (FdoDataType_Boolean != dv->GetDataType ())
    {
        RelinquishDataValue (dv);
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    bIsNull = dv->IsNull();
    if (!bIsNull)
        ret = ((FdoBooleanValue*)dv)->GetBoolean ();

    RelinquishDataValue (dv);
    
    return (ret);
}


double FdoExpressionEngineImp::GetDoubleResult (bool &bIsNull)
{
    double ret = 0.0;

    if (FdoPropertyType_DataProperty != GetResultPropertyType())
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));

    FdoDataValue* dv = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    if (FdoDataType_Double != dv->GetDataType ())
    {
        RelinquishDataValue (dv);
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    bIsNull = dv->IsNull();
    if (!bIsNull)
        ret = ((FdoDoubleValue*)dv)->GetDouble ();

    RelinquishDataValue (dv);
    
    return (ret);
}

float FdoExpressionEngineImp::GetSingleResult (bool &bIsNull)
{
    float ret = 0.0f;

    if (FdoPropertyType_DataProperty != GetResultPropertyType())
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));

    FdoDataValue* dv = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    if (FdoDataType_Single != dv->GetDataType ())
    {
        RelinquishDataValue (dv);
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    bIsNull = dv->IsNull();
    if (!bIsNull)
        ret = ((FdoSingleValue*)dv)->GetSingle ();

    RelinquishDataValue (dv);
    
    return (ret);
}

double FdoExpressionEngineImp::GetDecimalResult (bool &bIsNull)
{
    double ret = 0.0;

    if (FdoPropertyType_DataProperty != GetResultPropertyType())
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));

    FdoDataValue* dv = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    if (FdoDataType_Decimal != dv->GetDataType ())
    {
        RelinquishDataValue (dv);
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    bIsNull = dv->IsNull();
    if (!bIsNull)
        ret = ((FdoDecimalValue*)dv)->GetDecimal ();

    RelinquishDataValue (dv);
    
    return (ret);
}

FdoByte FdoExpressionEngineImp::GetByteResult (bool &bIsNull)
{
    FdoByte ret = 0;

    if (FdoPropertyType_DataProperty != GetResultPropertyType())
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));

    FdoDataValue* dv = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    if (FdoDataType_Byte != dv->GetDataType ())
    {
        RelinquishDataValue (dv);
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    bIsNull = dv->IsNull();
    if (!bIsNull)
        ret = ((FdoByteValue*)dv)->GetByte ();

    RelinquishDataValue (dv);
    
    return (ret);
}

FdoInt16 FdoExpressionEngineImp::GetInt16Result (bool &bIsNull)
{
    FdoInt16 ret = 0;

    if (FdoPropertyType_DataProperty != GetResultPropertyType())
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));

    FdoDataValue* dv = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    if (FdoDataType_Int16 != dv->GetDataType ())
    {
        RelinquishDataValue (dv);
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    bIsNull = dv->IsNull();
    if (!bIsNull)
        ret = ((FdoInt16Value*)dv)->GetInt16 ();

    RelinquishDataValue (dv);
    
    return (ret);
}

FdoInt32 FdoExpressionEngineImp::GetInt32Result (bool &bIsNull)
{
    FdoInt32 ret = 0;

    if (FdoPropertyType_DataProperty != GetResultPropertyType())
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));

    FdoDataValue* dv = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    if (FdoDataType_Int32 != dv->GetDataType ())
    {
        RelinquishDataValue (dv);
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    bIsNull = dv->IsNull();
    if (!bIsNull)
        ret = ((FdoInt32Value*)dv)->GetInt32 ();

    RelinquishDataValue (dv);
    
    return (ret);
}

FdoInt64 FdoExpressionEngineImp::GetInt64Result (bool &bIsNull)
{
    FdoInt64 ret = 0;

    if (FdoPropertyType_DataProperty != GetResultPropertyType())
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));

    FdoDataValue* dv = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    if (FdoDataType_Int64 != dv->GetDataType ())
    {
        RelinquishDataValue (dv);
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    bIsNull = dv->IsNull();
    if (!bIsNull)
        ret = ((FdoInt64Value*)dv)->GetInt64 ();

    RelinquishDataValue (dv);
    
    return (ret);
}

const wchar_t* FdoExpressionEngineImp::GetStringResult (bool &bIsNull)
{
    const wchar_t* ret = NULL;

    if (FdoPropertyType_DataProperty != GetResultPropertyType())
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));

    FdoDataValue* dv = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    if (FdoDataType_String != dv->GetDataType ())
    {
        RelinquishDataValue (dv);
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    bIsNull = dv->IsNull();
    if (!bIsNull)
        ret = ((FdoStringValue*)dv)->GetString ();

    RelinquishDataValue (dv);
    
    return (ret);
}

FdoDateTime FdoExpressionEngineImp::GetDateTimeResult (bool &bIsNull)
{
    FdoDateTime ret;

    if (FdoPropertyType_DataProperty != GetResultPropertyType())
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));

    FdoDataValue* dv = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    if (FdoDataType_DateTime != dv->GetDataType ())
    {
        RelinquishDataValue (dv);
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    bIsNull = dv->IsNull();
    if (!bIsNull)
        ret = ((FdoDateTimeValue*)dv)->GetDateTime ();

    RelinquishDataValue (dv);
    
    return (ret);
}

FdoGeometryValue* FdoExpressionEngineImp::GetGeometricResult (bool &bIsNull)
{
    FdoGeometryValue* ret = NULL;

    if (FdoPropertyType_GeometricProperty != GetResultPropertyType())
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));

    FdoGeometryValue* gv = (FdoGeometryValue*)m_retvals.back ();
    m_retvals.pop_back ();

    bIsNull = gv->IsNull();
    ret = gv;

    //We dont (yet) cache geometry values:
    //RelinquishDataValue (dv);
    
    return (ret);
}

FdoBooleanValue* FdoExpressionEngineImp::ObtainBooleanValue (bool bIsNull, bool value)
{
    FdoBooleanValue* ret;

    if (0 != mBooleanPool.size ())
    {
        ret = mBooleanPool.back ();
        mBooleanPool.pop_back ();
        if (bIsNull)
            ret->SetNull();
        else
            ret->SetBoolean (value);
        return ret;
    }

    if (0 != mPotentialBooleanPool.size ())
    {
        int  size = (int) mPotentialBooleanPool.size();
        for (int i=0; i<size; i++)
        {
            ret = mPotentialBooleanPool[i];
            if (ret->GetRefCount() == 1)
            {
                mPotentialBooleanPool.erase(mPotentialBooleanPool.begin() + i);
                if (bIsNull)
                    ret->SetNull();
                else
                    ret->SetBoolean (value);
                return ret;
            }
        }
    }

    // if the pool is empty, create new
    if (bIsNull)
        ret = FdoBooleanValue::Create();  // defaults to NULL
    else
        ret = FdoBooleanValue::Create (value);
    return ret;
}

FdoByteValue* FdoExpressionEngineImp::ObtainByteValue (bool bIsNull, FdoByte value)
{
    FdoByteValue* ret;

    if (0 != mBytePool.size ())
    {
        ret = mBytePool.back ();
        mBytePool.pop_back ();
        if (bIsNull)
            ret->SetNull();
        else
            ret->SetByte (value);
        return ret;
    }

    if (0 != mPotentialBytePool.size ())
    {
        int  size = (int) mPotentialBytePool.size();
        for (int i=0; i<size; i++)
        {
            ret = mPotentialBytePool[i];
            if (ret->GetRefCount() == 1)
            {
                mPotentialBytePool.erase(mPotentialBytePool.begin() + i);
                if (bIsNull)
                    ret->SetNull();
                else
                    ret->SetByte (value);
                return ret;
            }
        }
    }


    // if the pool is empty, create new
    if (bIsNull)
        ret = FdoByteValue::Create (); // defaults to NULL
    else
        ret = FdoByteValue::Create (value);
    return ret;
}

FdoDateTimeValue* FdoExpressionEngineImp::ObtainDateTimeValue (bool bIsNull, FdoDateTime value)
{
    FdoDateTimeValue* ret;

    if (0 != mDateTimePool.size ())
    {
        ret = mDateTimePool.back ();
        mDateTimePool.pop_back ();
        if (bIsNull)
            ret->SetNull();
        else
            ret->SetDateTime (value);
        return ret;
    }

    if (0 != mPotentialDateTimePool.size ())
    {
        int  size = (int) mPotentialDateTimePool.size();
        for (int i=0; i<size; i++)
        {
            ret = mPotentialDateTimePool[i];
            if (ret->GetRefCount() == 1)
            {
                mPotentialDateTimePool.erase(mPotentialDateTimePool.begin() + i);
                if (bIsNull)
                    ret->SetNull();
                else
                    ret->SetDateTime (value);
                return ret;
            }
        }
    }

    // if the pool is empty, create new
    if (bIsNull)
        ret = FdoDateTimeValue::Create ();  // defaults to NULL
    else
        ret = FdoDateTimeValue::Create (value);
    return ret;
}

FdoDecimalValue* FdoExpressionEngineImp::ObtainDecimalValue (bool bIsNull, double value)
{
    FdoDecimalValue* ret;

    if (0 != mDecimalPool.size ())
    {
        ret = mDecimalPool.back ();
        mDecimalPool.pop_back ();
        if (bIsNull)
            ret->SetNull();
        else
            ret->SetDecimal (value);
        return ret;
    }

    if (0 != mPotentialDecimalPool.size ())
    {
        int  size = (int) mPotentialDecimalPool.size();
        for (int i=0; i<size; i++)
        {
            ret = mPotentialDecimalPool[i];
            if (ret->GetRefCount() == 1)
            {
                mPotentialDecimalPool.erase(mPotentialDecimalPool.begin() + i);
                if (bIsNull)
                    ret->SetNull();
                else
                    ret->SetDecimal (value);
                return ret;
            }
        }
    }

    // if the pool is empty, create new
    if (bIsNull)
        ret = FdoDecimalValue::Create ();  // defaults to NULL
    else
        ret = FdoDecimalValue::Create (value);
    return ret;
}

FdoDoubleValue* FdoExpressionEngineImp::ObtainDoubleValue (bool bIsNull, double value)
{
    FdoDoubleValue* ret;

    if (0 != mDoublePool.size ())
    {
        ret = mDoublePool.back ();
        mDoublePool.pop_back ();
        if (bIsNull)
            ret->SetNull ();
        else
            ret->SetDouble (value);
        return ret;
    }

    if (0 != mPotentialDoublePool.size ())
    {
        int  size = (int) mPotentialDoublePool.size();
        for (int i=0; i<size; i++)
        {
            ret = mPotentialDoublePool[i];
            if (ret->GetRefCount() == 1)
            {
                mPotentialDoublePool.erase(mPotentialDoublePool.begin() + i);
                if (bIsNull)
                    ret->SetNull ();
                else
                    ret->SetDouble (value);
                return ret;
            }
        }
    }

    // if the pool is empty, create new
    if (bIsNull)
        ret = FdoDoubleValue::Create ();  // defaults to NULL
    else
        ret = FdoDoubleValue::Create (value);
    return ret;
}

FdoInt16Value* FdoExpressionEngineImp::ObtainInt16Value (bool bIsNull, FdoInt16 value)
{
    FdoInt16Value* ret;

    if (0 != mInt16Pool.size ())
    {
        ret = mInt16Pool.back ();
        mInt16Pool.pop_back ();
        if (bIsNull)
            ret->SetNull ();
        else
            ret->SetInt16 (value);
        return ret;
    }

    if (0 != mPotentialInt16Pool.size ())
    {
        int  size = (int) mPotentialInt16Pool.size();
        for (int i=0; i<size; i++)
        {
            ret = mPotentialInt16Pool[i];
            if (ret->GetRefCount() == 1)
            {
                mPotentialInt16Pool.erase(mPotentialInt16Pool.begin() + i);
                if (bIsNull)
                    ret->SetNull ();
                else
                    ret->SetInt16 (value);
                return ret;
            }
        }
    }

    // if the pool is empty, create new
    if (bIsNull)
        ret = FdoInt16Value::Create ();  // defaults to NULL
    else
        ret = FdoInt16Value::Create (value);
    return ret;
}

FdoInt32Value* FdoExpressionEngineImp::ObtainInt32Value (bool bIsNull, FdoInt32 value)
{
    FdoInt32Value* ret;

    if (0 != mInt32Pool.size ())
    {
        ret = mInt32Pool.back ();
        mInt32Pool.pop_back ();
        if (bIsNull)
            ret->SetNull ();
        else
            ret->SetInt32 (value);
        return ret;
    }
    if (0 != mPotentialInt32Pool.size ())
    {
        int  size = (int) mPotentialInt32Pool.size();
        for (int i=0; i<size; i++)
        {
            ret = mPotentialInt32Pool[i];
            if (ret->GetRefCount() == 1)
            {
                mPotentialInt32Pool.erase(mPotentialInt32Pool.begin() + i);
                if (bIsNull)
                    ret->SetNull ();
                else
                    ret->SetInt32 (value);
                return ret;
            }
        }
    }

    // if the pool is empty, create new
    if (bIsNull)
        ret = FdoInt32Value::Create ();  // defaults to NULL
    else
        ret = FdoInt32Value::Create (value);
    return ret;
}

FdoInt64Value* FdoExpressionEngineImp::ObtainInt64Value (bool bIsNull, FdoInt64 value)
{
    FdoInt64Value* ret;

    if (0 != mInt64Pool.size ())
    {
        ret = mInt64Pool.back ();
        mInt64Pool.pop_back ();
        if (bIsNull)
            ret->SetNull ();
        else
            ret->SetInt64 (value);
        return ret;
    }

    if (0 != mPotentialInt64Pool.size ())
    {
        int  size = (int) mPotentialInt64Pool.size();
        for (int i=0; i<size; i++)
        {
            ret = mPotentialInt64Pool[i];
            if (ret->GetRefCount() == 1)
            {
                mPotentialInt64Pool.erase(mPotentialInt64Pool.begin() + i);
                if (bIsNull)
                    ret->SetNull ();
                else
                    ret->SetInt64 (value);
                return ret;
            }
        }
    }

    // if the pool is empty, create new
    if (bIsNull)
        ret = FdoInt64Value::Create ();  // defaults to NULL
    else
        ret = FdoInt64Value::Create (value);
    return ret;
}

FdoSingleValue* FdoExpressionEngineImp::ObtainSingleValue (bool bIsNull, float value)
{
    FdoSingleValue* ret;

    if (0 != mSinglePool.size ())
    {
        ret = mSinglePool.back ();
        mSinglePool.pop_back ();
        if (bIsNull)
            ret->SetNull ();
        else
            ret->SetSingle (value);
        return ret;
    }

    if (0 != mPotentialSinglePool.size ())
    {
        int  size = (int) mPotentialSinglePool.size();
        for (int i=0; i<size; i++)
        {
            ret = mPotentialSinglePool[i];
            if (ret->GetRefCount() == 1)
            {
                mPotentialSinglePool.erase(mPotentialSinglePool.begin() + i);
                if (bIsNull)
                    ret->SetNull ();
                else
                    ret->SetSingle (value);
                return ret;
            }
        }
    }

    // if the pool is empty, create new
    if (bIsNull)
        ret = FdoSingleValue::Create ();  // defaults to NULL
    else
        ret = FdoSingleValue::Create (value);
    return ret;
}

FdoStringValue* FdoExpressionEngineImp::ObtainStringValue (bool bIsNull, FdoString* value)
{
    FdoStringValue* ret;

    if (0 != mStringPool.size ())
    {
        ret = mStringPool.back ();
        mStringPool.pop_back ();
        if (bIsNull)
            ret->SetNull ();
        else
            ret->SetString (value);
        return ret;
    }

    if (0 != mPotentialStringPool.size ())
    {
        int  size = (int) mPotentialStringPool.size();
        for (int i=0; i<size; i++)
        {
            ret = mPotentialStringPool[i];
            if (ret->GetRefCount() == 1)
            {
                mPotentialStringPool.erase(mPotentialStringPool.begin() + i);
                if (bIsNull)
                    ret->SetNull ();
                else
                    ret->SetString (value);
                return ret;
            }
        }
    }

    if (bIsNull)
        ret = FdoStringValue::Create ();  // defaults to NULL
    else
        ret = FdoStringValue::Create (value);
    return ret;
}

FdoBLOBValue* FdoExpressionEngineImp::ObtainBLOBValue (bool bIsNull, FdoByteArray* value)
{
    FdoBLOBValue* ret;

    if (0 != mBLOBPool.size ())
    {
        ret = mBLOBPool.back ();
        mBLOBPool.pop_back ();
        if (bIsNull)
            ret->SetNull ();
        else
            ret->SetData (value);
        return ret;
    }

    if (0 != mPotentialBLOBPool.size ())
    {
        int  size = (int) mPotentialBLOBPool.size();
        for (int i=0; i<size; i++)
        {
            ret = mPotentialBLOBPool[i];
            if (ret->GetRefCount() == 1)
            {
                mPotentialBLOBPool.erase(mPotentialBLOBPool.begin() + i);
                if (bIsNull)
                    ret->SetNull ();
                else
                    ret->SetData (value);
                return ret;
            }
        }
    }

    // if the pool is empty, create new
    if (bIsNull)
        ret = FdoBLOBValue::Create ();  // defaults to NULL;
    else
        ret = FdoBLOBValue::Create (value);  //TODO: there is probable a memory leak here
    return ret;
}

FdoCLOBValue* FdoExpressionEngineImp::ObtainCLOBValue (bool bIsNull, FdoByteArray* value)
{
    FdoCLOBValue* ret;

    if (0 != mCLOBPool.size ())
    {
        ret = mCLOBPool.back ();
        mCLOBPool.pop_back ();
        if (bIsNull)
            ret->SetNull ();
        else
            ret->SetData (value);
        return ret;
    }

    if (0 != mPotentialCLOBPool.size ())
    {
        int  size = (int) mPotentialCLOBPool.size();
        for (int i=0; i<size; i++)
        {
            ret = mPotentialCLOBPool[i];
            if (ret->GetRefCount() == 1)
            {
                mPotentialCLOBPool.erase(mPotentialCLOBPool.begin() + i);
                if (bIsNull)
                    ret->SetNull ();
                else
                    ret->SetData (value);
                return ret;
            }
        }
    }

    // if the pool is empty, create new
    if (bIsNull)
        ret = FdoCLOBValue::Create ();  // defaults to NULL
    else
        ret = FdoCLOBValue::Create (value);  //TODO: there is probable a memory leak here
    return ret;
}

FdoLiteralValueCollection* FdoExpressionEngineImp::ObtainLiteralValueCollection ()
{
    FdoLiteralValueCollection* ret;

    if (0 == mLiteralValueCollectionPool.size ())
    {
        // if the pool is empty, create new
        ret = FdoLiteralValueCollection::Create();
    }
    else
    {  // otherwise get an object from the pool and initialize it
        ret = mLiteralValueCollectionPool.back ();
        mLiteralValueCollectionPool.pop_back ();
    }

    return (ret);
}


FdoGeometryValue* FdoExpressionEngineImp::ObtainGeometryValue (bool bIsNull, FdoByteArray* value)
{
    FdoGeometryValue* ret;

    // No geometry pooling (yet)
    if (bIsNull)
        ret = FdoGeometryValue::Create ();
    else
        ret = FdoGeometryValue::Create (value);

    return ret;
}

void FdoExpressionEngineImp::RelinquishDataValue (FdoLiteralValue* data)
{
    if (data->GetLiteralValueType() == FdoLiteralValueType_Data)
    {
        switch (((FdoDataValue*)data)->GetDataType ())
        {
            case FdoDataType_Boolean:
                mBooleanPool.push_back ((FdoBooleanValue*)data);
                break;
            case FdoDataType_Byte:
                mBytePool.push_back ((FdoByteValue*)data);
                break;
            case FdoDataType_DateTime:
                mDateTimePool.push_back ((FdoDateTimeValue*)data);
                break;
            case FdoDataType_Decimal:
                mDecimalPool.push_back ((FdoDecimalValue*)data);
                break;
            case FdoDataType_Double:
                mDoublePool.push_back ((FdoDoubleValue*)data);
                break;
            case FdoDataType_Int16:
                mInt16Pool.push_back ((FdoInt16Value*)data);
                break;
            case FdoDataType_Int32:
                mInt32Pool.push_back ((FdoInt32Value*)data);
                break;
            case FdoDataType_Int64:
                mInt64Pool.push_back ((FdoInt64Value*)data);
                break;
            case FdoDataType_Single:
                mSinglePool.push_back ((FdoSingleValue*)data);
                break;
            case FdoDataType_String:
                mStringPool.push_back ((FdoStringValue*)data);
                break;
            case FdoDataType_BLOB:
                mBLOBPool.push_back ((FdoBLOBValue*)data);
                break;
            case FdoDataType_CLOB:
                mCLOBPool.push_back ((FdoCLOBValue*)data);
                break;
            default:
                data->Release ();
        }
    }
    else if (data->GetLiteralValueType() == FdoLiteralValueType_Geometry)
    {
        // nothing to do since we dont pool geometry values (yet)
    }
    else
        throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_57_UNEXPECTEDERROR)));
}

void FdoExpressionEngineImp::RelinquishLiteralValueCollection(FdoLiteralValueCollection* literals)
{
     mLiteralValueCollectionPool.push_back(literals);
}

void FdoExpressionEngineImp::ProcessBinaryLogicalOperator (FdoBinaryLogicalOperator& filter)
{
    FdoPtr<FdoFilter> left = filter.GetLeftOperand ();
    FdoPtr<FdoFilter> right = filter.GetRightOperand ();

    //evaluate left hand side
    left->Process (this);
    bool bIsNull;
    bool argLeft = GetBooleanResult (bIsNull);
    if (bIsNull)
    {
        m_retvals.push_back (ObtainBooleanValue (true, false));
        return;
    }

    // optimize binary logic by looking at the value of the left operand and see
    // if we have to evaluate the right one also
    if (argLeft && filter.GetOperation () == FdoBinaryLogicalOperations_Or)
    {
        m_retvals.push_back (ObtainBooleanValue (false, true));
        return;
    }
    else if (!argLeft && filter.GetOperation () == FdoBinaryLogicalOperations_And)
    {
        m_retvals.push_back (ObtainBooleanValue (false, false));
        return;
    }
    
    // evaluate right hand side
    right->Process (this);
    bool argRight = GetBooleanResult (bIsNull);
    if (bIsNull)
        m_retvals.push_back (ObtainBooleanValue (true, false));
    else switch (filter.GetOperation ())
    {
        case FdoBinaryLogicalOperations_And: 
            m_retvals.push_back (ObtainBooleanValue (false, argLeft && argRight)); 
            break;
        case FdoBinaryLogicalOperations_Or : 
            m_retvals.push_back (ObtainBooleanValue (false, argLeft || argRight));
            break;
        default: 
            throw FdoException::Create (FdoException::NLSGetMessage(FDO_NLSID(FDO_82_UNSUPPORTED_LOGICAL_OPERATION)));
            break;
    }
}

void FdoExpressionEngineImp::ProcessUnaryLogicalOperator (FdoUnaryLogicalOperator& filter)
{
    FdoPtr<FdoFilter> right = filter.GetOperand ();

    // evaluate right hand side
    right->Process (this);

    // get Process() results from return value stack
    bool bIsNull;
    bool argRight = GetBooleanResult (bIsNull);

    switch (filter.GetOperation ())
    {
        case FdoUnaryLogicalOperations_Not: 
            m_retvals.push_back (ObtainBooleanValue (bIsNull, bIsNull ? false : !argRight)); 
            break;
        default: 
            throw FdoException::Create (FdoException::NLSGetMessage(FDO_NLSID(FDO_82_UNSUPPORTED_LOGICAL_OPERATION)));break;
    }
}

void FdoExpressionEngineImp::ProcessComparisonCondition (FdoComparisonCondition& filter)
{
    FdoPtr<FdoExpression> left = filter.GetLeftExpression ();
    FdoPtr<FdoExpression> right = filter.GetRightExpression ();

    right->Process (this);
    left->Process (this);
    
    //get Process() results from return value stack
    FdoDataValue* argLeft = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();
    FdoDataValue* argRight = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    if (argLeft->IsNull() || argRight->IsNull())
        m_retvals.push_back (ObtainBooleanValue (true, false));
    else switch (filter.GetOperation ())
    {
        case FdoComparisonOperations_EqualTo : 
			m_retvals.push_back (ObtainBooleanValue (false, FdoCommonMiscUtil::IsEqualTo (argLeft, argRight))); 
            break;
        case FdoComparisonOperations_NotEqualTo : 
			m_retvals.push_back (ObtainBooleanValue (false, !FdoCommonMiscUtil::IsEqualTo (argLeft, argRight))); 
            break;
        case FdoComparisonOperations_GreaterThan : 
			m_retvals.push_back (ObtainBooleanValue (false, FdoCommonMiscUtil::IsGreaterThan (argLeft, argRight))); 
            break;
        case FdoComparisonOperations_GreaterThanOrEqualTo : 
			m_retvals.push_back (ObtainBooleanValue (false, !FdoCommonMiscUtil::IsLessThan (argLeft, argRight))); 
            break;
        case FdoComparisonOperations_LessThan : 
			m_retvals.push_back (ObtainBooleanValue (false, FdoCommonMiscUtil::IsLessThan (argLeft, argRight))); 
            break;
        case FdoComparisonOperations_LessThanOrEqualTo : 
			m_retvals.push_back (ObtainBooleanValue (false, !FdoCommonMiscUtil::IsGreaterThan (argLeft, argRight))); 
            break;
        case FdoComparisonOperations_Like :
            m_retvals.push_back (ObtainBooleanValue (false, Like (argLeft, argRight)));
            break;    
        default:
            RelinquishDataValue (argRight);
            RelinquishDataValue (argLeft);
            throw FdoException::Create (FdoException::NLSGetMessage(FDO_NLSID(FDO_83_UNSUPPORTED_COMPARISON_OPERATION)));
    }

    RelinquishDataValue (argRight);
    RelinquishDataValue (argLeft);
}

void FdoExpressionEngineImp::ProcessInCondition (FdoInCondition& filter)
{
    //first get the value of the property we are checking
    FdoPtr<FdoIdentifier> prop = filter.GetPropertyName ();
    ProcessIdentifier (*(prop.p));

    //get the property value off the stack
    FdoDataValue* argLeft = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    FdoPtr<FdoValueExpressionCollection> vals = filter.GetValues ();

    bool result = false;

    //see if the value collection contains the property value
    FdoInt32 count = vals->GetCount ();
    for (int i = 0; !result && (i < count); i++)
    {
        FdoPtr<FdoValueExpression> expr = vals->GetItem (i);
        expr->Process (this);

        FdoDataValue* argRight = (FdoDataValue*)m_retvals.back ();
        m_retvals.pop_back ();

		if (FdoCommonMiscUtil::IsEqualTo (argLeft, argRight))
            result = true;

        RelinquishDataValue (argRight);

        if (result)
            break;
    }

    //if prop val is not IN the val collection, push false on the stack
    m_retvals.push_back (ObtainBooleanValue (false, result));

    RelinquishDataValue (argLeft);
}

void FdoExpressionEngineImp::ProcessNullCondition (FdoNullCondition& filter)
{
    //first get the value of the property we are checking
    FdoPtr<FdoIdentifier> prop = filter.GetPropertyName ();
    
    bool isNull = m_reader->IsNull (prop->GetName ());

    m_retvals.push_back (ObtainBooleanValue (false, isNull));
}

void FdoExpressionEngineImp::ProcessSpatialCondition (FdoSpatialCondition& filter)
{
    FdoPtr<FdoIdentifier> idName = filter.GetPropertyName ();
    if( m_reader->IsNull(idName->GetName ()))
    {
        m_retvals.push_back (ObtainBooleanValue (false, false));
        return;
    }
    FdoPtr<FdoByteArray> fgf = m_reader->GetGeometry (idName->GetName ());

    //no geometry? trivially false.
    if (fgf->GetCount () == 0)
        m_retvals.push_back (ObtainBooleanValue (false, false));

    FdoPtr<FdoExpression> exprRight = filter.GetGeometry ();
    FdoGeometryValue* gvRight = dynamic_cast<FdoGeometryValue*>(exprRight.p);
    
    if (!gvRight)
        throw FdoException::Create (FdoException::NLSGetMessage(FDO_NLSID(FDO_84_SPATIAL_CONDITION_NOT_LITERAL_GEOMETRY)));

    FdoPtr<FdoByteArray> baRight = gvRight->GetGeometry ();

    FdoPtr<FdoFgfGeometryFactory> gf = FdoFgfGeometryFactory::GetInstance ();
    FdoPtr<FdoIGeometry> geomRight = gf->CreateGeometryFromFgf (baRight);
    FdoPtr<FdoIGeometry> geomLeft = gf->CreateGeometryFromFgf (fgf);

    //call on the geometry utility to evaluate the spatial operation
    bool ret = FdoSpatialUtility::Evaluate (geomLeft, filter.GetOperation (), geomRight);
    
    m_retvals.push_back (ObtainBooleanValue (false, ret));            
}

void FdoExpressionEngineImp::ProcessDistanceCondition (FdoDistanceCondition& filter)
{
    //TODO:
    printf ("distance condition on %ls\n", FdoPtr<FdoIdentifier>(filter.GetPropertyName ())->GetName());
    throw FdoException::Create (FdoException::NLSGetMessage(FDO_NLSID(FDO_85_DISTANCE_SPATIAL_CONDITION_NOT_SUPPORTED)));
}


void FdoExpressionEngineImp::ProcessBinaryExpression (FdoBinaryExpression& expr)
{
    FdoPtr<FdoExpression> left = expr.GetLeftExpression ();
    FdoPtr<FdoExpression> right = expr.GetRightExpression ();

    right->Process (this);
    left->Process (this);

    //get Process() results from return value stack
    FdoDataValue* argLeft = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();
    FdoDataValue* argRight = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    switch (expr.GetOperation ())
    {
        case FdoBinaryOperations_Add: 
            m_retvals.push_back (Add (argLeft, argRight));
            break;
        case FdoBinaryOperations_Multiply: 
            m_retvals.push_back (Multiply (argLeft, argRight));
            break;
        case FdoBinaryOperations_Subtract: 
            m_retvals.push_back (Subtract (argLeft, argRight));
            break;
        case FdoBinaryOperations_Divide: 
            m_retvals.push_back (Divide (argLeft, argRight));
            break;
        default: 
            RelinquishDataValue (argLeft);
            RelinquishDataValue (argRight);
            throw FdoException::Create (FdoException::NLSGetMessage(FDO_NLSID(FDO_86_UNSUPPORTED_BINARY_OPERATION)));
    }

    RelinquishDataValue (argLeft);
    RelinquishDataValue (argRight);
}

void FdoExpressionEngineImp::ProcessUnaryExpression (FdoUnaryExpression& expr)
{
    FdoPtr<FdoExpression> right = expr.GetExpression ();

    right->Process (this);

    FdoDataValue* argRight = (FdoDataValue*)m_retvals.back ();
    m_retvals.pop_back ();

    switch (expr.GetOperation ())
    {
        case FdoUnaryOperations_Negate:
            m_retvals.push_back (Negate (argRight));
            break;
        default:
            RelinquishDataValue (argRight);
            throw FdoException::Create (FdoException::NLSGetMessage(FDO_NLSID(FDO_87_UNSUPPORTED_UNARY_OPERATION)));
    }

    RelinquishDataValue (argRight);
}

bool FdoExpressionEngineImp::AddToCache(FdoString *functionName, FdoExpressionEngineIFunction *functionExtension, FdoFunction& expr, bool* isAggregate)
{
    bool bAdded = false;
    FdoPtr<FdoFunctionDefinition> function = functionExtension->GetFunctionDefinition();
    if (FdoCommonStringUtil::StringCompareNoCase(function->GetName(), functionName) == 0)
    {
		if (m_Current >= m_Size)
		{
            m_Size = m_Size*2;
			m_CacheFunc = (FunctionCache *) realloc(m_CacheFunc, sizeof(FunctionCache)*m_Size);
		}
		m_CacheFunc[m_Current].address = &expr;
        if (dynamic_cast<FdoExpressionEngineINonAggregateFunction *>(functionExtension) != NULL)
        {
            m_CacheFunc[m_Current].isAggregate = false;
		    m_CacheFunc[m_Current].function = static_cast<FdoExpressionEngineINonAggregateFunction *> (functionExtension->CreateObject());
            *isAggregate = false;
        }
        else
        {
            m_CacheFunc[m_Current].isAggregate = true;
		    m_CacheFunc[m_Current].function = NULL; // not used if aggregate function
            *isAggregate = true;
        }
		m_Current++; 
        bAdded = true;
    }

    return bAdded;
}

void FdoExpressionEngineImp::ProcessFunction (FdoFunction& expr)
{

	FdoExpressionEngineINonAggregateFunction *func = NULL;
    FdoString* name = expr.GetName ();
	int i;

    bool isAggregate = false;
    bool bFunctionFound = false;

	for (i=0; i<m_Current; i++)
	{
		if (m_CacheFunc[i].address == &expr)
		{
			func = m_CacheFunc[i].function;
            isAggregate = m_CacheFunc[i].isAggregate;
            bFunctionFound = true;
			break;
		}
	}
	if (i == m_Current)
	{
		if (m_UserDefinedFunctions)
		{
			for (i=0; i<m_UserDefinedFunctions->GetCount(); i++)
			{
				FdoPtr<FdoExpressionEngineIFunction> functionExtension = m_UserDefinedFunctions->GetItem(i);
                bool bAdded = AddToCache(name, functionExtension, expr, &isAggregate);
                if (bAdded)
                {
                    if (!isAggregate)
					    func = m_CacheFunc[m_Current-1].function;
                    bFunctionFound = true;
                    break;
                }
			}
		}
		if (m_UserDefinedFunctions == NULL || i == m_UserDefinedFunctions->GetCount())
		{
            try
            {
                mutex.Enter();
                FdoPtr<FdoExpressionEngineFunctionCollection> functions = initFunction.GetAllFunctions();
			    for (i=0; i<functions->GetCount(); i++)
			    {
                    FdoPtr<FdoExpressionEngineIFunction> functionDefinition = functions->GetItem(i);
                    bool bAdded = AddToCache(name, functionDefinition, expr, &isAggregate); 
                    if (bAdded)
                    {
                        if (!isAggregate)
                            func = m_CacheFunc[m_Current-1].function;
                        bFunctionFound = true;
                        break;
                    }
			    }
                functions = NULL;
                mutex.Leave();
            }
            catch (FdoException *)
            {
                mutex.Leave();
                throw;
            }
            catch (...)
            {
                mutex.Leave();
                throw;
            }
		}
	}

	if (!bFunctionFound)
        throw FdoException::Create (FdoException::NLSGetMessage(FDO_NLSID(FDO_89_UNSUPPORTED_FUNCTION), name));

    if (isAggregate)
	{
		if (m_processingAggregate)
		{
	        FdoLiteralValueCollection* functionParameters = ObtainLiteralValueCollection();
			FdoPtr<FdoExpressionCollection> args = expr.GetArguments ();
			for (int i=0; i<args->GetCount(); i++)
			{
				FdoPtr<FdoExpression> arg = args->GetItem(i);
				arg->Process(this);
            }

            for (int i=0; i<args->GetCount(); i++)
            {
				FdoDataValue* dv = (FdoDataValue*)m_retvals.back ();
                
				m_retvals.pop_back ();
				functionParameters->Insert(0, dv);

                // The geometries are not pooled so release them here.
                if (dv->GetLiteralValueType() == FdoLiteralValueType_Geometry)
                    FDO_SAFE_RELEASE(dv);
			}

			FdoExpressionEngineIAggregateFunction *func = m_AggregateFunctions.at(m_CurrentIndex);
			func->Process(functionParameters);
			for (int i=0; i<functionParameters->GetCount(); i++)
			{
				FdoPtr<FdoLiteralValue> literalValue = functionParameters->GetItem(i);
				RelinquishDataValue(literalValue);
			}
            functionParameters->Clear();
            RelinquishLiteralValueCollection(functionParameters);
		}
		else
		{
            bool aggrFuncFound = false;

            if ( mAggrIdents ) 
            {
                for (int i=0; i<mAggrIdents->GetCount(); i++)
                {
                    FdoFunction* aggrFunc = (*mAggrIdents)[i]; 
                    if (aggrFunc == &expr)
                    {
                        aggrFuncFound = true;

			            FdoExpressionEngineIAggregateFunction *func = m_AggregateFunctions.at(i);
   			            FdoPtr<FdoLiteralValue> value;
                        
                        if (m_dataRead)
                             value = func->GetResult();
                        else
                        {

                            FdoPropertyType propType;
                            FdoDataType dataType;
                            
                            PopulateFunctions();
					        FdoCommonMiscUtil::GetExpressionType(m_AllFunctions, m_classDefinition,  aggrFunc, propType, dataType);

                            switch (propType)
                            {
                                case FdoPropertyType_DataProperty:
                                    switch (dataType)
                                    {
                                        case FdoDataType_Boolean:
                                            value = FdoBooleanValue::Create(); 
                                            break;

                                        case FdoDataType_Byte:
                                            value = FdoByteValue::Create(); 
                                            break;

                                        case FdoDataType_DateTime:
                                            value = FdoDateTimeValue::Create();
                                            break;

                                        case FdoDataType_Decimal:
                                            value = FdoDecimalValue::Create();
                                            break;

                                        case FdoDataType_Double:
                                            value = FdoDoubleValue::Create();
                                            break;

                                        case FdoDataType_Int16:
                                            value = FdoInt16Value::Create();
                                            break;

                                        case FdoDataType_Int32:
                                            value = FdoInt32Value::Create();
                                            break;

                                        case FdoDataType_Int64:
                                            value = FdoInt64Value::Create();
                                            break;

                                        case FdoDataType_Single:
                                            value = FdoSingleValue::Create();
                                            break;

                                        case FdoDataType_String:
                                            value = FdoStringValue::Create();
                                            break;

                                        case FdoDataType_BLOB:
                                            value = FdoBLOBValue::Create();
                                            break;

                                        case FdoDataType_CLOB:
                                            value = FdoCLOBValue::Create();
                                            break;

                                        default:
                                            throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_57_UNEXPECTEDERROR)));
                                    }
                                    break;

                                case FdoPropertyType_GeometricProperty:
                                    value = FdoGeometryValue::Create();
                                break;

                            default:
                                throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_57_UNEXPECTEDERROR)));
                            }
                        }
                        PushLiteralValue(value);
                        break;
                    }
                }
            }

            if ( !aggrFuncFound ) 
            {
                // This aggregate function was not processed, which is an error condition.
                // This can happen if ProcessAggregateFunctions() was never called, or 
                // more specifically, an expression with aggregate function was passed
                // to an FdoISelect.
                throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_184_UNINITIALIZED_AGGREGATE)));
            }
		}
	}
	else
	{

	    FdoLiteralValueCollection* functionParameters = ObtainLiteralValueCollection();
		FdoPtr<FdoExpressionCollection> args = expr.GetArguments ();
		for (int i=0; i<args->GetCount(); i++)
		{
			FdoPtr<FdoExpression> arg = args->GetItem(i);
			arg->Process(this);
		}

        for (int i=0; i<args->GetCount(); i++)
        {
			FdoDataValue* dv = (FdoDataValue*)m_retvals.back ();
			m_retvals.pop_back ();
            functionParameters->Insert(0, dv);
        }

		FdoPtr<FdoLiteralValue> result = func->Evaluate(functionParameters);
        PushLiteralValue(result);
		for (int i=0; i<functionParameters->GetCount(); i++)
		{
			FdoPtr<FdoLiteralValue> literalValue = functionParameters->GetItem(i);
			RelinquishDataValue(literalValue);
		}
        functionParameters->Clear();
        RelinquishLiteralValueCollection(functionParameters);
		return;
	}
}


FdoPropertyDefinition* FdoExpressionEngineImp::GetProperty(FdoClassDefinition* cls, FdoString* propName )
{
	FdoPropertyDefinition* prop = FdoPtr<FdoPropertyDefinitionCollection>(cls->GetProperties())->FindItem( propName );
	if( prop == NULL )
	{
		try
		{
			prop = FdoPtr<FdoReadOnlyPropertyDefinitionCollection>(cls->GetBaseProperties())->GetItem( propName );
		}
		catch(FdoException *exp)
		{
			exp->Release();
			prop = NULL;
		}
	}
	return prop;
}

void FdoExpressionEngineImp::ProcessIdentifier (FdoIdentifier& expr)
{
	int   length;
	const wchar_t** scope = expr.GetScope( length );
	if( length == 0 )
	{
		ProcessIdentifier(expr.GetName());
		return;
	}
	// else it must be an association based filter
	FdoPtr<FdoPropertyDefinition>prop = GetProperty( m_classDefinition, scope[0] );
	if( prop->GetPropertyType() != FdoPropertyType_AssociationProperty )
        throw FdoException::Create(FdoException::NLSGetMessage (FDO_NLSID (FDO_71_DATA_TYPE_NOT_SUPPORTED), expr.GetName()));

	FdoPtr<FdoClassDefinition>assoc_class = ((FdoAssociationPropertyDefinition*)prop.p)->GetAssociatedClass();
	FdoPtr<FdoIFeatureReader>reader = static_cast<FdoIFeatureReader *>(m_reader)->GetFeatureObject( scope[0] );
	for(int i=1; i<length; i++ )
	{
		if( reader == NULL || ! reader->ReadNext() )
			break;
		prop = GetProperty( assoc_class, scope[i] );
		if( prop->GetPropertyType() != FdoPropertyType_AssociationProperty )
			return;
		FdoPtr<FdoClassDefinition>assoc_class = ((FdoAssociationPropertyDefinition*)prop.p)->GetAssociatedClass();

		reader = reader->GetFeatureObject( scope[i] );	
	}

	if( reader != NULL && reader->ReadNext() )
	{
		prop = GetProperty( assoc_class, expr.GetName() );

		if( prop->GetPropertyType() == FdoPropertyType_DataProperty )
			PushIdentifierValue( reader, expr.GetName(), ((FdoDataPropertyDefinition*)prop.p)->GetDataType() );
	}
}

void FdoExpressionEngineImp::ProcessComputedIdentifier (FdoComputedIdentifier& expr)
{
    //Computed identifiers:
    //Just evaluate the expression that the computed identifier represents.

    FdoPtr<FdoExpression> theexpr = expr.GetExpression ();
    theexpr->Process (this);
}

void FdoExpressionEngineImp::ProcessIdentifier (FdoString* name)
{
    FdoCommonPropertyStub* definition = GetPropInfo (name);

    if (definition != NULL)
    {
        if (definition->m_propertyType == FdoPropertyType_DataProperty)
        {
            bool bIsNull = (m_reader->IsNull(name));
            switch (definition->m_dataType)
            {
            case FdoDataType_Boolean : 
                m_retvals.push_back (ObtainBooleanValue (bIsNull, bIsNull ? false : m_reader->GetBoolean (name))); 
                break;
            case FdoDataType_Byte : 
                m_retvals.push_back (ObtainByteValue (bIsNull, bIsNull ? 0 : m_reader->GetByte (name))); 
                break;
            case FdoDataType_DateTime : 
                m_retvals.push_back (ObtainDateTimeValue (bIsNull, bIsNull ? FdoDateTime() : m_reader->GetDateTime (name))); 
                break;
            case FdoDataType_Decimal : 
                m_retvals.push_back (ObtainDecimalValue (bIsNull, bIsNull ? 0.0 : m_reader->GetDouble (name))); 
                break;
            case FdoDataType_Double : 
                m_retvals.push_back (ObtainDoubleValue (bIsNull, bIsNull ? 0.0 : m_reader->GetDouble (name))); 
                break;
            case FdoDataType_Int16 : 
                m_retvals.push_back (ObtainInt16Value (bIsNull, bIsNull ? 0 : m_reader->GetInt16 (name))); 
                break;
            case FdoDataType_Int32 : 
                m_retvals.push_back (ObtainInt32Value (bIsNull, bIsNull ? 0 : m_reader->GetInt32 (name))); 
                break;
            case FdoDataType_Int64 : 
                m_retvals.push_back (ObtainInt64Value (bIsNull, bIsNull ? 0 : m_reader->GetInt64 (name))); 
                break;
            case FdoDataType_Single : 
                m_retvals.push_back (ObtainSingleValue (bIsNull, bIsNull ? 0.0f : m_reader->GetSingle (name))); 
                break;
            case FdoDataType_String : 
                m_retvals.push_back (ObtainStringValue (bIsNull, bIsNull ? NULL : m_reader->GetString (name))); 
                break;
            case FdoDataType_BLOB : 
                if (!bIsNull)
                {
                    FdoPtr<FdoLOBValue> blob = m_reader->GetLOB (name);
                    FdoPtr<FdoByteArray> blobBytes = blob->GetData ();
                    m_retvals.push_back (ObtainBLOBValue (bIsNull, blobBytes)); 
                }
                else
                    m_retvals.push_back (ObtainBLOBValue (bIsNull, NULL)); 
                break;
            case FdoDataType_CLOB : 
                if (!bIsNull)
                {
                    FdoPtr<FdoLOBValue> clob = m_reader->GetLOB (name);
                    FdoPtr<FdoByteArray> clobBytes = clob->GetData ();
                    m_retvals.push_back (ObtainCLOBValue (bIsNull, clobBytes)); 
                }
                else
                    m_retvals.push_back (ObtainCLOBValue (bIsNull, NULL)); 
                break;
            default:
                throw FdoException::Create (FdoException::NLSGetMessage(FDO_NLSID(FDO_71_DATA_TYPE_NOT_SUPPORTED), FdoCommonMiscUtil::FdoDataTypeToString(definition->m_dataType)));
            }
        }
        else if (definition->m_propertyType == FdoPropertyType_GeometricProperty)
        {
            bool bIsNull = (m_reader->IsNull(name));
            if(!bIsNull)
            {
                FdoPtr<FdoByteArray> geomBytes = m_reader->GetGeometry(name);
                m_retvals.push_back (ObtainGeometryValue (bIsNull, geomBytes )); 
            }
            else
                m_retvals.push_back (ObtainGeometryValue (bIsNull, NULL)); 
        }
        else
            throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_70_PROPERTY_TYPE_NOT_SUPPORTED), FdoCommonMiscUtil::FdoPropertyTypeToString(definition->m_propertyType)));
    }
    else
    {
        //if user tried to use a computed identifier in an expression
        //we need some special handling
        FdoPtr<FdoComputedIdentifier> cid;
        if (m_compIdents) {
            cid = dynamic_cast<FdoComputedIdentifier*>(m_compIdents->GetItem (name));
        }
        
        if (cid==NULL)
            throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_81_IDENTIFIER_NOT_FOUND), name));
        
        cid->Process (this);
    }
}

void FdoExpressionEngineImp::ProcessParameter (FdoParameter& expr)
{
    printf ("parameter %ls", expr.GetName ());
    throw FdoException::Create (FdoException::NLSGetMessage(FDO_NLSID(FDO_68_COMMAND_PARAMETERS_NOT_SUPPORTED)));
}

void FdoExpressionEngineImp::ProcessBooleanValue (FdoBooleanValue& expr)
{
    m_retvals.push_back (ObtainBooleanValue (expr.IsNull(), expr.IsNull() ? false : expr.GetBoolean ()));
}

void FdoExpressionEngineImp::ProcessByteValue (FdoByteValue& expr)
{
    m_retvals.push_back (ObtainByteValue (expr.IsNull(), expr.IsNull() ? 0 : expr.GetByte ()));
}

void FdoExpressionEngineImp::ProcessDateTimeValue (FdoDateTimeValue& expr)
{
    m_retvals.push_back (ObtainDateTimeValue (expr.IsNull(), expr.IsNull() ? FdoDateTime() : expr.GetDateTime ()));
}

void FdoExpressionEngineImp::ProcessDecimalValue (FdoDecimalValue& expr)
{
    m_retvals.push_back (ObtainDecimalValue (expr.IsNull(), expr.IsNull() ? 0.0 : expr.GetDecimal ()));
}

void FdoExpressionEngineImp::ProcessDoubleValue (FdoDoubleValue& expr)
{
    m_retvals.push_back (ObtainDoubleValue (expr.IsNull(), expr.IsNull() ? 0.0 : expr.GetDouble ()));
}

void FdoExpressionEngineImp::ProcessInt16Value (FdoInt16Value& expr)
{
    m_retvals.push_back (ObtainInt16Value (expr.IsNull(), expr.IsNull() ? 0 : expr.GetInt16 ()));
}

void FdoExpressionEngineImp::ProcessInt32Value (FdoInt32Value& expr)
{
    m_retvals.push_back (ObtainInt32Value (expr.IsNull(), expr.IsNull() ? 0 : expr.GetInt32 ()));
}

void FdoExpressionEngineImp::ProcessInt64Value (FdoInt64Value& expr)
{
    m_retvals.push_back (ObtainInt64Value (expr.IsNull(), expr.IsNull() ? 0 : expr.GetInt64 ()));
}

void FdoExpressionEngineImp::ProcessSingleValue (FdoSingleValue& expr)
{
    m_retvals.push_back (ObtainSingleValue (expr.IsNull(), expr.IsNull() ? 0.0f : expr.GetSingle ()));
}

void FdoExpressionEngineImp::ProcessStringValue (FdoStringValue& expr)
{
    m_retvals.push_back (ObtainStringValue (expr.IsNull(), expr.IsNull() ? NULL : expr.GetString ()));
}

void FdoExpressionEngineImp::ProcessBLOBValue (FdoBLOBValue& expr)
{
    bool isNull = expr.IsNull();
    if(!isNull)
    {
        FdoPtr<FdoByteArray> blobBytes = expr.GetData ();
        m_retvals.push_back (ObtainBLOBValue (isNull, blobBytes));
    }
    else
        m_retvals.push_back (ObtainBLOBValue (isNull, NULL));
}

void FdoExpressionEngineImp::ProcessCLOBValue (FdoCLOBValue& expr)
{
    bool isNull = expr.IsNull();
    if(!isNull)
    {
        FdoPtr<FdoByteArray> clobBytes = expr.GetData ();
        m_retvals.push_back (ObtainCLOBValue (isNull, clobBytes));
    }
    else
        m_retvals.push_back (ObtainCLOBValue (isNull, NULL));
}

void FdoExpressionEngineImp::ProcessGeometryValue (FdoGeometryValue& expr)
{
    bool isNull = expr.IsNull();
    if(!isNull)
    {
        FdoPtr<FdoByteArray> geomBytes = expr.GetGeometry();
        m_retvals.push_back (ObtainGeometryValue (isNull, geomBytes ));
    }
    else
        m_retvals.push_back (ObtainGeometryValue (isNull, NULL));
}


//Matches strings according to the rules of the LIKE condition
// Wildcard characters supported in the pattern are:
//
// %    Any string of zero or more characters
// _    Any single character
// []   Any single character within the specified range ([a-f]) or set 
// ([abcdef]).
// [^]  Any single character not within the specified range ([^a-f]) or 
// set ([^abcdef]).
bool FdoExpressionEngineImp::MatchesHere (const wchar_t* pattern, const wchar_t* src)
{
    bool bMatches = false;

    if (*pattern == L'\0')
    {
        if (wcslen (src))
        {
            bMatches = false;
        }
        else
        {
            bMatches = true;
        }
    }
    else if (*pattern == L'%')
    {
        bMatches = MatchPercent (pattern+1, src);
    }
    else if (*pattern == L'[')
    {
        bMatches = MatchBracket (pattern+1, src);
    }
    else if (*src != L'\0' && ((*pattern == L'_') ||  FdoCommonOSUtil::wcsnicmp (pattern, src, 1) == 0))
    {
        bMatches = MatchesHere (pattern+1, src+1);
    }

    return bMatches;
}

// %    Any string of zero or more characters
bool FdoExpressionEngineImp::MatchPercent (const  wchar_t* pattern, const wchar_t* src)
{
    bool bMatches = false;
    
    while (!bMatches)
    {
        if (MatchesHere (pattern, src))
        {
            bMatches = true;
            break;
        }
        else if (*src == L'\0')
        {
            break;
        }
        src++;
    }

    return bMatches;
}

// []   Any single character within the specified range ([a-f]) or set 
// ([abcdef]).
// [^]  Any single character not within the specified range ([^a-f]) or 
// set ([^abcdef]).
bool FdoExpressionEngineImp::MatchBracket (const wchar_t* pattern, const wchar_t* src)
{
    bool bMatches = false;
    bool reverse = false;

    //invalid case, unclosed bracket
    if (*pattern == L'\0')
        bMatches = false;

    if (*pattern == L'^')
    {
        reverse = true;
        pattern++;
    }

    //case of [] -- not a useful case
    if (*pattern == L']')
    {
        if (*src == L'\0')
            bMatches = true;

        if (reverse)
            bMatches = MatchesHere (pattern+1, src+1);

        bMatches = false;
    }

    wchar_t start = *pattern++;

    //case of [a-b] or [^a-b]
    if (*pattern == L'-')
    {
        //go past - character and get range end character and go past range end character
        pattern++;
        wchar_t end = *pattern++;

        if ((*src >= start && *src <= end) ^ reverse)
        {
            bMatches = MatchesHere (pattern+1, src+1);
        }
        else
            bMatches = false;
    }

    //case of [abcdef] ot [^abcdef]
    std::vector<wchar_t> set;
    set.push_back (start);

    do 
    {
        set.push_back (*pattern++);
    }
    while (*pattern != L']');

    wchar_t srcchar = *src;
    bool isthere = false;

    //look for current character in the set
    for (unsigned i=0; i<set.size (); i++)
    {
        wchar_t current = set[i];
        if (FdoCommonOSUtil::wcsnicmp (&current, &srcchar, 1) == 0)
        {
            isthere = true;
            break;
        }
    }

    if (isthere ^ reverse)
        bMatches = MatchesHere (pattern+1, src+1);
    else
        bMatches = false;


    return bMatches;
}


bool FdoExpressionEngineImp::Like (FdoDataValue* argLeft, FdoDataValue* argRight)
{
    if (FdoDataType_String != argLeft->GetDataType ())
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    if (FdoDataType_String != argRight->GetDataType ())
        throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));

    return (MatchesHere (((FdoStringValue*)argRight)->GetString (), ((FdoStringValue*)argLeft)->GetString ()));
}



FdoDataValue* FdoExpressionEngineImp::Add (FdoDataValue* argLeft, FdoDataValue* argRight)
{
    FdoDataValue* ret;

    bool bRetNull = (argLeft->IsNull() || argRight->IsNull());

    switch (argLeft->GetDataType ())
    {
        case FdoDataType_Boolean:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            break;
        case FdoDataType_Byte:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainByteValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () + ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () + ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () + ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainInt16Value (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () + ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () + ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () + ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () + ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_DateTime:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
        case FdoDataType_Decimal:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () + ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () + ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () + ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () + ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () + ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () + ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () + ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Double:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () + ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () + ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () + ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () + ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () + ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () + ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () + ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Int16:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainInt16Value (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () + ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () + ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () + ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainInt16Value (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () + ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () + ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () + ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () + ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Int32:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () + ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () + ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () + ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () + ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () + ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () + ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () + ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Int64:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () + ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () + ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () + ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () + ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () + ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () + ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () + ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Single:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () + ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () + ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () + ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () + ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () + ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () + ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () + ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_String:
        case FdoDataType_BLOB:
        case FdoDataType_CLOB:
        default:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    return (ret);
}

FdoDataValue* FdoExpressionEngineImp::Subtract (FdoDataValue* argLeft, FdoDataValue* argRight)
{
    FdoDataValue* ret;

    bool bRetNull = (argLeft->IsNull() || argRight->IsNull());

    switch (argLeft->GetDataType ())
    {
        case FdoDataType_Boolean:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            break;
        case FdoDataType_Byte:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainByteValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () - ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () - ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () - ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainInt16Value (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () - ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () - ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () - ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () - ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_DateTime:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
        case FdoDataType_Decimal:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () - ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () - ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () - ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () - ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () - ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () - ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () - ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Double:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () - ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () - ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () - ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () - ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () - ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () - ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () - ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Int16:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainInt16Value (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () - ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () - ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () - ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainInt16Value (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () - ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () - ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () - ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () - ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Int32:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () - ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () - ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () - ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () - ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () - ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () - ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () - ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Int64:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () - ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () - ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () - ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () - ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () - ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () - ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () - ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Single:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () - ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () - ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () - ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () - ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () - ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () - ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () - ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_String:
        case FdoDataType_BLOB:
        case FdoDataType_CLOB:
        default:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    // Handle NULL values  (use a coding shortcut):
    if (argLeft->IsNull() || argRight->IsNull())
        ret->SetNull();

    return (ret);
}

FdoDataValue* FdoExpressionEngineImp::Multiply (FdoDataValue* argLeft, FdoDataValue* argRight)
{
    FdoDataValue* ret;

    bool bRetNull = (argLeft->IsNull() || argRight->IsNull());

    switch (argLeft->GetDataType ())
    {
        case FdoDataType_Boolean:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            break;
        case FdoDataType_Byte:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainByteValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () * ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () * ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () * ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainInt16Value (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () * ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () * ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () * ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () * ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_DateTime:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
        case FdoDataType_Decimal:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () * ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () * ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () * ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () * ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () * ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () * ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () * ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Double:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () * ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () * ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () * ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () * ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () * ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () * ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () * ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Int16:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainInt16Value (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () * ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () * ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () * ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainInt16Value (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () * ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () * ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () * ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () * ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Int32:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () * ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () * ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () * ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () * ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () * ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () * ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () * ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Int64:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () * ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () * ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () * ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () * ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () * ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () * ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () * ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Single:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () * ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () * ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () * ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () * ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () * ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () * ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () * ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_String:
        case FdoDataType_BLOB:
        case FdoDataType_CLOB:
        default:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    // Handle NULL values  (use a coding shortcut):
    if (argLeft->IsNull() || argRight->IsNull())
        ret->SetNull();

    return (ret);
}

FdoDataValue* FdoExpressionEngineImp::Divide (FdoDataValue* argLeft, FdoDataValue* argRight)
{
    FdoDataValue* ret;

    bool bRetNull = (argLeft->IsNull() || argRight->IsNull());

    switch (argLeft->GetDataType ())
    {
        case FdoDataType_Boolean:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            break;
        case FdoDataType_Byte:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : (FdoDouble)((FdoByteValue*)argLeft)->GetByte () / ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () / ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () / ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : (FdoDouble)((FdoByteValue*)argLeft)->GetByte () / ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : (FdoDouble)((FdoByteValue*)argLeft)->GetByte () / ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : (FdoDouble)(FdoDouble)((FdoByteValue*)argLeft)->GetByte () / ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoByteValue*)argLeft)->GetByte () / ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_DateTime:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
        case FdoDataType_Decimal:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () / ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () / ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () / ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () / ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () / ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () / ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDecimalValue*)argLeft)->GetDecimal () / ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Double:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () / ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () / ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () / ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () / ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () / ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () / ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoDoubleValue*)argLeft)->GetDouble () / ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Int16:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : (FdoDouble)((FdoInt16Value*)argLeft)->GetInt16 () / ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () / ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () / ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : (FdoDouble)((FdoInt16Value*)argLeft)->GetInt16 () / ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : (FdoDouble)((FdoInt16Value*)argLeft)->GetInt16 () / ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : (FdoDouble)((FdoInt16Value*)argLeft)->GetInt16 () / ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt16Value*)argLeft)->GetInt16 () / ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Int32:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : (FdoDouble)((FdoInt32Value*)argLeft)->GetInt32 () / ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () / ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () / ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : (FdoDouble)((FdoInt32Value*)argLeft)->GetInt32 () / ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : (FdoDouble)((FdoInt32Value*)argLeft)->GetInt32 () / ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : (FdoDouble)((FdoInt32Value*)argLeft)->GetInt32 () / ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt32Value*)argLeft)->GetInt32 () / ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Int64:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () / (FdoDouble)((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () / ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () / ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () / (FdoDouble)((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () / (FdoDouble)((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : (FdoDouble)((FdoInt64Value*)argLeft)->GetInt64 () / ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoInt64Value*)argLeft)->GetInt64 () / ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_Single:
            switch (argRight->GetDataType ())
            {
                case FdoDataType_Boolean:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Byte:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () / ((FdoByteValue*)argRight)->GetByte ());
                    break;
                case FdoDataType_DateTime:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
                case FdoDataType_Decimal:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () / ((FdoDecimalValue*)argRight)->GetDecimal ());
                    break;
                case FdoDataType_Double:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () / ((FdoDoubleValue*)argRight)->GetDouble ());
                    break;
                case FdoDataType_Int16:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () / ((FdoInt16Value*)argRight)->GetInt16 ());
                    break;
                case FdoDataType_Int32:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () / ((FdoInt32Value*)argRight)->GetInt32 ());
                    break;
                case FdoDataType_Int64:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () / ((FdoInt64Value*)argRight)->GetInt64 ());
                    break;
                case FdoDataType_Single:
                    ret = ObtainDoubleValue (bRetNull, bRetNull ? 0 : ((FdoSingleValue*)argLeft)->GetSingle () / ((FdoSingleValue*)argRight)->GetSingle ());
                    break;
                case FdoDataType_String:
                case FdoDataType_BLOB:
                case FdoDataType_CLOB:
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            }
            break;
        case FdoDataType_String:
        case FdoDataType_BLOB:
        case FdoDataType_CLOB:
        default:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    // Handle NULL values  (use a coding shortcut):
    if (argLeft->IsNull() || argRight->IsNull())
        ret->SetNull();

    return (ret);
}

FdoDataValue* FdoExpressionEngineImp::Negate (FdoDataValue* argLeft)
{
    FdoDataValue* ret;

    bool bRetNull = argLeft->IsNull();

    switch (argLeft->GetDataType ())
    {
        case FdoDataType_Boolean:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
            break;
        case FdoDataType_Byte:
            ret = ObtainByteValue (bRetNull, bRetNull ? 0 : -((FdoByteValue*)argLeft)->GetByte ());
            break;
        case FdoDataType_DateTime:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
        case FdoDataType_Decimal:
            ret = ObtainDecimalValue (bRetNull, bRetNull ? 0.0 : -((FdoDecimalValue*)argLeft)->GetDecimal ());
            break;
        case FdoDataType_Double:
            ret = ObtainDoubleValue (bRetNull, bRetNull ? 0.0 : -((FdoDoubleValue*)argLeft)->GetDouble ());
            break;
        case FdoDataType_Int16:
            ret = ObtainInt16Value (bRetNull, bRetNull ? 0 : -((FdoInt16Value*)argLeft)->GetInt16 ());
            break;
        case FdoDataType_Int32:
            ret = ObtainInt32Value (bRetNull, bRetNull ? 0 : -((FdoInt32Value*)argLeft)->GetInt32 ());
            break;
        case FdoDataType_Int64:
            ret = ObtainInt64Value (bRetNull, bRetNull ? 0 : -((FdoInt64Value*)argLeft)->GetInt64 ());
            break;
        case FdoDataType_Single:
            ret = ObtainSingleValue (bRetNull, bRetNull ? 0.0f : -((FdoSingleValue*)argLeft)->GetSingle ());
            break;
        case FdoDataType_String:
        case FdoDataType_BLOB:
        case FdoDataType_CLOB:
        default:
            throw FdoException::Create (FdoException::NLSGetMessage (FDO_NLSID (FDO_62_PROPERTYVALUEFETCHTYPEMISMATCH)));
    }

    return (ret);
}



void FdoExpressionEngineImp::ValidateFilter( FdoClassDefinition *cls, FdoFilter *filter, 
											  FdoIdentifierCollection *selIds,
											  FdoIFilterCapabilities *filterCapabilities )
{
    class FdoCommonFilterValidator : public virtual FdoIExpressionProcessor, public virtual FdoIFilterProcessor
    {
    private:
        FdoClassDefinition* m_class;
		FdoIdentifierCollection *m_selectedIds;
		FdoIFilterCapabilities	*m_filterCapabilities;

    protected:

        void HandleExpr( FdoExpression *exp )
        {
            exp->Process( this );
        }
        void HandleFilter( FdoFilter *filter )
        {
            filter->Process( this );
        }
    public:

        FdoCommonFilterValidator( FdoClassDefinition *cls, FdoIdentifierCollection *selIds, FdoIFilterCapabilities *filterCapabilities )
        {
            m_class = cls;
			m_selectedIds = selIds;
			m_filterCapabilities = filterCapabilities;
        }

        virtual void Dispose() { delete this; }

         /// <summary>Increase the reference count.</summary>
        /// <returns>Returns the new reference count (value for debugging use only).</returns>
        FdoInt32 AddRef()
        {
            // NOTE: due to multiple inheritance, there is an ambiguity in which AddRef() method to call.
            //  Calling BOTH AddRef() methods leads to instances of this class being prematurely released.
            return FdoIFilterProcessor::AddRef();
        }

        /// <summary>Decrease the reference count.</summary>
        /// <returns>Returns the new reference count (value for debugging use only).</returns>
        FdoInt32 Release ()
        {
            // NOTE: due to multiple inheritance, there is an ambiguity in which Release() method to call.
            //  Calling BOTH Release() methods leads to instances of this class being prematurely released.
            return FdoIFilterProcessor::Release();
        }

        virtual void ProcessBinaryExpression(FdoBinaryExpression& expr)
        {
            HandleExpr( FdoPtr<FdoExpression>(expr.GetLeftExpression()) );
            HandleExpr( FdoPtr<FdoExpression>(expr.GetRightExpression()) );
        }

// Supress warnings C4100: 'expr' : unreferenced formal parameter
#pragma warning( disable : 4100 )
        virtual void ProcessBooleanValue(FdoBooleanValue& expr) {  }
        virtual void ProcessByteValue(FdoByteValue& expr){   }
        virtual void ProcessDateTimeValue(FdoDateTimeValue& expr){   }
        virtual void ProcessDoubleValue(FdoDoubleValue& expr){   }
        virtual void ProcessDecimalValue(FdoDecimalValue& expr){   }
        virtual void ProcessInt16Value(FdoInt16Value& expr){  }
        virtual void ProcessInt32Value(FdoInt32Value& expr){  }
        virtual void ProcessInt64Value(FdoInt64Value& expr){  }
        virtual void ProcessSingleValue(FdoSingleValue& expr){  }
        virtual void ProcessStringValue(FdoStringValue& expr){  }
        virtual void ProcessBLOBValue(FdoBLOBValue& expr){   }
        virtual void ProcessCLOBValue(FdoCLOBValue& expr){  }
        virtual void ProcessFunction(FdoFunction& expr)
        {
            FdoPtr<FdoExpressionCollection>col = expr.GetArguments();
            for(int i=0; i<col->GetCount(); i++)
                HandleExpr( FdoPtr<FdoExpression>(col->GetItem( i ) ) );
        }
        virtual void ProcessGeometryValue(FdoGeometryValue& expr){  }
        virtual void ProcessParameter(FdoParameter& expr){  }
        virtual void ProcessUnaryExpression(FdoUnaryExpression& expr)
        {
            HandleExpr( FdoPtr<FdoExpression>(expr.GetExpression()) );
        }
        virtual void ProcessBinaryLogicalOperator(FdoBinaryLogicalOperator& filter)
        {
            HandleFilter( FdoPtr<FdoFilter>(filter.GetLeftOperand()) );
            HandleFilter( FdoPtr<FdoFilter>(filter.GetRightOperand()) );
        }
        virtual void ProcessComparisonCondition(FdoComparisonCondition& filter)
        {
            HandleExpr( FdoPtr<FdoExpression>(filter.GetLeftExpression()) );
            HandleExpr( FdoPtr<FdoExpression>(filter.GetRightExpression()) );
        }
        virtual void ProcessDistanceCondition(FdoDistanceCondition& filter){  }

        virtual void ProcessInCondition(FdoInCondition& filter)
        {
            ProcessIdentifier( *(FdoPtr<FdoIdentifier>(filter.GetPropertyName())) );
        }
        virtual void ProcessNullCondition(FdoNullCondition& filter)
        {
            ProcessIdentifier( *(FdoPtr<FdoIdentifier>(filter.GetPropertyName())) );
        }
        virtual void ProcessSpatialCondition(FdoSpatialCondition& filter)
		{  
            // Nothing to validate here if no filter capabilities supplied
            if ( m_filterCapabilities ) 
            {
			    FdoSpatialOperations	op = filter.GetOperation();
			    bool					found = false;
			    int						numSpatialOps;

			    FdoSpatialOperations	*supportedSpatialOps = m_filterCapabilities->GetSpatialOperations( numSpatialOps );

			    for ( int i =0; i < numSpatialOps && !found; i++ )
			    {
				    found = ( op == supportedSpatialOps[i] );
			    }
			    if ( !found )
				    throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_105_UNSUPPORTED_SPATIAL_OPERATION)));
            }
		}

        virtual void ProcessUnaryLogicalOperator(FdoUnaryLogicalOperator& filter)
        {
            HandleFilter( FdoPtr<FdoFilter>(filter.GetOperand()) );
        }

        virtual void ProcessIdentifier(FdoIdentifier& expr)
        {
			// Return in case ids validation not required.
			if ( m_class == NULL )
				return;

             // This is used to validate the property names referenced in the filter.
            FdoPtr<FdoPropertyDefinitionCollection> props = m_class->GetProperties();
            FdoPtr<FdoReadOnlyPropertyDefinitionCollection> baseProps = m_class->GetBaseProperties();
            FdoPtr<FdoPropertyDefinition> prop;

            int length;
            // If any of the identifiers is a decorated name of the form <obj proper>.<property>, then this is
            // an association or object proeperty; assume it exist for now.
            if( expr.GetScope( length ) && length > 0 )
                return;

			// Check properties
            prop = props->FindItem( expr.GetName() );
			bool	found = ( prop != NULL );

            if( !found )
			{
				// Check base properties
				try
				{
					prop = baseProps->GetItem( expr.GetName() );
					found = true;
				}
				catch(FdoException *exp )
				{
					exp->Release();
				}

				// Check computed identifiers
				if( !found && m_selectedIds )
				{
					FdoPtr<FdoIdentifier>	id = m_selectedIds->FindItem( expr.GetName() );
					found = ( id != NULL );
				}
			}

			if ( !found )
				throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_81_IDENTIFIER_NOT_FOUND), expr.GetName()));
        }

        virtual void ProcessComputedIdentifier(FdoComputedIdentifier& expr)
        {
            HandleExpr( FdoPtr<FdoExpression>(expr.GetExpression()) );
        }
    };

    FdoCommonFilterValidator  validator(cls, selIds, filterCapabilities);
    filter->Process( &validator ); 
}

FdoFilter* FdoExpressionEngineImp::OptimizeFilter( FdoFilter *filter )
{
	// This is mostly a place holder for potential filter optimization
    class FdoCommonFilterOptimizer :  public virtual FdoIFilterProcessor
    {
    private:
        StackFilter          m_stack;
        FdoPtr<FdoFgfGeometryFactory> m_gf;

    protected:
        void HandleFilter( FdoFilter *filter )
        {
            filter->Process( this );
        }
        enum FdoOptimizeResultType
        {
            FdoOptimizeResultType_Invalid        = 0,  // query is invalid
            FdoOptimizeResultType_NoOptimize     = 1,  // query cannot be optimized
            FdoOptimizeResultType_UseSmSpatial   = 2,  // use smaller geom filter
            FdoOptimizeResultType_UseBgSpatial   = 3,  // use bigger geom filter
        };
        static FdoOptimizeResultType GetOptimizeOperation(FdoSpatialOperations smallOperation, FdoSpatialOperations bigOperation)
        {
            // by default query cannot be optimized
            FdoOptimizeResultType retVal = FdoOptimizeResultType_NoOptimize;
            switch(bigOperation)
            {
                case FdoSpatialOperations_Contains:
                    switch (smallOperation)
                    {
                        case FdoSpatialOperations_Contains:
                            retVal = FdoOptimizeResultType_UseBgSpatial;
                            break;
                        case FdoSpatialOperations_Equals:
                        case FdoSpatialOperations_CoveredBy:
                        case FdoSpatialOperations_Inside:
                        case FdoSpatialOperations_Within:
                        case FdoSpatialOperations_Touches:
                        case FdoSpatialOperations_Crosses:
                        case FdoSpatialOperations_Overlaps:
                        case FdoSpatialOperations_Disjoint:
                            retVal = FdoOptimizeResultType_Invalid;
                            break;
                        default:
                            retVal = FdoOptimizeResultType_NoOptimize;
                            break;
                    }
                    break;
                case FdoSpatialOperations_Crosses:
                    switch (smallOperation)
                    {
                        case FdoSpatialOperations_Equals:
                        case FdoSpatialOperations_Within:
                        case FdoSpatialOperations_CoveredBy:
                        case FdoSpatialOperations_Inside:
                            retVal = FdoOptimizeResultType_Invalid;
                            break;
                        default:
                            retVal = FdoOptimizeResultType_NoOptimize;
                            break;
                    }
                    break;
                case FdoSpatialOperations_Overlaps:
                    switch (smallOperation)
                    {
                        case FdoSpatialOperations_Inside:
                        case FdoSpatialOperations_Within:
                        case FdoSpatialOperations_CoveredBy:
                        case FdoSpatialOperations_Equals:
                            retVal = FdoOptimizeResultType_Invalid;
                            break;
                        default:
                            retVal = FdoOptimizeResultType_NoOptimize;
                            break;
                    }
                    break;
                case FdoSpatialOperations_Touches:
                    switch (smallOperation)
                    {
                        case FdoSpatialOperations_Equals:
                        case FdoSpatialOperations_Within:
                        case FdoSpatialOperations_CoveredBy:
                        case FdoSpatialOperations_Inside:
                        case FdoSpatialOperations_Crosses:
                        case FdoSpatialOperations_Overlaps:
                        case FdoSpatialOperations_Contains:
                        case FdoSpatialOperations_Intersects:
                            retVal = FdoOptimizeResultType_Invalid;
                            break;
                        default:
                            retVal = FdoOptimizeResultType_NoOptimize;
                            break;
                    }
                    break;
                case FdoSpatialOperations_Equals:
                    switch (smallOperation)
                    {
                        case FdoSpatialOperations_Equals:
                        case FdoSpatialOperations_Within:
                        case FdoSpatialOperations_CoveredBy:
                        case FdoSpatialOperations_Inside:
                        case FdoSpatialOperations_Crosses:
                        case FdoSpatialOperations_Overlaps:
                            retVal = FdoOptimizeResultType_Invalid;
                            break;
                        default:
                            retVal = FdoOptimizeResultType_NoOptimize;
                            break;
                    }
                    break;
                case FdoSpatialOperations_Disjoint:
                    switch (smallOperation)
                    {
                        case FdoSpatialOperations_Equals:
                        case FdoSpatialOperations_Within:
                        case FdoSpatialOperations_CoveredBy:
                        case FdoSpatialOperations_Inside:
                        case FdoSpatialOperations_Touches:
                        case FdoSpatialOperations_Crosses:
                        case FdoSpatialOperations_Overlaps:
                        case FdoSpatialOperations_Contains:
                        case FdoSpatialOperations_Intersects:
                            retVal = FdoOptimizeResultType_Invalid;
                            break;
                        default:
                            retVal = FdoOptimizeResultType_NoOptimize;
                            break;
                    }
                    break;
                case FdoSpatialOperations_CoveredBy:
                    switch (smallOperation)
                    {
                        case FdoSpatialOperations_Equals:
                        case FdoSpatialOperations_Within:
                        case FdoSpatialOperations_CoveredBy:
                        case FdoSpatialOperations_Inside:
                            retVal = FdoOptimizeResultType_UseSmSpatial;
                            break;
                        default:
                            retVal = FdoOptimizeResultType_NoOptimize;
                            break;
                    }
                    break;
                case FdoSpatialOperations_Within:
                    switch(smallOperation)
                    {
                        case FdoSpatialOperations_Equals:
                        case FdoSpatialOperations_Within:
                        case FdoSpatialOperations_CoveredBy:
                        case FdoSpatialOperations_Inside:
                            retVal = FdoOptimizeResultType_UseSmSpatial;
                            break;
                        default:
                            retVal = FdoOptimizeResultType_NoOptimize;
                            break;
                    }
                    break;
                case FdoSpatialOperations_Inside:
                    switch(smallOperation)
                    {
                        case FdoSpatialOperations_Equals:
                        case FdoSpatialOperations_Within:
                        case FdoSpatialOperations_CoveredBy:
                        case FdoSpatialOperations_Inside:
                            retVal = FdoOptimizeResultType_UseSmSpatial;
                            break;
                        default:
                            retVal = FdoOptimizeResultType_NoOptimize;
                            break;
                    }
                    break;
                case FdoSpatialOperations_Intersects:
                    switch(smallOperation)
                    {
                        case FdoSpatialOperations_Touches:
                        case FdoSpatialOperations_Equals:
                        case FdoSpatialOperations_Within:
                        case FdoSpatialOperations_CoveredBy:
                        case FdoSpatialOperations_Inside:
                        case FdoSpatialOperations_Crosses:
                        case FdoSpatialOperations_Overlaps:
                        case FdoSpatialOperations_Intersects:
                        case FdoSpatialOperations_Contains:
                            retVal = FdoOptimizeResultType_UseSmSpatial;
                            break;
                        default:
                            retVal = FdoOptimizeResultType_NoOptimize;
                            break;
                    }
                    break;
                case FdoSpatialOperations_EnvelopeIntersects:
                    switch(smallOperation)
                    {
                        case FdoSpatialOperations_Touches:
                        case FdoSpatialOperations_Equals:
                        case FdoSpatialOperations_Within:
                        case FdoSpatialOperations_CoveredBy:
                        case FdoSpatialOperations_Inside:
                        case FdoSpatialOperations_Crosses:
                        case FdoSpatialOperations_Overlaps:
                        case FdoSpatialOperations_Intersects:
                        case FdoSpatialOperations_EnvelopeIntersects:
                        case FdoSpatialOperations_Contains:
                            retVal = FdoOptimizeResultType_UseSmSpatial;
                            break;
                        default:
                            retVal = FdoOptimizeResultType_NoOptimize;
                            break;
                    }
                    break;
            }
            return retVal;
        }
        // when resultIsInvalid = true that mean the whole AND filter can be replaced by the returned filter which will generate an empty result
        FdoFilter* TryOptimize(FdoSpatialCondition* leftFilter, FdoSpatialCondition* rightFilter, bool& resultIsInvalid)
        {
			try
            {
                resultIsInvalid = false;
			    FdoPtr<FdoExpression> lExpr = leftFilter->GetGeometry ();
			    FdoPtr<FdoExpression> rExpr = rightFilter->GetGeometry ();
			    FdoGeometryValue* lGv = static_cast<FdoGeometryValue*>(lExpr.p);
			    FdoGeometryValue* rGv = static_cast<FdoGeometryValue*>(rExpr.p);
                FdoPtr<FdoByteArray> lba = lGv->GetGeometry ();
                FdoPtr<FdoByteArray> rba = rGv->GetGeometry ();
                FdoPtr<FdoIGeometry> geomRight = m_gf->CreateGeometryFromFgf (rba);
                FdoPtr<FdoIGeometry> geomLeft = m_gf->CreateGeometryFromFgf (lba);
                
                FdoSpatialOperations leftOp = leftFilter->GetOperation();
                FdoSpatialOperations rightOp = rightFilter->GetOperation();
                    
                FdoOptimizeResultType retOpt = FdoOptimizeResultType_NoOptimize;
				if( FdoSpatialUtility::Evaluate (geomLeft, FdoSpatialOperations_Inside, geomRight) )
				{
                    retOpt = FdoCommonFilterOptimizer::GetOptimizeOperation(leftOp, rightOp);
                    switch(retOpt)
                    {
                        case FdoOptimizeResultType_NoOptimize:
					        return NULL;
                        case FdoOptimizeResultType_UseSmSpatial:
                            // keep left geom since we keept left spatial condition
					        return FDO_SAFE_ADDREF(leftFilter);
                        case FdoOptimizeResultType_UseBgSpatial:
                            // get right geom since we kept right spatial cond
                            return FDO_SAFE_ADDREF(rightFilter);
                        case FdoOptimizeResultType_Invalid:
                            // invalid spatial condition combination
                            break;
                    }
				}
                else if( FdoSpatialUtility::Evaluate (geomRight, FdoSpatialOperations_Inside, geomLeft ) )
			    {
                    retOpt = FdoCommonFilterOptimizer::GetOptimizeOperation(rightOp, leftOp);
                    switch(retOpt)
                    {
                        case FdoOptimizeResultType_NoOptimize:
                            return NULL;
                        case FdoOptimizeResultType_UseSmSpatial:
                            // get right geom since we kept right spatial cond
                            return FDO_SAFE_ADDREF(rightFilter);
                        case FdoOptimizeResultType_UseBgSpatial:
                            // keep left geom since we keept left spatial condition
					        return FDO_SAFE_ADDREF(leftFilter);
                        case FdoOptimizeResultType_Invalid:
                            // invalid spatial condition combination
                            break;
                    }
			    }
                if( (retOpt == FdoOptimizeResultType_Invalid ) || 
                    (FdoSpatialUtility::Evaluate (geomRight, FdoSpatialOperations_Disjoint, geomLeft ) &&
                    !((rightOp == FdoSpatialOperations_Crosses || rightOp == FdoSpatialOperations_Intersects || rightOp == FdoSpatialOperations_Overlaps || rightOp == FdoSpatialOperations_EnvelopeIntersects)
                    && (leftOp == FdoSpatialOperations_Crosses || leftOp == FdoSpatialOperations_Intersects || leftOp == FdoSpatialOperations_Overlaps || leftOp == FdoSpatialOperations_EnvelopeIntersects))))
				{
                    resultIsInvalid = true;
					// If the condition do not overlap, then replace it with a filter that returns 0 features.
#ifdef _WIN32
					double small_dbl  =(double)(-9223372036854775807i64 - 1);
#else
					double small_dbl  =(double)(-9223372036854775807LL - 1);
#endif
					double coords[] = { small_dbl, small_dbl, 
										small_dbl, small_dbl, 
										small_dbl, small_dbl, 
										small_dbl, small_dbl, 
										small_dbl, small_dbl }; 

					FdoPtr<FdoILinearRing> outer = m_gf->CreateLinearRing(0, 10, coords);

					FdoPtr<FdoIPolygon> poly = m_gf->CreatePolygon(outer, NULL);

					FdoPtr<FdoByteArray> polyfgf = m_gf->GetFgf(poly);
					FdoPtr<FdoGeometryValue> gv = FdoGeometryValue::Create(polyfgf);
					return FdoSpatialCondition::Create(FdoPtr<FdoIdentifier>(rightFilter->GetPropertyName())->GetName(), FdoSpatialOperations_EnvelopeIntersects, gv);
				}
            }
            catch(FdoException* exc)
            {
                exc->Release();
            }
            return NULL;
        }

    public:

		FdoCommonFilterOptimizer( )
        {
            m_gf = FdoFgfGeometryFactory::GetInstance ();
        }

        virtual void Dispose() { delete this; }
        
        void ClearFilterList(FilterList& lst)
        {
            for(FilterList::iterator it = lst.begin(); it < lst.end(); it++)
                FDO_SAFE_RELEASE(it->second);
            lst.clear();
        }

        // when it returns lst will contain max 1 filter!
        void OptimizeSubSet(FilterList& lst)
        {
            FilterList lstEnvInt, lstSpatCond, lstOtherCond;
            int cnt = lst.size();
            bool invalidFilter = false;
            for (int i = 0; i < cnt; i++)
            {
                if (lst[i].first == OptFilterType_SpaCond)
                {
                    FdoSpatialCondition* filter = static_cast<FdoSpatialCondition*>(lst[i].second);
                    if (filter->GetOperation() == FdoSpatialOperations_EnvelopeIntersects)
                        lstEnvInt.push_back(lst[i]);
                    else
                        lstSpatCond.push_back(lst[i]);
                }
                else
                    lstOtherCond.push_back(lst[i]);
            }
            FdoSpatialCondition* leftFlt = NULL;
            FdoSpatialCondition* rightFlt = NULL;
            cnt = lstEnvInt.size();
            if (cnt > 1)
            {
                // try optimize all EnvelopeIntersects in case we have more
                for (int i = 0; i < cnt; i++)
                {
                    if ((i + 1) < cnt)
                    {
                        leftFlt = static_cast<FdoSpatialCondition*>(lstEnvInt[i].second);
                        rightFlt = static_cast<FdoSpatialCondition*>(lstEnvInt[i+1].second);
                        FdoFilter* flt = TryOptimize(leftFlt, rightFlt, invalidFilter);
                        if (flt != NULL)
                        {
                            if (invalidFilter)
                            {
                                // we can reduce the whole filter to a filter which will generate empty results
                                lst.clear();
                                ClearFilterList(lstEnvInt);
                                ClearFilterList(lstSpatCond);
                                ClearFilterList(lstOtherCond);
                                lst.push_back(std::make_pair(OptFilterType_SpaCond, flt));
                                return;
                            }
                            // erase i and overwrite "i+1"
                            lstEnvInt.erase(lstEnvInt.begin()+i);
                            lstEnvInt[i] = std::make_pair(OptFilterType_SpaCond, flt);
                            FDO_SAFE_RELEASE(leftFlt);
                            FDO_SAFE_RELEASE(rightFlt);
                            i--;
                            cnt--;
                        }
                    }
                }
            }
            int cntSec = lstSpatCond.size();
            if (cntSec > 1)
            {
                // rare case when two spatial condition are in the same query
                // try optimize all spatial cond in case we have more
                for (int i = 0; i < cntSec; i++)
                {
                    if ((i + 1) < cntSec)
                    {
                        leftFlt = static_cast<FdoSpatialCondition*>(lstSpatCond[i].second);
                        rightFlt = static_cast<FdoSpatialCondition*>(lstSpatCond[i+1].second);
                        FdoFilter* flt = TryOptimize(leftFlt, rightFlt, invalidFilter);
                        if (flt != NULL)
                        {
                            if (invalidFilter)
                            {
                                // we can reduce the whole filter to a filter which will generate empty results
                                lst.clear();
                                ClearFilterList(lstEnvInt);
                                ClearFilterList(lstSpatCond);
                                ClearFilterList(lstOtherCond);
                                lst.push_back(std::make_pair(OptFilterType_SpaCond, flt));
                                return;
                            }
                            // erase i and overwrite "i+1"
                            lstSpatCond.erase(lstSpatCond.begin()+i);
                            lstSpatCond[i] = std::make_pair(OptFilterType_SpaCond, flt);
                            FDO_SAFE_RELEASE(leftFlt);
                            FDO_SAFE_RELEASE(rightFlt);
                            i--;
                            cntSec--;
                        }
                    }
                }
            }
            // often case when we have a EnvelopeIntersects and a spatial cond
            if (cnt > 0 && cntSec > 0)
            {
                // not nice but I do not see other way
                for (int i = 0; i < cnt; i++)
                {
                    for (int y = 0; y < cntSec; y++)
                    {
                        if ((i >= 0 && i < cnt) && (y >= 0 && y < cntSec))
                        {
                            leftFlt = static_cast<FdoSpatialCondition*>(lstEnvInt[i].second);
                            rightFlt = static_cast<FdoSpatialCondition*>(lstSpatCond[y].second);
                            FdoFilter* flt = TryOptimize(leftFlt, rightFlt, invalidFilter);
                            if (flt != NULL)
                            {
                                if (invalidFilter)
                                {
                                    // we can reduce the whole filter to a filter which will generate empty results
                                    lst.clear();
                                    ClearFilterList(lstEnvInt);
                                    ClearFilterList(lstSpatCond);
                                    ClearFilterList(lstOtherCond);
                                    lst.push_back(std::make_pair(OptFilterType_SpaCond, flt));
                                    return;
                                }
                                FdoSpatialCondition* newFilter = static_cast<FdoSpatialCondition*>(flt);
                                if (newFilter->GetOperation() == FdoSpatialOperations_EnvelopeIntersects)
                                {
                                    lstEnvInt[i] = std::make_pair(OptFilterType_SpaCond, flt);
                                    lstSpatCond.erase(lstSpatCond.begin()+y);
                                    y--;
                                    cntSec--;
                                }
                                else
                                {
                                    lstSpatCond[y] = std::make_pair(OptFilterType_SpaCond, flt);
                                    lstEnvInt.erase(lstEnvInt.begin()+i);
                                    i--;
                                    cnt--;
                                }
                                FDO_SAFE_RELEASE(leftFlt);
                                FDO_SAFE_RELEASE(rightFlt);
                            }
                        }
                    }
                }
            }
            // we can clear it since we have all filters in the opter lists
            lst.clear();
            if (lstEnvInt.size() != 0)
                lst.insert(lst.begin(), lstEnvInt.begin(), lstEnvInt.end());
            if (lstSpatCond.size() != 0)
                lst.insert(lst.end(), lstSpatCond.begin(), lstSpatCond.end());
            if (lstOtherCond.size() != 0)
                lst.insert(lst.end(), lstOtherCond.begin(), lstOtherCond.end());

            if (lst.size() > 1)
                lst.push_back(std::make_pair(OptFilterType_AndCond, GenerateAndFilter(lst)));
        }

        FdoFilter* GenerateAndFilter(FilterList& lst)
        {
            FdoPtr<FdoBinaryLogicalOperator> blof = FdoBinaryLogicalOperator::Create();
            FdoFilter* filter = FDO_SAFE_ADDREF(blof.p);
            int cnt = lst.size();

            FdoFilter* tmp = NULL;
            for (int i = 0; i < cnt; i++)
            {
                if ((i + 2) >= cnt)
                {
                    tmp = lst[i].second;
                    blof->SetLeftOperand(tmp);
                    FDO_SAFE_RELEASE(tmp);
                    
                    tmp = lst[i+1].second;
                    blof->SetRightOperand(tmp);
                    FDO_SAFE_RELEASE(tmp);

                    blof->SetOperation(FdoBinaryLogicalOperations_And);
                    break;
                }
                else
                {
                    tmp = lst[i].second;
                    blof->SetLeftOperand(tmp);
                    FDO_SAFE_RELEASE(tmp);

                    FdoPtr<FdoBinaryLogicalOperator> sblof = FdoBinaryLogicalOperator::Create();
                    sblof->SetOperation(FdoBinaryLogicalOperations_And);
                    blof->SetRightOperand(sblof);
                    blof = sblof;
                }
            }
            lst.clear();
            return filter;
        }


        virtual void ProcessBinaryLogicalOperator(FdoBinaryLogicalOperator& filter)
        {
            FilterList lst;
            FdoBinaryLogicalOperations bop = filter.GetOperation();
            if (bop == FdoBinaryLogicalOperations_Or )
            {
                OptFilterType rez = OptFilterType_ComCond;
                m_stack.push_back(&lst);
                FdoPtr<FdoFilter> leftNewFlt;
                FdoPtr<FdoFilter> rightNewFlt;

                HandleFilter( FdoPtr<FdoFilter>(filter.GetLeftOperand()) );
                if (lst.size() > 1)
                    OptimizeSubSet(lst);
                if (lst.size() == 1)
                    leftNewFlt = FDO_SAFE_ADDREF(lst[0].second);

                for (FilterList::iterator it = lst.begin(); it < lst.end(); it++)
                    FDO_SAFE_RELEASE(it->second);
                lst.clear();
                
                HandleFilter( FdoPtr<FdoFilter>(filter.GetRightOperand()) );
                if (lst.size() > 1)
                    OptimizeSubSet(lst);
                if (lst.size() == 1)
                    rightNewFlt = FDO_SAFE_ADDREF(lst[0].second);

                for (FilterList::iterator it = lst.begin(); it < lst.end(); it++)
                    FDO_SAFE_RELEASE(it->second);
                lst.clear();
                
                m_stack.pop_back();
                if (leftNewFlt != NULL && rightNewFlt != NULL)
                {
                    FdoFilter* flt = FdoBinaryLogicalOperator::Create(leftNewFlt, FdoBinaryLogicalOperations_Or, rightNewFlt);
                    m_stack.back()->push_back(std::make_pair(OptFilterType_OrCond, flt));
                }
                else if (leftNewFlt != NULL)
                {
                    FdoFilter* flt = leftNewFlt;
                    FDO_SAFE_ADDREF(flt);
                    m_stack.back()->push_back(std::make_pair(rez, flt));
                }
                else if (leftNewFlt != NULL)
                {
                    FdoFilter* flt = rightNewFlt;
                    FDO_SAFE_ADDREF(flt);
                    m_stack.back()->push_back(std::make_pair(rez, flt));
                }
            }
            else
            {
                HandleFilter( FdoPtr<FdoFilter>(filter.GetLeftOperand()) );
                HandleFilter( FdoPtr<FdoFilter>(filter.GetRightOperand()) );
            }
        }
        virtual void ProcessComparisonCondition(FdoComparisonCondition& filter)
        {
            FdoFilter* flt = &filter;
            FDO_SAFE_ADDREF(flt);
            m_stack.back()->push_back(std::make_pair(OptFilterType_ComCond, flt));
        }
        virtual void ProcessDistanceCondition(FdoDistanceCondition& filter)
		{  
            FdoFilter* flt = &filter;
            FDO_SAFE_ADDREF(flt);
            m_stack.back()->push_back(std::make_pair(OptFilterType_DistCond, flt));
		}

        virtual void ProcessInCondition(FdoInCondition& filter)
        {
            FdoFilter* flt = &filter;
            FDO_SAFE_ADDREF(flt);
            m_stack.back()->push_back(std::make_pair(OptFilterType_InCond, flt));
        }
        virtual void ProcessNullCondition(FdoNullCondition& filter)
        {
            FdoFilter* flt = &filter;
            FDO_SAFE_ADDREF(flt);
            m_stack.back()->push_back(std::make_pair(OptFilterType_NullCond, flt));
        }
        virtual void ProcessSpatialCondition(FdoSpatialCondition& filter)
		{  
            FdoFilter* flt = &filter;
            FDO_SAFE_ADDREF(flt);
            m_stack.back()->push_back(std::make_pair(OptFilterType_SpaCond, flt));
		}

        virtual void ProcessUnaryLogicalOperator(FdoUnaryLogicalOperator& filter)
        {
            FdoFilter* flt = &filter;
            FDO_SAFE_ADDREF(flt);
            m_stack.back()->push_back(std::make_pair(OptFilterType_UnarCond, flt));
        }

        FdoFilter* OptimizeFilter(FdoFilter* filter)
        {
            // in case we want to not change the filter we can copy it
            // however this will add some performance loss
            //m_copyFilter = FdoExpressionEngineCopyFilter::Copy(filter);
            FilterList baseFilterLst;
            m_stack.push_back(&baseFilterLst);
            
            filter->Process(this);
            OptimizeSubSet(baseFilterLst);

            if (baseFilterLst.size() == 0)
                return NULL; // here we should return an "empty" spatial filter
            else
            {
                filter = baseFilterLst[0].second; // it's already add ref
                m_stack.clear();
                return filter;
            }
        }
    };

    FdoCommonFilterOptimizer optimizer;
    return optimizer.OptimizeFilter(filter); 
}

FdoLiteralValue* FdoExpressionEngineImp::Evaluate(FdoExpression *expression)
{
    FdoCommonExpressionType exprType;
    
    bool bExpressionFound = false;
	for (int i=0; i<m_ExpressionCacheCurrent; i++)
	{
		if (m_ExpressionCache[i].m_Address == expression)
		{
            bExpressionFound = true;
            mAggrIdents = m_ExpressionCache[i].m_AggrIdents;
			break;
		}
	}

    if (bExpressionFound == false)
    {
        PopulateFunctions();
        mAggrIdents = FdoExpressionEngineUtilDataReader::GetAggregateFunctions(m_AllFunctions, expression, exprType);
        if (m_ExpressionCacheCurrent >= m_ExpressionCacheSize)
		{
            delete [] m_ExpressionCache;
            m_ExpressionCacheSize *= 2;
            m_ExpressionCache = new ExpressionCache[m_ExpressionCacheSize];
		}
   		m_ExpressionCache[m_ExpressionCacheCurrent].m_Address = expression;
        m_ExpressionCache[m_ExpressionCacheCurrent].m_AggrIdents = mAggrIdents;
        m_ExpressionCacheCurrent++;
    }

    if ((mAggrIdents != NULL) && (mAggrIdents->GetCount() > 0))
    {
        EvaluateAggregateExpression();
    }

	expression->Process (this);
    FdoLiteralValue* result = (FdoLiteralValue*)m_retvals.back ();
    PotentialRelinquishLiteralValue(result);
	m_retvals.pop_back ();
    return result;
};

FdoLiteralValue *FdoExpressionEngineImp::Evaluate(FdoIdentifier& expr)
{
	ProcessIdentifier(expr);
	FdoLiteralValue* value = (FdoLiteralValue*)m_retvals.back ();
    PotentialRelinquishLiteralValue(value);
	m_retvals.pop_back();
	return value;
}

FdoLiteralValue *FdoExpressionEngineImp::Evaluate(FdoString* name)
{
    ProcessIdentifier(name);
    FdoLiteralValue* value = (FdoLiteralValue*)m_retvals.back ();
    PotentialRelinquishLiteralValue(value);
    m_retvals.pop_back();
    return value;
}

FdoFunctionDefinitionCollection* FdoExpressionEngineImp::GetStandardFunctions()
{
    FdoFunctionDefinitionCollection *functionDefinitions = NULL;

    try
    {
        mutex.Enter();
        FdoPtr<FdoExpressionEngineFunctionCollection> functions = initFunction.GetStandardFunctions();
        functionDefinitions = DeepCopyFunctionDefinitions(functions);
        functions = NULL;
        mutex.Leave();
    }
    catch (FdoException *)
    {
        mutex.Leave();
        throw;
    }
    catch (...)
    {
        mutex.Leave();
        throw;
    }

	return functionDefinitions;

}

FdoFunctionDefinitionCollection *FdoExpressionEngineImp::GetAllFunctions()
{
    PopulateFunctions();
    return FDO_SAFE_ADDREF(m_AllFunctions.p);
}


void FdoExpressionEngineImp::ProcessAggregateFunctions()
{
    m_processingAggregate = true;

    for (FdoInt32 i=0; i<mAggrIdents->GetCount(); i++)
    {
	    FdoFunction* func = (*mAggrIdents)[i];
	    int j;

	    if (m_UserDefinedFunctions)
	    {
		    for (j=0; j<m_UserDefinedFunctions->GetCount(); j++)
		    {
			    FdoPtr<FdoExpressionEngineIFunction> functionExtension = m_UserDefinedFunctions->GetItem(j);
			    FdoPtr<FdoFunctionDefinition> function = functionExtension->GetFunctionDefinition();
			    if (FdoCommonStringUtil::StringCompareNoCase(function->GetName(), func->GetName()) == 0)
			    {
				    FdoExpressionEngineIAggregateFunction *aggregateFunction = static_cast<FdoExpressionEngineIAggregateFunction *>(functionExtension.p->CreateObject());
				    m_AggregateFunctions.push_back(aggregateFunction);
				    break;
			    }
		    }
	    }

        if (m_UserDefinedFunctions == NULL || i == m_UserDefinedFunctions->GetCount())
        {
            try
            {
                mutex.Enter();
                FdoPtr<FdoExpressionEngineFunctionCollection> functions = initFunction.GetAllFunctions();
	            for (j=0; j<functions->GetCount(); j++)
	            {
                    FdoPtr<FdoExpressionEngineIFunction> functionDefinition = functions->GetItem(j);
		            FdoPtr<FdoFunctionDefinition> function = functionDefinition->GetFunctionDefinition();
		            if (function->IsAggregate())
		            {
			            if (FdoCommonStringUtil::StringCompareNoCase(function->GetName(), func->GetName()) == 0)
			            {
				            FdoExpressionEngineIAggregateFunction *aggregateFunction = static_cast<FdoExpressionEngineIAggregateFunction *>(functionDefinition->CreateObject());
				            m_AggregateFunctions.push_back(aggregateFunction);
				            break;
			            }
		            }
	            }
	            if (j == functions->GetCount())
                {
                    throw FdoException::Create (FdoException::NLSGetMessage(FDO_NLSID(FDO_89_UNSUPPORTED_FUNCTION), func->GetName()));
                }
                functions = NULL;
                mutex.Leave();
            }
            catch (FdoException *)
            {
                mutex.Leave();
                throw;
            }
            catch (...)
            {
                mutex.Leave();
                throw;
            }
        }
    }
    m_dataRead = false;
    while (m_reader->ReadNext())
    {
        m_dataRead = true;
	    for (FdoInt32 i=0; i<mAggrIdents->GetCount(); i++)
	    {
		    m_CurrentIndex = i;
		    FdoFunction* func = (*mAggrIdents)[i];
		    func->Process(this);
	    }
    }
}



void FdoExpressionEngineImp::EvaluateAggregateExpression()
{
    ProcessAggregateFunctions();
	m_processingAggregate = false;
}

FdoPropertyValueCollection* FdoExpressionEngineImp::RunQuery()
{
	FdoCommonExpressionType exprType;

    PopulateFunctions();
    mAggrIdents = FdoExpressionEngineUtilDataReader::GetAggregateFunctions(m_AllFunctions, m_compIdents, exprType);

    if (mAggrIdents)
    {
        ProcessAggregateFunctions();
    }

	m_processingAggregate = false;

    FdoPtr<FdoPropertyValueCollection> retPropertyValues = FdoPropertyValueCollection::Create();
    for (FdoInt32 i=0; i<m_compIdents->GetCount(); i++)
    {
        FdoPtr<FdoIdentifier> selectedId = m_compIdents->GetItem(i);

        FdoPtr<FdoPropertyValue> pv = FdoPropertyValue::Create();
        pv->SetName(selectedId->GetName());
        FdoPtr<FdoLiteralValue> dv;

        selectedId->Process(this);

        switch (this->GetResultPropertyType())
        {
            case FdoPropertyType_DataProperty:
            {
                switch (this->GetResultDataType())
                {
                    case FdoDataType_Byte:
                    {
                        bool bIsNull;
                        FdoByte byte = GetByteResult(bIsNull);
                        if (bIsNull)
                            dv = FdoByteValue::Create();  // defaults to NULL
                        else
                            dv = FdoByteValue::Create(byte);
                    }
                    break;
                    case FdoDataType_Int32:
                    {
                        bool bIsNull;
                        FdoInt32 int32 = GetInt32Result(bIsNull);
                        if (bIsNull)
                            dv = FdoInt32Value::Create();  // defaults to NULL
                        else
                            dv = FdoInt32Value::Create(int32);
                    }
                    break;
                    case FdoDataType_Int64:   
                    {
                        bool bIsNull;
                        FdoInt64 int64 = GetInt64Result(bIsNull);
                        if (bIsNull)
                            dv = FdoInt64Value::Create();  // defaults to NULL
                        else
                            dv = FdoInt64Value::Create(int64);  
                    }
                    break;
                    case FdoDataType_Boolean: 
                    {
                        bool bIsNull;
                        bool boolean = GetBooleanResult(bIsNull);
                        if (bIsNull)
                            dv = FdoBooleanValue::Create();  // defaults to NULL
                        else
                            dv = FdoBooleanValue::Create(boolean);
                    }
                    break;
                    case FdoDataType_DateTime:
                    {
                        bool bIsNull;
                        FdoDateTime datetime = GetDateTimeResult(bIsNull);
                        if (bIsNull)
                            dv = FdoDateTimeValue::Create();  // defaults to NULL
                        else
                            dv = FdoDateTimeValue::Create(datetime);
                    }
                    break;
                    case FdoDataType_Double:  
                    {
                        bool bIsNull;
                        double dvalue = GetDoubleResult(bIsNull);
                        if (bIsNull)
                            dv = FdoDoubleValue::Create();  // defaults to NULL
                        else
                            dv = FdoDoubleValue::Create(dvalue);
                    }
                    break;
                    case FdoDataType_Decimal:  
                    {
                        bool bIsNull;
                        double dvalue = GetDecimalResult(bIsNull);
                        if (bIsNull)
                            dv = FdoDecimalValue::Create();  // defaults to NULL
                        else
                            dv = FdoDecimalValue::Create(dvalue);
                    }
                    break;
                    case FdoDataType_String:  
                    {
                        bool bIsNull;
                        FdoString* string = GetStringResult(bIsNull);
                        if (bIsNull)
                            dv = FdoStringValue::Create();  // defaults to NULL
                        else
                            dv = FdoStringValue::Create(string);
                    }
                    break;
                    default:
                        throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_71_DATA_TYPE_NOT_SUPPORTED), FdoCommonMiscUtil::FdoDataTypeToString(this->GetResultDataType())));
                }
            }
            break;

            case FdoPropertyType_GeometricProperty:
            {
                bool bIsNull;
                FdoPtr<FdoGeometryValue> geom = GetGeometricResult(bIsNull);
                if (bIsNull)
                    dv = FdoGeometryValue::Create();  // defaults to NULL
                else
                    dv = FDO_SAFE_ADDREF(geom.p);

            }
            break;

            default:
                throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_70_PROPERTY_TYPE_NOT_SUPPORTED), FdoCommonMiscUtil::FdoPropertyTypeToString(this->GetResultPropertyType())));
        }

        pv->SetValue(dv);
        retPropertyValues->Add(pv);
    }

    return FDO_SAFE_ADDREF(retPropertyValues.p);

}

bool FdoExpressionEngineImp::ProcessFilter(FdoFilter *filter)
{
    bool passedFilter;
    bool bIsNull;

    filter->Process(this);
    passedFilter = GetBooleanResult(bIsNull);

    return passedFilter;
}


void FdoExpressionEngineImp::PushIdentifierValue(FdoIReader* reader, FdoString* name, FdoDataType type )
{
	switch (type)
    {
		case FdoDataType_Boolean : 
            m_retvals.push_back (ObtainBooleanValue (false, m_reader->GetBoolean (name))); 
			break;
		case FdoDataType_Byte : 
			m_retvals.push_back(ObtainInt64Value(false, reader->GetByte(name))); 
			break;
		case FdoDataType_DateTime : 
			m_retvals.push_back(ObtainDateTimeValue(false, reader->GetDateTime(name))); 
			break;
		case FdoDataType_Decimal : 
			m_retvals.push_back(ObtainDoubleValue(false, reader->GetDouble(name))); 
			break;
		case FdoDataType_Double : 
			m_retvals.push_back(ObtainDoubleValue(false, reader->GetDouble(name))); 
			break;
		case FdoDataType_Int16 : 
			m_retvals.push_back(ObtainInt64Value(false, reader->GetInt16(name))); 
			break;
		case FdoDataType_Int32 : 
			m_retvals.push_back(ObtainInt64Value(false, reader->GetInt32(name))); 
			break;
		case FdoDataType_Int64 : 
			m_retvals.push_back(ObtainInt64Value(false, reader->GetInt64(name))); 
			break;
		case FdoDataType_Single : 
			m_retvals.push_back(ObtainDoubleValue(false, reader->GetSingle(name))); 
			break;
		case FdoDataType_String : 
			{
			wchar_t* val = NULL;
			wchar_t* str = (wchar_t*)reader->GetString(name);
			if( str )
			{
				val = new wchar_t[wcslen(str)+1];
				wcscpy(val,str);
			}
			m_retvals.push_back(ObtainStringValue(false, val)); 
			}
			break;
		default: 
            throw FdoException::Create(FdoException::NLSGetMessage (FDO_NLSID (FDO_71_DATA_TYPE_NOT_SUPPORTED), name));
			
    }
}

void FdoExpressionEngineImp::PushLiteralValue(FdoLiteralValue *literalValue )
{
    switch (literalValue->GetLiteralValueType())
    {
        case FdoLiteralValueType_Data:
        {
            FdoDataValue *data = static_cast<FdoDataValue *> (literalValue);

            bool bIsNull = data->IsNull();

            switch(data->GetDataType())
            {
            
                case FdoDataType_Boolean : 
                    m_retvals.push_back (ObtainBooleanValue (bIsNull, bIsNull ? false : (static_cast<FdoBooleanValue *>(data))->GetBoolean()));
                    break;
                case FdoDataType_Byte : 
                    m_retvals.push_back (ObtainByteValue (bIsNull, bIsNull ? 0 : (static_cast<FdoByteValue *>(data))->GetByte()));
                    break;
                case FdoDataType_DateTime : 
                    m_retvals.push_back (ObtainDateTimeValue (bIsNull, bIsNull ? FdoDateTime() : (static_cast<FdoDateTimeValue *>(data))->GetDateTime()));
                    break;
                case FdoDataType_Decimal : 
                    m_retvals.push_back (ObtainDecimalValue (bIsNull, bIsNull ? 0.0 : (static_cast<FdoDecimalValue *>(data))->GetDecimal()));
                    break;
                case FdoDataType_Double : 
                    m_retvals.push_back (ObtainDoubleValue (bIsNull, bIsNull ? 0.0 : (static_cast<FdoDoubleValue *>(data))->GetDouble()));
                    break;
                case FdoDataType_Int16 : 
                    m_retvals.push_back (ObtainInt16Value (bIsNull, bIsNull ? 0 : (static_cast<FdoInt16Value *>(data))->GetInt16()));
                    break;
                case FdoDataType_Int32 : 
                    m_retvals.push_back (ObtainInt32Value (bIsNull, bIsNull ? 0 : (static_cast<FdoInt32Value *>(data))->GetInt32()));
                    break;
                case FdoDataType_Int64 : 
                    m_retvals.push_back (ObtainInt64Value (bIsNull, bIsNull ? 0 : (static_cast<FdoInt64Value *>(data))->GetInt64()));
                    break;
                case FdoDataType_Single : 
                    m_retvals.push_back (ObtainSingleValue (bIsNull, bIsNull ? 0.0f : (static_cast<FdoSingleValue *>(data))->GetSingle()));
                    break;
                case FdoDataType_String : 
                    m_retvals.push_back (ObtainStringValue (bIsNull, bIsNull ? NULL : (static_cast<FdoStringValue *>(data))->GetString()));
                    break;
                case FdoDataType_BLOB : 
                    if (!bIsNull)
                    {
                        FdoPtr<FdoByteArray> blobBytes = (static_cast<FdoBLOBValue *>(data))->GetData();
                        m_retvals.push_back (ObtainBLOBValue (bIsNull, blobBytes));
                    }
                    else
                        m_retvals.push_back (ObtainBLOBValue (bIsNull, NULL));
                    break;
                case FdoDataType_CLOB : 
                    if (!bIsNull)
                    {
                        FdoPtr<FdoByteArray> clobBytes = (static_cast<FdoCLOBValue *>(data))->GetData();
                        m_retvals.push_back (ObtainCLOBValue (bIsNull, clobBytes));
                    }
                    else
                        m_retvals.push_back (ObtainCLOBValue (bIsNull, NULL));
                    break;
                default:
                    throw FdoException::Create (FdoException::NLSGetMessage(FDO_NLSID(FDO_71_DATA_TYPE_NOT_SUPPORTED), FdoCommonMiscUtil::FdoDataTypeToString(data->GetDataType())));
            }
            break;
        }

        case FdoLiteralValueType_Geometry:
        {
            FdoGeometryValue *geometry = static_cast<FdoGeometryValue *> (literalValue);
            bool bIsNull = geometry->IsNull();
            if (!bIsNull)
            {
                FdoPtr<FdoByteArray> geom = geometry->GetGeometry();
                m_retvals.push_back (ObtainGeometryValue (bIsNull, geom)); 
            }
            else
                m_retvals.push_back (ObtainGeometryValue (bIsNull, NULL)); 
            break;
        }

        default:
            throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_57_UNEXPECTEDERROR)));
    }
}


void FdoExpressionEngineImp::GetExpressionType(FdoFunctionDefinitionCollection *functionDefinitions, FdoClassDefinition* originalClassDef, FdoExpression *expr, FdoPropertyType &retPropType, FdoDataType &retDataType)
{
    FdoCommonMiscUtil::GetExpressionType(functionDefinitions, originalClassDef, expr, retPropType, retDataType);
}

void FdoExpressionEngineImp::GetExpressionType(FdoClassDefinition* originalClassDef, FdoExpression *expr, FdoPropertyType &retPropType, FdoDataType &retDataType)
{
    try
    {
        mutex.Enter();
        FdoPtr<FdoExpressionEngineFunctionCollection> functions = initFunction.GetAllFunctions();
        FdoPtr<FdoFunctionDefinitionCollection> functionDefinitions = FdoFunctionDefinitionCollection::Create();
        for (int i=0; i<functions->GetCount(); i++)
        {
            FdoPtr<FdoExpressionEngineIFunction> function = functions->GetItem(i);
            FdoPtr<FdoFunctionDefinition> functionDefinition = function->GetFunctionDefinition();
            functionDefinitions->Add(functionDefinition);
        }
        FdoCommonMiscUtil::GetExpressionType(functionDefinitions, originalClassDef, expr, retPropType, retDataType);
        functions = NULL;
        mutex.Leave();
    }
    catch (FdoException *)
    {
        mutex.Leave();
        throw;
    }
    catch (...)
    {
        mutex.Leave();
        throw;
    }
}

FdoFunctionDefinitionCollection *FdoExpressionEngineImp::DeepCopyFunctionDefinitions(FdoExpressionEngineFunctionCollection *functions)
{
    FdoPtr<FdoFunctionDefinitionCollection> newFunctions = FdoFunctionDefinitionCollection::Create();
    for (int i=0; i<functions->GetCount(); i++)
    {
        FdoPtr<FdoExpressionEngineIFunction> function = functions->GetItem(i);
        FdoPtr<FdoFunctionDefinition> functionDefinition = function->GetFunctionDefinition();
        FdoPtr<FdoFunctionDefinition> copyFunction = FdoExpressionEngineImp::DeepCopyFunctionDefinition(functionDefinition);
        newFunctions->Add(copyFunction);
    }
    return FDO_SAFE_ADDREF(newFunctions.p);
}

FdoFunctionDefinition *FdoExpressionEngineImp::DeepCopyFunctionDefinition(FdoFunctionDefinition *functionDefinition)
{

    FdoPtr<FdoReadOnlySignatureDefinitionCollection> signatures = functionDefinition->GetSignatures();
    FdoPtr<FdoSignatureDefinitionCollection> newSignatures = FdoSignatureDefinitionCollection::Create();
    for (int j=0; j<signatures->GetCount(); j++)
    {
        FdoPtr<FdoSignatureDefinition> signature = signatures->GetItem(j);
        FdoPtr<FdoReadOnlyArgumentDefinitionCollection> arguments = signature->GetArguments();
        FdoPtr<FdoArgumentDefinitionCollection> newArguments = FdoArgumentDefinitionCollection::Create();
        for (int k=0; k<arguments->GetCount(); k++)
        {
            FdoPtr<FdoArgumentDefinition> argument = arguments->GetItem(k);

            FdoPtr<FdoPropertyValueConstraintList> constraintList = argument->GetArgumentValueList();
            FdoPtr<FdoPropertyValueConstraintList> newConstraintList;
            if (constraintList)
            {
                FdoPtr<FdoDataValueCollection> dataValues = constraintList->GetConstraintList();
                newConstraintList = FdoPropertyValueConstraintList::Create();
                FdoPtr<FdoDataValueCollection> newDataValues = newConstraintList->GetConstraintList();

                for (int l=0; l<dataValues->GetCount(); l++)
                {
                    FdoPtr<FdoDataValue> dataValue = dataValues->GetItem(l);
                    FdoPtr<FdoDataValue> newDataValue = FdoCommonSchemaUtil::CopyDataValue(dataValue);
                    newDataValues->Add(newDataValue);
                }
            }
            FdoPtr<FdoArgumentDefinition> newArgument = FdoArgumentDefinition::Create(argument->GetName(), argument->GetDescription(), argument->GetPropertyType(), argument->GetDataType());
            if (newConstraintList)
                newArgument->SetArgumentValueList(newConstraintList);
            newArguments->Add(newArgument);
        }
        FdoPtr<FdoSignatureDefinition> newSignature = FdoSignatureDefinition::Create(signature->GetReturnPropertyType(), signature->GetReturnType(), newArguments);
        newSignatures->Add(newSignature);

    }
    FdoFunctionDefinition *newFunction = FdoFunctionDefinition::Create(functionDefinition->GetName(),functionDefinition->GetDescription(), functionDefinition->IsAggregate(), newSignatures, functionDefinition->GetFunctionCategoryType(),
        functionDefinition->SupportsVariableArgumentsList());

    return newFunction;
}


void FdoExpressionEngineImp::RegisterFunctions(FdoExpressionEngineFunctionCollection *userDefinedFunctions)
{
    try
    {
        mutex.Enter();
        initFunction.RegisterFunctions(userDefinedFunctions);
        mutex.Leave();
    }
    catch (FdoException *)
    {
        mutex.Leave();
        throw;
    }
    catch (...)
    {
        throw;
    }
}

// This method would only be usefully when calling from the Evaluate methods. The Evaluate method returns a FdoLiteralValue object to the user. This object should only be re-used
// by the Expression Egnine when the ref-count is 1(ie. the caller is not holding a reference to the object.)
void FdoExpressionEngineImp::PotentialRelinquishLiteralValue(FdoLiteralValue *value)
{
    if (value->GetLiteralValueType() == FdoLiteralValueType_Data)
    {
        switch (((FdoDataValue*)value)->GetDataType ())
        {
            case FdoDataType_Boolean:
                value->AddRef();
                mPotentialBooleanPool.push_back ((FdoBooleanValue*)value);
                break;
            case FdoDataType_Byte:
                value->AddRef();
                mPotentialBytePool.push_back ((FdoByteValue*)value);
                break;
            case FdoDataType_DateTime:
                value->AddRef();
                mPotentialDateTimePool.push_back ((FdoDateTimeValue*)value);
                break;
            case FdoDataType_Decimal:
                value->AddRef();
                mPotentialDecimalPool.push_back ((FdoDecimalValue*)value);
                break;
            case FdoDataType_Double:
                value->AddRef();
                mPotentialDoublePool.push_back ((FdoDoubleValue*)value);
                break;
            case FdoDataType_Int16:
                value->AddRef();
                mPotentialInt16Pool.push_back ((FdoInt16Value*)value);
                break;
            case FdoDataType_Int32:
                value->AddRef();
                mPotentialInt32Pool.push_back ((FdoInt32Value*)value);
                break;
            case FdoDataType_Int64:
                value->AddRef();
                mPotentialInt64Pool.push_back ((FdoInt64Value*)value);
                break;
            case FdoDataType_Single:
                value->AddRef();
                mPotentialSinglePool.push_back ((FdoSingleValue*)value);
                break;
            case FdoDataType_String:
                value->AddRef();
                mPotentialStringPool.push_back ((FdoStringValue*)value);
                break;
            case FdoDataType_BLOB:
                value->AddRef();
                mPotentialBLOBPool.push_back ((FdoBLOBValue*)value);
                break;
            case FdoDataType_CLOB:
                value->AddRef();
                mPotentialCLOBPool.push_back ((FdoCLOBValue*)value);
                break;
        }
    }
}

void FdoExpressionEngineImp::PopulateFunctions()
{
    try
    {
        if (m_FunctionsPopulated)
            return;

        m_FunctionsPopulated = true;
        if (m_UserDefinedFunctions)
        {
            for (int i=0; i<m_UserDefinedFunctions->GetCount(); i++)
            {
                FdoPtr<FdoExpressionEngineIFunction> userDefinedFunction = m_UserDefinedFunctions->GetItem(i);
                FdoPtr<FdoFunctionDefinition> function = userDefinedFunction->GetFunctionDefinition();
                m_AllFunctions->Add(function);
            }
        }

        mutex.Enter();
        FdoPtr<FdoExpressionEngineFunctionCollection> functions = initFunction.GetAllFunctions();
        for (int i=0; i<functions->GetCount(); i++)
        {
            FdoPtr<FdoExpressionEngineIFunction> functionDefinition = functions->GetItem(i);
            FdoPtr<FdoFunctionDefinition> function = functionDefinition->GetFunctionDefinition();

		    // Add it only if not user defined
            FdoPtr<FdoFunctionDefinition> func = m_AllFunctions->FindItem(function->GetName());
		    if ( func == NULL)
            {
                FdoPtr<FdoFunctionDefinition> copyFunc = DeepCopyFunctionDefinition(function);
			    m_AllFunctions->Add(copyFunc);
            }
        }
        functions = NULL;
        mutex.Leave();
    }
    catch(FdoException *)
    {
        mutex.Leave();
        throw;
    }
    catch(...)
    {
        mutex.Leave();
        throw;
    }
}