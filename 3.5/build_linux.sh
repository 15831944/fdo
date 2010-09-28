#!/bin/sh

##
## Copyright (C) 2004-2006  Autodesk, Inc.
## 
## This library is free software; you can redistribute it and/or
## modify it under the terms of version 2.1 of the GNU Lesser
## General Public License as published by the Free Software Foundation.
## 
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
## 
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
##

TYPEACTION=buildinstall
TYPEBUILD=release
TYPECONFIGURE=configure
BUILDDOCS=skip
PREFIXVAL=/usr/local/fdo-3.5.0

DEFMODIFY=no
FDOCOREENABLE=yes
FDOENABLE=no
UTILENABLE=no
SHPENABLE=yes
SDFENABLE=yes
WFSENABLE=yes
WMSENABLE=yes
ARCENABLE=yes
RDBMSENABLE=yes
GDALENABLE=yes
OGRENABLE=yes
POSTGISENABLE=yes
KINGORACLEENABLE=yes
SQLITEENABLE=yes
SHOWHELP=no


### study parameters ###
while test $# -gt 0
do
  arg="$1"
  shift
  case $arg in
  -h | --h | --help)
    SHOWHELP=yes
    break
    ;;
  -a | --a | --action)
    if test "$1" == buildinstall; then
        TYPEACTION=buildinstall
    elif test "$1" == build; then
        TYPEACTION=build
    elif test "$1" == install; then
        TYPEACTION=install
    elif test "$1" == uninstall; then
        TYPEACTION=uninstall
    elif test "$1" == clean; then
        TYPEACTION=clean
    else
        echo "$arg Invalid parameter $1"
    exit 1
    fi
    shift
    ;;
  -p | --p | --prefix)
    if test "$1" == ""; then
        echo "$arg Invalid parameter $1"
        exit 1
    else
        PREFIXVAL="$1"
    fi
    shift
    ;;
  -m | --m | --makefile)
    if test "$1" == configure; then
        TYPECONFIGURE=configure
    elif test "$1" == noconfigure; then
        TYPECONFIGURE=noconfigure
    else
        echo "$arg Invalid parameter $1"
    exit 1
    fi
    shift
    ;;
  -d | --d | --docs)
    if test "$1" == skip; then
        BUILDDOCS=skip
    elif test "$1" == build; then
        BUILDDOCS=build
    else
        echo "$arg Invalid parameter $1"
    exit 1
    fi
    shift
    ;;
  -c | --c | --config)
    if test "$1" == debug; then
        TYPEBUILD=debug
    elif test "$1" == release; then
        TYPEBUILD=release
    else
        echo "$arg Invalid parameter $1"
    exit 1
    fi
    shift
    ;;
  -w | --w | --with)
    if test "$DEFMODIFY" == no; then
       DEFMODIFY=yes
       FDOCOREENABLE=no
       FDOENABLE=no
       SHPENABLE=no
       SDFENABLE=no
       WFSENABLE=no
       WMSENABLE=no
       ARCENABLE=no
       RDBMSENABLE=no
       UTILENABLE=no
       GDALENABLE=no
       OGRENABLE=no
       POSTGISENABLE=no
       KINGORACLEENABLE=no
       SQLITEENABLE=no
    fi
    if test -z "$1"; then
       echo "$arg Invalid parameter $1"
       exit 1
    elif test "$1" == all; then
        FDOCOREENABLE=yes
        SHPENABLE=yes
        SDFENABLE=yes
        WFSENABLE=yes
        WMSENABLE=yes
        ARCENABLE=yes
        RDBMSENABLE=yes
        GDALENABLE=yes
        OGRENABLE=yes
        POSTGISENABLE=yes
        KINGORACLEENABLE=yes
        SQLITEENABLE=yes
    elif test "$1" == fdocore; then
	FDOCOREENABLE=yes
    elif test "$1" == fdo; then
        FDOENABLE=yes
    elif test "$1" == utilities; then
        UTILENABLE=yes
    elif test "$1" == providers; then
        SHPENABLE=yes
        SDFENABLE=yes
        WFSENABLE=yes
        WMSENABLE=yes
        ARCENABLE=yes
        RDBMSENABLE=yes
        GDALENABLE=yes
        OGRENABLE=yes
        POSTGISENABLE=yes
        KINGORACLEENABLE=yes
        SQLITEENABLE=yes
    elif test "$1" == shp; then
        SHPENABLE=yes
    elif test "$1" == sdf; then
        SDFENABLE=yes
    elif test "$1" == wfs; then
        WFSENABLE=yes
    elif test "$1" == wms; then
        WMSENABLE=yes
    elif test "$1" == arcsde; then
        ARCENABLE=yes
    elif test "$1" == rdbms; then
        RDBMSENABLE=yes
    elif test "$1" == gdal; then
        GDALENABLE=yes
    elif test "$1" == ogr; then
        OGRENABLE=yes
    elif test "$1" == postgis; then
        POSTGISENABLE=yes
    elif test "$1" == kingoracle; then
        KINGORACLEENABLE=yes
    elif test "$1" == sqlite; then
        SQLITEENABLE=yes
    else
        echo "$arg Invalid parameter $1"
        exit 1
    fi
    shift
    ;;
  -*)
    echo "The command option is not recognized: $arg"
    echo "Please use the format:"
    SHOWHELP=yes
    break
    ;;

  *)
    echo "The command is not recognized: $arg"
    echo "Please use the format:"
    SHOWHELP=yes
    break
    ;;
  esac
done
### end of study parameters ###

### Setup build environment ###
if test "$FDO" == "" ; then
    export FDO="$PWD/Fdo"
fi
if test "$FDOTHIRDPARTY" == "" ; then
    export FDOTHIRDPARTY="$PWD/Thirdparty"
fi
if test "$FDOUTILITIES" == "" ; then
    export FDOUTILITIES="$PWD/Utilities"
fi
### End Setup build environment ###

### Show Help Info ###
if test "$SHOWHELP" == yes; then

   echo "******************************************************************************************"
   echo "build_linux.sh [--h]"
   echo "               [--c BuildType]" 
   echo "               [--a Action]"
   echo "               [--w WithModule]" 
   echo "               [--d BuildDocs]"
   echo "               [--m ConfigMakefiles]"
   echo "               [--p Prefix]"
   echo "*"
   echo "Help:            --h[elp]"
   echo "BuildType:       --c[onfig] release(default), debug"
   echo "Action:          --a[ction] buildinstall(default), build, install, uninstall, clean"
   echo "BuildDocs:       --d[ocs] skip(default), build"
   echo "ConfigMakefiles: --m[akefile] configure(default), noconfigure"
   echo "Prefix:          --p[refix] <fdo install location>"

   HELPSTRINGWITH="WithModule:      --w[ith] all(default), fdocore, fdo, utilities, providers"
   if test -e "Providers/SHP/build_linux.sh"; then
   HELPSTRINGWITH="$HELPSTRINGWITH, shp"
   fi
   if test -e "Providers/SDF/build_linux.sh"; then
   HELPSTRINGWITH="$HELPSTRINGWITH, sdf"
   fi
   if test -e "Providers/WFS/build_linux.sh"; then
   HELPSTRINGWITH="$HELPSTRINGWITH, wfs"
   fi
   if test -e "Providers/WMS/build_linux.sh"; then
   HELPSTRINGWITH="$HELPSTRINGWITH, wms"
   fi
   if test -e "Providers/GDAL/build_linux.sh"; then
   HELPSTRINGWITH="$HELPSTRINGWITH, gdal"
   fi
   if test -e "Providers/ArcSDE/build_linux.sh"; then
   HELPSTRINGWITH="$HELPSTRINGWITH, arcsde"
   fi
   if test -e "Providers/GenericRdbms/build_linux.sh"; then
   HELPSTRINGWITH="$HELPSTRINGWITH, rdbms"
   fi
   if test -e "Providers/OGR/build_linux.sh"; then
   HELPSTRINGWITH="$HELPSTRINGWITH, ogr"
   fi
   if test -e "Providers/PostGIS/build_linux.sh"; then
   HELPSTRINGWITH="$HELPSTRINGWITH, postgis"
   fi
   if test -e "Providers/KingOracle/build_linux.sh"; then
   HELPSTRINGWITH="$HELPSTRINGWITH, kingoracle"
   fi
   if test -e "Providers/SQLite/build_linux.sh"; then
   HELPSTRINGWITH="$HELPSTRINGWITH, sqlite"
   fi
   echo "$HELPSTRINGWITH"
   echo "******************************************************************************************"

   exit 0
fi

### configure build ###
if test "$TYPECONFIGURE" == configure ; then
   if test "$FDOCOREENABLE" == yes || test "$THRPENABLE" == yes || test "$FDOENABLE" == yes || test "$UTILENABLE" == yes; then
      echo "configuring fdocore"
      aclocal
      libtoolize --force
      automake --add-missing --copy
      autoconf

      if test "$HOSTTYPE" == "i686" ; then
         export CPPFLAGS="-march=i686" 
      fi

      if test "$TYPEBUILD" == release; then
         ./configure --prefix="$PREFIXVAL"
      else
         ./configure --enable-debug=yes --prefix="$PREFIXVAL"
      fi
   fi
fi

### start build ###

CMDEX="--c $TYPEBUILD --a $TYPEACTION --d $BUILDDOCS --m $TYPECONFIGURE --p $PREFIXVAL"

#build all of fdocore
if test "$FDOCOREENABLE" == yes; then
   if test "$TYPEACTION" == buildinstall || test "$TYPEACTION" == build ; then
      make
   fi
   if test "$TYPEACTION" == clean ; then
      make clean
   fi
   if test "$TYPEACTION" == buildinstall || test "$TYPEACTION" == install ; then
      make install
   fi
   if test "$TYPEACTION" == uninstall ; then
      make uninstall
   fi
fi

#build Fdo
if test "$FDOENABLE" == yes; then
   pushd Fdo >& /dev/null
   
   if test "$TYPEACTION" == clean ; then
      make clean
   fi
   if test "$TYPEACTION" == buildinstall || test "$TYPEACTION" == build ; then
      make
   fi
   if test "$TYPEACTION" == buildinstall || test "$TYPEACTION" == install ; then
      make install
   fi
   if test "$TYPEACTION" == uninstall ; then
      make uninstall
   fi
   
   if test "$BUILDDOCS" == build ; then
      echo Creating Fdo unmanaged and managed html documentation
      rm -rf Docs/HTML/FDO_API
      mkdir -p Docs/HTML/FDO_API
      cp ../DocResources/geospatial.js Docs/HTML/FDO_API
      cp ../DocResources/osgeo.css Docs/HTML/FDO_API
      cp ../DocResources/FDO_architecture.png Docs/HTML/FDO_API

      rm -rf Docs/HTML/FDO_API_managed
      mkdir -p Docs/HTML/FDO_API_managed
      cp ../DocResources/geospatial.js Docs/HTML/FDO_API_managed
      cp ../DocResources/osgeo.css Docs/HTML/FDO_API_managed
      cp ../DocResources/FDO_architecture.png Docs/HTML/FDO_API_managed

      pushd Docs/doc_src >& /dev/null
      doxygen Doxyfile_FDOunmanaged >& /dev/null
      doxygen Doxyfile_FDOmanaged >& /dev/null
      popd >& /dev/null
   fi
   
   if test "$TYPEACTION" == buildinstall || test "$TYPEACTION" == install ; then
      rm -rf "$PREFIXVAL/docs/HTML/FDO_API"
      mkdir -p "$PREFIXVAL/docs/HTML"
      if test -e "Docs/HTML/FDO_API"; then  
         cp --force --recursive "Docs/HTML/FDO_API" "$PREFIXVAL/docs/HTML"
      fi
   fi
   
   popd >& /dev/null
fi

#build Utilities
if test "$UTILENABLE" == yes; then
   pushd Utilities >& /dev/null
   
   if test "$TYPEACTION" == clean ; then
      make clean
   fi
   if test "$TYPEACTION" == buildinstall || test "$TYPEACTION" == build ; then
      make
   fi
   if test "$TYPEACTION" == buildinstall || test "$TYPEACTION" == install ; then
      make install
   fi
   if test "$TYPEACTION" == uninstall ; then
      make uninstall
   fi
   
   popd >& /dev/null
fi

#build SHP Provider
if test "$SHPENABLE" == yes; then
   if test -e "Providers/SHP/build_linux.sh"; then
       pushd Providers/SHP >& /dev/null
       ./build_linux.sh $CMDEX
       popd >& /dev/null
   fi
fi

#build SDF Provider
if test "$SDFENABLE" == yes; then
   if test -e "Providers/SDF/build_linux.sh"; then
       pushd Providers/SDF >& /dev/null
       ./build_linux.sh $CMDEX
       popd >& /dev/null
   fi
fi

#build WFS Provider
if test "$WFSENABLE" == yes; then
   if test -e "Providers/WFS/build_linux.sh"; then
       pushd Providers/WFS >& /dev/null
       ./build_linux.sh $CMDEX
       popd >& /dev/null
   fi
fi

#build WMS Provider
if test "$WMSENABLE" == yes; then
   if test -e "Providers/WMS/build_linux.sh"; then
       pushd Providers/WMS >& /dev/null
       ./build_linux.sh $CMDEX
       popd >& /dev/null
   fi
fi

#build ArcSDE Provider
if test "$ARCENABLE" == yes; then
   if test -e "Providers/ArcSDE/build_linux.sh"; then
       pushd Providers/ArcSDE >& /dev/null
       ./build_linux.sh $CMDEX
       popd >& /dev/null
   fi
fi

#build GenericRdbms Providers
if test "$RDBMSENABLE" == yes; then
   if test -e "Providers/GenericRdbms/build_linux.sh"; then
       pushd Providers/GenericRdbms >& /dev/null
       ./build_linux.sh $CMDEX
       popd >& /dev/null
   fi
fi

#build GDAL Provider
if test "$GDALENABLE" == yes; then
   if test -e "Providers/GDAL/build_linux.sh"; then
       pushd Providers/GDAL >& /dev/null
       ./build_linux.sh $CMDEX
       popd >& /dev/null
   fi
fi

#build OGR Provider
if test "$OGRENABLE" == yes; then
   if test -e "Providers/OGR/build_linux.sh"; then
       pushd Providers/OGR >& /dev/null
       ./build_linux.sh $CMDEX
       popd >& /dev/null
   fi
fi

#build PostGIS Provider
if test "$POSTGISENABLE" == yes; then
   if test -e "Providers/PostGIS/build_linux.sh"; then
       pushd Providers/PostGIS >& /dev/null
       ./build_linux.sh $CMDEX
       popd >& /dev/null
   fi
fi

#build KingOracle Provider
if test "$KINGORACLEENABLE" == yes; then
   if test -e "Providers/KingOracle/build_linux.sh"; then
       pushd Providers/KingOracle >& /dev/null
       ./build_linux.sh $CMDEX
       popd >& /dev/null
   fi
fi

#build SSLite Provider
if test "$SQLITEENABLE" == yes; then
   if test -e "Providers/SQLite/build_linux.sh"; then
       pushd Providers/SQLite >& /dev/null
       ./build_linux.sh $CMDEX
       popd >& /dev/null
   fi
fi


exit 0
