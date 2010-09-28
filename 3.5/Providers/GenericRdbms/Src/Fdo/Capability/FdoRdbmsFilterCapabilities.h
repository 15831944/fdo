#ifndef _FDORDBMSFILTERCAPABILITIES_H_
#define _FDORDBMSFILTERCAPABILITIES_H_
#ifdef _WIN32
#pragma once
#endif
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


#include "Fdo.h"

class FdoRdbmsFilterCapabilities :  public FdoIFilterCapabilities
{
public:
    FdoRdbmsFilterCapabilities(void);

    virtual FdoConditionType* GetConditionTypes(int& length);

    virtual FdoSpatialOperations* GetSpatialOperations(int& length);

    virtual FdoDistanceOperations* GetDistanceOperations(int& length);

    virtual bool SupportsGeodesicDistance();

    virtual bool SupportsNonLiteralGeometricOperations();

protected:
    ~FdoRdbmsFilterCapabilities(void);
    virtual void Dispose();
};
#endif
