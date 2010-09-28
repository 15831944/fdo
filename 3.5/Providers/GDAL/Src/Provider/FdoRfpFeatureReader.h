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
 * Revision Control Modification History
 *
 *  $HeadURL: http://svn.hobu.net/frank/GdalFile/src/Provider/FdoRfpFeatureReader.h $
 *   $Author: fwarmerdam $
 *     $Date: 2006-01-25 14:22:51 -0500 (Wed, 25 Jan 2006) $
 * $Revision: 23 $
 *
 */

#ifndef FDORFPFEATUREREADER_H
#define FDORFPFEATUREREADER_H

#ifdef _WIN32
#pragma once
#endif // _WIN32

#include "FdoRfpCommonReader.h"
#include "FdoDefaultFeatureReader.h"
//
// Class forward declarations
//

//
// Implementation of FdoIFeatureReader
//
class FdoRfpFeatureReader :
	public FdoRfpCommonReader<FdoDefaultFeatureReader>
{
    typedef FdoRfpCommonReader<FdoDefaultFeatureReader> superclass;
//
// Data members
//
private:
	// Which class would this reader read
	FdoClassDefinitionP				m_classDef;
    FdoClassDefinitionP             m_classDefDynamic;

//
// Constructor(s), desctrucotr, factory function(s)
//
protected:
	FdoRfpFeatureReader(const FdoClassDefinitionP& classDef, const FdoPtr<FdoRfpQueryResult>& queryResult);
public:
	// Static factory function
	static FdoRfpFeatureReader* Create(const FdoClassDefinitionP& classDef, const FdoPtr<FdoRfpQueryResult>& queryResult);

// 
// Exposed functions
//
public:
    /// <summary>Gets the definition of the object currently being read. If the user
    /// has requested only a subset of the class properties, the class 
    /// definition reflects what the user has asked, rather than the full class 
    /// definition.</summary>
    /// <returns>Returns the class definition object.</returns> 
    virtual FdoClassDefinition* GetClassDefinition();

    /// <summary>Gets a value indicating the depth of nesting for the current reader.
    /// The depth value increases each time GetFeatureObject is called and a new 
    /// reader is returned. The outermost reader has a depth of 0.</summary>
    /// <returns>Returns the depth</returns> 
    virtual FdoInt32 GetDepth();


    /// <summary>Gets a reference to an FdoIFeatureReader to read the data contained in
    /// the object or object collection property. If the property is not an
    /// object property, an exception is thrown.</summary>
    /// <param name="propertyName">Input the property name.</param> 
    /// <returns>Returns the nested feature reader</returns> 
    virtual FdoIFeatureReader* GetFeatureObject(FdoString* propertyName);

    /// <summary>Gets the geometry value of the specified property as a byte array in 
    /// AGF format. Because no conversion is performed, the property must be
    /// of Geometric type; otherwise, an exception is thrown. 
    /// This method is a language-specific performance optimization that returns a
    /// pointer to the array data, rather than to an object that encapsulates
    /// the array.  The array's memory area is only guaranteed to be valid
    /// until a call to ReadNext() or Close(), or the disposal of this reader
    /// object.</summary>
    /// <param name="propertyName">Input the property name.</param> 
    /// <param name="count">Output the number of bytes in the array.</param> 
    /// <returns>Returns a pointer to the byte array in AGF format.</returns> 
    virtual const FdoByte * GetGeometry(FdoString* propertyName, FdoInt32 * count);

//
// Internally used helper functions
//
private:

};

typedef FdoPtr<FdoRfpFeatureReader> FdoRfpReatureReaderP;

#endif
