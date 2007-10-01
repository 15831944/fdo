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

#ifndef CPP_UNIT_ArcSDETests_H
#define CPP_UNIT_ArcSDETests_H

#ifdef _WIN32
#pragma once
#endif // _WIN32

// Updates the given name based on whether the unit tests are being run in
// release mode or debug mode.  This is necessary following recent performance
// changes which cause the provider to behave differently if run in
// debug vs release mode, namely that FDO metadata that impacts FDO class/property
// names is only retrieved in debug mode since ApplySchema is only supported in debug mode.
static FdoString* AdjustRdbmsName(FdoString* name)
{
    static int bufferIndex=0;
    static wchar_t buffer[10][1000];
    bufferIndex = (bufferIndex+1) % 10;
    wcscpy(buffer[bufferIndex], name);
#ifdef _DEBUG
    return buffer[bufferIndex];
#else
    return FdoCommonOSUtil::wcsupr(buffer[bufferIndex]);
#endif
}


// Represents one FDO data property with a corresponding data value
class UnitTestData
{
public:
    FdoStringP  mPropertyName;
    FdoStringP  mPropertyDescription;
    FdoDataType mPropertyType;
    int         mPropertyLength;
    int         mPropertyPrecision;
    int         mPropertyScale;
    bool        mPropertyNullable;
    bool        mPropertyReadOnly;
    bool        mPropertyAutoGenerated;
    std::vector<const wchar_t*> mPropertyData;

    UnitTestData (
        FdoString*  property_name,
        FdoString*  property_description,
        FdoDataType property_type,
        int         property_length,
        int         property_precision,
        int         property_scale,
        bool        property_nullable,
        bool        property_readonly,
        bool        property_auto,
        ...) :
        mPropertyName (property_name),
        mPropertyDescription (property_description),
        mPropertyType (property_type),
        mPropertyLength (property_length),
        mPropertyPrecision (property_precision),
        mPropertyScale (property_scale),
        mPropertyNullable (property_nullable),
        mPropertyReadOnly (property_readonly),
        mPropertyAutoGenerated (property_auto)
    {
        const wchar_t* data;
        va_list varArgs;
        mPropertyName = AdjustRdbmsName(mPropertyName);

        // Handle random list of property data:
        va_start (varArgs, property_auto);
        while (NULL != (data = va_arg (varArgs, const wchar_t*)))
            mPropertyData.push_back (data);
        va_end (varArgs);
    }
};

// Represents one FDO data property with a corresponding data value
class UnitTestClass
{
public:
    FdoStringP mName;
    FdoStringP mDescription;
    int mIdIndex;
    bool mSupportsLocking;
    bool mSupportsLT;
    std::vector<UnitTestData*> mProperties;

    UnitTestClass (
        FdoString*  name,
        FdoString*  description,
        int         id_index,
        bool        bSupportsLocking,
        bool        bSupportsLT,
        ...) :
        mName (name),
        mDescription (description),
        mIdIndex  (id_index),
        mSupportsLocking(bSupportsLocking),
        mSupportsLT(bSupportsLT)
    {
        UnitTestData* data;
        va_list varArgs;

        // Handle random list of properties:
        va_start (varArgs, bSupportsLT);
        while (NULL != (data = va_arg (varArgs, UnitTestData*)))
            mProperties.push_back (data);
        va_end (varArgs);
    }
};


// This class is the base class for all ArcSDE Provider unit tests:
class ArcSDETests :
    public CppUnit::TestCase
{
public:
    ArcSDETests(void);
    virtual ~ArcSDETests(void);

    static FdoIConnection* GetConnection ();

    static void fail (FdoException* ge);

	// deletes given class from given schema, if it exists; otherwise, does nothing:
	void CleanUpClass(FdoIConnection *connection, const wchar_t* schema_name, const wchar_t* class_name, bool bDeleteRowsOnly = false);

	// returns a handle to the default schema; this is for use when provider supports a single schema only:
	static FdoFeatureSchema* GetDefaultSchema(FdoIConnection* connection);

	// Converts the given FdoDataType to an equivalent string representation:
	static wchar_t* GetDataTypeString (FdoDataType type);

    // Parses the given Fdo string into an FdoExpression, making sure to validate and convert to the given type:
    static FdoExpression* ParseByDataType(const wchar_t* data, FdoDataType dataType);

    // Check that the given property in the given reader matches the given value:
    void checkEqual(FdoPtr<FdoIFeatureReader> reader, FdoString* propertyName, FdoDataType propertyType, FdoString* propertyData);

    // add the given class to the default schema
    static void CreateSchema (FdoIConnection* connection, UnitTestClass* cls);

    // delete the given class from the default schema
    static void DeleteSchema (FdoIConnection* connection, UnitTestClass* cls);

    // Apply the given capabilities to the given class:
    static void SetClassCapabilities(FdoClassDefinition *cls, bool bSupportLocking, bool bSupportLongTransactions);

    // insert an object with data given by index from the definition collection
    static int Insert (FdoIConnection* connection, UnitTestClass* definition, size_t index);

    // update the objects in the map (pairs of property index values & id numbers)
    static void Update (FdoIConnection* connection, UnitTestClass* definition, std::vector<int*> *map);

    // delete an object given by the id number
    static void Delete (FdoIConnection* connection, UnitTestClass* definition, int id);

    // check for the objects in the map (pairs of property index values & id numbers) by doing a select
    static void Expect (FdoIConnection* connection, UnitTestClass* definition, std::vector<int*> *map);

    // check that 2 doubles are roughly equal:
    static bool fuzzyEqual (const double d1, const double d2);

    // Accesses all the data on the current feature.
    void ProcessFeature (FdoIFeatureReader* featureReader, FdoFeatureClass* classDef=NULL, FdoIdentifierCollection* IDs = NULL, bool bVerifyPropertyPresence = true);

    // Activate the given spatial context; don't report error if spatial context not found:
    void ActivateSpatialContext(FdoIConnection* connection, FdoString* scName);

    // Compare two geometries for equivalence:
    bool GeometriesEquivalent(FdoIGeometry *geom1, FdoIGeometry *geom2);
    bool LinearRingsEquivalent(FdoILinearRing* lr1, FdoILinearRing* lr2);
    bool PointsEquivalent(FdoIDirectPosition* pos1, FdoIDirectPosition* pos2);

    // Return true IFF each unit test should only be creating schema (not running any actual tests):
    bool CreateSchemaOnly(void);

    // Get/set extents of spatial context:
    void set_extent(FdoICreateSpatialContext *cmdCreateSpatialContext, double minx, double miny, double maxx, double maxy);
    void get_extent(FdoISpatialContextReader *spatialContextReader, double &minx, double &miny, double &maxx, double &maxy);

    // Create or update a spatial context:
    void CreateOrUpdateSpatialContext(FdoIConnection *conn, FdoString* scName, double dXMin, double dYMin, double dXMax, double dYMax, FdoString* scCoordSys);

    // Return the number of locks a given user has on a given table:
    int GetLockCount(FdoIConnection *conn, FdoString* userName, FdoString* qualified_class_name);

    // Both print to console output and throw an exception with the same message:
    static void PrintAndThrow(FdoString* message);
};



// This class handles all data that can vary based on environment variables or based on which rdbms is being accessed:
class ArcSDETestConfig
{
private:
    enum ArcSDETestRdbmsType
    {
        ArcSDETestRdbmsType_Oracle,
        ArcSDETestRdbmsType_SqlServer,
        ArcSDETestRdbmsType_Unknown
    };

public:
    static ArcSDETestRdbmsType RdbmsType(void)
    {
        FdoStringP val = getenv("rdbms");
        if (0==val.ICompare(L"oracle"))
            return ArcSDETestRdbmsType_Oracle;
        else if (0==val.ICompare(L"sqlserver"))
            return ArcSDETestRdbmsType_SqlServer;
        else
            ArcSDETests::PrintAndThrow(L"Missing or incorrect rdbms=<oracle|sqlserver> command-line argument");
        return ArcSDETestRdbmsType_Unknown; // Fix Compiler Warning
    }

    static bool SupportsLocking(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return true;
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return false;
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static bool SupportsNamedDatabases(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return false;
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return true;
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static bool SupportsTransactionIsolation(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return true;
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return false; // NOTE: SQL Server has known issue with different connections 'seeing' into eachother's short transactions
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static bool SupportsUserManagedStringIds(void)
    {
        FdoStringP val = getenv("supportsusermanagedstringids");
        return 0==val.ICompare(L"yes");
    }

    static FdoStringP Server(void)
    {
        FdoStringP val = getenv("server");
        if (val.GetLength()>0)
            return val;
        else
            ArcSDETests::PrintAndThrow(L"Missing server=<server address> command-line argument");
        return FdoStringP(""); // Fix Compiler Warning
    }

    static FdoStringP Instance(void)
    {
        FdoStringP val = getenv("port_multi");
        return (val.GetLength()>0) ? val : L"5151/tcp";
    }

    static FdoStringP InstanceSingleDb(void)
    {
        FdoStringP val = getenv("port_single");
        return (val.GetLength()>0) ? val : L"5151/tcp";
    }

    static FdoStringP Datastore(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return L"Default Datastore";
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return L"sde";
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static FdoStringP DatastoreSingleDb(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return L"Default Datastore";
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return L"testsingledb";
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static FdoStringP DatastoreMultiDb(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return L"Default Datastore";
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return L"testmultidb";
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static FdoStringP SdeUsername(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return L"SDE";
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return L"sde";
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static FdoStringP SdePassword(void)
    {
        FdoStringP val = getenv("sdepwd");
        return (val.GetLength()>0) ? val : L"sde";
    }


    static FdoStringP SqlStmtCreateTable(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return L"create table bar (id NUMBER(4,0), count NUMBER(10,0), scale NUMBER(6,2), length NUMBER(15,4), description VARCHAR2(255), data LONG RAW, modified DATE)";
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return L"create table BAR (ID smallint NULL, COUNT int NULL, SCALE numeric(6,2) NULL, LENGTH numeric(15,4) NULL, DESCRIPTION varchar(255) NULL, DATA image NULL, MODIFIED datetime NULL)";
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }


    static FdoStringP SqlStmtInsert1(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return L"insert into bar values (42, 8272772, 1e-2, 10280288.2993, 'the quick brown fox jumps over a lazy dog', '252f82e3', sysdate)";
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return L"insert into BAR values (42, 8272772, 1e-2, 10280288.2993, 'the quick brown fox jumps over a lazy dog', 0x252f82e3, GETDATE())";
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static FdoStringP SqlStmtInsert2(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return L"insert into bar values (43, -37834, 5.53, -10280288.5, 'Arrr, matey', '252f82e3', TO_DATE('Jan 09 1998', 'MON DD YYYY'))";
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return L"insert into BAR values (43, -37834, 5.53, -10280288.5, 'Arrr, matey', 0x252f82e3, 'Jan 09 1998')";
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static FdoStringP SqlCountStarColumnName(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return L"COUNT(*)";
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return L"";
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static FdoStringP SqlStmtAggrQuery1(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return L"select ABS(SUM(length)) from bar where ABS(TRUNC(length))=10280288";
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return L"select ABS(SUM(length)) from bar where ABS(CEILING(length))<11111111";
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static double SqlStmtAggrQuery1Result(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return 0.2007;
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return 0.2007;
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static FdoStringP SqlAggrColumnName(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return L"ABS(SUM(length))";
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return L"";
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static FdoStringP FdoSchemaPrefixSde(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return L"";
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return L"SDE_";
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static FdoStringP FdoSchemaPrefixTestMultiDb(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return L"";
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return L"TESTMULTIDB_";
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }

    static FdoStringP FdoSchemaPrefixTestSingleDb(void)
    {
        if (RdbmsType()==ArcSDETestRdbmsType_Oracle)
            return L"";
        else if (RdbmsType()==ArcSDETestRdbmsType_SqlServer)
            return L"TESTSINGLEDB_";
        else
            throw FdoException::Create(L"Unhandled RDBMS type");
    }


    // Fdo user names (server/database-agnostic):
    ///////////////////////////////////////////////////////////////////////////

    static FdoStringP DatasetName(void)
    {
        FdoStringP val = getenv("dataset");
        if (val.GetLength()==0)
            ArcSDETests::PrintAndThrow(L"Missing dataset=<dataset name> command-line argument");
        return val;
    }

    static FdoStringP UserNameMetadcov(void)
    {
        return FdoStringP::Format(L"FDO_%ls_METADCOV", (FdoString*)DatasetName());
    }

    static FdoStringP UserNameAustralia(void)
    {
        return FdoStringP::Format(L"FDO_%ls_AUSTRALIA", (FdoString*)DatasetName());
    }

    static FdoStringP UserNameWheaton(void)
    {
        return FdoStringP::Format(L"FDO_%ls_WHEATON", (FdoString*)DatasetName());
    }

    static FdoStringP UserNameOzzie(void)
    {
        return FdoStringP::Format(L"FDO_%ls_OZZIE", (FdoString*)DatasetName());
    }

    static FdoStringP UserNameFdoTest1(void)
    {
        return FdoStringP::Format(L"FDO_%ls_FDOTEST1", (FdoString*)DatasetName());
    }

    static FdoStringP UserNameReadOnly(void)
    {
        return FdoStringP::Format(L"FDO_%ls_READONLY", (FdoString*)DatasetName());
    }


    // Fdo Connection strings (server/database-agnostic):
    ///////////////////////////////////////////////////////////////////////////

    /*  // TODO: direct connect not officially supported (should still work however)
    static FdoStringP ConnStringDirectConnect(void)
    {
        wchar_t buff[500];
        FdoCommonOSUtil::swprintf(buff, ELEMENTS(buff), L"Server=%ls;Instance=%ls;Username=%ls;Password=%ls;Datastore=%ls", DIRECTCONNECT_SERVER, DIRECTCONNECT_INSTANCE, (FdoString*)SdeUsername(), DIRECTCONNECT_PASSWORD, (FdoString*)Datastore());
        return buff; 
    }
    */

    static FdoStringP ConnStringSdeNodatastore(void)
    {
        return FdoStringP::Format(L"Server=%ls;Instance=%ls;Username=%ls;Password=%ls;", (FdoString*)Server(), (FdoString*)Instance(), (FdoString*)SdeUsername(), (FdoString*)SdePassword());
    }

    static FdoStringP ConnStringSdeWithDatastore(void)
    {
        return FdoStringP::Format(L"Server=%ls;Instance=%ls;Username=%ls;Password=%ls;Datastore=%ls;",  (FdoString*)Server(), (FdoString*)Instance(), (FdoString*)SdeUsername(), (FdoString*)SdePassword(), (FdoString*)Datastore());
    }

    static FdoStringP ConnStringAustralia(void)
    {
        return FdoStringP::Format(L"Server=%ls;Instance=%ls;Datastore=%ls;Username=%ls;Password=test;", (FdoString*)Server(), (FdoString*)Instance(), (FdoString*)Datastore(), (FdoString*)UserNameAustralia());
    }

    static FdoStringP ConnStringWheaton(void)
    {
        return FdoStringP::Format(L"Server=%ls;Instance=%ls;Datastore=%ls;Username=%ls;Password=test;", (FdoString*)Server(), (FdoString*)Instance(), (FdoString*)Datastore(), (FdoString*)UserNameWheaton());
    }

    static FdoStringP ConnStringOzzie(void)
    {
        return FdoStringP::Format(L"Server=%ls;Instance=%ls;Datastore=%ls;Username=%ls;Password=test;", (FdoString*)Server(), (FdoString*)Instance(), (FdoString*)Datastore(), (FdoString*)UserNameOzzie());
    }

    static FdoStringP ConnStringMetadcov(void)
    {
        return FdoStringP::Format(L"Server=%ls;Instance=%ls;Datastore=%ls;Username=%ls;Password=test;", (FdoString*)Server(), (FdoString*)Instance(), (FdoString*)Datastore(), (FdoString*)UserNameMetadcov());
    }

    static FdoStringP ConnStringFdoTest1(void)
    {
        return FdoStringP::Format(L"Server=%ls;Instance=%ls;Datastore=%ls;Username=%ls;Password=test;", (FdoString*)Server(), (FdoString*)Instance(), (FdoString*)Datastore(), (FdoString*)UserNameFdoTest1());
    }

    static FdoStringP ConnStringMetadcovSingleDb(void)
    {
        return FdoStringP::Format(L"Server=%ls;Instance=%ls;Datastore=%ls;Username=%ls;Password=test;", (FdoString*)Server(), (FdoString*)InstanceSingleDb(), (FdoString*)DatastoreSingleDb(), (FdoString*)UserNameMetadcov());
    }

    static FdoStringP ConnStringAustraliaMultiDb(void)
    {
        return FdoStringP::Format(L"Server=%ls;Instance=%ls;Datastore=%ls;Username=%ls;Password=test;", (FdoString*)Server(), (FdoString*)Instance(), (FdoString*)DatastoreMultiDb(), (FdoString*)UserNameAustralia());
    }

    static FdoStringP ConnStringReadOnly(void)
    {
        return FdoStringP::Format(L"Server=%ls;Instance=%ls;Datastore=%ls;Username=%ls;Password=test;", (FdoString*)Server(), (FdoString*)Instance(), (FdoString*)DatastoreMultiDb(), (FdoString*)UserNameReadOnly());
    }

    // Fdo Class and schema names (server/database-agnostic):
    ///////////////////////////////////////////////////////////////////////////

#define DECLARE_CLASS(DBMETHODNAME, USERMETHODNAME, CLASSMETHODNAME, REALCLASSNAME) \
static FdoStringP ClassSchema##CLASSMETHODNAME() { return FdoStringP::Format(L"%ls%ls", (FdoString*)FdoSchemaPrefix##DBMETHODNAME(), (FdoString*)UserName##USERMETHODNAME()); } \
static FdoStringP ClassName##CLASSMETHODNAME()  { return AdjustRdbmsName(REALCLASSNAME); } \
static FdoStringP QClassName##CLASSMETHODNAME() { return FdoStringP::Format(L"%ls:%ls", (FdoString*)ClassSchema##CLASSMETHODNAME(), (FdoString*)ClassName##CLASSMETHODNAME()); }

    DECLARE_CLASS(Sde, Metadcov, TestClassFunctions, L"");
    DECLARE_CLASS(Sde, Metadcov, TestClassSimple, L"TESTA");
    DECLARE_CLASS(Sde, Metadcov, Trees, L"TREES");
    DECLARE_CLASS(Sde, Metadcov, TestClassNew, L"TestClassNew");
    DECLARE_CLASS(Sde, Metadcov, TestClassGeomZm3, L"TestClassGeomZM3");
    DECLARE_CLASS(Sde, Metadcov, TestUserManagedId, L"TestUserManagedId");
    DECLARE_CLASS(Sde, Metadcov, TestClassNullable, L"TestClassNullable");
    DECLARE_CLASS(Sde, Metadcov, Sample, L"Sample");
    DECLARE_CLASS(Sde, Metadcov, SampleNonLocking, L"SampleNonLocking");
    DECLARE_CLASS(Sde, Metadcov, SampleWithGeom, L"SampleWithGeom");
    DECLARE_CLASS(Sde, Metadcov, City, L"CITY");
    DECLARE_CLASS(Sde, Metadcov, Beach, L"BEACH");
    DECLARE_CLASS(Sde, Metadcov, Address, L"Address");
    DECLARE_CLASS(Sde, Metadcov, Address2, L"Address2");
    DECLARE_CLASS(Sde, Metadcov, AddressWayTooLong, L"AddressWayWayWayWayWayWayWayWayWayWayWayWayTooLong"); // 51 chars long
    DECLARE_CLASS(Sde, Metadcov, Example, L"Example");
    DECLARE_CLASS(Sde, Australia, Soils, L"SOILS");
    DECLARE_CLASS(Sde, Australia, OntarioRoads, L"ROADS");
    DECLARE_CLASS(Sde, Australia, Parcels, L"PARCELS");
    DECLARE_CLASS(Sde, Australia, ScTestCls1s, L"SCTestCls1s");
    DECLARE_CLASS(Sde, Australia, Defect64335202, L"DEFECT64335202");
    DECLARE_CLASS(Sde, Australia, Lumpy, L"Lumpy");
    DECLARE_CLASS(Sde, Wheaton, Geodesy, L"Geodesy");
    DECLARE_CLASS(Sde, Wheaton, RowIdTest, L"rowidtest");
    DECLARE_CLASS(Sde, FdoTest1, Roads, L"Roads");
    DECLARE_CLASS(TestSingleDb, Metadcov, TestClassComplex, L"TESTB");
    DECLARE_CLASS(TestMultiDb, Metadcov, TestClassSimpleMultiDb, L"TESTA");
    DECLARE_CLASS(Sde, Australia, LargeWithGeom, L"LARGEWITHGEOM");

    static FdoStringP ClassSchemaTreesUniqueName() { return FdoStringP::Format(L"%ls%ls", (FdoString*)FdoSchemaPrefixSde(), (FdoString*)UserNameMetadcov()); } \
    static FdoStringP ClassNameTreesUniqueName()   { return AdjustRdbmsName((FdoString*)FdoStringP::Format(L"TREES_%ls", (FdoString*)DatasetName())); } \
    static FdoStringP QClassNameTreesUniqueName()  { return FdoStringP::Format(L"%ls:%ls", (FdoString*)ClassSchemaTreesUniqueName(), (FdoString*)ClassNameTreesUniqueName()); }
};

#endif // CPP_UNIT_ArcSDETests_H
