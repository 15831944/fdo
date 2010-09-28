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

#include "UnitTest.h"
#include <cppunit/TestResultCollector.h>
#include "RfpTestLogOutputter.h"
#include <fstream>


RfpTestLogOutputter::RfpTestLogOutputter(
			CppUnit::TestResultCollector *result,
			std::ofstream &stream,std::string encoding )
			: CppUnit::XmlOutputter(result, stream, encoding)
{
}

RfpTestLogOutputter::~RfpTestLogOutputter(void)
{
}

CppUnit::XmlOutputter::Node * RfpTestLogOutputter::makeRootNode()
{
	Node *rootNode = new Node("RfpTestRun");
	rootNode->addNode( makeUnitTestNode() ); 

	return rootNode;
}

CppUnit::XmlOutputter::Node * RfpTestLogOutputter::makeUnitTestNode()
{
	std::string nodeText = std::string("UnitTest");
	Node* node = new Node(nodeText);
	node->addAttribute("Time", _getCurrentTime(false));
	
	FailedTests failedTests;
	fillFailedTestsMap( failedTests );

	addFailedTests( failedTests, node );
	addSucessfulTests( failedTests, node );
	addStatistics( node );

	return node;	
}


#include <algorithm>
bool invalidChar(char c)
{
	return (c == '/' || c == ':');
}

// helper function to get current time.
std::string _getCurrentTime(bool bFormat)
{
	char dateBuff[128]; 	_strdate(dateBuff);
	char timeBuff[128];		_strtime(timeBuff);
	std::string ret;

	if (bFormat)
	{
		ret = std::string(dateBuff) + "_" + timeBuff;	
		replace_if(ret.begin(), ret.end(), invalidChar, '-');
	}
	else
		ret = std::string(dateBuff) + " " + timeBuff;

	return ret;
}