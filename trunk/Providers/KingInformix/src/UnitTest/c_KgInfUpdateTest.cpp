#include "StdAfx.h"

#include "c_KgInfoUtil.h"
#include "c_KgInfUpdateTest.h"


CPPUNIT_TEST_SUITE_REGISTRATION(c_KgInfUpdateTest);



c_KgInfUpdateTest::c_KgInfUpdateTest(void)
{
}

c_KgInfUpdateTest::~c_KgInfUpdateTest(void)
{
}




void c_KgInfUpdateTest::UpdateTest()
{

try
{
  // Connect:

  FdoPtr<FdoIConnection> conn = c_KgInfoUtil::OpenUnitTestConnection();

  FdoPtr<FdoIDescribeSchema> comm1 = (FdoIDescribeSchema*)conn->CreateCommand(FdoCommandType_DescribeSchema);
  FdoPtr<FdoFeatureSchemaCollection> schemas = comm1->Execute();

  FdoStringP src_classname = L"informix~cities~locn";
 

  FdoPtr<FdoIUpdate> comm= (FdoIUpdate*)conn->CreateCommand( FdoCommandType_Update);    
  comm->SetFeatureClassName( src_classname );
  FdoPtr<FdoPropertyValueCollection> propcol = comm->GetPropertyValues();
    
  // Geometry property
  FdoPtr<FdoFgfGeometryFactory> fgf = FdoFgfGeometryFactory::GetInstance();
  
  double ordinates[10] = { 5,5}; 
  
  FdoPtr<FdoIPoint> point = fgf->CreatePoint(0,ordinates); 
  FdoPtr<FdoByteArray> barray = fgf->GetFgf(point);
  FdoPtr<FdoGeometryValue> geom_val=  FdoGeometryValue::Create(barray);
  FdoPtr<FdoPropertyValue> propval_geom = FdoPropertyValue::Create(L"locn",geom_val);  
  
  FdoPtr<FdoStringValue> name_val=  (FdoStringValue*)FdoDataValue::Create(L"inserttest_name2");
  FdoPtr<FdoPropertyValue> propval_name = FdoPropertyValue::Create(L"name",name_val);
  
  propcol->Add(propval_geom);
  propcol->Add(propval_name);
  
  CString filter = L"name = 'inserttest_name'";
  comm->SetFilter(filter);
      
  int updated = comm->Execute(); 
                 

  if( updated != 1 )
  {
    CPPUNIT_FAIL( "c_KgInfUpdateTest::UpdateTest Update should returned 1 for num of changed rows" );
  }
 

  conn->Close();
}
catch(FdoException* ex)
{
  FdoStringP str = ex->GetExceptionMessage();
  ex->Release();
  CPPUNIT_FAIL( (const char*)str );
}
  
}//end of c_KgInfUpdateTest::UpdateTest





void c_KgInfUpdateTest::SetupSchema()
{
try
{
  FdoPtr<FdoIConnection> connection = c_KgInfoUtil::OpenUnitTestConnection();
  FdoPtr<FdoISQLCommand> sqlcomm = (FdoISQLCommand*)connection->CreateCommand( FdoCommandType_SQLCommand );
  
  sqlcomm->SetSQLStatement(L"DROP TABLE insertclass");
  sqlcomm->ExecuteNonQuery();
}
catch(FdoException* ex)
{
  FdoStringP str = ex->GetExceptionMessage();
  ex->Release();
  
}



try
{
  // Connect:
        FdoPtr<FdoIConnection> connection = c_KgInfoUtil::OpenUnitTestConnection();
        

        // Define one new schema:
        FdoPtr<FdoFeatureSchema> schema = FdoFeatureSchema::Create(L"insertshema", L"");
        FdoPtr<FdoClassCollection> classes = schema->GetClasses();

        // Define one new class with X/Y/Z lowercase property names (and an identity property and a geometry property):
        FdoPtr<FdoFeatureClass> newClass = FdoFeatureClass::Create(L"insertclass", L"");
        FdoPtr<FdoPropertyDefinitionCollection> newProps = newClass->GetProperties();
        FdoPtr<FdoDataPropertyDefinitionCollection> newIds = newClass->GetIdentityProperties();
        
        FdoPtr<FdoDataPropertyDefinition> dataPropID = FdoDataPropertyDefinition::Create(L"col_id", L"");
        dataPropID->SetDataType(FdoDataType_Int32);
        dataPropID->SetIsAutoGenerated(true);
        dataPropID->SetNullable(false);
        newProps->Add(dataPropID);
        newIds->Add(dataPropID);
        
        FdoPtr<FdoDataPropertyDefinition> datafid = FdoDataPropertyDefinition::Create(L"feat_id", L"");
        datafid->SetDataType(FdoDataType_Int32);
        datafid->SetNullable(false);
        newProps->Add(datafid);
        
        FdoPtr<FdoDataPropertyDefinition> dataPropX = FdoDataPropertyDefinition::Create(L"col_x", L"");
        dataPropX->SetDataType(FdoDataType_Double);
        dataPropX->SetNullable(true);
        newProps->Add(dataPropX);
        FdoPtr<FdoDataPropertyDefinition> dataPropY = FdoDataPropertyDefinition::Create(L"col_y", L"");
        dataPropY->SetDataType(FdoDataType_Double);
        dataPropY->SetNullable(true);
        newProps->Add(dataPropY);
        FdoPtr<FdoDataPropertyDefinition> dataPropZ = FdoDataPropertyDefinition::Create(L"col_z", L"");
        dataPropZ->SetDataType(FdoDataType_Double);
        dataPropZ->SetNullable(true);
        newProps->Add(dataPropZ);
        FdoPtr<FdoDataPropertyDefinition> dataPropstring = FdoDataPropertyDefinition::Create(L"col_name", L"");
        dataPropZ->SetDataType(FdoDataType_String);
        dataPropZ->SetNullable(true);
        newProps->Add(dataPropstring);
        FdoPtr<FdoGeometricPropertyDefinition> dataPropGeom = FdoGeometricPropertyDefinition::Create(L"col_geometry", L"");
        dataPropGeom->SetGeometryTypes(FdoGeometricType_Point);
        newProps->Add(dataPropGeom);
        newClass->SetGeometryProperty(dataPropGeom);
        classes->Add(newClass);

        // Create the new class:
        FdoPtr<FdoIApplySchema> as = (FdoIApplySchema*)connection->CreateCommand(FdoCommandType_ApplySchema);
        as->SetFeatureSchema(schema);
        as->Execute();
                    
        connection->Close();
  
}
catch(FdoException* ex)
{
  FdoStringP str = ex->GetExceptionMessage();
  ex->Release();
  CPPUNIT_FAIL( (const char*)str );
  
}
  
}//end of c_KgInfUpdateTest::SetupSchema