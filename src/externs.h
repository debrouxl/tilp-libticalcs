/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2003  Romain Lievin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __CALC_EXTERNS__
#define __CALC_EXTERNS__

#ifndef __CALC_HEADERS__
# include "headers.h"
#endif

#ifndef __CALC_DEFS__
# include "calc_def.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

  extern TicableLinkCable *cable;
  extern TicalcInfoUpdate *update;
  extern int ticalcs_calc_type;
  extern int lock;

#ifdef __cplusplus
}
#endif
#endif