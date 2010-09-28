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
#ifndef DATAVALUE_H
#define DATAVALUE_H

#define MAX_STRING_LENGTH 256

#define DataValueType		FdoDataType
#define Dvt_Boolean			FdoDataType_Boolean
#define Dvt_Int64			FdoDataType_Int64
#define Dvt_Double			FdoDataType_Double
#define Dvt_String			FdoDataType_String
#define Dvt_DateTime		FdoDataType_DateTime
#define Dvt_Null			(-1) // A null value indicator


//forward declare
class DataValuePool;

//Encapsulates a result of a query execution
//Knows how to perform operations on DataValues
class DataValue
{
public:

    //unfortunately we need to pass in the DataValue memory pool
    //for efficient allocation of return values
    //This can be cleaned up by overloaded new/delete on DataValue 
    virtual DataValue* Clone(DataValuePool* pPool) = 0;
    virtual DataValue* Add(DataValue& v2, DataValuePool* pPool) = 0;
    virtual DataValue* Subtract(DataValue& v2, DataValuePool* pPool) = 0;
    virtual DataValue* Multiply(DataValue& v2, DataValuePool* pPool)= 0;
    virtual DataValue* Divide(DataValue& v2, DataValuePool* pPool) = 0;
    virtual DataValue* Negate(DataValuePool* pPool) = 0;
    
    virtual bool IsEqualTo(DataValue&v2) = 0;
    virtual bool IsNotEqualTo(DataValue&v2) = 0;
    virtual bool IsGreaterThan(DataValue&v2) = 0;
    virtual bool IsGreaterThanOrEqualTo(DataValue&v2) = 0;
    virtual bool IsLessThan(DataValue&v2) = 0;
    virtual bool IsLessThanOrEqualTo(DataValue&v2) = 0;

    virtual double GetAsDouble() = 0;
    virtual FdoInt64 GetAsInt64() = 0;
    virtual wchar_t* GetAsString() = 0;
    virtual bool GetAsBoolean() = 0;
    virtual FdoDateTime GetAsDateTime() = 0;

    virtual DataValueType GetType() = 0;

private:

};


#endif
