#ifndef measures321_H
#define measures321_H

 //
 // Copyright (C) 2004-2010  Autodesk, Inc.
 // 
 // This library is free software; you can redistribute it and/or
 // modify it under the terms of version 2.1 of the GNU Lesser
 // General Public License as published by the Free Software Foundation.
 // 
 // This library is distributed in the hope that it will be useful,
 // but WITHOUT ANY WARRANTY; without even the implied warranty of
 // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 // Lesser General Public License for more details.
 // 
 // You should have received a copy of the GNU Lesser General Public
 // License along with this library; if not, write to the Free Software
 // Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

static const char* measures321[] = {
	"<?xml version=\"1.0\" encoding=\"UTF-8\"?>",
	"<schema targetNamespace=\"http://www.opengis.net/gml/3.2\" xmlns:gml=\"http://www.opengis.net/gml/3.2\" xmlns=\"http://www.w3.org/2001/XMLSchema\" elementFormDefault=\"qualified\" attributeFormDefault=\"unqualified\" xml:lang=\"en\" version=\"3.2.1.2\">",
	"	<annotation>",
	"		<appinfo source=\"urn:x-ogc:specification:gml:schema-xsd:measures:3.2.1\">measures.xsd</appinfo>",
	"		<documentation>See ISO/DIS 19136 17.3.",
	"gml:MeasureType is defined in the basicTypes schema.  The measure types defined here correspond with a set of convenience measure types described in ISO/TS 19103.  The XML implementation is based on the XML Schema simple type \"double\" which supports both decimal and scientific notation, and includes an XML attribute \"uom\" which refers to the units of measure for the value.  Note that, there is no requirement to store values using any particular format, and applications receiving elements of this type may choose to coerce the data to any other type as convenient.",
	"",
	"GML is an OGC Standard.",
	"Copyright (c) 2007,2010 Open Geospatial Consortium.",
	"To obtain additional rights of use, visit http://www.opengeospatial.org/legal/ .",
	"		</documentation>",
	"	</annotation>",
	"	<include schemaLocation=\"gml.xsd\"/>",
	"	<include schemaLocation=\"units.xsd\"/>",
	"	<element name=\"measure\" type=\"gml:MeasureType\">",
	"		<annotation>",
	"			<documentation>The value of a physical quantity, together with its unit.</documentation>",
	"		</annotation>",
	"	</element>",
	"	<complexType name=\"LengthType\">",
	"		<annotation>",
	"			<documentation>This is a prototypical definition for a specific measure type defined as a vacuous extension (i.e. aliases) of gml:MeasureType. In this case, the content model supports the description of a length (or distance) quantity, with its units. The unit of measure referenced by uom shall be suitable for a length, such as metres or feet.</documentation>",
	"		</annotation>",
	"		<simpleContent>",
	"			<extension base=\"gml:MeasureType\"/>",
	"		</simpleContent>",
	"	</complexType>",
	"	<complexType name=\"ScaleType\">",
	"		<simpleContent>",
	"			<extension base=\"gml:MeasureType\"/>",
	"		</simpleContent>",
	"	</complexType>",
	"	<complexType name=\"TimeType\">",
	"		<simpleContent>",
	"			<extension base=\"gml:MeasureType\"/>",
	"		</simpleContent>",
	"	</complexType>",
	"	<complexType name=\"GridLengthType\">",
	"		<simpleContent>",
	"			<extension base=\"gml:MeasureType\"/>",
	"		</simpleContent>",
	"	</complexType>",
	"	<complexType name=\"AreaType\">",
	"		<simpleContent>",
	"			<extension base=\"gml:MeasureType\"/>",
	"		</simpleContent>",
	"	</complexType>",
	"	<complexType name=\"VolumeType\">",
	"		<simpleContent>",
	"			<extension base=\"gml:MeasureType\"/>",
	"		</simpleContent>",
	"	</complexType>",
	"	<complexType name=\"SpeedType\">",
	"		<simpleContent>",
	"			<extension base=\"gml:MeasureType\"/>",
	"		</simpleContent>",
	"	</complexType>",
	"	<complexType name=\"AngleType\">",
	"		<simpleContent>",
	"			<extension base=\"gml:MeasureType\"/>",
	"		</simpleContent>",
	"	</complexType>",
	"	<element name=\"angle\" type=\"gml:AngleType\">",
	"		<annotation>",
	"			<documentation>The gml:angle property element is used to record the value of an angle quantity as a single number, with its units.</documentation>",
	"		</annotation>",
	"	</element>",
	"</schema>",
	NULL
};

#endif
