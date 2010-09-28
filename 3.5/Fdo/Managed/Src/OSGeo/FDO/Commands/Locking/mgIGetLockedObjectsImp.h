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

#include "FDO\Commands\Locking\mgIGetLockedObjects.h"
#include "FDO\Commands\mgICommandImp.h"

class FdoIGetLockedObjects;

BEGIN_NAMESPACE_OSGEO_FDO_COMMANDS_LOCKING
interface class ILockedObjectReader;

/// \ingroup (OSGeoFDOCommandsLocking)
/// \brief
/// The IGetLockedObjectsImp class is a concrete implementation of IGetLockedObjects.
/// The IGetLockedObjects interface defines the GetLockedObjects command, which
/// gets a list of all objects that are currently locked by a particular user.
private ref class IGetLockedObjectsImp : public NAMESPACE_OSGEO_FDO_COMMANDS::ICommandImp, 
                                          public NAMESPACE_OSGEO_FDO_COMMANDS_LOCKING::IGetLockedObjects
{
public:
    /// \brief
    /// Gets the name of the user whose locked objects you want to list.
    /// 
    /// \return
    /// Returns the name of the user
    /// 
    /// \brief
    /// Sets the name of the user whose locked objects you want to list.
    /// 
    /// \param value 
    /// Input the name of the user
    /// 
    /// \return
    /// Returns nothing
    /// 
    virtual property System::String^ LockOwner
    {
        System::String^ get();
        System::Void set(System::String^ value);
    }

    /// \brief
    /// Executes the GetLockedObjects command, returning an ILockedObjectReader.
    /// 
    /// \return
    /// Returns a list of all of the objects currently locked by a particular user.
    /// The list might be empty if the user does not have anything locked.
    /// 
	virtual NAMESPACE_OSGEO_FDO_COMMANDS_LOCKING::ILockedObjectReader^ Execute();

internal:
	IGetLockedObjectsImp(System::IntPtr unmanaged, System::Boolean autoDelete) : NAMESPACE_OSGEO_FDO_COMMANDS::ICommandImp(unmanaged, autoDelete)
	{

	}

	inline FdoIGetLockedObjects* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;
};

END_NAMESPACE_OSGEO_FDO_COMMANDS_LOCKING

