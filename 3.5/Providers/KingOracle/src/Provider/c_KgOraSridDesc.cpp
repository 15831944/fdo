/*
* Copyright (C) 2006  SL-King d.o.o
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
*/


#include "stdafx.h"
#include "c_KgOraSridDesc.h"

c_KgOraSridDesc::c_KgOraSridDesc(void)
{
  m_OraSrid = 0;
  m_IsGeodetic = false;  
  
  m_SDE_FalseX = 0.0;
  m_SDE_FalseY = 0.0;
  m_SDE_XYUnit = 1.0;
  
  m_SDE_FalseZ = 0.0;
  m_SDE_ZUnit = 1.0;
  m_SDE_FalseM = 0.0;
  m_SDE_MUnit = 1.0;
}

c_KgOraSridDesc::~c_KgOraSridDesc(void)
{
}