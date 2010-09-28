/*
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
 */

#ifndef BIGPERFORMANCETESTS_H
#define BIGPERFORMANCETESTS_H

#ifdef _WIN32
#pragma once
#endif // _WIN32

#include "ShpTests.h"

class BigPerformanceTests :
    public ShpTests
{
    CPPUNIT_TEST_SUITE (BigPerformanceTests);
    CPPUNIT_TEST (select);
    CPPUNIT_TEST_SUITE_END ();

    static FdoPtr<FdoIConnection> mConnection;

public:
    BigPerformanceTests (void);
    virtual ~BigPerformanceTests (void);
	void setUp ();
	void tearDown ();

protected:
	void select ();
};

#endif // BIGPERFORMANCETESTS_H
