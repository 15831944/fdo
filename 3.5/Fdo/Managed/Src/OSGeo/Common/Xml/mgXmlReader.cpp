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
#include "Common\mgException.h"
#include "Common\mgDictionary.h"
#include "Common\mgObjectFactory.h"
#include "Common\Io\mgIoStream.h"
#include "Common\Io\mgIoTextReader.h"
#include "Common\Xml\mgXmlReader.h"
#include "Common\Xml\mgXmlSaxHandler.h"
#include "Common\Xml\mgXmlSaxContext.h"

using namespace NAMESPACE_OSGEO_COMMON_IO;

NAMESPACE_OSGEO_COMMON_XML::XmlReader::XmlReader( String^ fileName) : Disposable(IntPtr::Zero, false)
{
	EXCEPTION_HANDLER(Attach(IntPtr(FdoXmlReader::Create(StringToUni(fileName))), true))
}

NAMESPACE_OSGEO_COMMON_XML::XmlReader::XmlReader(IoStream^ stream) : Disposable(IntPtr::Zero, false)
{
	EXCEPTION_HANDLER(Attach(IntPtr(FdoXmlReader::Create(stream->GetImpObj())), true))
}

NAMESPACE_OSGEO_COMMON_XML::XmlReader::XmlReader(IoTextReader^ reader ) : Disposable(IntPtr::Zero, false)
{
	EXCEPTION_HANDLER(Attach(IntPtr(FdoXmlReader::Create(reader->GetImpObj())), true))
}

NAMESPACE_OSGEO_COMMON_XML::XmlReader::XmlReader(IntPtr unmanaged, Boolean autoDelete) : Disposable(unmanaged, autoDelete)
{
	
}

FdoXmlReader* NAMESPACE_OSGEO_COMMON_XML::XmlReader::GetImpObj()
{
	return static_cast<FdoXmlReader*>(UnmanagedObject.ToPointer());
}

IntPtr NAMESPACE_OSGEO_COMMON_XML::XmlReader::GetDisposableObject()
{
    return IntPtr(static_cast<FdoIDisposable*>(GetImpObj()));
}

IoTextReader^ NAMESPACE_OSGEO_COMMON_XML::XmlReader::TextReader::get()
{
	IoTextReader^ reader;
	EXCEPTION_HANDLER(reader = ObjectFactory::CreateIoTextReader(IntPtr(GetImpObj()->GetTextReader()), true))
	return reader;
}

IoStream^ NAMESPACE_OSGEO_COMMON_XML::XmlReader::Stream::get()
{
	IoStream^ mgstream;
	EXCEPTION_HANDLER(mgstream= ObjectFactory::CreateIoStream(IntPtr(GetImpObj()->GetStream()), true))
	return mgstream;
}

System::Boolean NAMESPACE_OSGEO_COMMON_XML::XmlReader::Parse()
{
	FdoBoolean rv;
	EXCEPTION_HANDLER(rv = !!GetImpObj()->Parse(nullptr, nullptr, false))
	return rv;
}

System::Boolean NAMESPACE_OSGEO_COMMON_XML::XmlReader::Parse(NAMESPACE_OSGEO_COMMON_XML::IXmlSaxHandler^ saxHandler)
{
	System::Boolean rv;
    if (saxHandler != nullptr)
    {
        System::IDisposable^ baseDisp = static_cast<System::IDisposable^>(saxHandler);
        NAMESPACE_OSGEO_RUNTIME::Disposable^ fdoDisp = static_cast<NAMESPACE_OSGEO_RUNTIME::Disposable^>(baseDisp);
        FdoXmlSaxHandler* handler = static_cast<FdoXmlSaxHandler*>(fdoDisp->UnmanagedObject.ToPointer());
        EXCEPTION_HANDLER(rv = !!GetImpObj()->Parse(handler, nullptr, false));
    }
    else
    {
        EXCEPTION_HANDLER(rv = !!GetImpObj()->Parse(nullptr, nullptr, false));
    }
	return rv;
}

System::Boolean NAMESPACE_OSGEO_COMMON_XML::XmlReader::Parse(NAMESPACE_OSGEO_COMMON_XML::IXmlSaxHandler^ saxHandler, NAMESPACE_OSGEO_COMMON_XML::XmlSaxContext^ saxContext)
{
	FdoBoolean rv;
    if (saxHandler != nullptr)
    {
        System::IDisposable^ baseDisp = static_cast<System::IDisposable^>(saxHandler);
        NAMESPACE_OSGEO_RUNTIME::Disposable^ fdoDisp = static_cast<NAMESPACE_OSGEO_RUNTIME::Disposable^>(baseDisp);
        FdoXmlSaxHandler* handler = static_cast<FdoXmlSaxHandler*>(fdoDisp->UnmanagedObject.ToPointer());
        EXCEPTION_HANDLER(rv = !!GetImpObj()->Parse(handler, saxContext->GetImpObj(), false));
    }
    else
    {
        EXCEPTION_HANDLER(rv = !!GetImpObj()->Parse(nullptr, saxContext->GetImpObj(), false));
    }
	return rv;
}

System::Boolean NAMESPACE_OSGEO_COMMON_XML::XmlReader::Parse(NAMESPACE_OSGEO_COMMON_XML::IXmlSaxHandler^ saxHandler, NAMESPACE_OSGEO_COMMON_XML::XmlSaxContext^ saxContext, System::Boolean incremental)
{
	FdoBoolean rv;
    if (saxHandler != nullptr)
    {
        System::IDisposable^ baseDisp = static_cast<System::IDisposable^>(saxHandler);
        NAMESPACE_OSGEO_RUNTIME::Disposable^ fdoDisp = static_cast<NAMESPACE_OSGEO_RUNTIME::Disposable^>(baseDisp);
        FdoXmlSaxHandler* handler = static_cast<FdoXmlSaxHandler*>(fdoDisp->UnmanagedObject.ToPointer());
        EXCEPTION_HANDLER(rv = !!GetImpObj()->Parse(handler, saxContext->GetImpObj(), incremental));
    }
    else
    {
        EXCEPTION_HANDLER(rv = !!GetImpObj()->Parse(nullptr, saxContext->GetImpObj(), incremental));
    }
	return rv;
}


Boolean NAMESPACE_OSGEO_COMMON_XML::XmlReader::EOD::get()
{
	FdoBoolean rv;
	EXCEPTION_HANDLER(rv = !!GetImpObj()->GetEOD())
	return rv;
}

System::String^ NAMESPACE_OSGEO_COMMON_XML::XmlReader::DecodeName(System::String^ name)
{
    FdoStringP result;
    EXCEPTION_HANDLER(result = GetImpObj()->DecodeName(FdoStringP(StringToUni(name))))
    return CHECK_STRING((FdoString*) result);
}

DictionaryElementCollection^ NAMESPACE_OSGEO_COMMON_XML::XmlReader::Namespaces::get()
{
	DictionaryElementCollection^ mgD;
	EXCEPTION_HANDLER(mgD = ObjectFactory::CreateDictionaryElementCollection(IntPtr(GetImpObj()->GetNamespaces()), true))
	return mgD;
}

String^ NAMESPACE_OSGEO_COMMON_XML::XmlReader::PrefixToUri( String^ prefix )
{
	FdoString* uurl;
	EXCEPTION_HANDLER(uurl= GetImpObj()->PrefixToUri(StringToUni(prefix)))
	return gcnew String(uurl);
}
