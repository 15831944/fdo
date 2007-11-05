/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.31
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace OSGeo.OGR {

using System;
using System.Runtime.InteropServices;

public class FieldDefn : IDisposable {
  private HandleRef swigCPtr;
  protected object swigCMemOwner;

  internal FieldDefn(IntPtr cPtr, object cMemoryOwner) {
    swigCMemOwner = cMemoryOwner;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(FieldDefn obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }
  internal static HandleRef getCPtrAndDisown(FieldDefn obj, object cMemoryOwner) {
    obj.swigCMemOwner = cMemoryOwner;
    return getCPtr(obj);
  }

  ~FieldDefn() {
    Dispose();
  }

  public virtual void Dispose() {
  lock(this) {
      if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwner == null) {
        swigCMemOwner = new object();
        ogrPINVOKE.delete_FieldDefn(swigCPtr);
      }
      swigCPtr = new HandleRef(null, IntPtr.Zero);
      GC.SuppressFinalize(this);
    }
  }

  public FieldDefn(string name, int field_type) : this(ogrPINVOKE.new_FieldDefn(name, field_type), null) {
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
  }

  public string GetName() {
    string ret = ogrPINVOKE.FieldDefn_GetName(swigCPtr);
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string GetNameRef() {
    string ret = ogrPINVOKE.FieldDefn_GetNameRef(swigCPtr);
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void SetName(string name) {
    ogrPINVOKE.FieldDefn_SetName(swigCPtr, name);
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
  }

  public int GetFieldType() {
    int ret = ogrPINVOKE.FieldDefn_GetFieldType(swigCPtr);
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void SetType(int type) {
    ogrPINVOKE.FieldDefn_SetType(swigCPtr, type);
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
  }

  public int GetJustify() {
    int ret = ogrPINVOKE.FieldDefn_GetJustify(swigCPtr);
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void SetJustify(int justify) {
    ogrPINVOKE.FieldDefn_SetJustify(swigCPtr, justify);
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
  }

  public int GetWidth() {
    int ret = ogrPINVOKE.FieldDefn_GetWidth(swigCPtr);
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void SetWidth(int width) {
    ogrPINVOKE.FieldDefn_SetWidth(swigCPtr, width);
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
  }

  public int GetPrecision() {
    int ret = ogrPINVOKE.FieldDefn_GetPrecision(swigCPtr);
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void SetPrecision(int precision) {
    ogrPINVOKE.FieldDefn_SetPrecision(swigCPtr, precision);
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
  }

  public string GetFieldTypeName(int type) {
    string ret = ogrPINVOKE.FieldDefn_GetFieldTypeName(swigCPtr, type);
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}

}
