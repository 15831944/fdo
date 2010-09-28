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

#include "Pch.h"
#include "OdbcBaseSetup.h"
#include "UnitTestUtil.h"

FdoPtr<FdoStringCollection> OdbcBaseSetup::OracleDataSupportCreated = FdoStringCollection::Create();


void OdbcBaseSetup::DestroyDataStore(FdoIConnection* pConnection, FdoString *suffix)
{
	FdoStringP pDatastore = UnitTestUtil::GetEnviron("datastore", suffix);

	FdoStringP pQueryStringDrp;
	if (DataBaseType_Oracle == m_typeDB)
	{
		pQueryStringDrp = L"DROP USER ";
		pQueryStringDrp += pDatastore;
		pQueryStringDrp = pQueryStringDrp.Upper();
		pQueryStringDrp += L" CASCADE";
	}
	else
	{
		pQueryStringDrp = L"DROP DATABASE ";
		pQueryStringDrp += pDatastore;
	}
	pQueryStringDrp += L";";
	try
	{
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
	}
	catch( FdoException *ex )
	{
		// In case of exception ignore it.
		ex->Release();
	}
}

void OdbcBaseSetup::CreateDataStore(FdoIConnection* pConnection, FdoString *suffix)
{
	if (DataBaseType_Oracle == m_typeDB && OdbcBaseSetup::OracleDataSupportCreated->IndexOf(suffix)!=-1)
		return;
	FdoStringP pDatastore = UnitTestUtil::GetEnviron("datastore", suffix);
	// clean the before start
	DestroyDataStore(pConnection, suffix);

	FdoStringP pQueryStringDrp;
	if (DataBaseType_Oracle == m_typeDB)
	{
        OdbcBaseSetup::OracleDataSupportCreated->Add(suffix);
		FdoStringP pUDatastore = pDatastore;
		pUDatastore = pUDatastore.Upper();
		pQueryStringDrp = (FdoStringP)L"CREATE USER \"" + pUDatastore + L"\"  PROFILE \"DEFAULT\" IDENTIFIED BY \"";
		pQueryStringDrp += UnitTestUtil::GetEnviron("password");
		pQueryStringDrp += L"\" DEFAULT TABLESPACE \"USERS\" TEMPORARY TABLESPACE \"TEMP\" ACCOUNT UNLOCK;";
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT ALTER SESSION TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT CREATE PROCEDURE TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT CREATE SEQUENCE TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT CREATE TRIGGER TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT CREATE SESSION TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT CREATE SYNONYM TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT CREATE TABLE TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT CREATE VIEW TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT UNLIMITED TABLESPACE TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT \"WM_ADMIN_ROLE\" TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT DELETE ON \"MDSYS\".\"SDO_GEOM_METADATA_TABLE\" TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT INSERT ON \"MDSYS\".\"SDO_GEOM_METADATA_TABLE\" TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT SELECT ON \"MDSYS\".\"SDO_GEOM_METADATA_TABLE\" TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
		pQueryStringDrp = (FdoStringP)L"GRANT UPDATE ON \"MDSYS\".\"SDO_GEOM_METADATA_TABLE\" TO " + pUDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );

		pConnection->Close();
		pConnection->SetConnectionString ( UnitTestUtil::GetConnectionString(Connection_NoDatastore, suffix) );
		pConnection->Open();
		UnitTestUtil::Sql2Db( (const wchar_t**) mOracleAcadTestData, pConnection );
		UnitTestUtil::Sql2Db( (const wchar_t**) mOracleNonAcadTest, pConnection );
	}
	else
	{
		pQueryStringDrp = L"CREATE DATABASE ";
		pQueryStringDrp += pDatastore;
		UnitTestUtil::Sql2Db( (const wchar_t*) pQueryStringDrp, pConnection );
	}
	
	pConnection->Close();
	pConnection->SetConnectionString ( UnitTestUtil::GetConnectionString(Connection_WithDatastore, suffix) );
	pConnection->Open();
}

void OdbcBaseSetup::CreateAcadSchema(FdoIConnection* pConnection)
{
	if (DataBaseType_MySQL == m_typeDB)
		UnitTestUtil::Sql2Db( (const wchar_t**) mMySqlAcadTest, pConnection );
	else if (DataBaseType_SqlServer == m_typeDB)
		UnitTestUtil::Sql2Db( (const wchar_t**) mSqlServerAcadTest, pConnection );
	else if (DataBaseType_Sybase == m_typeDB)
		UnitTestUtil::Sql2Db( (const wchar_t**) mSybaseAcadTest, pConnection );
}

void OdbcBaseSetup::CreateNonAcadSchema(FdoIConnection* pConnection)
{
	if (DataBaseType_SqlServer == m_typeDB)
		UnitTestUtil::Sql2Db( (const wchar_t**) mSqlServerNonAcadTest, pConnection );
	else if (DataBaseType_Sybase == m_typeDB) // Sybase uses same syntax as SQL Server.
		UnitTestUtil::Sql2Db( (const wchar_t**) mSqlServerNonAcadTest, pConnection );
	else if (DataBaseType_MySQL == m_typeDB)
		UnitTestUtil::Sql2Db( (const wchar_t**) mMySqlNonAcadTest, pConnection );
}

const wchar_t* OdbcBaseSetup::mOracleAcadTestData[] = {
    L"CREATE TABLE ACDB3DPOLYLINE (",
    L"LAYER VARCHAR2(10), ",
    L"COLOR VARCHAR2(32), ",
    L"SEGCOUNT NUMBER(10), ",
    L"BOOLEAN NUMBER(1), ",
    L"BYTE NUMBER(3), ",
    L"DATETIME DATE, ",
    L"DECIMAL1 NUMBER(10, 2), ",
    L"DOUBLE NUMBER, ",
    L"INT16 NUMBER(5), ",
    L"INT32 NUMBER(10), ",
    L"INT64 NUMBER(20), ",
    L"SINGLE NUMBER, ",
    L"CLASSID NUMBER(20) NOT NULL, ",
    L"REVISIONNUMBER NUMBER NOT NULL,", 
    L"GEOMETRY MDSYS.SDO_GEOMETRY, ",
    L"FEATID NUMBER(20) NOT NULL, ",
	L"DELSTATUS NUMBER, ",
    L"LTLOCK VARCHAR2(100), ",
    L"CONSTRAINT PK_ACDB3DPOLYLINE PRIMARY KEY(FEATID));",
	L"",
	L"CREATE TABLE TESTCLASS (",
	L"AGE NUMBER(10) NOT NULL, ",
	L"FIRSTNAME VARCHAR2(32) NOT NULL,",
	L"LASTNAME VARCHAR2(32) NOT NULL, ",
	L"VERSION NUMBER, ",
	L"NEXTVER VARCHAR2(500), ",
	L"DELSTATUS NUMBER, ",
	L"LTLOCK VARCHAR2(100), ",
	L"CONSTRAINT PK_TESTCLASS PRIMARY KEY(VERSION, LASTNAME, FIRSTNAME));",
	L"",
	L"insert into TESTCLASS (AGE, FIRSTNAME, LASTNAME, VERSION, NEXTVER, DELSTATUS ) values (36, 'UserFN', 'UserLN', 0, -1, 10 );",
	L"",
    L"insert into ACDB3DPOLYLINE (LAYER, COLOR, SEGCOUNT, CLASSID, REVISIONNUMBER, FEATID, DELSTATUS ) values (200, 256, 7, 11, 0, 2, 10);",
	L"",
    L"insert into ACDB3DPOLYLINE (LAYER, COLOR, SEGCOUNT, CLASSID, REVISIONNUMBER, FEATID, DELSTATUS ) values (200, 256, 10, 11, 0, 8, 10);",
	L"",
	NULL
};

const wchar_t* OdbcBaseSetup::mMySqlAcadTest[] = {
	L"create table acdb3dpolyline (",
	L"layer varchar(10) default NULL,",
	L"color varchar(32) default NULL,",
	L"segcount int default NULL,",
	L"boolean tinyint default NULL,",
	L"byte tinyint unsigned default NULL,",
	L"datetime1 datetime default NULL,",
	L"decimal1 decimal default NULL,",
	L"double1 double default NULL,",
	L"int16 smallint default NULL,",
	L"int32 int default NULL,",
	L"int64 bigint default NULL,",
	L"single1 double default NULL,",
	L"classid bigint NOT NULL,",
	L"revisionnumber double NOT NULL,",
	L"geometry geometry default NULL,",
	L"featid bigint NOT NULL auto_increment,",
	L"PRIMARY KEY (featid)",
	L") ENGINE=MyISAM;",
    L"",
    L"insert into acdb3dpolyline (layer, color, double1, single1, segcount, classid, revisionnumber, featid ) values ('TestLayer1', '256', 1, 2, 7, 11, 0, 2);",
	L"",
    L"insert into acdb3dpolyline (layer, color, double1, single1, segcount, classid, revisionnumber, featid ) values ('TestLayer2', '156', 3, 4, 10, 11, 0, 8);",
	L"",
	L"create table testClass (",
	L"age int NOT NULL,",
	L"firstname varchar(32) NOT NULL,",
	L"lastname varchar(32) NOT NULL,",
	L"PRIMARY KEY (lastname,firstname)",
	L") ENGINE=MyISAM;",
    L"",
    NULL
};

const wchar_t* OdbcBaseSetup::mSqlServerAcadTest[] = {
	L"create table acdb3dpolyline (",
	L"layer varchar(10) NULL,",
	L"color varchar(32) NULL,",
	L"segcount int NULL,",
	L"boolean bit NULL,",
	L"byte tinyint NULL,",
	L"datetime1 datetime NULL,",
	L"decimal1 decimal NULL,",
	L"double1 real NULL,",
	L"int16 smallint NULL,",
	L"int32 int NULL,",
	L"int64 bigint NULL,",
	L"single1 real NULL,",
	L"classid bigint NOT NULL,",
	L"revisionnumber real NOT NULL,",
	L"geometry image NULL,",
	L"GEOMETRY_SI_1 varchar (255) NULL,",
	L"GEOMETRY_SI_2 varchar (255) NULL,",
	L"featid bigint IDENTITY NOT NULL",
	L");",
    L"",
    L"insert into acdb3dpolyline (layer, color, double1, single1, segcount, classid, revisionnumber ) values ('TestLayer1', '256', 1, 2, 7, 11, 0);",
    L"",
    L"insert into acdb3dpolyline (layer, color, double1, single1, segcount, classid, revisionnumber ) values ('TestLayer2', '156', 3, 4, 10, 12, 1);",
    L"",
    NULL
};

const wchar_t* OdbcBaseSetup::mSybaseAcadTest[] = {
	L"create table acdb3dpolyline (",
	L"layer varchar(10) NULL,",
	L"color varchar(32) NULL,",
	L"segcount int NULL,",
    /* Sybase BIT type must be non-NULL. */
    /* ODBC Provider cannot populate it though, due to string conversion. */
	// L"boolean bit NOT NULL,", 
	L"byte tinyint NULL,",
	L"datetime1 datetime NULL,",
	L"decimal1 decimal NULL,",
	L"double1 real NULL,",
	L"int16 smallint NULL,",
    /* Sybase 15.0 can use bigint, but Sybase 12.5.4 cannot. */
	L"int32 int NULL,",
	//L"int64 bigint NULL,",
	L"int64 int NULL,",
	L"single1 real NULL,",
    /* Sybase 15.0 can use bigint, but Sybase 12.5.4 cannot. */
	//L"classid bigint NOT NULL,",
	L"classid int NOT NULL,",
	L"revisionnumber real NOT NULL,",
	L"geometry image NULL,",
	L"GEOMETRY_SI_1 varchar (255) NULL,",
	L"GEOMETRY_SI_2 varchar (255) NULL,",
    /* Sybase 15.0 can use bigint, but Sybase 12.5.4 cannot. */
	//L"featid bigint IDENTITY NOT NULL,",
	L"featid int IDENTITY NOT NULL,",
    /* As a variation, one can substitute IDENTITY (implying an
     * auto-generated primary key) with an explicit a primary key
     * constraint (no auto-generation).
     */
    //L"constraint pk_acdb3dpolyline primary key (featid)",
	L");",
    L"",
    L"insert into acdb3dpolyline (layer, color, double1, single1, segcount, classid, revisionnumber ) values ('TestLayer1', '256', 0, 2, 7, 11, 0);",
    L"",
    L"insert into acdb3dpolyline (layer, color, double1, single1, segcount, classid, revisionnumber ) values ('TestLayer2', '156', 3, 4, 10, 12, 1);",
    L"",
    NULL
};

const wchar_t* OdbcBaseSetup::mSqlServerNonAcadTest[] = {
	L"create table cities (",
	L"cityid int NOT NULL,",
	L"name varchar(50) NULL,",
	L"city varchar(50) NULL,",
	L"cdate datetime NULL",
	L");",
    L"",
	L"insert into cities ",
	L"( cityid, name, city )",
	L" values ",
	L"( 1, 'Marin', 'San Rafael' );",
    L"",
	L"insert into cities ",
	L"( cityid, name, city )",
	L" values ",
	L"( 2, 'Boop', 'San Bebop' );",
    L"",
	L"create table table1 (",
	L"featid1 int NOT NULL,",
	L"name varchar(50) NULL,",
	L"x real NULL,",
	L"y real NULL",
	L");",
    L"",
	L"insert into table1 ",
	L"( featid1, name, x, y )",
	L" values ",
	L"( 1, 'MyName', 10, 15 );",
    L"",
	L"insert into table1 ",
	L"( featid1, name, x, y )",
	L" values ",
	L"( 2, 'His''Name', 20, 25 );",
    L"",
    L"create view view1 ( featid1, name, x, y ) as select featid1, name, x, y from table1;", 
    L"",
	L"create table table6 (",
	L"featid1 int NOT NULL,",
	L"name1 varchar(50) NULL,",
	L"x real NULL,",
	L"y numeric(11,3) NULL,",
	L"z float NULL",
	L");",
    L"",
	L"create table table7 (",
	L"featid1 int NOT NULL,",
	L"name1 varchar(50) NULL,",
	L"x tinyint NULL,",
	L"y smallint NULL,",
	L"z int NULL",
	L");",
    L"",
	L"create table table8 (",
	L"featid1 int NOT NULL,",
	L"name1 varchar(50) NULL,",
	L"x bigint NULL,",
	L"y float NULL,",
	L"z decimal(16,6)  NULL",
	L");",
    L"",
    NULL
};

const wchar_t* OdbcBaseSetup::mMySqlNonAcadTest[] = {
	L"create table cities (",
	L"cityid int NOT NULL,",
	L"name varchar(10) default NULL,",
	L"city varchar(32) default NULL,",
	L"cdate datetime default NULL,",
	L"PRIMARY KEY (cityid)",
	L") ENGINE=MyISAM;",
    L"",
	L"insert into cities ",
	L"( cityid, name, city )",
	L" values ",
	L"( 1, 'Marin', 'San Rafael' );",
    L"",
	L"insert into cities ",
	L"( cityid, name, city )",
	L" values ",
	L"( 2, 'Boop', 'San Bebop' );",
    L"",
	L"create table table1 (",
	L"featid1 int NOT NULL,",
	L"name varchar(50) default NULL,",
	L"x double NULL,",
	L"y double NULL,",
	L"PRIMARY KEY (featid1)",
	L");",
    L"",
	L"insert into table1 ",
	L"( featid1, name, x, y )",
	L" values ",
	L"( 1, 'MyName', 10, 15 );",
    L"",
	L"insert into table1 ",
	L"( featid1, name, x, y )",
	L" values ",
	L"( 2, 'His''Name', 20, 25 );",
    L"",
    L"create view view1 ( featid1, name, x, y ) as select featid1, name, x, y from table1;", 
    L"",
	L"create table table6 (",
	L"featid1 int NOT NULL,",
	L"name1 varchar(50) NULL,",
	L"x float NULL,",
	L"y decimal(11,3) NULL,",
	L"z double NULL",
	L");",
    L"",
	L"create table table7 (",
	L"featid1 int NOT NULL,",
	L"name1 varchar(50) NULL,",
	L"x tinyint NULL,",
	L"y smallint NULL,",
	L"z int NULL",
	L");",
    L"",
	L"create table table8 (",
	L"featid1 int NOT NULL,",
	L"name1 varchar(50) NULL,",
	L"x bigint NULL,",
	L"y double NULL,",
	L"z decimal(16,6)  NULL",
	L");",
    L"",
    NULL
};

const wchar_t* OdbcBaseSetup::mOracleNonAcadTest[] = {
	L"create table CITIES (",
	L"CITYID NUMBER(10) primary key,",
	L"NAME VARCHAR(50),",
	L"CITY VARCHAR(50),",
	L"CDATE date",
	L");",
    L"",
	L"insert into CITIES ",
	L"( CITYID, NAME, CITY )",
	L" values ",
	L"( 1, 'Marin', 'San Rafael' );",
    L"",
	L"insert into CITIES ",
	L"( CITYID, NAME, CITY )",
	L" values ",
	L"( 2, 'Boop', 'San Bebop' );",
    L"",
	L"create table TABLE1 (",
	L"FEATID1 NUMBER(10) primary key,",
	L"NAME VARCHAR(50),",
	L"X NUMBER,",
	L"Y NUMBER",
	L");",
    L"",
	L"insert into TABLE1 ",
	L"( FEATID1, NAME, X, Y )",
	L" values ",
	L"( 1, 'MyName', 10, 15 );",
    L"",
	L"insert into TABLE1 ",
	L"( FEATID1, NAME, X, Y )",
	L" values ",
	L"( 2, 'His''Name', 20, 25 );",
    L"",
	L"create view VIEW1 as select FEATID1, NAME, X, Y from TABLE1;",
    L"",
	L"create table TABLE6 (",
	L"FEATID1 NUMBER(10) primary key,",
	L"NAME1 VARCHAR(50),",
	L"X BINARY_FLOAT,",
	L"Y NUMBER(11,3),",
	L"Z NUMBER",
	L");",
    L"",
	L"create table TABLE7 (",
	L"FEATID1 NUMBER(10) primary key,",
	L"NAME1 VARCHAR(50),",
	L"X NUMBER(3,0),",
	L"Y SMALLINT,",
	L"Z INTEGER",
	L");",
    L"",
	L"create table TABLE8 (",
	L"FEATID1 NUMBER(10) primary key,",
	L"NAME1 VARCHAR(50),",
	L"X NUMBER(20,0),",
	L"Y REAL,",
	L"Z DECIMAL(16,6)",
	L");",
    L"",
	L"create table ALLDBTYPES (",
	L"DBTYPE_VARCHAR2 VARCHAR2(10),",
	L"DBTYPE_NUMBER NUMBER,",
    L"DBTYPE_NUMBER_20_0 NUMBER(20,0),",
    L"DBTYPE_NUMBER_27_5 NUMBER(27,5),",
    L"DBTYPE_INTEGER INTEGER,",
    L"DBTYPE_SMALLINT SMALLINT,",
    L"DBTYPE_DECIMAL DECIMAL,",
    L"DBTYPE_DECIMAL_26 DECIMAL(26),",
    L"DBTYPE_NUMERIC NUMERIC,",
    L"DBTYPE_NUMERIC_25_3 NUMERIC(25,3),",
    L"DBTYPE_FLOAT FLOAT,",
    L"DBTYPE_REAL REAL,",
    L"DBTYPE_VARCHAR VARCHAR(10),",
    L"DBTYPE_DATE DATE,",
    L"DBTYPE_CHAR CHAR,",
    L"DBTYPE_TIMESTAMP TIMESTAMP,",
    L"DBTYPE_RAW10 RAW(10),",
    L"DBTYPE_LONGRAW LONG RAW,",
    L"DBTYPE_TIMESTAMP_TZ TIMESTAMP WITH TIME ZONE,",
    L"DBTYPE_INTERVAL_YM INTERVAL YEAR TO MONTH,",
    L"DBTYPE_INTERVAL_DS INTERVAL DAY TO SECOND,",
    L"DBTYPE_TIMESTAMP_LTZ TIMESTAMP WITH LOCAL TIME ZONE",
	L");",
    L"",
	L"create table ALLDBTYPES2 (",
	L"DBTYPE_VARCHAR2 VARCHAR2(10),",
    L"DBTYPE_LONG LONG",
	L");",
	L"create table ALLDBTYPES3 (",
	L"DBTYPE_VARCHAR2 VARCHAR2(10),",
    L"DBTYPE_LONGVARCHAR LONG VARCHAR",
	L");",
    L"",
    NULL
};