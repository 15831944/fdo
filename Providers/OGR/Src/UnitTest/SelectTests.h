#ifndef SELECT_TESTS_H
#define SELECT_TESTS_H

#include <cppunit/extensions/HelperMacros.h>

class SelectTests : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(SelectTests);
    CPPUNIT_TEST(TestCase_SelectAll);
    CPPUNIT_TEST(TestCase_SelectAllExplicitPropertyList);
    CPPUNIT_TEST(TestCase_SelectAllComputedProperties);
    CPPUNIT_TEST(TestCase_SelectAggregateCount);
    CPPUNIT_TEST(TestCase_SelectAggregateDistinct);
    CPPUNIT_TEST(TestCase_SelectAggregateSpatialExtent);
    CPPUNIT_TEST(TestCase_SelectMixedAttributeAndSpatialFilter);
    CPPUNIT_TEST(TestCase_EvalQuotedIdentifier);
    CPPUNIT_TEST_SUITE_END();

public:
    SelectTests();
    ~SelectTests();

    void TestCase_SelectAll();
    void TestCase_SelectAllExplicitPropertyList();
    void TestCase_SelectAllComputedProperties();
    void TestCase_SelectAggregateCount();
    void TestCase_SelectAggregateDistinct();
    void TestCase_SelectAggregateSpatialExtent();
    void TestCase_SelectMixedAttributeAndSpatialFilter();

    // Test case for: https://trac.osgeo.org/fdo/ticket/946
    // NOTE: Nothing wrong with the OGR provider, the test relates to the expression engine
    // and there's no existing standalone expression engine test cases, they are always tied
    // to any FDO provider test suite, so this suite was chosen as the home
    void TestCase_EvalQuotedIdentifier();
};

#endif //SELECT_TESTS_H