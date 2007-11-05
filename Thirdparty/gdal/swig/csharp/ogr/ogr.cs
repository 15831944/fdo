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

public class ogr {
  public static void UseExceptions() {
    ogrPINVOKE.UseExceptions();
  }

  public static void DontUseExceptions() {
    ogrPINVOKE.DontUseExceptions();
  }

  public static Geometry CreateGeometryFromWkb(int len, SpatialReference reference) {
    IntPtr cPtr = ogrPINVOKE.CreateGeometryFromWkb(len, SpatialReference.getCPtr(reference));
    Geometry ret = (cPtr == IntPtr.Zero) ? null : new Geometry(cPtr, true? null : new object());
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Geometry CreateGeometryFromWkt(SWIGTYPE_p_p_char val, SpatialReference reference) {
    IntPtr cPtr = ogrPINVOKE.CreateGeometryFromWkt(SWIGTYPE_p_p_char.getCPtr(val), SpatialReference.getCPtr(reference));
    Geometry ret = (cPtr == IntPtr.Zero) ? null : new Geometry(cPtr, true? null : new object());
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Geometry CreateGeometryFromGML(string input_string) {
    IntPtr cPtr = ogrPINVOKE.CreateGeometryFromGML(input_string);
    Geometry ret = (cPtr == IntPtr.Zero) ? null : new Geometry(cPtr, true? null : new object());
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static int GetDriverCount() {
    int ret = ogrPINVOKE.GetDriverCount();
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static int GetOpenDSCount() {
    int ret = ogrPINVOKE.GetOpenDSCount();
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static int SetGenerate_DB2_V72_BYTE_ORDER(int bGenerate_DB2_V72_BYTE_ORDER) {
    int ret = ogrPINVOKE.SetGenerate_DB2_V72_BYTE_ORDER(bGenerate_DB2_V72_BYTE_ORDER);
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static void RegisterAll() {
    ogrPINVOKE.RegisterAll();
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
  }

  public static DataSource GetOpenDS(int ds_number) {
    IntPtr cPtr = ogrPINVOKE.GetOpenDS(ds_number);
    DataSource ret = (cPtr == IntPtr.Zero) ? null : new DataSource(cPtr, false? null : new object());
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static DataSource Open(string filename, int update) {
    IntPtr cPtr = ogrPINVOKE.Open(filename, update);
    DataSource ret = (cPtr == IntPtr.Zero) ? null : new DataSource(cPtr, true? null : new object());
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static DataSource OpenShared(string filename, int update) {
    IntPtr cPtr = ogrPINVOKE.OpenShared(filename, update);
    DataSource ret = (cPtr == IntPtr.Zero) ? null : new DataSource(cPtr, true? null : new object());
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Driver GetDriverByName(string name) {
    IntPtr cPtr = ogrPINVOKE.GetDriverByName(name);
    Driver ret = (cPtr == IntPtr.Zero) ? null : new Driver(cPtr, false? null : new object());
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Driver GetDriver(int driver_number) {
    IntPtr cPtr = ogrPINVOKE.GetDriver(driver_number);
    Driver ret = (cPtr == IntPtr.Zero) ? null : new Driver(cPtr, false? null : new object());
    if (ogrPINVOKE.SWIGPendingException.Pending) throw ogrPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static readonly int wkb25Bit = ogrPINVOKE.wkb25Bit_get();
  public static readonly int wkbUnknown = ogrPINVOKE.wkbUnknown_get();
  public static readonly int wkbPoint = ogrPINVOKE.wkbPoint_get();
  public static readonly int wkbLineString = ogrPINVOKE.wkbLineString_get();
  public static readonly int wkbPolygon = ogrPINVOKE.wkbPolygon_get();
  public static readonly int wkbMultiPoint = ogrPINVOKE.wkbMultiPoint_get();
  public static readonly int wkbMultiLineString = ogrPINVOKE.wkbMultiLineString_get();
  public static readonly int wkbMultiPolygon = ogrPINVOKE.wkbMultiPolygon_get();
  public static readonly int wkbGeometryCollection = ogrPINVOKE.wkbGeometryCollection_get();
  public static readonly int wkbNone = ogrPINVOKE.wkbNone_get();
  public static readonly int wkbLinearRing = ogrPINVOKE.wkbLinearRing_get();
  public static readonly int wkbPoint25D = ogrPINVOKE.wkbPoint25D_get();
  public static readonly int wkbLineString25D = ogrPINVOKE.wkbLineString25D_get();
  public static readonly int wkbPolygon25D = ogrPINVOKE.wkbPolygon25D_get();
  public static readonly int wkbMultiPoint25D = ogrPINVOKE.wkbMultiPoint25D_get();
  public static readonly int wkbMultiLineString25D = ogrPINVOKE.wkbMultiLineString25D_get();
  public static readonly int wkbMultiPolygon25D = ogrPINVOKE.wkbMultiPolygon25D_get();
  public static readonly int wkbGeometryCollection25D = ogrPINVOKE.wkbGeometryCollection25D_get();
  public static readonly int OFTInteger = ogrPINVOKE.OFTInteger_get();
  public static readonly int OFTIntegerList = ogrPINVOKE.OFTIntegerList_get();
  public static readonly int OFTReal = ogrPINVOKE.OFTReal_get();
  public static readonly int OFTRealList = ogrPINVOKE.OFTRealList_get();
  public static readonly int OFTString = ogrPINVOKE.OFTString_get();
  public static readonly int OFTStringList = ogrPINVOKE.OFTStringList_get();
  public static readonly int OFTWideString = ogrPINVOKE.OFTWideString_get();
  public static readonly int OFTWideStringList = ogrPINVOKE.OFTWideStringList_get();
  public static readonly int OFTBinary = ogrPINVOKE.OFTBinary_get();
  public static readonly int OFTDate = ogrPINVOKE.OFTDate_get();
  public static readonly int OFTTime = ogrPINVOKE.OFTTime_get();
  public static readonly int OFTDateTime = ogrPINVOKE.OFTDateTime_get();
  public static readonly int OJUndefined = ogrPINVOKE.OJUndefined_get();
  public static readonly int OJLeft = ogrPINVOKE.OJLeft_get();
  public static readonly int OJRight = ogrPINVOKE.OJRight_get();
  public static readonly int wkbXDR = ogrPINVOKE.wkbXDR_get();
  public static readonly int wkbNDR = ogrPINVOKE.wkbNDR_get();
  public static readonly string OLCRandomRead = ogrPINVOKE.OLCRandomRead_get();
  public static readonly string OLCSequentialWrite = ogrPINVOKE.OLCSequentialWrite_get();
  public static readonly string OLCRandomWrite = ogrPINVOKE.OLCRandomWrite_get();
  public static readonly string OLCFastSpatialFilter = ogrPINVOKE.OLCFastSpatialFilter_get();
  public static readonly string OLCFastFeatureCount = ogrPINVOKE.OLCFastFeatureCount_get();
  public static readonly string OLCFastGetExtent = ogrPINVOKE.OLCFastGetExtent_get();
  public static readonly string OLCCreateField = ogrPINVOKE.OLCCreateField_get();
  public static readonly string OLCTransactions = ogrPINVOKE.OLCTransactions_get();
  public static readonly string OLCDeleteFeature = ogrPINVOKE.OLCDeleteFeature_get();
  public static readonly string OLCFastSetNextByIndex = ogrPINVOKE.OLCFastSetNextByIndex_get();
  public static readonly string ODsCCreateLayer = ogrPINVOKE.ODsCCreateLayer_get();
  public static readonly string ODsCDeleteLayer = ogrPINVOKE.ODsCDeleteLayer_get();
  public static readonly string ODrCCreateDataSource = ogrPINVOKE.ODrCCreateDataSource_get();
  public static readonly string ODrCDeleteDataSource = ogrPINVOKE.ODrCDeleteDataSource_get();
}

}
