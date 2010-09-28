@echo off

rem 
rem Copyright (C) 2004-2006  Autodesk, Inc.
rem 
rem This library is free software; you can redistribute it and/or
rem modify it under the terms of version 2.1 of the GNU Lesser
rem General Public License as published by the Free Software Foundation.
rem 
rem This library is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
rem Lesser General Public License for more details.
rem 
rem You should have received a copy of the GNU Lesser General Public
rem License along with this library; if not, write to the Free Software
rem Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
rem 

SET DEFMODIFYCHK=no
SET FDOALLENABLECHK=yes
SET FDOCOREENABLECHK=no
SET FDOPROVIDERSENABLECHK=no
SET THRPENABLECHK=no
SET FDOENABLECHK=no
SET UTILENABLECHK=no
SET SHPENABLECHK=no
SET SDFENABLECHK=no
SET WFSENABLECHK=no
SET WMSENABLECHK=no
SET ARCENABLECHK=no
SET RDBMSENABLECHK=no
SET GDALENABLECHK=no
SET KINGORACLEENABLECHK=no
SET KINGSPATIALENABLECHK=no
SET OGRENABLECHK=no
SET POSTGISENABLECHK=no
SET SQLITEENABLECHK=no

if (%FDO_SVN_SOURCEDIR%)==() SET FDO_SVN_SOURCEDIR=trunk
if (%FDO_SVN_DESTDIR%)==() SET FDO_SVN_DESTDIR=%cd%

:study_params
if (%1)==() goto start_checkout

if "%1"=="-help"    goto help_show
if "%1"=="-h"       goto help_show

if "%1"=="-s"       goto get_source
if "%1"=="-source"  goto get_source

if "%1"=="-o"       goto get_path
if "%1"=="-outpath" goto get_path

if "%1"=="-w"       goto get_with
if "%1"=="-with"    goto get_with

goto custom_error

:get_with
if "%DEFMODIFYCHK%"=="yes" goto stp0_get_with
	SET DEFMODIFYCHK=yes
	SET FDOALLENABLECHK=no
	SET FDOPROVIDERSENABLECHK=no
	SET THRPENABLECHK=no
	SET FDOCOREENABLECHK=no
	SET FDOENABLECHK=no
	SET SHPENABLECHK=no
	SET SDFENABLECHK=no
	SET WFSENABLECHK=no
	SET WMSENABLECHK=no
	SET ARCENABLECHK=no
	SET RDBMSENABLECHK=no
	SET UTILENABLECHK=no
	SET GDALENABLECHK=no
	SET KINGORACLEENABLECHK=no
	SET KINGSPATIALENABLECHK=no
	SET OGRENABLECHK=no
	SET POSTGISENABLECHK=no
	SET SQLITEENABLECHK=no
:stp0_get_with
if not "%2"=="providers" goto stp1_get_with
	SET FDOPROVIDERSENABLECHK=yes
	goto next_param
:stp1_get_with
if not "%2"=="all" goto stp2_get_with
	SET FDOALLENABLECHK=yes
	SET FDOPROVIDERSENABLECHK=no
	SET THRPENABLECHK=no
	SET FDOENABLECHK=no
	SET UTILENABLECHK=no
	SET FDOCOREENABLECHK=no
	SET SHPENABLECHK=no
	SET SDFENABLECHK=no
	SET WFSENABLECHK=no
	SET WMSENABLECHK=no
	SET ARCENABLECHK=no
	SET RDBMSENABLECHK=no
	SET GDALENABLECHK=no
	SET KINGORACLEENABLECHK=no
	SET KINGSPATIALENABLECHK=no
	SET OGRENABLECHK=no
	SET POSTGISENABLECHK=no
	SET SQLITEENABLECHK=no
	goto next_param
:stp2_get_with
if not "%2"=="fdocore" goto stp3_get_with
	SET FDOCOREENABLECHK=yes
	SET THRPENABLECHK=no
	SET FDOENABLECHK=no
	SET UTILENABLECHK=no
	goto next_param
:stp3_get_with
if not "%2"=="thirdparty" goto stp4_get_with
	SET THRPENABLECHK=yes
	goto next_param
:stp4_get_with
if not "%2"=="utilities" goto stp5_get_with
	SET UTILENABLECHK=yes
	goto next_param
:stp5_get_with
if not "%2"=="fdo" goto stp6_get_with
	SET FDOENABLECHK=yes
	goto next_param
:stp6_get_with
if not "%2"=="shp" goto stp7_get_with
	SET SHPENABLECHK=yes
	goto next_param
:stp7_get_with
if not "%2"=="sdf" goto stp8_get_with
	SET SDFENABLECHK=yes
	goto next_param
:stp8_get_with
if not "%2"=="wfs" goto stp9_get_with
	SET WFSENABLECHK=yes
	goto next_param
:stp9_get_with
if not "%2"=="wms" goto stp10_get_with
	SET WMSENABLECHK=yes	
	goto next_param
:stp10_get_with
if not "%2"=="arcsde" goto stp11_get_with
	SET ARCENABLECHK=yes	
	goto next_param
:stp11_get_with
if not "%2"=="rdbms" goto stp12_get_with
	SET RDBMSENABLECHK=yes
    goto next_param
:stp12_get_with
if not "%2"=="gdal" goto stp13_get_with
	SET GDALENABLECHK=yes
    goto next_param
:stp13_get_with
if not "%2"=="kingoracle" goto stp14_get_with
	SET KINGORACLEENABLECHK=yes
    goto next_param
:stp14_get_with
if not "%2"=="ogr" goto stp15_get_with
	SET OGRENABLECHK=yes
    goto next_param
:stp15_get_with
if not "%2"=="kingspatial" goto stp16_get_with
	SET KINGSPATIALENABLECHK=yes
    goto next_param
:stp16_get_with
if not "%2"=="postgis" goto stp17_get_with
	SET POSTGISENABLECHK=yes
    goto next_param
:stp17_get_with
if not "%2"=="sqlite" goto custom_error
	SET SQLITEENABLECHK=yes
    goto next_param

:get_source
if (%2)==() goto custom_error
SET FDO_SVN_SOURCEDIR=%2
goto next_param

:get_path
if (%2)==() goto custom_error
if not exist "%~2" mkdir "%~2"
pushd "%~2"
if errorlevel 1 goto error
SET FDO_SVN_DESTDIR=%cd%
popd

:next_param
shift
shift
goto study_params

:start_checkout

if "%FDO_SVN_SOURCEDIR%"=="" goto custom_error
if not exist "%FDO_SVN_DESTDIR%" mkdir "%FDO_SVN_DESTDIR%"

:checkout_all
if "%FDOALLENABLECHK%"=="no" goto checkout_core
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR% "%FDO_SVN_DESTDIR%"
if errorlevel 1 goto error

goto end

:checkout_core
if "%FDOCOREENABLECHK%"=="no" goto checkout_fdo
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Fdo
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Fdo "%FDO_SVN_DESTDIR%\Fdo"
if errorlevel 1 goto error
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Utilities
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Utilities "%FDO_SVN_DESTDIR%\Utilities"
if errorlevel 1 goto error
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Thirdparty
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Thirdparty "%FDO_SVN_DESTDIR%\Thirdparty"
if errorlevel 1 goto error

goto checkout_providers

:checkout_fdo
if "%FDOENABLECHK%"=="no" goto checkout_thirdparty
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Fdo
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Fdo "%FDO_SVN_DESTDIR%\Fdo"
if errorlevel 1 goto error

:checkout_thirdparty
if "%THRPENABLECHK%"=="no" goto checkout_utilities
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Thirdparty
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Thirdparty "%FDO_SVN_DESTDIR%\Thirdparty"
if errorlevel 1 goto error

:checkout_utilities
if "%UTILENABLECHK%"=="no" goto checkout_providers
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Utilities
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Utilities "%FDO_SVN_DESTDIR%\Utilities"
if errorlevel 1 goto error

:checkout_providers
if "%FDOPROVIDERSENABLECHK%"=="no" goto checkout_shp
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers "%FDO_SVN_DESTDIR%\Providers"
if errorlevel 1 goto error

goto end

:checkout_shp
if "%SHPENABLECHK%"=="no" goto checkout_sdf
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/SHP
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/SHP "%FDO_SVN_DESTDIR%\Providers\SHP"
if errorlevel 1 goto error

:checkout_sdf
if "%SDFENABLECHK%"=="no" goto checkout_wfs
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/SDF
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/SDF "%FDO_SVN_DESTDIR%\Providers\SDF"
if errorlevel 1 goto error

:checkout_wfs
if "%WFSENABLECHK%"=="no" goto checkout_wms
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/WFS
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/WFS "%FDO_SVN_DESTDIR%\Providers\WFS"
if errorlevel 1 goto error

:checkout_wms
if "%WMSENABLECHK%"=="no" goto checkout_arcsde
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/WMS
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/WMS "%FDO_SVN_DESTDIR%\Providers\WMS"
if errorlevel 1 goto error

:checkout_arcsde
if "%ARCENABLECHK%"=="no" goto checkout_generic
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/ArcSDE
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/ArcSDE "%FDO_SVN_DESTDIR%\Providers\ArcSDE"
if errorlevel 1 goto error

:checkout_generic
if "%RDBMSENABLECHK%"=="no" goto checkout_gdal
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/GenericRdbms
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/GenericRdbms "%FDO_SVN_DESTDIR%\Providers\GenericRdbms"
if errorlevel 1 goto error

:checkout_gdal
if "%GDALENABLECHK%"=="no" goto checkout_kingoracle
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/GDAL
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/GDAL "%FDO_SVN_DESTDIR%\Providers\GDAL"
if errorlevel 1 goto error

:checkout_kingoracle
if "%KINGORACLEENABLECHK%"=="no" goto checkout_ogr
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/KingOracle
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/KingOracle "%FDO_SVN_DESTDIR%\Providers\KingOracle"
if errorlevel 1 goto error

:checkout_ogr
if "%OGRENABLECHK%"=="no" goto checkout_kingspatial
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/OGR
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/OGR "%FDO_SVN_DESTDIR%\Providers\OGR"
if errorlevel 1 goto error

:checkout_kingspatial
if "%KINGSPATIALENABLECHK%"=="no" goto checkout_postgis
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/KingMsSqlSpatial
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/KingMsSqlSpatial "%FDO_SVN_DESTDIR%\Providers\KingMsSqlSpatial"
if errorlevel 1 goto error

:checkout_postgis
if "%POSTGISENABLECHK%"=="no" goto checkout_sqlite
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/PostGIS
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/PostGIS "%FDO_SVN_DESTDIR%\Providers\PostGIS"
if errorlevel 1 goto error

:checkout_sqlite
if "%SQLITEENABLECHK%"=="no" goto end
echo Checking out https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/SQLite
svn checkout https://svn.osgeo.org/fdo/%FDO_SVN_SOURCEDIR%/Providers/SQLite "%FDO_SVN_DESTDIR%\Providers\SQLite"
if errorlevel 1 goto error

:end
exit /B 0

:error
echo Failed to checkout files.
exit /B 1

:custom_error
echo The command is not recognized.
echo Please use the format:
:help_show
echo **************************************************************
echo checkoutsvn.bat [-h] 
echo                 [-o=OutFolder] 
echo                 [-w=WithModule] 
echo                 -s=Source
echo *
echo Help:           -h[elp]
echo Branch:         -s[ource]=location of source file, either
echo                           branch location, tag location or trunk
echo OutFolder:      -o[utpath]=destination folder for files
echo WithModule:     -w[ith]=all(default), 
echo                         fdo, 
echo                         fdocore, 
echo                         thirdparty, 
echo                         providers, 
echo                         utilities, 
echo                         shp, 
echo                         sdf, 
echo                         wfs, 
echo                         wms, 
echo                         arcsde, 
echo                         rdbms, 
echo                         gdal,
echo                         kingoracle,
echo                         kingspatial,
echo                         ogr,
echo                         postgis,
echo                         sqlite
echo User:           -u[ser]=user id
echo Password:       -p[assword]=user password
echo **************************************************************
exit /B 0