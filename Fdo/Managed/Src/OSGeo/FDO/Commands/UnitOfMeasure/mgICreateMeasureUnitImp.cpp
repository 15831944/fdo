/*
* 
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

#include "stdafx.h"
#include <Fdo\Commands\UnitOfMeasure\ICreateMeasureUnit.h>

#include "FDO\Commands\UnitOfMeasure\mgICreateMeasureUnitImp.h"

FdoICreateMeasureUnit* NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::GetImpObj()
{
    return static_cast<FdoICreateMeasureUnit*>(__super::UnmanagedObject.ToPointer());
}

System::String^ NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::Abbreviation::get()
{
	FdoString* result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetAbbreviation())

	return CHECK_STRING(result);
}

System::Void NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::Abbreviation::set(System::String^ value)
{
	EXCEPTION_HANDLER(GetImpObj()->SetAbbreviation(StringToUni(value)))
}

System::String^ NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::Name::get()
{
	FdoString* result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetName())

	return CHECK_STRING(result);
}

System::Void NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::Name::set(System::String^ value)
{
	EXCEPTION_HANDLER(GetImpObj()->SetName(StringToUni(value)))
}

System::String^ NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::Description::get()
{
	FdoString* result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetDescription())

	return CHECK_STRING(result);
}

System::Void NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::Description::set(System::String^ value)
{
	EXCEPTION_HANDLER(GetImpObj()->SetDescription(StringToUni(value)))
}

NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::BaseUnit NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::BaseUnit::get()
{
	FdoBaseUnit result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetBaseUnit())

	return static_cast<NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::BaseUnit>(result);
}

System::Void NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::BaseUnit::set(NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::BaseUnit value)
{
	EXCEPTION_HANDLER(GetImpObj()->SetBaseUnit(static_cast<FdoBaseUnit>(value)))
}

System::Double NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::ScaleFactor::get()
{
	System::Double result;

	EXCEPTION_HANDLER(result = GetImpObj()->GetScaleFactor())

	return result;
}

System::Void NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::ScaleFactor::set(System::Double value)
{
	EXCEPTION_HANDLER(GetImpObj()->SetScaleFactor(value))
}

System::Boolean NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::UpdateExisting::get()
{
	System::Boolean result;

	EXCEPTION_HANDLER(result = !!GetImpObj()->GetUpdateExisting())

	return result;
}

System::Void NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::UpdateExisting::set(System::Boolean value)
{
	EXCEPTION_HANDLER(GetImpObj()->SetUpdateExisting(value))
}

System::Void NAMESPACE_OSGEO_FDO_COMMANDS_UNITOFMEASURE::ICreateMeasureUnitImp::Execute()
{
	EXCEPTION_HANDLER(GetImpObj()->Execute())
}
