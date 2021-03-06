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

#include "FDO\Commands\mgICommandImp.h"
#include "FDO\Commands\LongTransaction\mgIChangeLongTransactionPrivileges.h"
#include "FDO\Commands\LongTransaction\mgLongTransactionPrivilegeOperations.h"

class FdoIChangeLongTransactionPrivileges;

BEGIN_NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION

/// \ingroup (OSGeoFDOCommandsLongTransaction)
/// \brief
/// The IChangeLongTransactionPrivilegesImp class is a 
/// concrete implementation of IChangeLongTransactionPrivileges.
/// The IChangeLongTransactionPrivileges interface defines the
/// ChangeLongTransactionPrivileges command, which grants or revokes long
/// transaction privileges for the named user. The ability to change the 
/// privileges might be restricted by the datastore security if the long 
/// transaction doesn�t belong to the user executing the command.
private ref class IChangeLongTransactionPrivilegesImp : public NAMESPACE_OSGEO_FDO_COMMANDS::ICommandImp, 
                                                         public NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION::IChangeLongTransactionPrivileges
{
public:
    /// \brief
    /// Gets the name of the long transaction for which privileges should be
    /// changed.
    /// 
    /// \return
    /// Returns the name of the long transaction
    /// 
    /// \brief
    /// Sets the name of the long transaction for which privileges should be
    /// changed.
    /// 
    /// \param value 
    /// Input the name of the long transaction
    /// 
    /// \return
    /// Returns nothing
    /// 
    virtual property System::String^ LongTransactionName
    {
        System::String^ get();
        System::Void set(System::String^ value);
    }

    /// \brief
    /// Gets the name of the user for which to grant or revoke privileges.
    /// 
    /// \return
    /// Returns the user name
    /// 
    /// \brief
    /// Sets the name of the user for which to grant or revoke privileges.
    /// 
    /// \param value 
    /// Input the user name
    /// 
    /// \return
    /// Returns nothing
    /// 
    virtual property System::String^ UserName
    {
        System::String^ get();
        System::Void set(System::String^ value);
    }

    /// \brief
    /// Gets the privilege flags to grant or revoke.The returned value may be
    /// any combination of the values from the LongTransactionPrivileges
    /// enumeration combined via a bit-wise or operation.
    /// 
    /// \return
    /// Returns the privileges as a bit masked value based on LongTransactionPrivileges enumeration
    /// 
    /// \brief
    /// Sets the privilege flags to grant or revoke. The privileges value may be
    /// any combination of the values from the LongTransactionPrivileges
    /// enumeration combined via a bit-wise or operation.
    /// 
    /// \param value 
    /// Input the privileges as a bit masked value based on LongTransactionPrivileges enumeration
    /// 
    /// \return
    /// Returns nothing
    /// 
    virtual property System::Int32 Privileges
    {
        System::Int32 get();
        System::Void set(System::Int32 value);
    }

    /// \brief
    /// Gets the operation to perform, grant, or revoke.
    /// 
    /// \return
    /// Returns the operation to perform
    /// 
    /// \brief
    /// Sets the operation to perform, grant, or revoke.
    /// 
    /// \param value 
    /// Input the operation to perform
    /// 
    /// \return
    /// Returns nothing
    /// 
    virtual property NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION::LongTransactionPrivilegeOperations Operation
    {
        NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION::LongTransactionPrivilegeOperations get();
        System::Void set(NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION::LongTransactionPrivilegeOperations value);
    }

    /// \brief
    /// Executes the ChangeLongTransactionPrivilege command.
    /// 
    /// \return
    /// Returns nothing
    /// 
	virtual System::Void Execute();

internal:
	IChangeLongTransactionPrivilegesImp(System::IntPtr unmanaged, System::Boolean autoDelete) : NAMESPACE_OSGEO_FDO_COMMANDS::ICommandImp(unmanaged, autoDelete)
	{

	}

	inline FdoIChangeLongTransactionPrivileges* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;
};

END_NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION


