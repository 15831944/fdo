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
#include <Fdo\Commands\CommandType.h>
#include <Fdo\Expression\ComputedIdentifier.h>

#include "FDO\Expression\mgComputedIdentifier.h"
#include "FDO\Expression\mgExpression.h"
#include "FDO\Expression\mgIExpressionProcessorImp.h"
#include "FDO\mgObjectFactory.h"

NAMESPACE_OSGEO_FDO_EXPRESSION::ComputedIdentifier::ComputedIdentifier(IntPtr unmanaged, Boolean autoDelete) : Identifier(unmanaged, autoDelete)
{

}

NAMESPACE_OSGEO_FDO_EXPRESSION::ComputedIdentifier::ComputedIdentifier() : Identifier(IntPtr::Zero, false)
{
	EXCEPTION_HANDLER(Attach(IntPtr(FdoComputedIdentifier::Create()), true))
}

NAMESPACE_OSGEO_FDO_EXPRESSION::ComputedIdentifier::ComputedIdentifier(System::String^ name, NAMESPACE_OSGEO_FDO_EXPRESSION::Expression^ expression) : Identifier(IntPtr::Zero, false)
{
	EXCEPTION_HANDLER(Attach(IntPtr(FdoComputedIdentifier::Create(StringToUni(name), expression->GetImpObj())), true))
}

FdoComputedIdentifier* NAMESPACE_OSGEO_FDO_EXPRESSION::ComputedIdentifier::GetImpObj()
{
	return static_cast<FdoComputedIdentifier*>(UnmanagedObject.ToPointer());
}

IntPtr NAMESPACE_OSGEO_FDO_EXPRESSION::ComputedIdentifier::GetDisposableObject()
{
    return IntPtr(static_cast<FdoIDisposable*>(GetImpObj()));
}

NAMESPACE_OSGEO_FDO_EXPRESSION::Expression^ NAMESPACE_OSGEO_FDO_EXPRESSION::ComputedIdentifier::Expression::get()
{
	FdoExpression* result;
    EXCEPTION_HANDLER(result = NAMESPACE_OSGEO_FDO_EXPRESSION::ComputedIdentifier::GetImpObj()->GetExpression())
	return static_cast<NAMESPACE_OSGEO_FDO_EXPRESSION::Expression^>(ObjectFactory::CreateExpression(IntPtr(result), true));
}

System::Void NAMESPACE_OSGEO_FDO_EXPRESSION::ComputedIdentifier::Expression::set(NAMESPACE_OSGEO_FDO_EXPRESSION::Expression^ value)
{
	EXCEPTION_HANDLER(NAMESPACE_OSGEO_FDO_EXPRESSION::ComputedIdentifier::GetImpObj()->SetExpression((value == nullptr ? nullptr : value->GetImpObj())))
}

System::Void NAMESPACE_OSGEO_FDO_EXPRESSION::ComputedIdentifier::Process(NAMESPACE_OSGEO_FDO_EXPRESSION::IExpressionProcessor^ processor)
{
	EXCEPTION_HANDLER(GetImpObj()->Process((static_cast<NAMESPACE_OSGEO_FDO_EXPRESSION::IExpressionProcessorImp^>(processor))->GetImpObj()))
}

System::String^ NAMESPACE_OSGEO_FDO_EXPRESSION::ComputedIdentifier::ToString()
{
	FdoString* unstr;
	EXCEPTION_HANDLER(unstr = GetImpObj()->ToString())
	return CHECK_STRING(unstr);
}
