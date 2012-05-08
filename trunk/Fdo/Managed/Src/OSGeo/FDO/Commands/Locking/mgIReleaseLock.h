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

#include "FDO\Commands\mgIFeatureCommand.h"

BEGIN_NAMESPACE_OSGEO_FDO_COMMANDS_LOCKING
interface class ILockConflictReader;

/// \ingroup (OSGeoFDOCommandsLocking)
/// \interface OSGeo::FDO::Commands::Locking::IReleaseLock
/// \brief
/// The IReleaseLock interface defines the ReleaseLock command, which releases
/// locks from feature instances of a given class that match the specified
/// criteria. Input to the release lock command includes the name of the class
/// and filter criteria by which to identify the instances to be unlocked. The
/// filter may be specified either as text or as an expression tree (most likely
/// produced by a query builder). The ability to successfully execute the command 
/// might be restricted by the datastore security if the lock doesn�t belong to the 
/// user executing the command.
public interface class IReleaseLock : public NAMESPACE_OSGEO_FDO_COMMANDS::IFeatureCommand
{
public:
    /// \brief
    /// Gets the name of the user who owns the lock to release.
    /// 
    /// \return
    /// Returns name of the lock owner. If the name was not set, it returns the name of the caller.
    /// 
    /// \brief
    /// Sets the name of the user who owns the lock to release.
    /// 
    /// \param value 
    /// Input the name of the user.
    /// 
    /// \return
    /// Returns nothing
    /// 
    property System::String^ LockOwner
    {
        System::String^ get();
        System::Void set(System::String^ value);
    }

    /// \brief
    /// Executes the release lock command, returning an ILockConflictReader.
    /// 
    /// \return
    /// Returns a list of feature instances whose lock is owned by someone else.
    /// 
	NAMESPACE_OSGEO_FDO_COMMANDS_LOCKING::ILockConflictReader^ Execute();
};

END_NAMESPACE_OSGEO_FDO_COMMANDS_LOCKING


