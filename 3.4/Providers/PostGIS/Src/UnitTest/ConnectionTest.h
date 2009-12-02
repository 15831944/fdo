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
#ifndef FDOPOSTGIS_CONNECTIONTEST_H_INCLUDED
#define FDOPOSTGIS_CONNECTIONTEST_H_INCLUDED

#include "BaseTestCase.h"

class ConnectionTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(ConnectionTest);
    CPPUNIT_TEST(testGetConnectionManager);
    CPPUNIT_TEST(testCreateConnection);
    CPPUNIT_TEST(testConnect);
    CPPUNIT_TEST(testEmptyConnectionString);
    CPPUNIT_TEST_SUITE_END();

public:

    ConnectionTest();
    virtual ~ConnectionTest();

	virtual void setUp();
	virtual void tearDown();

    //
    // Tests
    //

    void testGetConnectionManager();
    void testCreateConnection();
    void testConnect();
    void testEmptyConnectionString();

private:
    
    FdoPtr<FdoIConnection> mConn;
};

#endif // FDOPOSTGIS_CONNECTIONTEST_H_INCLUDED
