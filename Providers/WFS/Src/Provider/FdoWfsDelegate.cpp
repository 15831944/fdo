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
 *
 */

#include "stdafx.h"
#include "FdoWfsDelegate.h"
#include "FdoWfsServiceMetadata.h"
#include <OWS/FdoOwsGetCapabilities.h>
#include <OWS/FdoOwsResponse.h>
#include "FdoWfsDescribeFeatureType.h"
#include "FdoWfsSchemaMerger.h"
#include "FdoWfsGetFeature.h"
#include <Fdo/Xml/FeatureReader.h>
#include <Fdo/Xml/FeatureFlags.h>


FdoWfsDelegate::FdoWfsDelegate(FdoString* defaultUrl, FdoString* userName, FdoString* passwd, FdoString* proxy_location, FdoString* proxy_port, FdoString* proxy_user, FdoString* proxy_password, bool invert_axis):
     FdoOwsDelegate(defaultUrl, userName, passwd, proxy_location, proxy_port, proxy_user, proxy_password), invertAxis(invert_axis)
{
}

FdoWfsDelegate::~FdoWfsDelegate()
{
}

FdoWfsDelegate* FdoWfsDelegate::Create(FdoString* defaultUrl, FdoString* userName, FdoString* passwd,  FdoString* proxy_location, FdoString* proxy_port, FdoString* proxy_user, FdoString* proxy_password, bool invert_axis)
{
    return new FdoWfsDelegate(defaultUrl, userName, passwd,  proxy_location, proxy_port, proxy_user, proxy_password, invert_axis);
}

FdoWfsServiceMetadata* FdoWfsDelegate::GetCapabilities(FdoString* version)
{
    FdoPtr<FdoOwsGetCapabilities> request = FdoOwsGetCapabilities::Create(FdoWfsGlobals::WFS);
    if (version == NULL || wcslen(version) == 0)
        request->SetVersion(FdoWfsGlobals::WfsVersion);
    else
        request->SetVersion(version, false); // version already in URL.

    FdoPtr<FdoOwsResponse> response = Invoke(request);
    FdoPtr<FdoIoStream> stream = response->GetStream();
    FdoPtr<FdoWfsServiceMetadata> rv = FdoWfsServiceMetadata::Create();
    rv->ReadXml(stream);

    return FDO_SAFE_ADDREF(rv.p);
}

FdoFeatureSchemaCollection* FdoWfsDelegate::DescribeFeatureType(FdoStringCollection* typeNames, FdoString* version)
{
    FdoPtr<FdoWfsDescribeFeatureType> request = FdoWfsDescribeFeatureType::Create(typeNames,version);

    FdoPtr<FdoOwsResponse> response = Invoke(request);
    FdoPtr<FdoIoStream> stream = response->GetStream();

    // now we should merge stream and all its referenced schema
	FdoWfsSchemaMerger merger(version);
    stream = merger.MergeSchema(stream, GetUrl(), L"");

	// WFS 2.0.0 uses xmlns:gml="http://www.opengis.net/gml/3.2", causing style sheet transform fail to recognize geometry types.
	// Solution is to replace by xmlns:gml="http://www.opengis.net/gml".
	if (wcscmp(version, FdoWfsGlobals::WfsVersion200) == 0)
	{
		FdoStringP text = L"";		
		FdoByte buffer[4097];
		const int readSize = sizeof(buffer) / sizeof(FdoByte) - 1;
		do
		{
			FdoSize cntRead = stream->Read(buffer, readSize);
			if (cntRead == 0)
				break;		
			buffer[cntRead] = '\0';
			text += FdoStringP((const char*)buffer);			
		} while (true);
		text = text.Replace(L"/gml/3.2", L"/gml");
		FdoPtr<FdoIoMemoryStream> tempStream = FdoIoMemoryStream::Create();
		tempStream->Write((FdoByte*)(const char*)text, strlen((const char*)text));
		stream->Reset();
		tempStream->Reset();
		stream = tempStream;
	}

    FdoPtr<FdoFeatureSchemaCollection> schemas = FdoFeatureSchemaCollection::Create(NULL);
    FdoPtr<FdoXmlFlags> flags = FdoXmlFlags::Create(FdoWfsGlobals::fdo_customer, FdoXmlFlags::ErrorLevel_VeryLow);
    flags->SetSchemaNameAsPrefix(true);
    flags->SetElementDefaultNullability(true);
    schemas->ReadXml(stream, flags);

    return FDO_SAFE_ADDREF(schemas.p);
}

FdoIFeatureReader* FdoWfsDelegate::GetFeature(FdoFeatureSchemaCollection* schemas, 
                                              FdoPhysicalSchemaMappingCollection* schemaMappings, 
                                              FdoString* targetNamespace,
                                              FdoString* targetNamespaceName,
                                              FdoString* srsName,
                                              FdoStringCollection* propertiesToSelect,
                                              FdoString* from,
                                              FdoFilter* where,
                                              FdoString* schemaName,
                                              FdoString* version,
                                              FdoWfsCancelExecutationHander handler,
                                              void* handleData)
{
    FdoPtr<FdoWfsGetFeature> request = FdoWfsGetFeature::Create(targetNamespace, 
                                                                targetNamespaceName,
                                                                srsName, 
                                                                propertiesToSelect, 
                                                                from, 
                                                                where,
                                                                schemaName,
                                                                version,
                                                                invertAxis);
    FdoPtr<FdoOwsResponse> response;
    FdoException* exc1 = NULL;
    try
    {
        response = Invoke(request);
    }
    catch(FdoException* exc) // some servers request to have the class or namespace name in the front of properties, so we will try to place them
    {
        exc1 = exc;
        request->EncodeWithNamespaceName(true);
        try
        {
            response = Invoke(request);
        }
        catch (FdoException* exc2)
        {
            exc2->Release();
            request->EncodeWithNamespaceName(false);
            request->EncodeWithClassName(true);
            try
            {
                response = Invoke(request);
            }
            catch (FdoException* exc3) // rare cases
            {
                exc3->Release();
                request->SetSchemaName(L""); // remove schema name
                request->EncodeWithClassName(false);
                try
                {
                    response = Invoke(request);
                }
                catch (FdoException* exc4)
                {
                    exc4->Release();
                    request->EncodeWithClassName(true);
                    try
                    {
                        response = Invoke(request);
                    }
                    catch (FdoException* exc5)
                    {
                        exc5->Release();
                        throw exc1;
                    }
                }
            }
        }
        exc1->Release();
    }
    FdoPtr<FdoIoStream> stream = response->GetStream();
	FdoPtr<FdoIoStream> tempStream = preProcessStream(stream, handler, handleData);
    FdoPtr<FdoXmlReader> xmlReader = FdoXmlReader::Create(tempStream);
	FdoPtr<FdoXmlFeatureFlags> flags = FdoXmlFeatureFlags::Create(FdoWfsGlobals::fdo_customer, FdoXmlFlags::ErrorLevel_VeryLow);
	flags->SetSchemaMappings(schemaMappings);
    flags->SetInvertAxis(this->invertAxis);
    FdoPtr<FdoXmlFeatureReader> xmlFeatureReader = FdoXmlFeatureReader::Create(xmlReader, flags);
    xmlFeatureReader->SetFeatureSchemas(schemas);
	FdoPtr<FdoWfsFeatureReader> featureReader = new FdoWfsFeatureReader;	
	featureReader->SetXmlFeatureReader(FDO_SAFE_ADDREF(xmlFeatureReader.p));

    return FDO_SAFE_ADDREF(featureReader.p);
}

FdoIoStream* FdoWfsDelegate::preProcessStream(FdoIoStream *stream, FdoWfsCancelExecutationHander handler, void* handleData)
{
	FdoIoMemoryStreamP tempStream = FdoIoMemoryStream::Create();
	
	FdoByte buffer[4096];
	bool bCanceled = false;
	const int readSize = sizeof(buffer)/sizeof(FdoByte);
	do
	{
		FdoSize cntRead = stream->Read(buffer,readSize);
		if (cntRead == 0)
			break;
		for (FdoSize i =0;i<cntRead;i++)
		{
			int num = (int)buffer[i];
			if ((num >= 1 && num <= 7) || 
				(num >= 11 && num <=19))
				buffer[i] = ' ';
		}
		tempStream->Write(buffer,cntRead);
		bCanceled = handler(handleData);
	}
	while (!bCanceled);

	if (bCanceled)
		tempStream->SetLength(0);

	tempStream->Reset();
	return FDO_SAFE_ADDREF(tempStream.p);

}


