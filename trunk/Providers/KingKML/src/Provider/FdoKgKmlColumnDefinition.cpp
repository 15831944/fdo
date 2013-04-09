/*
 * Copyright (C) 2010  SL-King d.o.o
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

#include "stdafx.h"
#include <FdoKgKmlColumnDefinition.h>

FdoKgKmlColumnDefinition* FdoKgKmlColumnDefinition::Create()
{
    FdoKgKmlColumnDefinition* pObject = new FdoKgKmlColumnDefinition();

    return pObject;
}

FdoKgKmlColumnDefinition::FdoKgKmlColumnDefinition(void)
{
}

FdoKgKmlColumnDefinition::~FdoKgKmlColumnDefinition(void)
{
}

void FdoKgKmlColumnDefinition::Dispose(void)
{
    delete this;
}

void FdoKgKmlColumnDefinition::InitFromXml(FdoXmlSaxContext* Context, FdoXmlAttributeCollection* Attrs)
{
    BaseType::InitFromXml(Context, Attrs);
}

FdoXmlSaxHandler* FdoKgKmlColumnDefinition::XmlStartElement(FdoXmlSaxContext* Context,FdoString* Uri, FdoString* Name, FdoString* QName, FdoXmlAttributeCollection* Attrs)
{
    return BaseType::XmlStartElement(Context, Uri, Name, QName, Attrs);
}

FdoBoolean FdoKgKmlColumnDefinition::XmlEndElement(FdoXmlSaxContext* Context,FdoString* Uri, FdoString* Name, FdoString* QName)
{
	return BaseType::XmlEndElement(Context, Uri, Name, QName);
}

void FdoKgKmlColumnDefinition::_writeXml(FdoXmlWriter* Writer, const FdoXmlFlags* Flags)
{
     Writer->WriteStartElement(FdoKgKmlXmlGlobals::g_KgOraColumnElement);

    BaseType::_writeXml(Writer, Flags);

    Writer->WriteEndElement();
}//end of FdoKgKmlColumnDefinition::_writeXml




