/*
 * The Apache Software License, Version 1.1
 *
 *
 * Copyright (c) 1999-2004 The Apache Software Foundation.  All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Xalan" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written 
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation and was
 * originally based on software copyright (c) 1999, International
 * Business Machines, Inc., http://www.ibm.com.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */
#include "XercesCommentWrapper.hpp"



#include <xercesc/dom/DOMComment.hpp>



#include <xalanc/DOMSupport/DOMServices.hpp>



#include "XercesWrapperHelper.hpp"
#include "XercesWrapperNavigator.hpp"
#include "XercesDOMWrapperException.hpp"
#include "XercesDocumentWrapper.hpp"



XALAN_CPP_NAMESPACE_BEGIN



XercesCommentWrapper::XercesCommentWrapper(
			const DOMCommentType*			theXercesComment,
			const XercesWrapperNavigator&	theNavigator) :
	XalanComment(),
	m_xercesNode(theXercesComment),
	m_navigator(theNavigator)
{
	assert(theXercesComment != 0);
}



XercesCommentWrapper::~XercesCommentWrapper()
{
}



const XalanDOMString&
XercesCommentWrapper::getNodeName() const
{
	return m_navigator.getPooledString(m_xercesNode->getNodeName());
}



const XalanDOMString&
XercesCommentWrapper::getNodeValue() const
{
	return m_navigator.getPooledString(m_xercesNode->getNodeValue());
}



XercesCommentWrapper::NodeType
XercesCommentWrapper::getNodeType() const
{
	return COMMENT_NODE;
}



XalanNode*
XercesCommentWrapper::getParentNode() const
{
	return m_navigator.getParentNode(m_xercesNode);
}



const XalanNodeList*
XercesCommentWrapper::getChildNodes() const
{
	return 0;
}



XalanNode*
XercesCommentWrapper::getFirstChild() const
{
	return 0;
}



XalanNode*
XercesCommentWrapper::getLastChild() const
{
	return 0;
}



XalanNode*
XercesCommentWrapper::getPreviousSibling() const
{
	return m_navigator.getPreviousSibling(m_xercesNode);
}



XalanNode*
XercesCommentWrapper::getNextSibling() const
{
	return m_navigator.getNextSibling(m_xercesNode);
}



const XalanNamedNodeMap*
XercesCommentWrapper::getAttributes() const
{
	return 0;
}



XalanDocument*
XercesCommentWrapper::getOwnerDocument() const
{
	return m_navigator.getOwnerDocument();
}



#if defined(XALAN_NO_COVARIANT_RETURN_TYPE)
XalanNode*
#else
XercesCommentWrapper*
#endif
XercesCommentWrapper::cloneNode(bool	/* deep */) const
{
	throw XercesDOMWrapperException(XercesDOMWrapperException::NOT_SUPPORTED_ERR);

	return 0;
}



XalanNode*
XercesCommentWrapper::insertBefore(
			XalanNode*	/* newChild */,
			XalanNode*	/* refChild */)
{
	throw XercesDOMWrapperException(XercesDOMWrapperException::HIERARCHY_REQUEST_ERR);

	return 0;
}



XalanNode*
XercesCommentWrapper::replaceChild(
			XalanNode*	/* newChild */,
			XalanNode*	/* oldChild */)
{
	throw XercesDOMWrapperException(XercesDOMWrapperException::HIERARCHY_REQUEST_ERR);

	return 0;
}



XalanNode*
XercesCommentWrapper::removeChild(XalanNode*	/* oldChild */)
{
	throw XercesDOMWrapperException(XercesDOMWrapperException::HIERARCHY_REQUEST_ERR);

	return 0;
}



XalanNode*
XercesCommentWrapper::appendChild(XalanNode*	 /* newChild */)
{
	throw XercesDOMWrapperException(XercesDOMWrapperException::HIERARCHY_REQUEST_ERR);

	return 0;
}



bool
XercesCommentWrapper::hasChildNodes() const
{
	return false;
}



void
XercesCommentWrapper::setNodeValue(const XalanDOMString&	/* nodeValue */)
{
	throw XalanDOMException(XalanDOMException::NO_MODIFICATION_ALLOWED_ERR);
}



void
XercesCommentWrapper::normalize()
{
	throw XalanDOMException(XalanDOMException::NO_MODIFICATION_ALLOWED_ERR);
}



bool
XercesCommentWrapper::isSupported(
			const XalanDOMString&	feature,
			const XalanDOMString&	version) const
{
	return XercesWrapperHelper::isSupported(m_xercesNode, feature, version);
}



const XalanDOMString&
XercesCommentWrapper::getNamespaceURI() const
{
	return m_navigator.getPooledString(m_xercesNode->getNamespaceURI());
}



const XalanDOMString&
XercesCommentWrapper::getPrefix() const
{
	return m_navigator.getPooledString(m_xercesNode->getPrefix());
}



const XalanDOMString&
XercesCommentWrapper::getLocalName() const
{
	return m_navigator.getPooledString(m_xercesNode->getLocalName());
}



void
XercesCommentWrapper::setPrefix(const XalanDOMString&	/* prefix */)
{
	throw XalanDOMException(XalanDOMException::NO_MODIFICATION_ALLOWED_ERR);
}



bool
XercesCommentWrapper::isIndexed() const
{
	return m_navigator.getOwnerDocument()->isIndexed();
}



XercesCommentWrapper::IndexType
XercesCommentWrapper::getIndex() const
{
	return m_navigator.getIndex();
}



const XalanDOMString&
XercesCommentWrapper::getData() const
{
	return m_navigator.getPooledString(m_xercesNode->getData());
}



unsigned int
XercesCommentWrapper::getLength() const
{
	return m_xercesNode->getLength();
}



XalanDOMString
XercesCommentWrapper::substringData(
			unsigned int	offset, 
			unsigned int	count) const
{
	return XercesWrapperHelper::substringData(m_xercesNode, offset, count);
}



void
XercesCommentWrapper::appendData(const XalanDOMString&		/* arg */)
{
	throw XalanDOMException(XalanDOMException::NO_MODIFICATION_ALLOWED_ERR);
}



void
XercesCommentWrapper::insertData(
			unsigned int			/* offset */,
			const  XalanDOMString& 	/* arg */)
{
	throw XalanDOMException(XalanDOMException::NO_MODIFICATION_ALLOWED_ERR);
}



void
XercesCommentWrapper::deleteData(
			unsigned int	/* offset */,
			unsigned int	/* count */)
{
	throw XalanDOMException(XalanDOMException::NO_MODIFICATION_ALLOWED_ERR);
}



void
XercesCommentWrapper::replaceData(
			unsigned int			/* offset */,
			unsigned int			/* count */,
			const XalanDOMString&	/* arg */)
{
	throw XalanDOMException(XalanDOMException::NO_MODIFICATION_ALLOWED_ERR);
}



XALAN_CPP_NAMESPACE_END