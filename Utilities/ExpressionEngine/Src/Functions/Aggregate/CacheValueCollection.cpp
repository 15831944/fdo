// 
//  
//  Copyright (C) 2004-2007  Autodesk, Inc.
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

#include <stdafx.h>
#include <Functions/Aggregate/CacheValueCollection.h>

CacheValueCollection *CacheValueCollection::Create ()
{

	return new CacheValueCollection();

}  //  Create ()

CacheValueCollection *CacheValueCollection::Create (CacheValue** arguments, FdoInt32 length)
{

    CacheValueCollection *pColl = new CacheValueCollection();

    try {

      for (FdoInt32 i = 0; i < length; i++)
        pColl->Add(arguments[i]);

    }  //  try ...

    catch (FdoException* pExcept) {

      FDO_SAFE_RELEASE(pColl);
      throw pExcept;

    }  //  catch ...

    return pColl;

}  //  Create ()

void CacheValueCollection::Dispose()
{

    delete this;

}  //  Dispose ()

