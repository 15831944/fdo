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
 */

#ifndef SHPINSERTCOMMAND_H
#define SHPINSERTCOMMAND_H

#ifdef _WIN32
#pragma once
#endif // _WIN32


class ShpInsertCommand :
    public FdoCommonFeatureCommand<FdoIInsert, ShpConnection>
{
    friend class ShpConnection;

protected:
    FdoPtr<FdoBatchParameterValueCollection> mBatchParameters;

private:
    ShpInsertCommand (FdoIConnection *connection);
    virtual ~ShpInsertCommand (void);

public:
    //
    // Prevent the use of the Assignment Operation by defining it and not implementing it.
    // DO NOT IMPLEMENT
    ShpInsertCommand & operator= (const ShpInsertCommand &right);

    //
    // FdoIInsertCommand interface
    //

    /// <summary>Gets the FdoBatchParameterValueCollection that can be used for optimized
    /// batch inserts of multiple features with a single insert command. Batch
    /// inserts can be performed by using Parameters for each of the property
    /// values, then adding collections of parameter values to the
    /// FdoBatchParameterValueCollection. Each FdoParameterValueCollection in the
    /// FdoBatchParameterValueCollection should contain one FdoParameterValue for each
    /// of the parameters specified for property values.</summary>
    /// <returns>Returns FdoBatchParameterValueCollection</returns>
    virtual FdoBatchParameterValueCollection* GetBatchParameterValues ();

    /// <summary>Executes the insert command and returns a reference to an
    /// FdoIFeatureReader. Some feature providers can generate automatic identity
    /// values for features. This will happen automatically as the features are
    /// inserted. The returned FdoIFeatureReader allows the client to obtain the
    /// automatic identity property value(s) of newly inserted object(s). The
    /// returned feature reader at a minimum will read the unique identity
    /// properties of the objects just inserted. Multiple objects will be
    /// returned through the reader in the case of a batch insert.</summary>
    /// <returns>Returns an FdoIFeatureReader</returns> 
    virtual FdoIFeatureReader* Execute ();
};

#endif // SHPINSERTCOMMAND_H

