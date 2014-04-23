/*
 * Tux Droid - Version
 * Copyright (C) 2008 C2ME Sa <Acness : remi.jocaille@c2me.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
 /* SVN : $Id: version.h 5330 2009-09-04 11:18:09Z remi $ */

#ifndef _VERSION_H_
#define _VERSION_H_

#include "svnrev.h"

#define VER_MAJOR           0
#define VER_MINOR           0
#define VER_UPDATE          6
#define VER_REVISION        SVN_REV
#define VER_DATE            SVN_REVDATE
#define VER_STATE           " "
#ifdef WIN32
#   define VER_ARCH         "WIN32"
#else
#   define VER_ARCH         "UNIX"
#endif

#endif /* _VERSION_H_ */
