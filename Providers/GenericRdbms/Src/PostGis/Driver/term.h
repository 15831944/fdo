/*
 * Copyright (C) 2006 Refractions Research, Inc. 
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
 *
 */

#ifndef POSTGIS_TERM_H
#define POSTGIS_TERM_H

#ifdef _WIN32
#pragma once
#endif // _WIN32

/**
 * Terminate PostGIS session.
 * It does free PostGIS context resources.
 *
 * @param contextp [out] - pointer to PostGIS context being deallocated.
 * @return Error code of operation state, RDBI_SUCCESS on success.
 */
int postgis_term(void **contextp);

#endif /* POSTGIS_TERM_H */