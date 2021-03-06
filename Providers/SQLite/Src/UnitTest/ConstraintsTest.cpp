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

#include "stdafx.h"
#include "ConstraintsTest.h"
#include "UnitTestUtil.h"
#include "FdoCommonFile.h"

CPPUNIT_TEST_SUITE_REGISTRATION( ConstraintsTest );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ConstraintsTest, "ConstraintsTest");

#ifdef _WIN32
static const wchar_t* CONSTRAINTS_TEST_FILE = L"..\\..\\TestData\\ConstraintsTest.sqlite";
#else
#include <unistd.h>
static const wchar_t* CONSTRAINTS_TEST_FILE = L"../../TestData/ConstraintsTest.sqlite";
#endif

ConstraintsTest::ConstraintsTest()
{
}

ConstraintsTest::~ConstraintsTest()
{
}

void ConstraintsTest::CreateConnection( Context& context, FdoBoolean recreateDb )
{
    context.connection = UnitTestUtil::OpenConnection( CONSTRAINTS_TEST_FILE, recreateDb, true );
}

FdoBoolean ConstraintsTest::FdoValidatesData()
{
    return true;
}

FdoString* ConstraintsTest::GetDefaultSchemaName(void)
{
    return L"Default";
}

FdoDouble ConstraintsTest::GetDoubleRounding(FdoDataType)
{
    return (FdoDouble) 0.00000001;
}

FdoBoolean ConstraintsTest::CanRestrictCheckConstraint()
{
    return false;
}