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
#include "PostGisDescribeSchemaTest.h"
#include "UnitTestUtil.h"

CPPUNIT_TEST_SUITE_REGISTRATION (PostGisDescribeSchemaTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( PostGisDescribeSchemaTest, "DescribeSchemaTest");

void PostGisDescribeSchemaTest::set_provider()
{
    mInputData = mData;
    UnitTestUtil::SetProvider( "PostGIS" );
}

FdoInt32 PostGisDescribeSchemaTest::GetLockTypeCount()
{
    return 0;
}

const wchar_t* PostGisDescribeSchemaTest::mDataPostGis[] = {
    L"",
    L"delete from f_schemainfo where schemaname = 'Acad';",
    L"",
    L"insert into f_schemainfo ( schemaname, description, creationdate, owner, ",
    L"schemaversionid ) values ( 'Acad', null, '03-10-07', 'br', 1 );                          ",
    L"                                                                                ",
    L"insert into f_schemainfo ( schemaname, description, creationdate, owner, ",
    L"schemaversionid ) values ( 'Electric', 'Electric Utility Model', '03-10-07', 'br', 1 );   ",
    L"                                                                                                                                                                ",
    L"insert into f_schemainfo ( schemaname, description, creationdate, owner, ",
    L"schemaversionid ) values ( 'Vine', 'Vine schema', '03-10-16', 'br', 1 );               ",
    L"                                                                                ",
    L"insert into f_schemainfo ( schemaname, description, creationdate, owner, ",
    L"schemaversionid ) values ( 'Error', 'Test schema errors', '03-10-16', 'br', 1 );       ",
    L"                                                                                ",
    L"insert into f_schemainfo ( schemaname, description, creationdate, owner, ",
    L"schemaversionid ) values ( 'Postal', 'more error tests', '03-10-17', 'br', 1 ); ",
    L"",
    L"/*-------101*/", 
    L"delete from f_classdefinition where schemaname = 'Acad';",
    L"",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('AcDbEntity', ",
    L"'Acad', 'AcDbEntity', 2, null, 0, null );                                                             ",
    L"",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, issystem, ",
    L"isreadonly, sequencename, owner, description ) values ( 'AcDbEntity', IDENT_CURRENT('f_classdefinition'), 'layer', ",
    L"'layer', 'string', 255, 0, 'string', 1, 0, 0, 0, null, null, 'Autocad layer' );      ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description, idposition ) values ( 'AcDbEntity', IDENT_CURRENT('f_classdefinition'), ",
    L"'id', 'Id', 'NUMBER', 10, 0, 'int32', 1, 1, 0, 0, null, null, null, 1 );                           ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, issystem,",
    L"isreadonly, sequencename, owner, description ) values ( 'AcDbEntity_AcXData', IDENT_CURRENT('f_classdefinition'), ",
    L"'xdata', 'xdata', 'AcXData', 0, 0, 'AcXData', 1, 0, 0, 0, null, null, null );           ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, issystem, ",
    L"isreadonly, sequencename, owner, description ) values ( 'AcDbEntity_AcXData', IDENT_CURRENT('f_classdefinition'), ",
    L"'seq', 'xdata.seq', 'NUMBER', 10, 0, 'int32', 1, 0, 0, 0, null, null, null );           ",
    L"                                                                                ",
    L"/*-------102*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('AcDbLine', ",
    L"'Acad', 'AcDbLine', 2, null, 0, 'AcDbEntity' );                                                     ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( 'AcDbEntity_AcXData', ",
    L"IDENT_CURRENT('f_classdefinition'), 'xdata', 'xdata', 'AcXData', 0, 0, 'AcXData', 1, 0, 0, 0, null, null, null );           ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( ",
    L"'AcDbEntity_AcXData', IDENT_CURRENT('f_classdefinition'), 'acdbentity_id', 'xdata.acdbentity_id', 'NUMBER', 10, 0, ",
    L"'int32', 1, 1, 0, 0, null, null, null );                                                                       ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, issystem, ",
    L"isreadonly, sequencename, owner, description ) values ( 'AcDbEntity_AcXData', IDENT_CURRENT('f_classdefinition'), ",
    L"'seq', 'xdata.seq', 'NUMBER', 10, 0, 'int32', 1, 0, 0, 0, null, null, null );           ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( 'AcDbEntity_AcXData',",
    L"IDENT_CURRENT('f_classdefinition'), 'datavalue', 'xdata.datavalue', 'VARCHAR2', 4000, 0, 'string', 1, 0, 0, 0, ",
    L"null, null, null );                                                                          ",
    L"                                                                                ",
    L"/*-------103*/",
    L"insert into f_classdefinition (classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('ElectricDevice', ",
    L"'Electric', 'ElectricDevice', 2, null, 1, null );                                                     ",
    L"",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( ",
    L"'ElectricDevice_AcDbEntity', IDENT_CURRENT('f_classdefinition'), 'graphic', 'graphic', 'Acad:AcDbEntity', 0, 0, ",
    L"'Acad:AcDbEntity', 1, 0, 0, 0, null, null, null );                                                                ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( 'F_GEOMETRY_0', ",
    L"IDENT_CURRENT('f_classdefinition'), 'RDBMS_GEOM', 'Geometry', 'SDO_GEOMETRY', 0, 0, '2', 1, 0, 0, ",
    L"0, null, null, '' );                                                                            ",
    L"                                                                                ",
    L"/*-------104--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('Conductor', ",
    L"'Electric', 'Conductor', 2, 'Electrical Conductor', 0, 'ElectricDevice' );                        ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( 'Conductor', ",
    L"IDENT_CURRENT('f_classdefinition'), 'Cost', 'Cost', 'NUMBER', 10, 2, 'decimal', 1, 0, 0, 0, null, null, ",
    L"'Installation Cost' );     ",
    L"                                                                                ",
    L"/*-------105--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('Transformer', ",
    L"'Electric', 'Transformer', 2, null, 0, 'ElectricDevice' );                                          ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, ",
    L"isnullable, isfeatid, issystem, isreadonly, sequencename, owner, description ) values ",
    L"( 'Transformer', IDENT_CURRENT('f_classdefinition'), 'VOLTAGE', 'voltage', 'double', 0, 0, 'double', 1, 0, 0, 0, ",
    L"null, null, 'number of volts' ); ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( 'Transformer', ",
    L"105, 'PHASE', 'phase', 'string', 1, 0, 'string', 1, 0, 0, 0, null, null, ",
    L"'an electrical term' );  ",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( 'Transformer', ",
    L"IDENT_CURRENT('f_classdefinition'), 'EntityId', 'EntityId', 'NUMBER', 38, 8, 'double', 1, 0, 0, 0, null, null, null );             ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, issystem, ",
    L"isreadonly, sequencename, owner, description ) values ( 'Transformer_AcXData', 105, ",
    L"'xdata', 'xdata', 'AcXData', 0, 0, 'AcXData', 1, 0, 0, 0, null, null, null );          ",
    L"                                                                                ",
    L"/*-------106--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('AcXData', ",
    L"'Acad', 'AcXData', 1, null, 0, null );                                                                ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, issystem, ",
    L"isreadonly, sequencename, owner, description ) values ( 'AcXData', IDENT_CURRENT('f_classdefinition'), 'datavalue', ",
    L"'DataValue', 'VARCHAR2', 4000, 0, 'string', 1, 0, 0, 0, null, null, null );           ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, attributename",
    L", columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, issystem, isread",
    L"only, sequencename, owner, description ) values ( 'AcDbXData', IDENT_CURRENT('f_classdefinition'), 'seq', 'seq', ",
    L"'NUMBER', 10, 0, 'int32', 1, 0, 0, 0, null, null, null );                          ",
    L"",
    L"/*---107--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('AbstractText', ",
    L"'Acad', 'AcDbText', 2, 'text base class', 1, 'AcDbEntity' );                                  ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( 'F_GEOMETRY_0',",
    L"IDENT_CURRENT('f_classdefinition'), 'RDBMS_GEOM', 'InsertPoint', 'SDO_GEOMETRY', 0, 0, '1', 1, 0, 0, 0, null,",
    L"null, null );   ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( 'F_GEOMETRY_0', ",
    L"IDENT_CURRENT('f_classdefinition'), 'C_1', 'Position', 'SDO_GEOMETRY', 0, 0, '3', 1, 0, 0, 0, null, null, null );",
    L"",
    L"/*---108--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('AcDb3dPolyline', ",
    L"'Acad', 'AcDb3dPolyline', 2, '3d polyline', 0, 'Acad:AcDbEntity' );                               ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( 'AcDb3dPolyline', ",
    L"IDENT_CURRENT('f_classdefinition'), 'layer', 'layer', 'string', 255, 0, 'string', 1, 0, 0, 0, null, null, ",
    L"'Autocad layer' );  ",
    L"                                                                                ",
    L"/*---109--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values (",
    L"'AcDbBlockAttribute', 'Acad', 'AcDbBlockAttribute', 2, null, 0, 'AcDbEntity' );                                           ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, ",
    L"isnullable, isfeatid, issystem, isreadonly, sequencename, owner, description ) values ",
    L"( 'F_GEOMETRY_0', IDENT_CURRENT('f_classdefinition'), 'RDBMS_GEOM', 'InsertPoint', 'SDO_GEOMETRY', 0, 0, '1', 1, ",
    L"0, 0, 0, null, null, null );   ",
    L"",
    L"/*---110--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ( 'AcDbHatch', ",
    L"'Acad', 'AcDbHatch', 2, 'a hatched polygon', 0, 'AcDbEntity' );                                  ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"/*---111--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('AcDbMText', ",
    L"'Acad', 'AcDbText', 2, 'multiline text', 0, 'AbstractText' );                                   ",
    L"",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"/*---112--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('AcDbText', ",
    L"'Acad', 'AcDbText', 2, 'single line text', 0, 'AbstractText' );                                  ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"/*---113--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values (",
    L"'AeccDbAlignment', 'Vine', 'AeccDbAlignment', 2, null, 0, 'Acad:AcDbEntity' );                                        ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"/*---114--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('AeccDbSurface', ",
    L"'Vine', 'AeccDbSurface', 2, null, 0, 'Acad:AcDbEntity' );                                          ",
    L"",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"/*---115--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('BadTable', ",
    L"'Error', 'BadTable', 1, 'Table does not exist', 0, null );                                         ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"/*---116--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('LoopA', 'Error', ",
    L"'LoopA', 1, null, 0, 'LoopC' );                                                           ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"/*---117--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('LoopB', 'Error', ",
    L"'LoopB', 1, null, 0, 'LoopA' );                                                           ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"/*---118--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('LoopC', ",
    L"'Error', 'LoopC', 1, null, 0, 'LoopB' );                                                           ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"/*---119--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('LoopD', ",
    L"'Error', 'LoopD', 1, null, 0, 'LoopC' );                                                           ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"/*---120--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('BadParent', ",
    L"'Error', 'BadParent', 1, 'parent class does not exist', 0, 'NoClass' );                          ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"/*---121--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('Address', ",
    L"'Postal', 'Address', 1, null, 1, null );                                                             ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description, idposition ) values ( 'Address', IDENT_CURRENT('f_classdefinition'), ",
    L"'num', 'Number', 'NUMBER', 5, 0, 'int16', 1, 0, 0, 0, null, null, null, 1 );                         ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description, idposition ) values ( 'Address', IDENT_CURRENT('f_classdefinition'), ",
    L"'street', 'Street', 'VARCHAR2', 50, 0, 'string', 1, 0, 0, 0, null, null, null, 2 );                  ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description, idposition ) values ( 'Address', IDENT_CURRENT('f_classdefinition'), ",
    L"'city', 'City', 'VARCHAR2', 50, 0, 'string', 1, 0, 0, 0, null, null, null, 3 );                      ",
    L"                                                                                ",
    L"/*---122--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('AddressAddId', ",
    L"'Postal', 'AddressAddId', 1, null, 0, 'Address' );                                                 ",
    L"",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description, idposition ) values ( 'AddressAddId',",
    L" IDENT_CURRENT('f_classdefinition'), 'city', 'City', 'VARCHAR2', 50, 0, 'string', 1, 0, 0, 0, null, null, null, 3 );                 ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description, idposition ) values ( 'AddressAddId', IDENT_CURRENT('f_classdefinition'), ",
    L"'num', 'Number', 'NUMBER', 5, 0, 'int16', 1, 0, 0, 0, null, null, null, 1 );                    ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, attributename",
    L", columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, issystem, isread",
    L"only, sequencename, owner, description, idposition ) values ( 'AddressAddId', IDENT_CURRENT('f_classdefinition'), 'street', ",
    L"'Street', 'VARCHAR2', 50, 0, 'string', 1, 0, 0, 0, null, null, null, 2 );             ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description, idposition ) values ( 'AddressAddId', ",
    L"IDENT_CURRENT('f_classdefinition'), 'province', 'Province', 'VARCHAR2', 50, 0, 'string', 1, 0, 0, 0, null, null, null, 4 );         ",
    L"                                                                                ",
    L"/*---123--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('AddressBadId', ",
    L"'Postal', 'AddressBadId', 1, null, 0, null );                                                        ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, attributename",
    L", columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, issystem, isread",
    L"only, sequencename, owner, description, idposition ) values ( 'AddressBadId', IDENT_CURRENT('f_classdefinition'), 'city', 'C",
    L"ity', 'VARCHAR2', 50, 0, 'string', 1, 0, 0, 0, null, null, null, 3 );                 ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description, idposition ) values ( 'AddressBadId', ",
    L"IDENT_CURRENT('f_classdefinition'), 'num', 'Number', 'NUMBER', 5, 0, 'int16', 1, 0, 0, 0, null, null, null, 1 );                    ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( 'AddressBadId', ",
    L"IDENT_CURRENT('f_classdefinition'), 'AddressType', 'AddressType', 'CHAR', 1, 0, 'string', 1, 0, 0, 1, null, null, null );        ",
    L"                                                                                ",
    L"/*---124--*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('AddressInhId', ",
    L"'Postal', 'AddressInhId', 1, null, 0, 'Address' );                                                 ",
    L"                                                                                ",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"/*---125-*/",
    L"insert into f_classdefinition ( classname, schemaname, tablename, ",
    L"classtype, description, isabstract, parentclassname ) values ('Customer', ",
    L"'Postal', 'Customer', 1, null, 0, null );                                                            ",
    L"",
    L"delete from f_attributedefinition where classid in (select IDENT_CURRENT('f_classdefinition'));",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description, idposition ) values ( 'Customer', IDENT_CURRENT('f_classdefinition'), ",
    L"'Province', 'Province', 'VARCHAR2', 50, 0, 'string', 1, 0, 0, 0, null, null, null, 2 );             ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description, idposition ) values ( 'Customer', ",
    L"IDENT_CURRENT('f_classdefinition'), 'Name', 'Name', 'VARCHAR2', 100, 0, 'string', 1, 0, 0, 0, null, null, null, 1 );                    ",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, attributename",
    L", columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, issystem, isread",
    L"only, sequencename, owner, description ) values ( 'Customer_HqAddresses', IDENT_CURRENT('f_classdefinition'), 'H",
    L"qAddresses', 'HqAddresses', 'Address', 0, 0, 'Address', 1, 0, 0, 0, null, null, '",
    L"' );                                                                            ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, issystem, ",
    L"isreadonly, sequencename, owner, description ) values ( 'Customer_RetailAddresses', ",
    L"IDENT_CURRENT('f_classdefinition'), 'RetailAddresses', 'RetailAddresses', 'Address', 0, 0, 'Address', 1, 0, ",
    L"0, 0, null, null, null );                                                                ",
    L"",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( ",
    L"'Customer_FactoryAddresses', IDENT_CURRENT('f_classdefinition'), 'FactoryAddresses', 'FactoryAddresses', 'Address', ",
    L"0, 0, 'Address', 1, 0, 0, 0, null, null, null );                                                             ",
    L"                                                                                ",
    L"insert into f_attributedefinition ( tablename, classid, columnname, ",
    L"attributename, columntype, columnsize, columnscale, attributetype, isnullable, isfeatid, ",
    L"issystem, isreadonly, sequencename, owner, description ) values ( ",
    L"'Customer_StorageAddresses', IDENT_CURRENT('f_classdefinition'), 'storageAddresses', 'storageAddresses', 'Address', ",
    L"0, 0, 'Address', 1, 0, 0, 0, null, null, null );                                                             ",
    L"                                                                                ",
    L"",
    L"                                                                                ",
    L"insert into f_attributedependencies ( pktablename, pkcolumnnames, fktablename, fkcolumnnames, fkcardinality, identitycolumn, ordertype ) values ( 'Conductor', ' ', 'ElectricDevice_AcDbEntity', ' ', 1, '', 'a');                                                                                                           ",
    L"insert into f_attributedependencies ( pktablename, pkcolumnnames, fktablename, fkcolumnnames, fkcardinality, identitycolumn, ordertype ) values ( 'AcDbEntity', 'Id', 'AcDbEntity_AcXData', 'AcDbEntity_id', -1, 'seq', 'a');                                                                                                ",
    L"insert into f_attributedependencies ( pktablename, pkcolumnnames, fktablename, fkcolumnnames, fkcardinality, identitycolumn, ordertype ) values ( 'AcDbLine', 'Id', 'AcDbEntity_AcXData', 'AcDbEntity_id', -1, 'seq', 'a');                                                                                                  ",
    L"insert into f_attributedependencies ( pktablename, pkcolumnnames, fktablename, fkcolumnnames, fkcardinality, identitycolumn, ordertype ) values ( 'Customer', 'Name, Province', 'Customer_HqAddresses', 'Customer_Name, Customer_Province', -1, '', 'a');                                                                    ",
    L"insert into f_attributedependencies ( pktablename, pkcolumnnames, fktablename, fkcolumnnames, fkcardinality, identitycolumn, ordertype ) values ( 'Customer', 'NoName', 'Customer_RetailAddresses', 'Customer_Name', -1, 'seq', 'a');                                                                                        ",
    L"insert into f_attributedependencies ( pktablename, pkcolumnnames, fktablename, fkcolumnnames, fkcardinality, identitycolumn, ordertype ) values ( 'Customer', 'Name', 'Customer_FactoryAddresses', 'Customer_NoName', -1, 'seq', 'a');                                                                                       ",
    L"",
    L"                                                                                ",
    L"delete from f_sad; ",
    L"                                                                                ",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Electric', 'Electric', 'schema', 'Author', 'Thomas Edison' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Electric', 'Electric', 'schema', 'Sharable', 'Yes' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Vine', 'Vine', 'schema', 'Author', 'Tarzan' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Acad', 'AbstractText', 'class', 'Entity', 'Yes' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Acad', 'AbstractText', 'class', 'Text', 'Yes' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Acad', 'AbstractText', 'class', 'Encoding', 'ASCII' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Acad', 'AcDbEntity', 'class', 'Entity', 'Yes' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Electric', 'Conductor', 'class', 'Construction Template', 'linear' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Electric', 'ElectricDevice', 'class', 'Construction Template', 'general' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Electric', 'ElectricDevice', 'class', 'Maintenance Template', 'standard' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Electric', 'Transformer', 'class', 'Construction Template', 'point' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Acad:AcDbEntity', 'layer', 'property', 'Classified', 'Yes' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Acad:AcDbEntity', 'xdata', 'property', 'LinkList', 'Yes' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Acad:AcDbEntity', 'xdata', 'property', 'Ordered', 'No' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Electric:Conductor', 'Cost', 'property', 'Copper', '5' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Electric:Conductor', 'Cost', 'property', 'Aluminum', '3' ); ",
    L"",
    L"insert into f_sad ( ownername, elementname, elementtype, name, value ) ",
    L" values ( 'Electric:Transformer', 'voltage', 'property', 'Formula', 'Ohms Law' ); ",
    L"",
    NULL
};
