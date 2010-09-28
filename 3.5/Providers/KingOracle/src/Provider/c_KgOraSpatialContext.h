/*
* Copyright (C) 2006-2009  SL-King d.o.o
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


#ifndef _c_KgOraSpatialContext_h
#define _c_KgOraSpatialContext_h

#include "c_KgOraSridDesc.h"

class c_KgOraSpatialContext : public FdoDisposable
{
private:
    FdoStringP m_Name;
    FdoStringP m_Description;
    FdoStringP m_CoordSysName;
    FdoStringP m_Wkt;
    FdoSpatialContextExtentType m_ExtentType;
    FdoPtr<FdoEnvelopeImpl> m_ExtentEnvelope;
    
    
	  bool m_IsExtentUpdated;
    double m_XYTolerance;
    double m_ZTolerance;
    
    c_KgOraSridDesc m_OraSridDesc;

public:
    c_KgOraSpatialContext (void);
    virtual ~c_KgOraSpatialContext ();

    virtual FdoString* GetName ();
    virtual void SetName (FdoString* name);
    virtual FdoString* GetDescription ();
    virtual void SetDescription (FdoString* desc);
    virtual FdoString* GetCoordinateSystem ();
    virtual void SetCoordSysName (FdoString* csName);
    virtual FdoString* GetCoordinateSystemWkt ();
    virtual void SetCoordinateSystemWkt (FdoString* wkt);
    virtual FdoSpatialContextExtentType GetExtentType ();
    virtual void SetExtentType (FdoSpatialContextExtentType type);
    
    virtual FdoByteArray* GetExtent ();
    virtual FdoIEnvelope* GetExtentEnvelope();
    
    virtual void ExpandExtent (FdoIEnvelope* Expand);
    
    virtual double GetXYTolerance ();
    virtual void SetXYTolerance (double tol);
    virtual double GetZTolerance ();
    virtual void SetZTolerance (double tol);

    virtual FdoBoolean CanSetName() { return true; }
	  virtual bool GetIsExtentUpdated();
	  void SetIsExtentUpdated( bool value );	   
	  
	  virtual const c_KgOraSridDesc& GetOraSridDesc();
	  virtual void SetOraSridDesc(const c_KgOraSridDesc& SRID);
};



class c_KgOraSpatialContextCollection : public FdoNamedCollection<c_KgOraSpatialContext, FdoException>
{
public:
    c_KgOraSpatialContextCollection(void) {}

    virtual ~c_KgOraSpatialContextCollection() {}

    virtual void Dispose() 
    { 
        delete this; 
    }
};



#endif // _c_KGORASPATIALCONTEXT_H