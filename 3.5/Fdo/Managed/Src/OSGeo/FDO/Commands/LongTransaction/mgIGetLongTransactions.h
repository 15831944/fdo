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

#include "FDO\Commands\mgICommand.h"

BEGIN_NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION
interface class ILongTransactionReader;

/// \ingroup (OSGeoFDOCommandsLongTransaction)
/// \interface OSGeo::FDO::Commands::LongTransaction::IGetLongTransactions
/// \brief
/// The IGetLongTransactions interface defines the GetLongTransactions
/// command. It allows a user to retrieve long transaction information for all or
/// a specific long transaction. The command returns a ILongTransactionReader
/// object enumerating the identified long transactions.
public interface class IGetLongTransactions : public NAMESPACE_OSGEO_FDO_COMMANDS::ICommand
{
public:
    /// \brief
    /// Sets the name of the long transaction for which information
    /// should be retrieved. The function can also be used to nullify the
    /// property by using a nullptr pointer as the function's parameter. If the
    /// the property is not set, the function returns information for all
    /// available long transactions.
    /// 
    /// \return
    /// Returns nothing
    /// 
    /// \brief
    /// Gets the currently set name of the long transaction for which
    /// the long transaction information should be retrieved. If the property
    /// is not set, the function returns a nullptr pointer.
    /// 
    /// \return
    /// Returns a long transaction name.
    /// 
    property System::String^ Name
    {
        System::String^ get();
        System::Void set(System::String^ value);
    }

    /// \brief
    /// Executes the GetLongTransactions command returning a reference to an
    /// ILongTransactionReader.
    /// 
    /// \return
    /// Returns a long transaction reader
    /// 
	NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION::ILongTransactionReader^ Execute();
};

END_NAMESPACE_OSGEO_FDO_COMMANDS_LONGTRANSACTION

