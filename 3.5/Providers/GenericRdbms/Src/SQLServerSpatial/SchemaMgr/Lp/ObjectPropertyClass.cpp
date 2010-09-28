/*
 * Copyright (C) 2004-2008  Autodesk, Inc.
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
#include "ObjectPropertyClass.h"

FdoSmLpSqsObjectPropertyClass::FdoSmLpSqsObjectPropertyClass(
	FdoSmLpObjectPropertyDefinition* pParent,
	FdoSmLpClassDefinition* pParentType,
	FdoSmLpPropertyMappingDefinition* pMapping,
	FdoSmLpPropertyMappingType mappingType,
    FdoPhysicalClassMapping* pOverrides
) :
	FdoSmLpGrdObjectPropertyClass(pParent, pParentType, pMapping, mappingType, pOverrides),
	FdoSmLpSqsClassDefinition(	MakeName(pParent),
						L"Autogenerated Object Property Class",
						pMapping,
						NULL,
						FdoSmOvTableMappingType_ConcreteTable,
						pParent->GetContainingDbObjectName(),
						pParent->GetContainingDbObject(),
						pParent->GetElementState()
	),
	FdoSmLpClassBase(	MakeName(pParent),
						L"Autogenerated Object Property Class",
						pMapping,
						NULL,
						FdoSmOvTableMappingType_ConcreteTable,
						pParent->GetContainingDbObjectName(),
						pParent->GetContainingDbObject(),
						pParent->GetElementState(),
                        pParent->GetIsFromFdo()
	)
{
}

FdoSmLpSqsObjectPropertyClass::FdoSmLpSqsObjectPropertyClass(
	FdoSmLpObjectPropertyClassP pBase,
	FdoSmLpObjectPropertyDefinition* pParent,
	FdoSmLpClassDefinition* pParentType,
	FdoSmLpPropertyMappingDefinition* pMapping,
	FdoSmLpPropertyMappingType mappingType,
    FdoPhysicalClassMapping* pOverrides
) :
	FdoSmLpGrdObjectPropertyClass(pBase, pParent, pParentType, pMapping, mappingType, pOverrides),
	FdoSmLpSqsClassDefinition(	MakeName(pParent),
						L"Autogenerated Object Property Class",
						pMapping,
						pBase.p->SmartCast<FdoSmLpClassDefinition>(),
						FdoSmOvTableMappingType_ConcreteTable,
						pParent->GetContainingDbObjectName(),
						pParent->GetContainingDbObject(),
						pParent->GetElementState()
	),
	FdoSmLpClassBase(	MakeName(pParent),
						L"Autogenerated Object Property Class",
						pMapping,
						pBase.p->SmartCast<FdoSmLpClassDefinition>(),
						FdoSmOvTableMappingType_ConcreteTable,
						pParent->GetContainingDbObjectName(),
						pParent->GetContainingDbObject(),
						pParent->GetElementState(),
                        pParent->GetIsFromFdo()
	)
{
}

FdoSmLpSqsObjectPropertyClass::~FdoSmLpSqsObjectPropertyClass(void)
{
}

void FdoSmLpSqsObjectPropertyClass::Update(
    FdoClassDefinition* pFdoClass,
    FdoSchemaElementState elementState,
    FdoPhysicalSchemaMapping* pOverrides,
    bool bIgnoreStates
)
{
    FdoSmLpGrdClassDefinition::Update( pFdoClass, elementState, pOverrides, bIgnoreStates );
    FdoSmLpGrdObjectPropertyClass::Update( pFdoClass, elementState, pOverrides, bIgnoreStates );
}

void FdoSmLpSqsObjectPropertyClass::PostFinalize()
{
    FdoSmLpGrdObjectPropertyClass::PostFinalize();
    FdoSmLpSqsClassDefinition::PostFinalize();
}