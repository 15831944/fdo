/*
 * Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef FdoSpatialUtilityVectorAngle___H
#define FdoSpatialUtilityVectorAngle___H	1

// Angle of a vector in 3D XYZ-coordinate system.
struct FdoSpatialUtilityVectorAngle
{
    double m_theta; // (-PI, PI]: radian of the vector on the XY-Plane project relative to X-axis. */
    double m_phi;   // [-PI/2, PI/2]  : radian of the vector relative to XY-plane
};

#endif // FdoSpatialUtilityVectorAngle___H

