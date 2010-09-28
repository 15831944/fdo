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

#include "FDO\Connections\Capabilities\mgICommandCapabilities.h"

class FdoICommandCapabilities;

BEGIN_NAMESPACE_OSGEO_FDO_CONNECTIONS_CAPABILITIES

/// \ingroup (OSGeoFDOConnectionsCapabilities)
/// \brief
/// The ICommandCapabilities interface declares the feature provider's level
/// of support for Commands.
public ref class ICommandCapabilitiesImp : public NAMESPACE_OSGEO_RUNTIME::Disposable, 
                                            public NAMESPACE_OSGEO_FDO_CONNECTIONS_CAPABILITIES::ICommandCapabilities
{
public:
    /// \brief
    /// Gets an array of the CommandType values supported by the feature provider.
    /// 
    /// \return
    /// Returns the list of commands
    /// 
    virtual property array<System::Int32>^ Commands
    {
        array<System::Int32>^ get();
    }

    /// \brief
    /// Determines if commands support parameterization.
    /// 
    /// \return
    /// Returns true if commands support parameterization
    /// 
	virtual System::Boolean SupportsParameters();

    /// \brief
    /// Determines if the feature provider supports command execution timeout.
    /// 
    /// \return
    /// Returns true if the feature provider supports timeout.
    /// 
	virtual System::Boolean SupportsTimeout();

    /// \brief
    /// Determines if the feature provider can use expressions for properties with Select and SelectAggregate commands.
    /// 
    /// \return
    /// Returns true if the feature provider supports select expressions.
    /// 
	virtual System::Boolean SupportsSelectExpressions();

    /// \brief
    /// Determines if simple functions can be used in Select and SelectAggregates command. 
    /// Aggregate functions can only be used in the SelectAggregates command. Capability for using SelectAggregates is found in 
    /// CommandCapabilities.
    /// 
    /// \return
    /// Returns true if the feature provider supports select simple functions.
    /// 
	virtual System::Boolean SupportsSelectFunctions();

    /// \brief
    /// Determines if Distinct can be used with SelectAggregates. This can be true only 
    /// if the SelectAggregates command is supported by the provider.
    /// 
    /// \return
    /// Returns true if the feature provider supports select distinct.
    /// 
	virtual System::Boolean SupportsSelectDistinct();

    /// \brief
    /// Determines  if ordering is available in the Select and SelectAggregates command.
    /// 
    /// \return
    /// Returns true if the feature provider supports select ordering.
    /// 
	virtual System::Boolean SupportsSelectOrdering();

    /// \brief
    /// Determines if a grouping criteria is available in the SelectAggregates command. 
    /// This can be true only if the SelectAggregates command is supported by the provider. 
    /// 
    /// \return
    /// Returns true if the feature provider supports select grouping.
    /// 
    /// Note: Aggregate functions can be supported without also supporting grouping criteria 
    /// (but not vice versa).
	virtual System::Boolean SupportsSelectGrouping();

internal:
	ICommandCapabilitiesImp(System::IntPtr unmanaged, System::Boolean autoDelete);

	inline FdoICommandCapabilities* GetImpObj();
public:
    virtual IntPtr GetDisposableObject() override;
};

END_NAMESPACE_OSGEO_FDO_CONNECTIONS_CAPABILITIES

