// strToFdoType.h
// converts strings containing Fdo enumerated types values to the Fdo type


#ifndef _STRTOFDOTYPE_H_
#define _STRTOFDOTYPE_H_
//
// Copyright (C) 2008  Autodesk, Inc.
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
//

class StrToFdoType {
public:
	FdoClassType convStrToFdoClassType(const char * classType);
	FdoPropertyType convStrToFdoPropertyType(const char * propertyType);
	FdoDataType convStrToFdoDataType(const char * dataType);
	FdoGeometricType convStrToFdoGeometricType(const char * geometricType);
	FdoGeometryType convStrToFdoGeometryType(const char * geometryType);
private:
	char errString[256];

};

#endif
