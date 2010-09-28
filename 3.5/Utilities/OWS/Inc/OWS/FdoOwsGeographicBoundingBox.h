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

#ifndef GEOGRAPHICBOUNDINGBOX_H
#define GEOGRAPHICBOUNDINGBOX_H

#ifdef _WIN32
#pragma once
#endif // _WIN32

class FdoOwsGeographicBoundingBox : public FdoXmlSaxHandler, public FdoIDisposable
{
	typedef FdoXmlSaxHandler BaseType;

public:
    FDOOWS_API static FdoOwsGeographicBoundingBox* Create(void);

public:
    FDOOWS_API FdoDouble GetWestBoundLongitude() const;
    FDOOWS_API void SetWestBoundLongitude(FdoDouble value);

    FDOOWS_API FdoDouble GetEastBoundLongitude() const;
    FDOOWS_API void SetEastBoundLongitude(FdoDouble value);

    FDOOWS_API FdoDouble GetSouthBoundLatitude() const;
    FDOOWS_API void SetSouthBoundLatitude(FdoDouble value);

    FDOOWS_API FdoDouble GetNorthBoundLatitude() const;
    FDOOWS_API void SetNorthBoundLatitude(FdoDouble value);

    FDOOWS_API void InitFromXml(FdoXmlSaxContext* context, FdoXmlAttributeCollection* attrs);

    FDOOWS_API virtual FdoXmlSaxHandler* XmlStartElement(
        FdoXmlSaxContext* context, 
        FdoString* uri, 
        FdoString* name, 
        FdoString* qname, 
        FdoXmlAttributeCollection* atts
    );

    FDOOWS_API FdoBoolean XmlEndElement(FdoXmlSaxContext* context, FdoString* uri, FdoString* name, FdoString* qname);

protected:
    FdoOwsGeographicBoundingBox(void);
    virtual ~FdoOwsGeographicBoundingBox(void);
    void Dispose();

private:
    FdoDouble mWestBoundLongitude;
    FdoDouble mEastBoundLongitude;
    FdoDouble mSouthBoundLatitude;
    FdoDouble mNorthBoundLatitude;
    FdoXmlCharDataHandlerP mXmlContentHandler;
};

typedef FdoPtr<FdoOwsGeographicBoundingBox> FdoOwsGeographicBoundingBoxP;

#endif // GEOGRAPHICBOUNDINGBOX_H

