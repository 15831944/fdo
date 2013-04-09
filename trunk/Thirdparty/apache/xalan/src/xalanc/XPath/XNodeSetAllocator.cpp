/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the  "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Class header file.
#include "XNodeSetAllocator.hpp"



XALAN_CPP_NAMESPACE_BEGIN



XNodeSetAllocator::XNodeSetAllocator(MemoryManager&      theManager, size_type  theBlockCount) :
    m_allocator(theManager, theBlockCount)
{
}



XNodeSetAllocator::~XNodeSetAllocator()
{
}




XNodeSetAllocator::nodeset_type*
XNodeSetAllocator::createNodeSet(BorrowReturnMutableNodeRefList&    value)
{
    nodeset_type* const theBlock = m_allocator.allocateBlock();
    assert(theBlock != 0);

    nodeset_type* const theResult = new(theBlock) nodeset_type(value, m_allocator.getMemoryManager());

    m_allocator.commitAllocation(theBlock);

    return theResult;
}






bool
XNodeSetAllocator::destroy(nodeset_type*    theNodeSet)
{
    return m_allocator.destroyObject(theNodeSet);
}



void 
XNodeSetAllocator::reset()
{
    m_allocator.reset();
}



XALAN_CPP_NAMESPACE_END
