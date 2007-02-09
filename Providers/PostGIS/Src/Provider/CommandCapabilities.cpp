//
// Copyright (C) 2007 Refractions Research, Inc. 
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of version 2.1 of the GNU Lesser
// General Public License as published by the Free Software Foundation.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
#include "stdafx.h"

#include "PostGisProvider.h"
#include "CommandCapabilities.h"

#include <cassert>

namespace fdo { namespace postgis {

CommandCapabilities::CommandCapabilities()
{
}

CommandCapabilities::~CommandCapabilities()
{
}

///////////////////////////////////////////////////////////////////////////////
// FdoIDisposable interface
///////////////////////////////////////////////////////////////////////////////

void CommandCapabilities::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
// FdoICommandCapabilities interface
///////////////////////////////////////////////////////////////////////////////

FdoInt32* CommandCapabilities::GetCommands(FdoInt32& size)
{
    // TODO: Verify list of commands

    static FdoCommandType commands[] =
    {
        FdoCommandType_Select,
        FdoCommandType_Insert,
        FdoCommandType_Update,
        FdoCommandType_Delete,
        FdoCommandType_SQLCommand,
        FdoCommandType_DescribeSchema,
        FdoCommandType_CreateDataStore,
        FdoCommandType_DestroyDataStore,
        FdoCommandType_ListDataStores,
        FdoCommandType_GetSpatialContexts
    };

    size = (sizeof(commands) / sizeof(FdoCommandType));

    return (reinterpret_cast<FdoInt32*>(commands));
}

bool CommandCapabilities::SupportsParameters()
{
    return false;
}

bool CommandCapabilities::SupportsTimeout()
{
    return false;
}

bool CommandCapabilities::SupportsSelectExpressions()
{
    return false;
}

bool CommandCapabilities::SupportsSelectFunctions()
{
    return false;
}

bool CommandCapabilities::SupportsSelectDistinct()
{
    return false;
}

bool CommandCapabilities::SupportsSelectOrdering()
{
    return false;
}

bool CommandCapabilities::SupportsSelectGrouping()
{
    return false;
}

}} // namespace fdo::postgis
