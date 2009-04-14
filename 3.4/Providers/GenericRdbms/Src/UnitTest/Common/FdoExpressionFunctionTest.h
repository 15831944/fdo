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
 *
 */

#ifndef FDO_EXPRESSIONFUNCTIONTEST_H
#define FDO_EXPRESSIONFUNCTIONTEST_H


// =============================================================================
// | The file contains the definition of the class FdoExpressionFunctionTest. It
// | defines the common expression function tests
// =============================================================================


// ---------------------------------------------------------------------------
//                                  Constants                                 
// ---------------------------------------------------------------------------

//  DATASTORE_EXTENSION:
//      The constants specifies the extension to the data store name to be
//      used when creating the data store

#define DATASTORE_EXTENSION      L"_EX_FCT_B"


//  *_TEST_CASE_CODE_*:
//      The constants specify specific test case codes for various test cases
//      where the returned value for the computed property varies.


// ====================== AGGREGATE FUNCTIONS (001+) =====================

#define COUNT_TEST_CASE_CODE_1               1
#define COUNT_TEST_CASE_CODE_2               2
#define COUNT_TEST_CASE_CODE_3               3
#define COUNT_TEST_CASE_CODE_4               4


// ===================== CONVERSION FUNCTIONS (100+) =====================



// ======================== DATE FUNCTIONS (200+) ========================

#define EXTRACT_TEST_CASE_CODE_1           200
#define EXTRACTTODOUBLE_TEST_CASE_CODE_1   201
#define EXTRACTTOINT_TEST_CASE_CODE_1      202


// ====================== GEOMETRY FUNCTIONS (300+) ======================



// ======================== MATH FUNCTIONS (400+) ========================

#define EXP_TEST_CASE_CODE_1               400
#define POWER_TEST_CASE_CODE_1             401


// ======================= NUMERIC FUNCTIONS (500+) ======================



// ======================= STRING FUNCTIONS (600+) =======================





class FdoExpressionFunctionTest : public CppUnit::TestCase
{

  CPPUNIT_TEST_SUITE(FdoExpressionFunctionTest);
    // =========================================
    // ====  AGGREGATE FUNCTION UNIT TESTS  ====
    // =========================================
    // CPPUNIT_TEST(TestAvgFunction);
    // CPPUNIT_TEST(TestCountFunction);
    // CPPUNIT_TEST(TestMaxFunction);
    // CPPUNIT_TEST(TestMedianFunction);
    // CPPUNIT_TEST(TestMinFunction);
    //CPPUNIT_TEST(TestSpatialExtents);
    // CPPUNIT_TEST(TestStddevFunction);
    // CPPUNIT_TEST(TestSumFunction);
    // =========================================
    // ==== CONVERSION FUNCTION UNIT TESTS  ====
    // =========================================
    // CPPUNIT_TEST(TestNullValueFunction);
    // CPPUNIT_TEST(TestToDateFunction);
    // CPPUNIT_TEST(TestToDoubleFunction);
    // CPPUNIT_TEST(TestToFloatFunction);
    // CPPUNIT_TEST(TestToInt32Function);
    // CPPUNIT_TEST(TestToInt64Function);
    // CPPUNIT_TEST(TestToStringFunction);
    // =========================================
    // ====    DATE FUNCTION UNIT TESTS     ====
    // =========================================
    // CPPUNIT_TEST(TestAddMonthsFunction);
    // CPPUNIT_TEST(TestCurrentDateFunction);
    // CPPUNIT_TEST(TestExtractFunction);
    // CPPUNIT_TEST(TestExtractToDoubleFunction);
    // CPPUNIT_TEST(TestExtractToIntFunction);
    // CPPUNIT_TEST(TestMonthsBetweenFunction);
    // =========================================
    // ====    MATH FUNCTION UNIT TESTS     ====
    // =========================================
    // CPPUNIT_TEST(TestAbsFunction);
    // CPPUNIT_TEST(TestAcosFunction);
    // CPPUNIT_TEST(TestAsinFunction);
    // CPPUNIT_TEST(TestAtanFunction);
    // CPPUNIT_TEST(TestAtan2Function);
    // CPPUNIT_TEST(TestCosFunction);
    // CPPUNIT_TEST(TestExpFunction);
    // CPPUNIT_TEST(TestLnFunction);
    // CPPUNIT_TEST(TestLogFunction);
    // CPPUNIT_TEST(TestModFunction);
    // CPPUNIT_TEST(TestPowerFunction);
    // CPPUNIT_TEST(TestRemainderFunction);
    // CPPUNIT_TEST(TestSinFunction);
    // CPPUNIT_TEST(TestSqrtFunction);
    // CPPUNIT_TEST(TestTanFunction);
    // =========================================
    // ====   NUMERIC FUNCTION UNIT TESTS   ====
    // =========================================
    // CPPUNIT_TEST(TestCeilFunction);
    // CPPUNIT_TEST(TestFloorFunction);
    // CPPUNIT_TEST(TestRoundFunction);
    // CPPUNIT_TEST(TestSignFunction);
    // CPPUNIT_TEST(TestTruncFunction);
    // CPPUNIT_TEST(TestTruncFunctionAsFilter);
    // =========================================
    // ====   STRING FUNCTION UNIT TESTS    ====
    // =========================================
    // CPPUNIT_TEST(TestConcatFunction);
    // CPPUNIT_TEST(TestInstrFunction);
    // CPPUNIT_TEST(TestInstrFunctionAsFilter);
    // CPPUNIT_TEST(TestLengthFunction);
    // CPPUNIT_TEST(TestLowerFunction);
    // CPPUNIT_TEST(TestLpadFunction);
    // CPPUNIT_TEST(TestLtrimFunction);
    // CPPUNIT_TEST(TestRpadFunction);
    // CPPUNIT_TEST(TestRtrimFunction);
    // CPPUNIT_TEST(TestSoundexFunction);
    // CPPUNIT_TEST(TestSubstrFunction);
    // CPPUNIT_TEST(TestTranslateFunction);
    // CPPUNIT_TEST(TestTrimFunction);
    // CPPUNIT_TEST(TestUpperFunction);
    // =========================================
    // ====   GEOMETRY FUNCTION UNIT TESTS  ====
    // =========================================
	// CPPUNIT_TEST(TestLength2DFunction);
	// CPPUNIT_TEST(TestArea2DFunction);
    // =========================================
    // ====       RUN ALL UNIT TESTS        ====
    // =========================================
    CPPUNIT_TEST(RunAllExpFctTests);
    // =========================================
    // ====       SPECIAL UNIT TESTS        ====
    // =========================================
    // CPPUNIT_TEST(RunDateInTest);
    // CPPUNIT_TEST(RunTestsInPittsburghContext);
    // CPPUNIT_TEST(RunUpdate);
  CPPUNIT_TEST_SUITE_END();

public:
            FdoExpressionFunctionTest ();
    virtual ~FdoExpressionFunctionTest ();
    void    setUp ();
    void    tearDown ();


protected:

    //-------------------------------------------------------------------------
    //                       Test Environment Setup Functions
    //-------------------------------------------------------------------------

    //  AddFeature:
    //      The function adds a new object for the specified class.The values
    //      being added are predefined based on the predefined schema.

    virtual void AddFeature (FdoIConnection *current_connection,
                             FdoString      *class_name,
                             bool           is_spatial,
                             int            index);

    //  AddTestSchema:
    //      The function establishes the test schema in the FDO data store
    //      the provided connection identifies.

    virtual void AddTestSchema (FdoIConnection *current_connection,
                                FdoString      *schema_name);

    //  CloseConnection:
    //      The function closes all open connections to the data store used in
    //      the executed unit tests.

    virtual void CloseConnection ();

    //  Connect:
    //      The function establishes the connection to the data store used in
    //      the executed test suites.

	virtual void Connect ();

    //  CreateDataProperty :
    //      The function creates a data property object based on the provided
    //      data and returns it back to the calling routine.

    virtual FdoDataPropertyDefinition *CreateDataProperty (
                                                  FdoString   *property_name,
                                                   FdoDataType data_type,
                                                   FdoInt32    data_size,
                                                   FdoInt32    scale,
                                                   bool        is_nullable);

    //  CreateFdoClass:
    //      The function creates a predefined non-feature class of the given
    //      name.

    virtual FdoClass *CreateFdoClass (FdoString *class_name);

    //  CreateFdoFeatureClass:
    //      The function creates a predefined feature class of the given name.

    virtual FdoFeatureClass *CreateFdoFeatureClass (FdoString *class_name);

    //  CreateGeometricProperty:
    //      The function creates the named geometry property and returns it
    //      back to the calling routine.

    virtual FdoGeometricPropertyDefinition *CreateGeometricProperty (
                                                    FdoString *property_name);

    //  CreateTestDataStore:
    //      The function creates the test data store.

    virtual void CreateTestDataStore (FdoString *datastore_extension);

    //  DropTestDataStores:
    //      The function drops the test data store.

    virtual void DropTestDataStore (FdoString *datastore_extension);

    //  GetDimensionality:
    //      The function returns the dimensionality the provider supports.

    virtual FdoInt32 GetDimensionality ();   

    //  SetProvider:
    //      Sets the provider for the current test. Any Provider that supports
    //      these tests need to implement it.

    virtual void SetProvider ();   

    //  SetupUnitTestEnvironment:
    //      The function controls the establishing of the test environment in
    //      the FDO data store the provided connection identifies.

    virtual void SetupUnitTestEnvironment (FdoIConnection *current_connection);


    //-------------------------------------------------------------------------
    //                           Unit Test Functions
    //-------------------------------------------------------------------------

    //  RunAllExpFctTests:
    //      The function controls the execution of all unit tests. This way,
    //      the test setup is done only once.

    virtual void RunAllExpFctTests ();


    // ========================= AGGREGATE FUNCTIONS =========================

    //  TestAvgFunction:
    //      The function executes the test for the expression engine function
    //      AVG when used as a select-parameter.

    virtual void TestAvgFunction ();

    //  TestCountFunction:
    //      The function executes the test for the expression engine function
    //      COUNT when used as a select-parameter.

    virtual void TestCountFunction ();

    //  TestMaxFunction:
    //      The function executes the test for the expression engine function
    //      MAX when used as a select-parameter.

    virtual void TestMaxFunction ();

    //  TestMedianFunction:
    //      The function executes the test for the expression engine function
    //      MEDIAN when used as a select-parameter.

    virtual void TestMedianFunction ();

    //  TestMinFunction:
    //      The function executes the test for the expression engine function
    //      MIN when used as a select-parameter.

    virtual void TestMinFunction ();

    //  TestSpatialExtents:
    //      The function executes the test for the expression engine function
    //      SPATIALEXTENTS when used as a select-parameter.

    virtual void TestSpatialExtents ();

    //  TestStddevFunction:
    //      The function executes the test for the expression engine function
    //      STDDEV when used as a select-parameter.

    virtual void TestStddevFunction ();

    //  TestSumFunction:
    //      The function executes the test for the expression engine function
    //      SUM when used as a select-parameter.

    virtual void TestSumFunction ();


    // ======================== CONVERSION FUNCTIONS =========================

    //  TestNullValueFunction:
    //      The function executes the test for the expression engine function
    //      NULLVALUE when used as a select-parameter.

    virtual void TestNullValueFunction ();

    //  TestToDateFunction:
    //      The function executes the test for the expression engine function
    //      TODATE when used as a select-parameter.

    virtual void TestToDateFunction ();

    //  TestToDoubleFunction:
    //      The function executes the test for the expression engine function
    //      TODOUBLE when used as a select-parameter.

    virtual void TestToDoubleFunction ();

    //  TestToFloatFunction:
    //      The function executes the test for the expression engine function
    //      TOFLOAT when used as a select-parameter.

    virtual void TestToFloatFunction ();

    //  TestToInt32Function:
    //      The function executes the test for the expression engine function
    //      TOINT32 when used as a select-parameter.

    virtual void TestToInt32Function ();

    //  TestToInt64Function:
    //      The function executes the test for the expression engine function
    //      TOINT64 when used as a select-parameter.

    virtual void TestToInt64Function ();

    //  TestToStringFunction:
    //      The function executes the test for the expression engine function
    //      TOSTRING when used as a select-parameter.

    virtual void TestToStringFunction ();


    // =========================== DATE FUNCTIONS ============================

    //  TestAddMonthsFunction:
    //      The function executes the test for the expression engine function
    //      ADDMONTHS when used as a select-parameter.

    virtual void TestAddMonthsFunction ();

    //  TestCurrentDateFunction:
    //      The function executes the test for the expression engine function
    //      CURRENTDATE when used as a select-parameter.

    virtual void TestCurrentDateFunction ();

    //  TestExtractFunction:
    //      The function controls the execution of the tests for the expression
    //      engine function EXTRACT when used as a select-parameter.

    virtual void TestExtractFunction ();

    //  TestExtractToDoubleFunction:
    //      The function executes the test for the expression engine function
    //      EXTRACTTODOUBLE when used as a select-parameter.

    virtual void TestExtractToDoubleFunction ();

    //  TestExtractToIntFunction:
    //      The function executes the test for the expression engine function
    //      EXTRACTTOINT when used as a select-parameter.

    virtual void TestExtractToIntFunction ();

    //  TestMonthsBetweenFunction:
    //      The function executes the test for the expression engine function
    //      MONTHSBETWEEN when used as a select-parameter.

    virtual void TestMonthsBetweenFunction ();


    // ========================= GEOMETRY FUNCTIONS ==========================

    //  TestArea2DFunction:
    //      The function executes the test for the expression engine function
    //      AREA2D when used as a select-parameter.

    virtual void TestArea2DFunction ();

    //  TestLength2DFunction:
    //      The function executes the test for the expression engine function
    //      LENGTH2D when used as a select-parameter.

    virtual void TestLength2DFunction ();


    // =========================== MATH FUNCTIONS ============================

    //  TestAbsFunction:
    //      The function executes the test for the expression engine function
    //      ABS when used as a select-parameter.

    virtual void TestAbsFunction ();

    //  TestAcosFunction:
    //      The function executes the test for the expression engine function
    //      ACOS when used as a select-parameter.

    virtual void TestAcosFunction ();

    //  TestAsinFunction:
    //      The function executes the test for the expression engine function
    //      ASIN when used as a select-parameter.

    virtual void TestAsinFunction ();

    //  TestAtanFunction:
    //      The function executes the test for the expression engine function
    //      ATAN when used as a select-parameter.

    virtual void TestAtanFunction ();

    //  TestAtan2Function:
    //      The function executes the test for the expression engine function
    //      ATAN2 when used as a select-parameter.

    virtual void TestAtan2Function ();

    //  TestCosFunction:
    //      The function executes the test for the expression engine function
    //      COS when used as a select-parameter.

    virtual void TestCosFunction ();

    //  TestExpFunction:
    //      The function executes the test for the expression engine function
    //      EXP when used as a select-parameter.

    virtual void TestExpFunction ();

    //  TestLnFunction:
    //      The function executes the test for the expression engine function
    //      LN when used as a select-parameter.

    virtual void TestLnFunction ();

    //  TestLogFunction:
    //      The function executes the test for the expression engine function
    //      LOG when used as a select-parameter.

    virtual void TestLogFunction ();

    //  TestModFunction:
    //      The function executes the test for the expression engine function
    //      MOD when used as a select-parameter.

    virtual void TestModFunction ();

    //  TestPowerFunction:
    //      The function executes the test for the expression engine function
    //      POWER when used as a select-parameter.

    virtual void TestPowerFunction ();

    //  TestRemainderFunction:
    //      The function executes the test for the expression engine function
    //      REMAINDER when used as a select-parameter.

    virtual void TestRemainderFunction ();

    //  TestSinFunction:
    //      The function executes the test for the expression engine function
    //      SIN when used as a select-parameter.

    virtual void TestSinFunction ();

    //  TestSqrtFunction:
    //      The function executes the test for the expression engine function
    //      SQRT when used as a select-parameter.

    virtual void TestSqrtFunction ();

    //  TestTanFunction:
    //      The function executes the test for the expression engine function
    //      TAN when used as a select-parameter.

    virtual void TestTanFunction ();


    // ========================== NUMERIC FUNCTIONS ==========================

    //  TestCeilFunction:
    //      The function executes the test for the expression engine function
    //      CEIL when used as a select-parameter.

    virtual void TestCeilFunction ();

    //  TestFloorFunction:
    //      The function executes the test for the expression engine function
    //      FLOOR when used as a select-parameter.

    virtual void TestFloorFunction ();

    //  TestRoundFunction:
    //      The function executes the test for the expression engine function
    //      ROUND when used as a select-parameter.

    virtual void TestRoundFunction ();

    //  TestSignFunction:
    //      The function executes the test for the expression engine function
    //      SIGN when used as a select-parameter.

    virtual void TestSignFunction ();

    //  TestTruncFunction:
    //      The function executes the test for the expression engine function
    //      TRUNC when used as a select-parameter.

    virtual void TestTruncFunction ();

    //  TestTruncFunctionAsFilter:
    //      The function executes the test for the expression engine function
    //      TRUNC when used in a filter.

    virtual void TestTruncFunctionAsFilter ();


    // ========================== STRING FUNCTIONS ===========================

    //  TestConcatFunction:
    //      The function executes the test for the expression engine function
    //      CONCAT when used as a select-parameter.

    virtual void TestConcatFunction ();

    //  TestInstrFunction:
    //      The function executes the test for the expression engine function
    //      INSTR when used as a select-parameter.

    virtual void TestInstrFunction ();

    //  TestInstrFunctionAsFilter:
    //      The function executes the test for the expression engine function
    //      INSTR when used in a filter.

    virtual void TestInstrFunctionAsFilter ();

    //  TestLengthFunction:
    //      The function executes the test for the expression engine function
    //      LENGTH when used as a select-parameter.

    virtual void TestLengthFunction ();

    //  TestLowerFunction:
    //      The function executes the test for the expression engine function
    //      LOWER when used as a select-parameter.

    virtual void TestLowerFunction ();

    //  TestLpadFunction:
    //      The function executes the test for the expression engine function
    //      LPAD when used as a select-parameter.

    virtual void TestLpadFunction ();

    //  TestLtrimFunction:
    //      The function executes the test for the expression engine function
    //      LTRIM when used as a select-parameter.

    virtual void TestLtrimFunction ();

    //  TestRpadFunction:
    //      The function executes the test for the expression engine function
    //      RPAD when used as a select-parameter.

    virtual void TestRpadFunction ();

    //  TestRtrimFunction:
    //      The function executes the test for the expression engine function
    //      RTRIM when used as a select-parameter.

    virtual void TestRtrimFunction ();

    //  TestSoundexFunction:
    //      The function executes the test for the expression engine function
    //      SOUNDEX when used as a select-parameter.

    virtual void TestSoundexFunction ();

    //  TestSubstrFunction:
    //      The function executes the test for the expression engine function
    //      SUBSTR when used as a select-parameter.

    virtual void TestSubstrFunction ();

    //  TestTranslateFunction:
    //      The function executes the test for the expression engine function
    //      SUBSTR when used as a select-parameter.

    virtual void TestTranslateFunction ();

    //  TestTrimFunction:
    //      The function executes the test for the expression engine function
    //      TRIM when used as a select-parameter.

    virtual void TestTrimFunction ();

    //  TestUpperFunction:
    //      The function executes the test for the expression engine function
    //      UPPER when used as a select-parameter.

    virtual void TestUpperFunction ();


    //-------------------------------------------------------------------------
    //                      Unit Test Execution Functions
    //-------------------------------------------------------------------------

    //  ExecuteTestExtractFunction:
    //      The function executes the tests for the expression engine function
    //      EXTRACT when used as a select-parameter.

    virtual void ExecuteTestExtractFunction (FdoString *class_name);

    //  ExecuteTestExtractToDoubleFunction:
    //      The function executes the tests for the expression engine function
    //      EXTRACTTODOUBLE when used as a select-parameter.

    virtual void ExecuteTestExtractToDoubleFunction (FdoString *class_name);

    //  ExecuteTestExtractToIntFunction:
    //      The function executes the tests for the expression engine function
    //      EXTRACTTOINT when used as a select-parameter.

    virtual void ExecuteTestExtractToIntFunction (FdoString *class_name);


    //-------------------------------------------------------------------------
    //                         Special Test Functions
    //-------------------------------------------------------------------------

    //  RunDateInTest:
    //      The function executes a select statement using a filter that
    //      selects data based on matching dates defined with a IN clause.

    virtual void RunDateInTest ();

    //  RunTestsInPittsburghContext:
    //      This function executes tests in a given data store for which issues
    //      have been reported.

    virtual void RunTestsInPittsburghContext ();

    //  RunUpdate:
    //      ->> FILL

    virtual void RunUpdate ();


    //-------------------------------------------------------------------------
    //                       General Supporting Functions
    //-------------------------------------------------------------------------

    //  CheckReader:
    //      The function checks whether or not the provided reader contains
    //      the expected data and throws an exception if this is not the 
    //      case.

    virtual void CheckReader (FdoIDataReader *data_reader,
                              bool           include_id_check,
                              FdoInt32       expected_id_value,
                              FdoDouble      expected_value);

    virtual void CheckReader (FdoIFeatureReader *data_reader,
                              bool              include_id_check,
                              FdoInt32          expected_id_value,
                              FdoDouble         expected_value);

    //  CheckReader16:
    //      The function checks whether or not the provided reader contains
    //      the expected data and throws an exception if this is not the 
    //      case.

    void CheckReader16 (FdoIFeatureReader *data_reader,
                        bool              include_id_check,
                        FdoInt32          expected_id_value,
                        FdoInt16          expected_value);

    //  CheckReader32:
    //      The function checks whether or not the provided reader contains
    //      the expected data and throws an exception if this is not the 
    //      case.

    virtual void CheckReader32 (FdoIDataReader *data_reader,
                                bool           include_id_check,
                                FdoInt32       expected_id_value,
                                FdoInt32       expected_value);

    virtual void CheckReader32 (FdoIFeatureReader *data_reader,
                                bool              include_id_check,
                                FdoInt32          expected_id_value,
                                FdoInt32          expected_value);

    //  CheckReader64:
    //      The function checks whether or not the provided reader contains
    //      the expected data and throws an exception if this is not the 
    //      case.

    virtual void CheckReader64 (FdoIDataReader *data_reader,
                                bool           include_id_check,
                                FdoInt32       expected_id_value,
                                FdoInt64       expected_value);

    virtual void CheckReader64 (FdoIFeatureReader *data_reader,
                                bool              include_id_check,
                                FdoInt32          expected_id_value,
                                FdoInt64          expected_value);

    //  CheckReaderByte:
    //      The function checks whether or not the provided reader contains
    //      the expected data and throws an exception if this is not the 
    //      case.

    void CheckReaderByte (FdoIDataReader *data_reader,
                          bool           include_id_check,
                          FdoInt32       expected_id_value,
                          FdoByte        expected_value);

    void CheckReaderByte (FdoIFeatureReader *data_reader,
                          bool              include_id_check,
                          FdoInt32          expected_id_value,
                          FdoByte           expected_value);

    //  CheckReaderDt:
    //      The function checks whether or not the provided reader contains
    //      the expected data and throws an exception if this is not the 
    //      case.

    virtual void CheckReaderDt (FdoIDataReader *data_reader,
                                      bool           include_id_check,
                                      bool           is_extract_request,
                                      FdoInt16       extract_type_id,
                                      FdoInt32       expected_id_value,
                                      FdoDateTime    expected_cmp_id_value);

    virtual void CheckReaderDt (
                                FdoIFeatureReader *data_reader,
                                bool              include_id_check,
                                bool              is_extract_request,
                                FdoInt16          extract_type_id,
                                FdoInt32          expected_id_value,
                                FdoDateTime       expected_cmp_id_value);

    //  CheckReaderGeometry:
    //      The function checks whether or not the provided reader contains
    //      the expected data and throws an exception if this is not the 
    //      case.

    virtual void CheckReaderGeometry (FdoIDataReader *data_reader,
                                      FdoInt32       expected_data_count,
                                      FdoInt32       dimensionality,
                                      FdoDouble      expected_min_x,
                                      FdoDouble      expected_min_y,
                                      FdoDouble      expected_min_z,
                                      FdoDouble      expected_max_x,
                                      FdoDouble      expected_max_y,
                                      FdoDouble      expected_max_z);

    //  CheckReaderNumberString:
    //      The function checks whether or not the provided reader contains
    //      the expected data and throws an exception if this is not the 
    //      case.

    void CheckReaderNumberString (FdoIFeatureReader *data_reader,
                                  FdoInt32          expected_id_value,
                                  FdoString         *expected_cmp_id_value);

    //  CheckReaderSgl:
    //      The function checks whether or not the provided reader contains
    //      the expected data and throws an exception if this is not the 
    //      case.

    virtual void CheckReaderSgl (FdoIDataReader *data_reader,
                                 bool           include_id_check,
                                 FdoInt32       expected_id_value,
                                 FdoFloat       expected_value);

    virtual void CheckReaderSgl (FdoIFeatureReader *data_reader,
                                 bool              include_id_check,
                                 FdoInt32          expected_id_value,
                                 FdoFloat          expected_value);

    //  CheckReaderStddev:
    //      This function represents a special cross-check function for the
    //      expression function STDDEV. It is required because the behavior
    //      of the function for a special use-case (where the function is
    //      executed on a single row only) is different in SQL Server from
    //      the behavior in MySQL, Oracle or the standard implementation of
    //      the expression function in the Expression Engine.

    virtual void CheckReaderStddev (FdoIDataReader *data_reader,
                                    FdoDouble      expected_cmp_id_value);

    //  CheckReaderString:
    //      The function checks whether or not the provided reader contains
    //      the expected data and throws an exception if this is not the 
    //      case.

    virtual void CheckReaderString (
                                FdoIFeatureReader *data_reader,
                                FdoInt32          expected_id_value,
                                FdoString         *expected_cmp_id_value);

    //  ExecuteSelAggrCommand:
    //      The function executes a select-aggregate command using the pro-
    //      vided data and returns the generated reader back to the calling
    //      routine.

    virtual FdoIDataReader *ExecuteSelAggrCommand (FdoString *class_name,
                                                   FdoFilter *filter,
                                                   bool      inc_id_prop,
                                                   FdoString *cmp_id_str);

    //  ExecuteSelectCommand:
    //      The function executes a select-command using the provided data
    //      and returns the generated reader back to the calling routine.

    virtual FdoIFeatureReader *ExecuteSelectCommand (
                                                    FdoString *class_name,
                                                    FdoFilter *filter,
                                                    bool      inc_id_prop,
                                                    FdoString *cmp_id_str);

    //  ExecuteSelectCommand:
    //      The function is used to select the geometry information of objects
    //      of the named class. The returned reader contains the requested in-
    //      formation.

    virtual FdoIFeatureReader *ExecuteSelectCommand (
                                                    FdoString *class_name,
                                                    FdoFilter *filter,
                                                    FdoString *id_str);

    //  ExecuteSelectCommand:
    //      The function executes a select-command to retrieve the values of
    //      the named properties and returns the generated reader back to the
    //      calling routine.

    FdoIFeatureReader *ExecuteSelectCommand (FdoString *class_name,
                                             FdoFilter *filter,
                                             FdoString *prop1,
                                             FdoString *prop2,
                                             FdoString *prop3,
                                             FdoString *prop4,
                                             FdoString *prop5,
                                             FdoString *prop6);

    //  GetDate:
    //      The function retrieves the date set for a property identified by
    //      the provided filter and returns this one back to the calling pro-
    //      cedure.

    virtual FdoDateTime GetDate (FdoString *class_name,
                                 FdoString *property_name,
                                 FdoFilter *filter);

    //  GetExpectedErrorMessage:
    //      Unit tests also execute error case testing. The returned error
    //      message may differ depending of the underlying system. This
    //      function retrieves the expected error message for those cases.

    virtual FdoStringP GetExpectedErrorMessage (FdoInt16 test_case_id);

    //  GetExpectedValue:
    //      The function returns the expected value for a computed identifier
    //      in case the expression function request is treated differently in
    //      MySQL, Oracle, SQL Server and the standard implementation.

    virtual FdoDouble GetExpectedValue (FdoInt16 test_case_id);

    //  GetStringValue:
    //      The function retrieves the value for a string property identified
    //      by the provided filter and returns it back to the calling pro-
    //      cedure.

    virtual FdoStringP GetStringValue (FdoString *class_name,
                                       FdoString *property_name,
                                       FdoFilter *filter);

    //  IsExpectedErrorMessage:
    //      The function checks whether or not the two given error messages
    //      'match'. 

    virtual bool IsExpectedErrorMessage (FdoStringP expected_error_message,
                                         FdoStringP current_error_message,
                                         FdoInt16   test_case_id);


    //-------------------------------------------------------------------------
    //                                Variables
    //-------------------------------------------------------------------------

    //  m_connection:
    //      The variable holds the connection to the test data store.

    FdoPtr<FdoIConnection> m_connection;

};  //  class FdoExpressionFunctionTest

#endif
