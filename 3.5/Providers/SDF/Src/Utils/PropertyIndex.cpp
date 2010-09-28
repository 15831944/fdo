// 
//  
//  Copyright (C) 2004-2006  Autodesk, Inc.
//  
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//  
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//  
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//  
#include "stdafx.h"
#include "PropertyIndex.h"

PropertyIndex::PropertyIndex(FdoClassDefinition* clas, unsigned int fcid)
{
    
    //inherited properties 
    FdoPtr<FdoReadOnlyPropertyDefinitionCollection> bpdc = clas->GetBaseProperties();

    //class properties 
    FdoPtr<FdoPropertyDefinitionCollection> pdc = clas->GetProperties();

    m_bHasAutoGen = false;

    m_numProps = bpdc->GetCount() + pdc->GetCount();
    m_vProps = new PropertyStub[m_numProps];
	m_lastIndex = 0;
    int index = 0;

    //now iterate over inherited properties 
    for (int i=0; i<bpdc->GetCount(); i++)
    {
        FdoPtr<FdoPropertyDefinition> pd = bpdc->GetItem(i);

        PropertyStub* ps = &m_vProps[index];
        ps->m_name = (wchar_t*)pd->GetName();
        ps->m_recordIndex = index++;
        ps->m_propertyType = pd->GetPropertyType();
        if ( ps->m_propertyType == FdoPropertyType_DataProperty )
        {
			FdoDataPropertyDefinition* dpd = (FdoDataPropertyDefinition*)(pd.p);
            ps->m_dataType = dpd->GetDataType();
            ps->m_isAutoGen = dpd->GetIsAutoGenerated();
            if (ps->m_isAutoGen) 
                m_bHasAutoGen = true;
        }
        else
        {
            // Save data type, or -1 for non data property.
            ps->m_dataType = (FdoDataType)-1;
            ps->m_isAutoGen = false;
        }
        
    }


    //class properties
    for (int i=0; i<pdc->GetCount(); i++)
    {
        FdoPtr<FdoPropertyDefinition> pd = pdc->GetItem(i);    
        
        PropertyStub* ps = &m_vProps[index];
        ps->m_name = (wchar_t*)pd->GetName();
        ps->m_recordIndex = index++;
		ps->m_propertyType = pd->GetPropertyType();
        if ( ps->m_propertyType == FdoPropertyType_DataProperty )
        {
			FdoDataPropertyDefinition* dpd = (FdoDataPropertyDefinition*)(pd.p);
            ps->m_dataType = dpd->GetDataType();
            ps->m_isAutoGen = dpd->GetIsAutoGenerated();
            if (ps->m_isAutoGen) 
                m_bHasAutoGen = true;
        }
        else
        {
            //Save data type, or -1 for non data property.
            ps->m_dataType = (FdoDataType)-1;
            ps->m_isAutoGen = false;
        }
    }


    //find base classes
    m_baseClass = clas;
    
    m_baseFeatureClass = (clas->GetClassType() == FdoClassType_FeatureClass) 
        ? (FdoFeatureClass*)clas : NULL;
    
    FdoPtr<FdoClassDefinition> base = FDO_SAFE_ADDREF(clas);

    while ((base = base->GetBaseClass()) != NULL)
    {
        m_baseClass = base;
        m_baseFeatureClass = (base->GetClassType() == FdoClassType_FeatureClass)
            ? (FdoFeatureClass*)(base.p) : NULL;
    }
    
    FDO_SAFE_ADDREF(m_baseClass);
    FDO_SAFE_ADDREF(m_baseFeatureClass);

    m_fcid = fcid;
}


PropertyIndex::~PropertyIndex()
{
    FDO_SAFE_RELEASE(m_baseClass);
    FDO_SAFE_RELEASE(m_baseFeatureClass);
    
    delete [] m_vProps;
}


PropertyStub* PropertyIndex::GetPropInfo(FdoString* name)
{
    PropertyStub* ps;

    //search for the correct PropertyStub -- iterate linearly
    //since we have relatively few properties. Using a hash_map
    //was slow for this
    for (int i=m_lastIndex; i<m_numProps; i++)
    {
        ps = &m_vProps[i];

        if (wcscmp((wchar_t*)name, ps->m_name) == 0)
		{
			m_lastIndex = i;
            return ps;
		}
    }
	for (int i=0; i<m_lastIndex; i++)
    {
        ps = &m_vProps[i];

        if (wcscmp((wchar_t*)name, ps->m_name) == 0)
		{
			m_lastIndex = i;
            return ps;
		}
    }

    return NULL;
}

bool PropertyIndex::IsPropAutoGen(FdoString* name)
{
    PropertyStub* ps;

    //search for the correct PropertyStub -- iterate linearly
    //since we have relatively few properties. Using a hash_map
    //was slow for this. I guess wcscmp is pretty speedy.
    for (int i=0; i<m_numProps; i++)
    {
        ps = &m_vProps[i];

        if (wcscmp((wchar_t*)name, ps->m_name) == 0)
            break;
    }

    return ps->m_isAutoGen;
}

int PropertyIndex::GetNumProps()
{
    return m_numProps;
}


FdoClassDefinition* PropertyIndex::GetBaseClass()
{
    return m_baseClass;
}

FdoFeatureClass* PropertyIndex::GetBaseFeatureClass()
{
    return m_baseFeatureClass;
}

unsigned int PropertyIndex::GetFCID()
{
    return m_fcid;
}

bool PropertyIndex::HasAutoGen()
{
    return m_bHasAutoGen;
}

PropertyStub* PropertyIndex::GetPropInfo(int index)
{
    if (index < 0 || index >= m_numProps)
        return NULL;

    return &m_vProps[index];
}

FdoGeometricPropertyDefinition* PropertyIndex::FindGeomProp(FdoClassDefinition* clas)
{
    if (clas->GetClassType() != FdoClassType_FeatureClass)
        return NULL;

    FdoPtr<FdoGeometricPropertyDefinition> gpd = ((FdoFeatureClass*)clas)->GetGeometryProperty();
    FdoPtr<FdoClassDefinition> base = FDO_SAFE_ADDREF(clas);

    //navigate up class hierarchy until we find a geom property or 
    //until we reach a NULL base class, in which case the geom prop is NULL
    while (    gpd.p == NULL 
           && (base = base->GetBaseClass()) != NULL )
        gpd = ((FdoFeatureClass*)base.p)->GetGeometryProperty();

    return FDO_SAFE_ADDREF(gpd.p);
}
