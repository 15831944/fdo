/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.31
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace OSGeo.GDAL {

using System;
using System.Runtime.InteropServices;

public class XMLNode : IDisposable {
  private HandleRef swigCPtr;
  protected object swigCMemOwner;

  internal XMLNode(IntPtr cPtr, object cMemoryOwner) {
    swigCMemOwner = cMemoryOwner;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(XMLNode obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }
  internal static HandleRef getCPtrAndDisown(XMLNode obj, object cMemoryOwner) {
    obj.swigCMemOwner = cMemoryOwner;
    return getCPtr(obj);
  }

  ~XMLNode() {
    Dispose();
  }

  public virtual void Dispose() {
  lock(this) {
      if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwner == null) {
        swigCMemOwner = new object();
        gdalPINVOKE.delete_XMLNode(swigCPtr);
      }
      swigCPtr = new HandleRef(null, IntPtr.Zero);
      GC.SuppressFinalize(this);
    }
  }

  public XMLNodeType Type {
    get {
      XMLNodeType ret = (XMLNodeType)gdalPINVOKE.XMLNode_Type_get(swigCPtr);
      if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public string Value {
    get {
      string ret = gdalPINVOKE.XMLNode_Value_get(swigCPtr);
      if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public XMLNode Next {
    get {
      IntPtr cPtr = gdalPINVOKE.XMLNode_Next_get(swigCPtr);
      XMLNode ret = (cPtr == IntPtr.Zero) ? null : new XMLNode(cPtr, false? null : this);
      if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public XMLNode Child {
    get {
      IntPtr cPtr = gdalPINVOKE.XMLNode_Child_get(swigCPtr);
      XMLNode ret = (cPtr == IntPtr.Zero) ? null : new XMLNode(cPtr, false? null : this);
      if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public XMLNode(string pszString) : this(gdalPINVOKE.new_XMLNode(pszString), null) {
    if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
  }

  public string SerializeXMLTree() {
    string ret = gdalPINVOKE.XMLNode_SerializeXMLTree(swigCPtr);
    if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public XMLNode SearchXMLNode(string pszElement) {
    IntPtr cPtr = gdalPINVOKE.XMLNode_SearchXMLNode(swigCPtr, pszElement);
    XMLNode ret = (cPtr == IntPtr.Zero) ? null : new XMLNode(cPtr, false? null : this);
    if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public XMLNode GetXMLNode(string pszPath) {
    IntPtr cPtr = gdalPINVOKE.XMLNode_GetXMLNode(swigCPtr, pszPath);
    XMLNode ret = (cPtr == IntPtr.Zero) ? null : new XMLNode(cPtr, false? null : this);
    if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string GetXMLValue(string pszPath, string pszDefault) {
    string ret = gdalPINVOKE.XMLNode_GetXMLValue(swigCPtr, pszPath, pszDefault);
    if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void AddXMLChild(XMLNode psChild) {
    gdalPINVOKE.XMLNode_AddXMLChild(swigCPtr, XMLNode.getCPtr(psChild));
    if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
  }

  public int RemoveXMLChild(XMLNode psChild) {
    int ret = gdalPINVOKE.XMLNode_RemoveXMLChild(swigCPtr, XMLNode.getCPtr(psChild));
    if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void AddXMLSibling(XMLNode psNewSibling) {
    gdalPINVOKE.XMLNode_AddXMLSibling(swigCPtr, XMLNode.getCPtr(psNewSibling));
    if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
  }

  public XMLNode CreateXMLElementAndValue(string pszName, string pszValue) {
    IntPtr cPtr = gdalPINVOKE.XMLNode_CreateXMLElementAndValue(swigCPtr, pszName, pszValue);
    XMLNode ret = (cPtr == IntPtr.Zero) ? null : new XMLNode(cPtr, false? null : this);
    if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public XMLNode CloneXMLTree(XMLNode psTree) {
    IntPtr cPtr = gdalPINVOKE.XMLNode_CloneXMLTree(swigCPtr, XMLNode.getCPtr(psTree));
    XMLNode ret = (cPtr == IntPtr.Zero) ? null : new XMLNode(cPtr, true? null : this);
    if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int SetXMLValue(string pszPath, string pszValue) {
    int ret = gdalPINVOKE.XMLNode_SetXMLValue(swigCPtr, pszPath, pszValue);
    if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void StripXMLNamespace(string pszNamespace, int bRecurse) {
    gdalPINVOKE.XMLNode_StripXMLNamespace(swigCPtr, pszNamespace, bRecurse);
    if (gdalPINVOKE.SWIGPendingException.Pending) throw gdalPINVOKE.SWIGPendingException.Retrieve();
  }

}

}
