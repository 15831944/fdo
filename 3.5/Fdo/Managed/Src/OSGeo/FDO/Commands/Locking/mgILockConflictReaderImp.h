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

#include "FDO\Commands\Locking\mgILockConflictReader.h"
#include "FDO\Commands\Locking\mgConflictType.h"

class FdoILockConflictReader;

BEGIN_NAMESPACE_OSGEO_FDO_COMMANDS
ref class PropertyValueCollection;
END_NAMESPACE_OSGEO_FDO_COMMANDS

BEGIN_NAMESPACE_OSGEO_FDO_COMMANDS_LOCKING

/// \ingroup (OSGeoFDOCommandsLocking)
/// \brief
/// The ILockConflictReaderImp class is a concrete implementation of ILockConflictReaderImp.
/// The ILockConflictReader interface provides forward only, read-only
/// functionality for identifying features that have a lock conflict.
/// Features with a lock conflict are those that the caller is attempting to modify 
/// that are already locked by someone else. 
/// \remarks
/// A reference to an ILockConflictReader is returned from the
/// IAcquireLock, ISelect, IReleaseLock, IUpdate, and IDelete commands. The
/// initial position of the ILockConflictReader is prior to the first item,
/// thus you must call ReadNext to begin accessing any data.
private ref class ILockConflictReaderImp : public NAMESPACE_OSGEO_RUNTIME::Disposable, 
                                            public NAMESPACE_OSGEO_FDO_COMMANDS_LOCKING::ILockConflictReader
{
public:
    /// \brief
    /// Gets the class name of the feature currently being read.
    /// 
    /// \remarks
    /// The value of the class name will be null in the event that the class
    /// name could not be resolved. This can happen 
    /// if the table has no class id field and multiple classes have been
    /// written to the table.
    /// 
    /// \return
    /// Returns the class name.
    /// 
	virtual System::String^ GetFeatureClassName();

    /// \brief
    /// Returns an PropertyValueCollection containing the property values
    /// that uniquely identify the feature currently being read.
    /// 
    /// \return
    /// Returns the property collection identifying the feature.
    /// 
	virtual NAMESPACE_OSGEO_FDO_COMMANDS::PropertyValueCollection^ GetIdentity();

    /// \brief
    /// Gets the name of the owner who holds a lock on the feature 
    /// currently being read.
    /// 
    /// \return
    /// Returns the name of the owner
    /// 
	virtual System::String^ GetLockOwner();

    /// \brief
    /// Gets the name of the long transaction in which the feature 
    /// currently being read is locked.
    /// 
    /// \return
    /// Returns the name of the long transaction in which the feature
    /// currently being read is locked.
    /// 
	virtual System::String^ GetLongTransaction();

    /// \brief
    /// Gets the conflict type for the conflict currently being read.
    /// 
    /// \return
    /// Returns the conflict type for the conflict currently being read.
    /// 
	virtual NAMESPACE_OSGEO_FDO_COMMANDS_LOCKING::ConflictType GetConflictType();

    /// \brief
    /// Advances the reader to the next item. The default position of the 
    /// reader is prior to the first item. Thus, you must call ReadNext
    /// to begin accessing any data.
    /// 
    /// \return
    /// Returns true if there is a next item.
    /// 
	virtual System::Boolean ReadNext();

    /// \brief
    /// Closes the ILockConflictReader object, freeing any resources it may
    /// be holding.
    /// 
    /// \return
    /// Returns nothing
    /// 
	virtual System::Void Close();

internal:
	ILockConflictReaderImp(System::IntPtr unmanaged, System::Boolean autoDelete) : NAMESPACE_OSGEO_RUNTIME::Disposable(unmanaged, autoDelete)
	{

	}

	inline FdoILockConflictReader* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;
};

END_NAMESPACE_OSGEO_FDO_COMMANDS_LOCKING

