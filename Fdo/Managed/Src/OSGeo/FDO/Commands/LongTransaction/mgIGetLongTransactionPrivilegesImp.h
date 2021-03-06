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
#include "FDO\Commands\LongTransaction\mgIGetLongTransactionPrivileges.h"

class FdoIGetLongTransactionPrivileges;

BEGIN_NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION
interface class ILongTransactionPrivilegeReader;

/// \ingroup (OSGeoFDOCommandsLongTransaction)
/// \brief
/// The IGetLongTransactionPrivilegesImp class is a 
/// concrete implementation of IGetLongTransactionPrivileges.
/// The IGetLongTransactionPrivileges interface defines the
/// GetLongTransactionPrivileges command, which allows the user to enumerate the
/// privileges of a long transaction established for each user. The ability to 
/// successfully execute the command might be restricted by the datastore security 
/// if the long transaction doesn�t belong to the user executing the command.
private ref class IGetLongTransactionPrivilegesImp : public NAMESPACE_OSGEO_FDO_COMMANDS::ICommandImp, 
                                                      public NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION::IGetLongTransactionPrivileges
{
public:
    /// \brief
    /// Gets the name of the long transaction for which privileges should be
    /// enumerated.
    /// 
    /// \return
    /// Returns the long transaction name
    /// 
    /// \brief
    /// Sets the name of the long transaction for which privileges should be
    /// enumerated.
    /// 
    /// \param value 
    /// Input the long transaction name
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
    /// Executes the GetLongTransactionPrivileges command, returning a reference
    /// to an ILongTransactionPrivilegeReader.
    /// 
    /// \return
    /// Returns the long transaction privilege reader.
    /// 
	virtual NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION::ILongTransactionPrivilegeReader^ Execute();

internal:
	IGetLongTransactionPrivilegesImp(System::IntPtr unmanaged, System::Boolean autoDelete) : NAMESPACE_OSGEO_FDO_COMMANDS::ICommandImp(unmanaged, autoDelete)
	{

	}

	inline FdoIGetLongTransactionPrivileges* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;
};

END_NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION


