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
#include <Fdo\Expression\Parameter.h>

#include "FDO\Expression\mgParameter.h"
#include "FDO\Expression\mgIExpressionProcessorImp.h"

NAMESPACE_OSGEO_FDO_EXPRESSION::Parameter::Parameter(IntPtr unmanaged, Boolean autoDelete) : ValueExpression(unmanaged, autoDelete)
{

}

NAMESPACE_OSGEO_FDO_EXPRESSION::Parameter::Parameter() : ValueExpression(IntPtr::Zero, false)
{
	EXCEPTION_HANDLER(Attach(IntPtr(FdoParameter::Create()), true))
}

NAMESPACE_OSGEO_FDO_EXPRESSION::Parameter::Parameter(System::String^ name) : ValueExpression(IntPtr::Zero, false)
{
	EXCEPTION_HANDLER(Attach(IntPtr(FdoParameter::Create(StringToUni(name))), true)) 
}

FdoParameter* NAMESPACE_OSGEO_FDO_EXPRESSION::Parameter::GetImpObj()
{
	return static_cast<FdoParameter*>(UnmanagedObject.ToPointer());
}

IntPtr NAMESPACE_OSGEO_FDO_EXPRESSION::Parameter::GetDisposableObject()
{
    return IntPtr(static_cast<FdoIDisposable*>(GetImpObj()));
}

System::String^ NAMESPACE_OSGEO_FDO_EXPRESSION::Parameter::Name::get()
{
	FdoString* unstr;
	EXCEPTION_HANDLER(unstr = GetImpObj()->GetName())
	return CHECK_STRING(unstr);
}

System::Void NAMESPACE_OSGEO_FDO_EXPRESSION::Parameter::Name::set(System::String^ value)
{
	EXCEPTION_HANDLER(GetImpObj()->SetName(StringToUni(value)))
}

System::Void NAMESPACE_OSGEO_FDO_EXPRESSION::Parameter::Process(NAMESPACE_OSGEO_FDO_EXPRESSION::IExpressionProcessor^ processor)
{
	EXCEPTION_HANDLER(GetImpObj()->Process((static_cast<NAMESPACE_OSGEO_FDO_EXPRESSION::IExpressionProcessorImp^>(processor))->GetImpObj()))
}

System::String^ NAMESPACE_OSGEO_FDO_EXPRESSION::Parameter::ToString()
{
	FdoString* unstr;
	EXCEPTION_HANDLER(unstr = GetImpObj()->ToString())
	return CHECK_STRING(unstr);
}
