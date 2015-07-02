/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace OSGeo.GDAL {

using System;
using System.Runtime.InteropServices;

public class ColorTable : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;
  protected object swigParentRef;
  
  protected static object ThisOwn_true() { return null; }
  protected object ThisOwn_false() { return this; }

  public ColorTable(IntPtr cPtr, bool cMemoryOwn, object parent) {
    swigCMemOwn = cMemoryOwn;
    swigParentRef = parent;
    swigCPtr = new HandleRef(this, cPtr);
  }

  public static HandleRef getCPtr(ColorTable obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }
  public static HandleRef getCPtrAndDisown(ColorTable obj, object parent) {
    if (obj != null)
    {
      obj.swigCMemOwn = false;
      obj.swigParentRef = parent;
      return obj.swigCPtr;
    }
    else
    {
      return new HandleRef(null, IntPtr.Zero);
    }
  }
  public static HandleRef getCPtrAndSetReference(ColorTable obj, object parent) {
    if (obj != null)
    {
      obj.swigParentRef = parent;
      return obj.swigCPtr;
    }
    else
    {
      return new HandleRef(null, IntPtr.Zero);
    }
  }

  ~ColorTable() {
    Dispose();
  }

  public virtual void Dispose() {
  lock(this) {
      if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
        swigCMemOwn = false;
        GdalPINVOKE.delete_ColorTable(swigCPtr);
      }
      swigCPtr = new HandleRef(null, IntPtr.Zero);
      swigParentRef = null;
      GC.SuppressFinalize(this);
    }
  }

  public ColorTable(PaletteInterp palette) : this(GdalPINVOKE.new_ColorTable((int)palette), true, null) {
    if (GdalPINVOKE.SWIGPendingException.Pending) throw GdalPINVOKE.SWIGPendingException.Retrieve();
  }

  public ColorTable Clone() {
    IntPtr cPtr = GdalPINVOKE.ColorTable_Clone(swigCPtr);
    ColorTable ret = (cPtr == IntPtr.Zero) ? null : new ColorTable(cPtr, true, ThisOwn_true());
    if (GdalPINVOKE.SWIGPendingException.Pending) throw GdalPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public PaletteInterp GetPaletteInterpretation() {
    PaletteInterp ret = (PaletteInterp)GdalPINVOKE.ColorTable_GetPaletteInterpretation(swigCPtr);
    if (GdalPINVOKE.SWIGPendingException.Pending) throw GdalPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int GetCount() {
    int ret = GdalPINVOKE.ColorTable_GetCount(swigCPtr);
    if (GdalPINVOKE.SWIGPendingException.Pending) throw GdalPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public ColorEntry GetColorEntry(int entry) {
    IntPtr cPtr = GdalPINVOKE.ColorTable_GetColorEntry(swigCPtr, entry);
    ColorEntry ret = (cPtr == IntPtr.Zero) ? null : new ColorEntry(cPtr, false, ThisOwn_false());
    if (GdalPINVOKE.SWIGPendingException.Pending) throw GdalPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int GetColorEntryAsRGB(int entry, ColorEntry centry) {
    int ret = GdalPINVOKE.ColorTable_GetColorEntryAsRGB(swigCPtr, entry, ColorEntry.getCPtr(centry));
    if (GdalPINVOKE.SWIGPendingException.Pending) throw GdalPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void SetColorEntry(int entry, ColorEntry centry) {
    GdalPINVOKE.ColorTable_SetColorEntry(swigCPtr, entry, ColorEntry.getCPtr(centry));
    if (GdalPINVOKE.SWIGPendingException.Pending) throw GdalPINVOKE.SWIGPendingException.Retrieve();
  }

  public void CreateColorRamp(int nStartIndex, ColorEntry startcolor, int nEndIndex, ColorEntry endcolor) {
    GdalPINVOKE.ColorTable_CreateColorRamp(swigCPtr, nStartIndex, ColorEntry.getCPtr(startcolor), nEndIndex, ColorEntry.getCPtr(endcolor));
    if (GdalPINVOKE.SWIGPendingException.Pending) throw GdalPINVOKE.SWIGPendingException.Retrieve();
  }

}

}
