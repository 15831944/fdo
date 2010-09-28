/*
* Copyright (C) 2007  Haris Kurtagic
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
#include "StdAfx.h"

#include "c_FdoMss_API.h"


c_KgMssCreateSpatialContext::c_KgMssCreateSpatialContext(c_KgMssConnection *Conn)
  : c_KgMssFdoCommand(Conn)
{
    m_Name = D_SPATIALCONTEXT_DEFAULT_NAME;    
    m_Description = "";    
    
    m_CoordinateSystem = D_SPATIALCONTEXT_CSYS_DEFAULT_NAME;
    m_CoordinateSystemWkt = D_SPATIALCONTEXT_CSYS_DEFAULT_WKT;
    
    m_SpatialContextExtentType = FdoSpatialContextExtentType_Dynamic;
    
    //FdoPtr<FdoByteArray> m_Extent;
    
    m_XYTolerance = D_SPATIALCONTEXT_DEFAULT_XY_TOLERANCE;
    m_ZTolerance = D_SPATIALCONTEXT_DEFAULT_Z_TOLERANCE;
    m_UpdateExisting = false;
}

c_KgMssCreateSpatialContext::~c_KgMssCreateSpatialContext(void)
{
}

 /// \brief
/// Gets the name of the context to create as a string.
/// 
/// \return
/// Returns the name of the spatial context
/// 
FdoString* c_KgMssCreateSpatialContext::GetName()
{
  return m_Name;
}

/// \brief
/// Sets the name of the context to create as a string.
/// 
/// \param value 
/// Input the name of the spatial context
/// 
/// \return
/// Returns nothing
/// 
void c_KgMssCreateSpatialContext::SetName(FdoString* Name)
{
  m_Name = Name;
}

/// \brief
/// Gets the description of the context to create as a string.
/// 
/// \return
/// Returns the description of the spatial context
/// 
FdoString* c_KgMssCreateSpatialContext::GetDescription()
{
  return m_Description;
}

/// \brief
/// Sets the description of the context to create as a string.
/// 
/// \param value 
/// Input the description of the spatial context
/// 
/// \return
/// Returns nothing
/// 
void c_KgMssCreateSpatialContext::SetDescription(FdoString* Desc)
{
  m_Description = Desc;
}

/// \brief
/// Gets the coordinate system name as a string.
/// 
/// \return
/// Returns the coordinate system
/// 
FdoString* c_KgMssCreateSpatialContext::GetCoordinateSystem()
{
  return m_CoordinateSystem;
}

/// \brief
/// Sets the coordinate system name as a string.
/// 
/// \param value 
/// Input the coordinate system
/// 
/// \return
/// Returns nothing
/// 
void c_KgMssCreateSpatialContext::SetCoordinateSystem(FdoString* CoordinateSystem)
{ 
  m_CoordinateSystem = CoordinateSystem;
}

/// \brief
/// Gets the coordinate system definition in OGC SRS WKT format for the 
/// spatial context currently being created. If the result is an empty string, 
/// the coordinate system WKT description has not been set.
/// 
/// \return
/// Returns the coordinate system description in WKT of of the spatial context.
/// 
FdoString* c_KgMssCreateSpatialContext::GetCoordinateSystemWkt()
{
  return m_CoordinateSystemWkt;
}

/// \brief
/// Sets the coordinate system definition in OGC SRS WKT 
/// format for the spatial context currently being created.
/// 
/// \param value 
/// Input the coordinate system WKT
/// 
/// \return
/// Returns nothing
/// 
void c_KgMssCreateSpatialContext::SetCoordinateSystemWkt(FdoString* CoordinateSystemWkt)
{
  m_CoordinateSystemWkt = CoordinateSystemWkt;
}

/// \brief
/// Gets the desired extent type of the context to create, either static or dynamic.
/// 
/// \return
/// Returns the extent type
/// 
FdoSpatialContextExtentType c_KgMssCreateSpatialContext::GetExtentType()
{
  return m_SpatialContextExtentType;
}

/// \brief
/// Sets the desired extent type of the context to create, either static or dynamic.
/// 
/// \param value 
/// Input the extent type
/// 
/// \return
/// Returns nothing
/// 
void c_KgMssCreateSpatialContext::SetExtentType(FdoSpatialContextExtentType SpatialContextExtentType)
{
  m_SpatialContextExtentType = SpatialContextExtentType;
}

/// \brief
/// Gets the extent of the context to create as a byte array in AGF format.
/// If the extent type is dynamic, any value specified via this function is ignored.
/// 
/// \return
/// Returns the extent of the spatial context
/// 
FdoByteArray* c_KgMssCreateSpatialContext::GetExtent()
{
  return FDO_SAFE_ADDREF(m_Extent.p);
}

/// \brief
/// Sets the extent of the context to create as a byte array in AGF format.
/// If the extent type is dynamic this is optional and any value specified is ignored.
/// 
/// \param value 
/// Input the extent of the spatial context
/// 
/// \return
/// Returns nothing
/// 
void c_KgMssCreateSpatialContext::SetExtent(FdoByteArray* Extent)
{
  m_Extent = Extent;
  FDO_SAFE_ADDREF(m_Extent.p);
}

/// \brief
/// Gets the tolerance value to use for X/Y ordinates. Tolerances are used
/// in some geometric tests, mostly for equality between coordinates.  This
/// occurs frequently in spatial queries, especially with "on boundary" or "just
/// touching" conditions.  It is also an objective amount that can be used
/// when formulating step distances for parametric curve interpolation.
/// 
/// \return
/// Returns the tolerance
/// 
const double c_KgMssCreateSpatialContext::GetXYTolerance()
{
  return m_XYTolerance;
}

/// \brief
/// Sets the tolerance value to use for X/Y ordinates. Tolerances are used
/// in some geometric tests, mostly for equality between coordinates.  This
/// occurs frequently in spatial queries, especially with "on boundary" or "just
/// touching" conditions.  It is also an objective amount that can be used
/// when formulating step distances for parametric curve interpolation.
/// 
/// \param value 
/// Input the tolerance
/// 
/// \return
/// Returns nothing
/// 
void c_KgMssCreateSpatialContext::SetXYTolerance(const double XYTolerance)
{
  m_XYTolerance = XYTolerance;
}

/// \brief
/// Gets the tolerance value to use for Z ordinates. Tolerances are used in
/// some geometric tests, mostly for equality between coordinates.  This
/// occurs frequently in spatial queries, especially with "on boundary" or "just
/// touching" conditions.  It is also an objective amount that can be used
/// when formulating step distances for parametric curve interpolation.
/// 
/// \return
/// Returns the tolerance
/// 
const double c_KgMssCreateSpatialContext::GetZTolerance()
{
  return m_ZTolerance;
}

/// \brief
/// Sets the tolerance value to use for Z ordinates. Tolerances are used in
/// some geometric tests, mostly for equality between coordinates.  This
/// occurs frequently in spatial queries, especially with "on boundary" or "just
/// touching" conditions.  It is also an objective amount that can be used
/// when formulating step distances for parametric curve interpolation.
/// 
/// \param value 
/// Input the tolerance
/// 
/// \return
/// Returns nothing
/// 
void c_KgMssCreateSpatialContext::SetZTolerance(const double ZTolerance)
{
  m_ZTolerance = ZTolerance;
}

/// \brief
/// Gets a Boolean flag that indicates how the CreateSpatialContext command
/// should behave if the spatial context already exists. If update existing
/// is true and a spatial context with the specified name already exists
/// then it will be updated. If update existing is false and a spatial
/// context with the specified name already exists, command execution
/// will fail.
/// 
/// \return
/// Returns true if command should update existing contexts
/// 
const bool c_KgMssCreateSpatialContext::GetUpdateExisting()
{
  return m_UpdateExisting;
}

/// \brief
/// Sets a Boolean flag that indicates how the CreateSpatialContext command
/// should behave if the spatial context already exists. If update existing
/// is true and a spatial context with the specified name already exists
/// then it will be updated. If update existing is false and a spatial
/// context with the specified name already exists, command execution
/// will fail.
/// 
/// \param value 
/// Input true if existing contexts should be updated
/// 
/// \return
/// Returns nothing
/// 
void c_KgMssCreateSpatialContext::SetUpdateExisting(const bool UpdateExisting)
{
  m_UpdateExisting = UpdateExisting;
}

/// \brief
/// Executes the CreateSpatialContext command. An exception is thrown if
/// the spatial context already exists, or if the coordinate system or
/// extent type is not supported by the provider.
/// 
/// \return
/// Returns nothing
/// 

// This will simulate adding new spatial context.
// It will just add into current spatial context list of connection
// It will add if I can find same coordinate system into oracle

// This function is implemented in such way - so ApplySchema could find this spatial context
// Client would call create spatial context before apply command
// So when Apply command is checking for spatial context this one will be found.
// Important thing is that create SC and Apply Schema are call one after other without closing conection
// so this spatial context list will stay.
void c_KgMssCreateSpatialContext::Execute()
{
  FdoPtr<c_KgMssSpatialContextCollection> sccol = m_Connection->GetSpatialContexts();
  
  FdoPtr<c_KgMssSpatialContext> spcontext = sccol->FindItem( GetName() );
  
  if( spcontext.p  )
  {
  // already exist - do nothing
    return;
  }

// spatial context doesnt exist - create one
  
  FdoStringP orig_name,orig_csname,orig_wkt;
  orig_name = GetName();
  orig_csname = GetCoordinateSystem();
  orig_wkt = GetCoordinateSystemWkt();
  
  c_KgMssSridDesc orasrid;
  FdoStringP name = GetName();
  FdoStringP temp = name.Mid(0,10);
  if( temp.ICompare(D_MSS_SRID_NAME_NULL) == 0)
  {
    orasrid.m_OraSrid = -1;
    orig_wkt = L"";
    orig_csname = L"";
    orasrid.m_IsGeodetic = false;
  }
  else
  {
    if( temp.ICompare(D_MSS_SRID_NAME_PREFIX) == 0)
    {
      FdoStringP temp = name.Mid(10,name.GetLength()-10);
      orasrid.m_OraSrid = temp.ToLong();
      
      string wkt;
      if( c_FdoMss_API::GetCoordinateSystemWkt(m_Connection,orasrid.m_OraSrid,wkt) )
      {
        orig_wkt = wkt.c_str();
      }
      
          
      orasrid.m_IsGeodetic = c_FdoMss_API::IsGeodeticCoordSystem((const char*)orig_wkt);
    }
    else
    {  
    // find this coordinate system in oracle
     
      FdoStringP tempname = GetCoordinateSystem();    
      FdoStringP temp = tempname.Mid(0,10);
      if( temp.ICompare(D_MSS_SRID_NAME_PREFIX) == 0)
      {
        FdoStringP temp = tempname.Mid(10,tempname.GetLength()-10);
        orasrid.m_OraSrid = temp.ToLong();
        orasrid.m_IsGeodetic = false;
        try
        {
          string wkt;
          if( c_FdoMss_API::GetCoordinateSystemWkt(m_Connection,orasrid.m_OraSrid,wkt) )
          {
            orig_wkt = wkt.c_str();
          }
          orasrid.m_IsGeodetic = c_FdoMss_API::IsGeodeticCoordSystem(wkt.c_str());
        }
        catch(c_KgOtlException& ea)
        {
          FdoStringP gstr = ea.what();
          //throw FdoCommandException::Create( gstr );    
        }    
      }
      else
      {
        // find from WKT
        tempname = GetCoordinateSystemWkt();      
        try
        {
          string csname = tempname;
          string wkt = tempname;
          orasrid.m_OraSrid = c_FdoMss_API::GetSridFromWkt(m_Connection,csname.c_str());
          orasrid.m_IsGeodetic = c_FdoMss_API::IsGeodeticCoordSystem(wkt.c_str());
        }
        catch(c_KgOtlException& ea)
        {
          throw FdoCommandException::Create( ea.what() );    
        }
        
        if( !orasrid.m_OraSrid )
        {
          throw FdoCommandException::Create( L"c_KgMssCreateSpatialContext::Execute: Could not find match for coordinate system in Oracle!" );    
        }
      }
    }
  }
  FdoPtr<c_KgMssSpatialContext> newsc = new c_KgMssSpatialContext();
  
  newsc->SetName( orig_name );
  newsc->SetDescription( GetDescription() );
  newsc->SetCoordSysName(orig_csname);
  newsc->SetCoordinateSystemWkt(orig_wkt);
  
  newsc->SetXYTolerance( GetXYTolerance() );
  newsc->SetZTolerance( GetZTolerance() );
  
  
  newsc->SetOraSridDesc(orasrid);
  
  sccol->Add( newsc );
}//end of c_KgMssCreateSpatialContext::Execute