/*
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

#include "Pch.h"
#include "WmsTestCustomCommands.h"
#include "WMS/FdoWmsCommandType.h"
#include "WMS/IGetImageFormats.h"
#include "WMS/IGetFeatureClassStyles.h"
#include "WMS/IGetFeatureClassCRSNames.h"
#include "WMS/IGetFeatureInfo.h"
#include "WMS/IGetFeatureInfoFormats.h"

CPPUNIT_TEST_SUITE_REGISTRATION( WmsTestCustomCommands );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( WmsTestCustomCommands, "WmsTestCustomCommands");

WmsTestCustomCommands::WmsTestCustomCommands ()
{
}

WmsTestCustomCommands::~WmsTestCustomCommands ()
{
}

//http://www2.dmsolutions.ca/cgi-bin/mswms_gmap
void WmsTestCustomCommands::testGetImageFormats ()
{
    try
    {
        FdoPtr<FdoIConnection> connection = WmsTests::GetConnection ();

        FdoStringP sServer = FdoStringP::Format(L"FeatureServer=http://www2.dmsolutions.ca/cgi-bin/mswms_gmap");
        connection->SetConnectionString((FdoString*)sServer);
        FdoConnectionState state = connection->Open ();
        
        //test for formats command
        FdoPtr<FdoWmsIGetImagefromats> cmdGIF = static_cast<FdoWmsIGetImagefromats *> (connection->CreateCommand(FdoWmsCommandType_GetImageFormats));
        FdoStringsP formats = cmdGIF->Execute ();
        CPPUNIT_ASSERT(formats->GetCount() == 7);
#ifdef _DEBUG
		for (int i = 0; i < formats->GetCount(); ++i)
		{
			wprintf (L"image format: %ls\n", formats->GetString(i));
		}
#endif
        connection->Close ();
    }
    catch (FdoException* e)
    {
        fail(e);
    }
}

void WmsTestCustomCommands::testGetFeatureStyles ()
{
    try
    {
        FdoPtr<FdoIConnection> connection = WmsTests::GetConnection ();

        FdoStringP sServer = FdoStringP::Format(L"FeatureServer=http://www2.dmsolutions.ca/cgi-bin/mswms_gmap");
        connection->SetConnectionString((FdoString*)sServer);
        FdoConnectionState state = connection->Open ();
        
        //test for get styles command
        FdoPtr<FdoWmsIGetFeatureClassStyles> cmdGLS = static_cast<FdoWmsIGetFeatureClassStyles *> (connection->CreateCommand(FdoWmsCommandType_GetFeatureClassStyles));
        cmdGLS->SetFeatureClassName(L"drainage");
        FdoStringsP styles = cmdGLS->Execute();
        CPPUNIT_ASSERT(styles->GetCount() == 1);
#ifdef _DEBUG
		for (int i = 0; i < styles->GetCount(); ++i)
		{
			wprintf (L"Layer Style: %ls\n", styles->GetString(i));
		}
#endif
		
        connection->Close ();
    }
    catch (FdoException* e)
    {
        fail(e);
    }
}

void WmsTestCustomCommands::testSpacialFeatureClassName ()
{
    try
    {
        FdoPtr<FdoIConnection> connection = WmsTests::GetConnection ();

        FdoStringP sServer = FdoStringP::Format(L"FeatureServer=http://demo.cubewerx.com/demo/cubeserv/cubeserv.cgi?version=1.3.0");
        connection->SetConnectionString((FdoString*)sServer);
        FdoConnectionState state = connection->Open ();
        
        //test for get styles command
        FdoPtr<FdoWmsIGetFeatureClassStyles> cmdGLS = static_cast<FdoWmsIGetFeatureClassStyles *> (connection->CreateCommand(FdoWmsCommandType_GetFeatureClassStyles));
        cmdGLS->SetFeatureClassName(L"Foundation coastl_1m"); // layer name is Foundation.barrierl_1m
        FdoStringsP styles = cmdGLS->Execute();
        CPPUNIT_ASSERT(styles->GetCount() == 1);
#ifdef _DEBUG
		for (int i = 0; i < styles->GetCount(); ++i)
		{
			wprintf (L"Layer Style: %ls\n", styles->GetString(i));
		}
#endif
        //test for get CRS command
        FdoPtr<FdoWmsIGetFeatureClassCRSNames> cmdGLCRS = static_cast<FdoWmsIGetFeatureClassCRSNames *> (connection->CreateCommand(FdoWmsCommandType_GetFeatureClassCRSNames));
        cmdGLCRS->SetFeatureClassName(L"Foundation coastl_1m");//layer name is Foundation.barrierl_1m
        FdoStringsP crsNames = cmdGLCRS->Execute();
        CPPUNIT_ASSERT(crsNames->GetCount() == 1);
#ifdef _DEBUG
		for (int i = 0; i < crsNames->GetCount(); ++i)
		{
			wprintf (L"CRS Name: %ls\n", crsNames->GetString(i));
		}
#endif		

        connection->Close ();
    }
    catch (FdoException* e)
    {
        fail(e);
    }
}

void WmsTestCustomCommands::testGetFeatureCRSNames ()
{
    try
    {
        FdoPtr<FdoIConnection> connection = WmsTests::GetConnection ();

        FdoStringP sServer = FdoStringP::Format(L"FeatureServer=http://www2.dmsolutions.ca/cgi-bin/mswms_gmap");
        connection->SetConnectionString((FdoString*)sServer);
        FdoConnectionState state = connection->Open ();
        
        //test for get CRS command
        FdoPtr<FdoWmsIGetFeatureClassCRSNames> cmdGLCRS = static_cast<FdoWmsIGetFeatureClassCRSNames *> (connection->CreateCommand(FdoWmsCommandType_GetFeatureClassCRSNames));
        cmdGLCRS->SetFeatureClassName(L"drainage");
        FdoStringsP crsNames = cmdGLCRS->Execute();
        CPPUNIT_ASSERT(crsNames->GetCount() == 2);
#ifdef _DEBUG
		for (int i = 0; i < crsNames->GetCount(); ++i)
		{
			wprintf (L"CRS Name: %ls\n", crsNames->GetString(i));
		}
#endif		
        connection->Close ();
    }
    catch (FdoException* e)
    {
        fail(e);
    }
}

void WmsTestCustomCommands::testInvalidParamForStyle ()
{
    FdoPtr<FdoIConnection> connection;
    try
    {
		connection = WmsTests::GetConnection ();

        FdoStringP sServer = FdoStringP::Format(L"FeatureServer=http://www2.dmsolutions.ca/cgi-bin/mswms_gmap");
        connection->SetConnectionString((FdoString*)sServer);
        FdoConnectionState state = connection->Open ();
        
        //test for get styles command
        FdoPtr<FdoWmsIGetFeatureClassStyles> cmdGLS = static_cast<FdoWmsIGetFeatureClassStyles *> (connection->CreateCommand(FdoWmsCommandType_GetFeatureClassStyles));
        FdoStringsP styles = cmdGLS->Execute();
		
        connection->Close ();

		std::string errorMsg;
		errorMsg.append("Fail to catch none feature class input error!");
		CPPUNIT_FAIL(errorMsg);
    }
    catch (FdoException* e)
    {
        e->Release();
        connection->Close ();
    }
}

void WmsTestCustomCommands::testInvalidParamForCRS ()
{
    FdoPtr<FdoIConnection> connection;
    try
    {
		connection = WmsTests::GetConnection ();
        FdoStringP sServer = FdoStringP::Format(L"FeatureServer=http://www2.dmsolutions.ca/cgi-bin/mswms_gmap");
        connection->SetConnectionString((FdoString*)sServer);
        FdoConnectionState state = connection->Open ();
        
        //test for get CRS command
        FdoPtr<FdoWmsIGetFeatureClassCRSNames> cmdGLCRS = static_cast<FdoWmsIGetFeatureClassCRSNames *> (connection->CreateCommand(FdoWmsCommandType_GetFeatureClassCRSNames));
        FdoStringsP crsNames = cmdGLCRS->Execute();
		
        connection->Close ();

		std::string errorMsg;
		errorMsg.append("Fail to catch none feature class input error!");
		CPPUNIT_FAIL(errorMsg);
    }
    catch (FdoException* e)
    {
        e->Release();
		connection->Close ();
    }
}

void WmsTestCustomCommands::testGetFeatureInfo ()
{
    try
    {
        FdoPtr<FdoIConnection> connection = WmsTests::GetConnection ();

		FdoStringP sServer = FdoStringP::Format(L"FeatureServer=http://www.geosignal.org/cgi-bin/wmsmap?");
        connection->SetConnectionString((FdoString*)sServer);
        FdoConnectionState state = connection->Open ();

		// Parameters
		FdoSize resolutionX = 1420;
		FdoSize resolutionY = 998;
		FdoPtr<FdoFgfGeometryFactory> pGeometryFactory = FdoFgfGeometryFactory::GetInstance();
		FdoPtr<FdoIDirectPosition> pPosition = pGeometryFactory->CreatePositionXY(940, 179);
		FdoPtr<FdoIEnvelope> pBoundingBox = pGeometryFactory->CreateEnvelopeXY(4.663093,49.958461,4.909094,50.131303);
		FdoPtr<FdoIdentifier> pFeatureClassId = FdoIdentifier::Create(L"Communes");

		// Do first a map request to fill up the required cache
		FdoPtr<FdoISelect> cmd = static_cast<FdoISelect *> (connection->CreateCommand (FdoCommandType_Select));
		cmd->SetFeatureClassName (pFeatureClassId);
	    FdoPtr<FdoIFeatureReader> featReader = cmd->Execute ();

        //test GetFeatureInfo command
        FdoPtr<FdoWmsIGetFeatureInfo> cmdGetFeatureInfo = static_cast<FdoWmsIGetFeatureInfo*> (connection->CreateCommand(FdoWmsCommandType_GetFeatureInfo));
		cmdGetFeatureInfo->SetWidth(resolutionX);
		cmdGetFeatureInfo->SetHeight(resolutionY);
		cmdGetFeatureInfo->SetBoundingBox(pBoundingBox);
		cmdGetFeatureInfo->SetPosition(pPosition);
		cmdGetFeatureInfo->SetFeatureClassName(pFeatureClassId);
		cmdGetFeatureInfo->SetOutputFormat(L"text/plain");
		FdoPtr<FdoIoStream> stream = cmdGetFeatureInfo->Execute ();

		// Verify the stream
		const FdoSize bytesToRead = 1024;
		unsigned char buffer[bytesToRead];
		memset(buffer, '\0', bytesToRead);
		FdoSize bytesRead = stream->Read(buffer, bytesToRead);
        CPPUNIT_ASSERT(bytesRead >= 1);

        connection->Close ();
    }
    catch (FdoException* e)
    {
        fail(e);
    }
}

void WmsTestCustomCommands::testGetFeatureInfoFormats ()
{
    try
    {
        FdoPtr<FdoIConnection> connection = WmsTests::GetConnection ();

		FdoStringP sServer = FdoStringP::Format(L"FeatureServer=https://mapsengine.google.com/12700653833057327143-17379123781214007005-4/wms/?");
        connection->SetConnectionString((FdoString*)sServer);
        FdoConnectionState state = connection->Open ();
        
        //test for formats command
        FdoPtr<FdoWmsIGetFeatureInfoFormats> cmdGetFeatureInfoFormats = static_cast<FdoWmsIGetFeatureInfoFormats*> (connection->CreateCommand(FdoWmsCommandType_GetFeatureInfoFormats));
        FdoStringsP formats = cmdGetFeatureInfoFormats->Execute ();
        CPPUNIT_ASSERT(formats->GetCount() >= 1);
#ifdef _DEBUG
		wprintf (L"FeatureInfo format test\n");
		for (int i = 0; i < formats->GetCount(); ++i)
		{
			wprintf (L"FeatureInfo format: %ls\n", formats->GetString(i));
		}
#endif
        connection->Close ();
    }
    catch (FdoException* e)
    {
        fail(e);
    }
}
