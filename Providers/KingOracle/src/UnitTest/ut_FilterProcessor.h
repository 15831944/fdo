#pragma once

class ut_FilterProcessor : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( ut_FilterProcessor );
    CPPUNIT_TEST( ProcessSpatialCondition );    
    CPPUNIT_TEST( ProcessSpatialConditionSRID );    
  CPPUNIT_TEST_SUITE_END();
  
public:
  ut_FilterProcessor(void);
public:
  ~ut_FilterProcessor(void);
  
  void ProcessSpatialCondition();   
  void ProcessSpatialConditionSRID();
};