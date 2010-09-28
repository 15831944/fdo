/*
* Copyright (C) 2009  SL-King d.o.o
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
#include "c_KgOraFilterProcessor.h"
#include "c_FgfToSdoGeom.h"
#include "c_Ora_API2.h"


#define D_FILTER_OPEN_PARENTH L" ( "
#define D_FILTER_CLOSE_PARENTH L" ) "

#define D_FILTER_COMMA L" , "
#define D_FILTER_LOGICAL_AND L" AND "
#define D_FILTER_LOGICAL_OR L" OR "
#define D_FILTER_LOGICAL_NOT L" NOT "
#define D_FILTER_IN L" IN "
#define D_FILTER_IS_NULL L" IS NULL "

// For comparison operation
#define D_FILTER_EQUAL_OP               L" = "
#define D_FILTER_NOT_EQUAL_OP           L" <> "
#define D_FILTER_GREATER_THAN_OP        L" > "
#define D_FILTER_GREATER_OR_EQUAL_OP    L" >= "
#define D_FILTER_LESS_THAN_OP           L" < "
#define D_FILTER_LESS_OR_EQUAL_OP       L" <= "
#define D_FILTER_LIKE_OP                L" LIKE "





c_KgOraFilterProcessor::c_KgOraFilterProcessor(int OracleMainVersion,c_KgOraSchemaDesc *KgOraSchemaDesc,FdoIdentifier* ClassId,const c_KgOraSridDesc& OraSridDesc)
  : m_ExpressionProcessor( &m_StringBuff,KgOraSchemaDesc,ClassId,OraSridDesc ) // they will share same string buffer
{
  m_OracleMainVersion = OracleMainVersion;
  m_KgOraSchemaDesc = KgOraSchemaDesc;
  FDO_SAFE_ADDREF(m_KgOraSchemaDesc.p);
  
  m_ClassId = ClassId;
  FDO_SAFE_ADDREF(m_ClassId.p);
  
  if( m_KgOraSchemaDesc.p && m_ClassId.p )
  {
    FdoPtr<FdoKgOraPhysicalSchemaMapping> phschemamapping = m_KgOraSchemaDesc->GetPhysicalSchemaMapping();
    m_ClassDef = phschemamapping->FindByClassName( m_ClassId->GetName() );
    
    /*
    if( phys_class && phys_class->GetIsPointGeometry() )
    {      
      AppendString( phys_class->GetOraTableAlias() );
      AppendString( "." );
    }
    */
  }
  
  m_OraSridDesc = OraSridDesc;
  
}

c_KgOraFilterProcessor::~c_KgOraFilterProcessor(void)
{
  
}

void c_KgOraFilterProcessor::ProcessFilter(FdoFilter* Filter)
{
  Filter->Process(this);
}//end of c_KgOraFilterProcessor::ProcessFilter

void c_KgOraFilterProcessor::ProcessExpresion( FdoExpression* Expr,bool IsSpatialCondition,bool IsEnvelopeIntersect)
{  
  Expr->Process( &m_ExpressionProcessor );
   
}


//
// Add a string to the end of the buffer
void c_KgOraFilterProcessor::AppendString(const wchar_t *Str)
{
  m_StringBuff.AppendString(Str);
}

//
// Add a string to the biginning of the buffer
void c_KgOraFilterProcessor::PrependString(const wchar_t *Str)
{
  m_StringBuff.PrependString(Str);
}




/// \brief
/// Processes the FdoBinaryLogicalOperator passed in as an argument.
/// 
/// \param Filter 
/// Input the FdoBinaryLogicalOperator
/// 
/// \return
/// Returns nothing
/// 
void c_KgOraFilterProcessor::ProcessBinaryLogicalOperator(FdoBinaryLogicalOperator& Filter)
{
  FdoPtr<FdoFilter>leftop = Filter.GetLeftOperand();
  FdoPtr<FdoFilter>rightop = Filter.GetRightOperand();
  AppendString(D_FILTER_OPEN_PARENTH);
  if( Filter.GetOperation() == FdoBinaryLogicalOperations_And )
  {
    
    ProcessFilter( leftop );  
    AppendString( D_FILTER_LOGICAL_AND );
    ProcessFilter( rightop );
  }
  else
  {   
    ProcessFilter( leftop );
    AppendString( D_FILTER_LOGICAL_OR );
    ProcessFilter( rightop );
  }

  AppendString(D_FILTER_CLOSE_PARENTH);
  
}//end of c_KgOraFilterProcessor::ProcessBinaryLogicalOperator

void c_KgOraFilterProcessor::ProcessUnaryLogicalOperator(FdoUnaryLogicalOperator& Filter)
{
    FdoPtr<FdoFilter>unaryop = Filter.GetOperand();
    if( unaryop == NULL ) throw FdoFilterException::Create(L"FdoUnaryLogicalOperator is missing the operand");

    AppendString(D_FILTER_OPEN_PARENTH);
    if ( Filter.GetOperation() == FdoUnaryLogicalOperations_Not )
    {
        AppendString( D_FILTER_LOGICAL_NOT );
    }
    else
        throw FdoFilterException::Create(L"FdoUnaryLogicalOperator supports only the 'Not' operation");

    ProcessFilter( unaryop );
    AppendString(D_FILTER_CLOSE_PARENTH);
}//end of c_KgOraFilterProcessor::ProcessUnaryLogicalOperator

void c_KgOraFilterProcessor::ProcessComparisonCondition(FdoComparisonCondition& Filter)
{
    FdoPtr<FdoExpression> leftexp = Filter.GetLeftExpression();
    FdoPtr<FdoExpression> rightexp = Filter.GetRightExpression();
    if( leftexp == NULL  ) throw FdoFilterException::Create(L"FdoComparisonCondition is missing the left expression");
    if( rightexp == NULL ) throw FdoFilterException::Create(L"FdoComparisonCondition is missing the right expression");

    AppendString( D_FILTER_OPEN_PARENTH );
    ProcessExpresion( leftexp );

    switch ( Filter.GetOperation() )
    {
        case FdoComparisonOperations_EqualTo: 
          AppendString( D_FILTER_EQUAL_OP );
        break;

        case FdoComparisonOperations_NotEqualTo: 
          AppendString( D_FILTER_NOT_EQUAL_OP );
        break;

        case FdoComparisonOperations_GreaterThan: 
          AppendString( D_FILTER_GREATER_THAN_OP );
        break;

        case FdoComparisonOperations_GreaterThanOrEqualTo: 
          AppendString( D_FILTER_GREATER_OR_EQUAL_OP );
        break;

        case FdoComparisonOperations_LessThan: 
          AppendString( D_FILTER_LESS_THAN_OP );
        break;

        case FdoComparisonOperations_LessThanOrEqualTo: 
          AppendString( D_FILTER_LESS_OR_EQUAL_OP );
        break;

        case FdoComparisonOperations_Like: 
          AppendString( D_FILTER_LIKE_OP );
        break;
        default:
          throw FdoFilterException::Create(L"FdoComparisonCondition unkown comparison operation");
        break;
    }
    ProcessExpresion( rightexp );
    AppendString( D_FILTER_CLOSE_PARENTH );
}//end of c_KgOraFilterProcessor::ProcessComparisonCondition


void c_KgOraFilterProcessor::ProcessInCondition(FdoInCondition& Filter)
{
    int i;
    FdoPtr<FdoExpression> exp;
    FdoPtr<FdoIdentifier> id = Filter.GetPropertyName();
    if( id == NULL )
        throw FdoFilterException::Create(L"FdoInCondition is missing the property name");

    FdoPtr<FdoValueExpressionCollection>expressions = Filter.GetValues();
    if( expressions == NULL || expressions->GetCount() <= 0 )
        throw FdoFilterException::Create(L"FdoInCondition has an empty value list");

    AppendString(D_FILTER_OPEN_PARENTH);
    ProcessExpresion( id );
    AppendString( D_FILTER_IN );
    AppendString(D_FILTER_OPEN_PARENTH);
    for(i=0; i < expressions->GetCount()-1; i++ )
    {
        exp = expressions->GetItem(i);
        ProcessExpresion( exp );
        AppendString( D_FILTER_COMMA );
    }
    exp = expressions->GetItem( i );
    ProcessExpresion(exp);
    AppendString(D_FILTER_CLOSE_PARENTH);
    AppendString(D_FILTER_CLOSE_PARENTH);
}//end of c_KgOraFilterProcessor::ProcessInCondition

void c_KgOraFilterProcessor::ProcessNullCondition(FdoNullCondition& Filter)
{
    FdoPtr<FdoIdentifier>id = Filter.GetPropertyName();
    if( id == NULL )
        throw FdoFilterException::Create(L"FdoNullCondition is missing the property name");

    AppendString(D_FILTER_OPEN_PARENTH);
    ProcessExpresion( id );
    AppendString( D_FILTER_IS_NULL );
    AppendString(D_FILTER_CLOSE_PARENTH);
    
}//end of c_KgOraFilterProcessor::ProcessNullCondition


void c_KgOraFilterProcessor::ProcessSpatialCondition(FdoSpatialCondition& Filter)
{

FdoPtr<FdoIdentifier> geomprop = Filter.GetPropertyName();

FdoPtr<FdoExpression> geomexp = Filter.GetGeometry();

// If class is from SDE then need to apply only primary check of min max
if( m_ClassDef.p && m_ClassDef->GetIsSdeClass() )
{
  FdoGeometryValue* geomval = dynamic_cast<FdoGeometryValue*>(geomexp.p);
  if (geomval)
  {
    FdoPtr<FdoByteArray> fgf = geomval->GetGeometry();
    FdoPtr<FdoFgfGeometryFactory> gf = FdoFgfGeometryFactory::GetInstance();
    FdoPtr<FdoIGeometry> fgfgeom = gf->CreateGeometryFromFgf(fgf);
    FdoPtr<FdoIEnvelope> envelope = fgfgeom->GetEnvelope();

    double minx = envelope->GetMinX();
    double miny = envelope->GetMinY();

    double maxx = envelope->GetMaxX();
    double maxy = envelope->GetMaxY();

    FdoStringP buff;

    AppendString(D_FILTER_OPEN_PARENTH);
    AppendString(L" NOT ");
    AppendString(D_FILTER_OPEN_PARENTH);

    AppendString(L"eminx");
    AppendString(L">");       
    buff = FdoStringP::Format(L"%.8lf",maxx);
    AppendString((FdoString*)buff);

    AppendString(L" or ");

    AppendString(L"emaxx");
    AppendString(L"<");        
    buff = FdoStringP::Format(L"%.8lf",minx);
    AppendString((FdoString*)buff);

    AppendString(L" or ");

    AppendString(L"eminy");
    AppendString(L">");        
    buff = FdoStringP::Format(L"%.8lf",maxy);
    AppendString((FdoString*)buff);

    AppendString(L" or ");

    AppendString(L"emaxy");
    AppendString(L"<");        
    buff = FdoStringP::Format(L"%.8lf",miny);
    AppendString((FdoString*)buff);

    AppendString(D_FILTER_CLOSE_PARENTH);        
    AppendString(D_FILTER_CLOSE_PARENTH);        
  }    
  return;
}

switch( Filter.GetOperation() )
{
  case FdoSpatialOperations_EnvelopeIntersects:
  {
    if( m_ClassDef.p &&  m_ClassDef->GetIsPointGeometry() )
    {
      FdoStringP str_xcol =  m_ClassDef->GetPointXOraColumn();
      FdoStringP str_ycol =  m_ClassDef->GetPointYOraColumn();
      
      FdoGeometryValue* geomval = dynamic_cast<FdoGeometryValue*>(geomexp.p);
      if (geomval)
      {
        FdoPtr<FdoByteArray> fgf = geomval->GetGeometry();
        FdoPtr<FdoFgfGeometryFactory> gf = FdoFgfGeometryFactory::GetInstance();
        FdoPtr<FdoIGeometry> fgfgeom = gf->CreateGeometryFromFgf(fgf);
        FdoPtr<FdoIEnvelope> envelope = fgfgeom->GetEnvelope();

        double minx = envelope->GetMinX();
        double miny = envelope->GetMinY();
        
        double maxx = envelope->GetMaxX();
        double maxy = envelope->GetMaxY();

        FdoStringP buff;
        
        AppendString(D_FILTER_OPEN_PARENTH);
        
        AppendString(str_xcol);
        AppendString(L">=");       
        buff = FdoStringP::Format(L"%.8lf",minx);
        AppendString((FdoString*)buff);
        
        AppendString(L" and ");
        
        AppendString(str_xcol);
        AppendString(L"<=");        
        buff = FdoStringP::Format(L"%.8lf",maxx);
        AppendString((FdoString*)buff);
        
        AppendString(L" and ");
        
        AppendString(str_ycol);
        AppendString(L">=");        
        buff = FdoStringP::Format(L"%.8lf",miny);
        AppendString((FdoString*)buff);
        
        AppendString(L" and ");
        
        AppendString(str_ycol);
        AppendString(L"<=");        
        buff = FdoStringP::Format(L"%.8lf",maxy);
        AppendString((FdoString*)buff);
        
        AppendString(D_FILTER_CLOSE_PARENTH);        
      }      
    }
    else
    {
     
      AppendString(D_FILTER_OPEN_PARENTH);
      AppendString(L"SDO_FILTER(");
      ProcessExpresion( geomprop );
      AppendString(L",");
      
      FdoGeometryValue* geomval = dynamic_cast<FdoGeometryValue*>(geomexp.p);
      if (geomval)
      {
        GetExpressionProcessor().ProcessGeometryValueRect(*geomval);
      }
      else
      {
        ProcessExpresion( geomexp,true );
      }
         
      if( m_OracleMainVersion < 10 )
      {
        AppendString(L",'querytype = WINDOW')='TRUE'");
      }
      else
      {
        AppendString(L")='TRUE'");
      }
      AppendString(D_FILTER_CLOSE_PARENTH);
    
    
    /*
      AppendString(D_FILTER_OPEN_PARENTH);
      AppendString(L"SDO_FILTER(");
      ProcessExpresion( geomprop );
      AppendString(L",");
      
      // Here I need spatial case of processing becase i want to create
      // optimezed rect (not polygons) for  this query - needed for geodetic data 
      FdoGeometryValue* geomval = dynamic_cast<FdoGeometryValue*>(geomexp.p);
      if (geomval)
      {
        FdoPtr<FdoByteArray> fgf = geomval->GetGeometry();
        FdoPtr<FdoFgfGeometryFactory> gf = FdoFgfGeometryFactory::GetInstance();
        FdoPtr<FdoIGeometry> fgfgeom = gf->CreateGeometryFromFgf(fgf);
        FdoPtr<FdoIEnvelope> envelope = fgfgeom->GetEnvelope();

        double minx = envelope->GetMinX();
        double miny = envelope->GetMinY();
        
        double maxx = envelope->GetMaxX();
        double maxy = envelope->GetMaxY();

        if( m_OraSridDesc.m_IsGeodetic )
        {
        // if it is geodetic data I need to check boundaries
        // that will not go over 180 and 90
        // MapGuide will send bigger and than oracle wan't return geometries
        // so this is workarround for it
        
        
          if( minx < - 180 ) minx = -180.0;
          if( maxx > 180 ) maxx = 180.0;
          
          if( maxx < minx ) { minx=-180; maxx=180; }
          
          if( miny < -90.0 ) miny = -90.0;
          if( maxy > 90.0 ) maxy = 90;
          
          if( maxy < miny ) { miny=-90; maxy=90; }
          if( maxx < minx ) { minx=-180; maxx=180; }
        
        }

      // create optimize rect
        SDO_GEOMETRY *sdorect = c_Ora_API2::CreateOptimizedRect(m_OraSridDesc.m_OraSrid,minx,miny,maxx,maxy);
        char * buff = c_Ora_API2::SdoGeomToString(sdorect);
      
        if( buff )
        {
          AppendString( buff );
          delete [] buff;
        }
        
        delete sdorect;
      }
      else
      {
        ProcessExpresion( geomexp,true );
      }
      
      
      AppendString(L")='TRUE'");
      AppendString(D_FILTER_CLOSE_PARENTH);
    
    */
    }    
  }
  break;
  case FdoSpatialOperations_Intersects:
  {
    if( m_ClassDef.p &&  m_ClassDef->GetIsPointGeometry() )
    {
      FdoStringP str_xcol =  m_ClassDef->GetPointXOraColumn();
      FdoStringP str_ycol =  m_ClassDef->GetPointYOraColumn();
      
      FdoGeometryValue* geomval = dynamic_cast<FdoGeometryValue*>(geomexp.p);
      if (geomval)
      {
        FdoPtr<FdoByteArray> fgf = geomval->GetGeometry();
        FdoPtr<FdoFgfGeometryFactory> gf = FdoFgfGeometryFactory::GetInstance();
        FdoPtr<FdoIGeometry> fgfgeom = gf->CreateGeometryFromFgf(fgf);
        FdoPtr<FdoIEnvelope> envelope = fgfgeom->GetEnvelope();

        double minx = envelope->GetMinX();
        double miny = envelope->GetMinY();
        
        double maxx = envelope->GetMaxX();
        double maxy = envelope->GetMaxY();    

        FdoStringP buff;
        
        AppendString(D_FILTER_OPEN_PARENTH);
        
        AppendString(str_xcol);
        AppendString(L">=");       
        buff = FdoStringP::Format(L"%.8lf",minx);
        AppendString((FdoString*)buff);
        
        AppendString(L" and ");
        
        AppendString(str_xcol);
        AppendString(L"<=");        
        buff = FdoStringP::Format(L"%.8lf",maxx);
        AppendString((FdoString*)buff);
        
        AppendString(L" and ");
        
        AppendString(str_ycol);
        AppendString(L">=");        
        buff = FdoStringP::Format(L"%.8lf",miny);
        AppendString((FdoString*)buff);
        
        AppendString(L" and ");
        
        AppendString(str_ycol);
        AppendString(L"<=");        
        buff = FdoStringP::Format(L"%.8lf",maxy);
        AppendString((FdoString*)buff);
        
        AppendString(D_FILTER_CLOSE_PARENTH);        
      }
    }
    else
    {    
      AppendString(D_FILTER_OPEN_PARENTH);
      AppendString(L"SDO_ANYINTERACT(");
      ProcessExpresion( geomprop );
      AppendString(L",");
      
      ProcessExpresion( geomexp,true );
      /*
      // Here I need spatial case of processing becase i want to create
      // optimezed rect (not polygons) for  this query - needed for geodetic data 
      FdoGeometryValue* geomval = dynamic_cast<FdoGeometryValue*>(geomexp.p);
      if (geomval)
      {
        FdoPtr<FdoByteArray> fgf = geomval->GetGeometry();
        FdoPtr<FdoFgfGeometryFactory> gf = FdoFgfGeometryFactory::GetInstance();
        FdoPtr<FdoIGeometry> fgfgeom = gf->CreateGeometryFromFgf(fgf);
        FdoPtr<FdoIEnvelope> envelope = fgfgeom->GetEnvelope();

        double minx = envelope->GetMinX();
        double miny = envelope->GetMinY();
        
        double maxx = envelope->GetMaxX();
        double maxy = envelope->GetMaxY();

        if( m_OraSridDesc.m_IsGeodetic )
        {
        // if it is geodetic data I need to check boundaries
        // that will not go over 180 and 90
        // MapGuide will send bigger and than oracle wan't return geometries
        // so this is workarround for it
          if( minx < - 180 ) minx = -180.0;
          if( maxx > 180 ) maxx = 180.0;
          
          if( miny < -90.0 ) miny = -90.0;
          if( maxy > 90.0 ) maxy = 90;
        }

      // create optimize rect
        SDO_GEOMETRY *sdorect = c_Ora_API2::CreateOptimizedRect(m_OraSridDesc.m_OraSrid,minx,miny,maxx,maxy);
        char * buff = c_Ora_API2::SdoGeomToString(sdorect);
      
        if( buff )
        {
          AppendString( buff );
          delete [] buff;
        }
        
        delete sdorect;
        
      }
      else
      {
        ProcessExpresion( geomexp,true );
      }
      */
      
      
      AppendString(L")='TRUE'");
      AppendString(D_FILTER_CLOSE_PARENTH);
    }
  }
  break;
  
  
  /// Test to see if the geometric property value spatially contains the
  /// literal geometric value.
  case FdoSpatialOperations_Contains:
  {
    AppendString(D_FILTER_OPEN_PARENTH);
    AppendString(L"SDO_RELATE(");
    ProcessExpresion( geomprop );
    AppendString(L",");
    ProcessExpresion( geomexp,true );
    AppendString(L",'mask=CONTAINS')='TRUE'");
    AppendString(D_FILTER_CLOSE_PARENTH);
  }
  break;

  /// Test to see if the geometric property value spatially crosses the given
  /// geometry.
  case FdoSpatialOperations_Crosses:
  {
    AppendString(D_FILTER_OPEN_PARENTH);
    AppendString(L"SDO_RELATE(");
    ProcessExpresion( geomprop );
    AppendString(L",");
    ProcessExpresion( geomexp,true );
    AppendString(L",'mask=OVERLAPBDYDISJOINT')='TRUE'");
    AppendString(D_FILTER_CLOSE_PARENTH);
  }
  break;

  case FdoSpatialOperations_Disjoint:
  {
    AppendString(D_FILTER_OPEN_PARENTH);
    AppendString(L"SDO_RELATE(");
    ProcessExpresion( geomprop );
    AppendString(L",");
    ProcessExpresion( geomexp,true );
    AppendString(L",'mask=DISJOINT')='TRUE'");
    AppendString(D_FILTER_CLOSE_PARENTH);
  }
  break;
  /// Test to see if the geometric property value spatially overlaps the given
  /// geometry
  case FdoSpatialOperations_Overlaps:
    {
    AppendString(D_FILTER_OPEN_PARENTH);
    AppendString(L"SDO_RELATE(");
    ProcessExpresion( geomprop );
    AppendString(L",");
    ProcessExpresion( geomexp,true );
    AppendString(L",'mask=OVERLAPBDYINTERSECT')='TRUE'");
    AppendString(D_FILTER_CLOSE_PARENTH);
  }
  break;

    /// Test to see if the geometric property value spatially touches the given
    /// geometry.
  case FdoSpatialOperations_Touches:
  {
    AppendString(D_FILTER_OPEN_PARENTH);
    AppendString(L"SDO_RELATE(");
    ProcessExpresion( geomprop );
    AppendString(L",");
    ProcessExpresion( geomexp,true );
    AppendString(L",'mask=TOUCH')='TRUE'");
    AppendString(D_FILTER_CLOSE_PARENTH);
  }
  break;

    /// Test to see if the geometric property value is spatially within the
    /// given geometry.
  case FdoSpatialOperations_Within:
  {
    AppendString(D_FILTER_OPEN_PARENTH);
    AppendString(L"SDO_RELATE(");
    ProcessExpresion( geomprop );
    AppendString(L",");
    ProcessExpresion( geomexp,true );
    AppendString(L",'mask=COVERS')='TRUE'");
    AppendString(D_FILTER_CLOSE_PARENTH);
  }
  break;

    /// Test to see if the geometric property value is covered by the interior
    /// and boundary of the given geometry.
  case FdoSpatialOperations_CoveredBy:
  {
    AppendString(D_FILTER_OPEN_PARENTH);
    AppendString(L"SDO_RELATE(");
    ProcessExpresion( geomprop );
    AppendString(L",");
    ProcessExpresion( geomexp,true );
    AppendString(L",'mask=COVERDBY')='TRUE'");
    AppendString(D_FILTER_CLOSE_PARENTH);
  }
  break;

  /// Test to see if the geometric property value is inside the interior
  /// of the given geometry, not touching the boundary.
  case FdoSpatialOperations_Inside:
  {
      AppendString(D_FILTER_OPEN_PARENTH);
      AppendString(L"SDO_RELATE(");
      ProcessExpresion( geomprop );
      AppendString(L",");
      ProcessExpresion( geomexp,true );
      AppendString(L",'mask=INSIDE')='TRUE'");
      AppendString(D_FILTER_CLOSE_PARENTH);
    }
  break;

  

    /// Test to see if the geometric property value is spatially equal to the
    /// given geometry.
    case FdoSpatialOperations_Equals:
    {
      AppendString(D_FILTER_OPEN_PARENTH);
      AppendString(L"SDO_RELATE(");
      ProcessExpresion( geomprop );
      AppendString(L",");
      ProcessExpresion( geomexp,true );
      AppendString(L",'mask=EQUAL')='TRUE'");
      AppendString(D_FILTER_CLOSE_PARENTH);
    }
    break;
    
    
  default:
    throw FdoFilterException::Create(L"c_KgOraFilterProcessor::ProcessSpatialCondition Unsupported Spatial Condition Operation");
  break;  
}
}//end of c_KgOraFilterProcessor::ProcessSpatialCondition

void c_KgOraFilterProcessor::ProcessDistanceCondition(FdoDistanceCondition& Filter)
{
FdoPtr<FdoIdentifier> geomprop = Filter.GetPropertyName();

FdoPtr<FdoExpression> geomval = Filter.GetGeometry();
double dist = Filter.GetDistance();
switch( Filter.GetOperation() )
{
  case FdoDistanceOperations_Within:
  {
    // sprintf(sbuff,"SDO_ANYINTERACT(a.%s,%s)='TRUE'",(const char*)gcolname,sbuff2);
    
    AppendString(D_FILTER_OPEN_PARENTH);
    AppendString(L"SDO_WITHIN_DISTANCE(");
    ProcessExpresion( geomprop );
    AppendString(L",");
    ProcessExpresion( geomval );
    FdoStringP tmpbuff = FdoStringP::Format(L",'distance=%.6lf'",dist);
    AppendString((FdoString*)tmpbuff);
    AppendString(L")='TRUE'");
    AppendString(D_FILTER_CLOSE_PARENTH);
  }
  break;
  case FdoDistanceOperations_Beyond:
  {
    AppendString(D_FILTER_OPEN_PARENTH);
    AppendString(L"SDO_WITHIN_DISTANCE(");
    ProcessExpresion( geomprop );
    AppendString(L",");
    ProcessExpresion( geomval );
    FdoStringP tmpbuff = FdoStringP::Format(L",'distance=%.6lf'",dist);
    AppendString((FdoString*)tmpbuff);
    AppendString(L")='FALSE'");
    AppendString(D_FILTER_CLOSE_PARENTH);
  }
  break;
  default:
    throw FdoFilterException::Create(L"c_KgOraFilterProcessor::ProcessDistanceCondition Unsupported Spatial Condition Operation");
  break;  
}
}//end of c_KgOraFilterProcessor::ProcessDistanceCondition
