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

#pragma once

#include "FDO\Commands\mgIFeatureCommandImp.h"
#include "FDO\Commands\LongTransaction\mgIRollbackLongTransaction.h"

class FdoIRollbackLongTransactionImp;

BEGIN_NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION

/// \ingroup (OSGeoFDOCommandsLongTransaction)
/// \brief
/// The IRollbackLongTransactionImp class is a concrete implementation of IRollbackLongTransaction.
/// The IRollbackLongTransaction interface defines the RollbackLongTransaction
/// command, which allows a user to execute rollback operations on a long
/// transaction. Two different rollback operations are distinguished: full and partial.
///
/// <ol>
/// <li>A full rollback operation is executed on all data within a long transaction and all its descendents. The data is removed from the database and all versions involved in the process deleted.
/// <li> A partial rollback operation is executed on a subset of the data within a long transaction (which could be all data). The specified data in this long transaction and all its descendents are removed from the database. The versions involved in the process are not removed. The subset of the data to be rolled back is defined by a class name and a set of filter criteria.
/// </ol>
///
/// To perform a rollback operation, the user requires the access and
/// rollback privileges for the long transaction to be rolled back (in the case
/// of a full rollback, the user also requires the remove privilege for the
/// long transaction) and the access privilege for all the descendent
/// versions.
private ref class IRollbackLongTransactionImp : public NAMESPACE_OSGEO_FDO_COMMANDS::IFeatureCommandImp, public NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION::IRollbackLongTransaction
{
public:
    /// \brief
    /// Gets the name of the long transaction to roll back as a string.
    /// 
    /// \return
    /// Returns the name of the long transaction.
    /// 
    /// \brief
    /// Sets the name of the long transaction to roll back as a string.
    /// 
    /// \param value 
    /// Input the name of the long transaction.
    /// 
    /// \return
    /// Returns nothing
    /// 
    virtual property System::String^ Name
    {
        System::String^ get();
        System::Void set(System::String^ value);
    }

    /// \brief
    /// Executes the RollbackLongTransaction command.
    /// 
    /// \return
    /// Returns nothing
    /// 
	virtual System::Void Execute();

internal:
	IRollbackLongTransactionImp(System::IntPtr unmanaged, System::Boolean autoDelete) : NAMESPACE_OSGEO_FDO_COMMANDS::IFeatureCommandImp(unmanaged, autoDelete)
	{

	}

	inline FdoIRollbackLongTransaction* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;
};

END_NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION

