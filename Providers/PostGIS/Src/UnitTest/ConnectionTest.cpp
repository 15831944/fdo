//
// Copyright (C) 2007 Refractions Research, Inc. 
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
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
#include "Pch.h"
#include "ConnectionTest.h"
#include "TestConfig.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ConnectionTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ConnectionTest, "ConnectionTest");

using namespace fdo::postgis::test;

ConnectionTest::ConnectionTest() : mConnection(NULL)
{
}

ConnectionTest::~ConnectionTest()
{
}

void ConnectionTest::setUp()
{
}

void ConnectionTest::tearDown()
{
    mConnection = NULL;
    FdoPtr<IConnectionManager> mgr = FdoFeatureAccessManager::GetConnectionManager();
    mgr->FreeLibrary(gTestConfig.getProviderFullName());

}

void ConnectionTest::testGetConnectionManager()
{
    FdoPtr<IConnectionManager> mgr = FdoFeatureAccessManager::GetConnectionManager();
    
    CPPUNIT_ASSERT_MESSAGE("Connection manager is NULL", NULL != mgr);
}

void ConnectionTest::testCreateConnection()
{
    FdoPtr<IConnectionManager> mgr = FdoFeatureAccessManager::GetConnectionManager();
    mConnection = mgr->CreateConnection(gTestConfig.getProviderFullName());

    CPPUNIT_ASSERT_MESSAGE("Connection is NULL", NULL != mConnection);
}
