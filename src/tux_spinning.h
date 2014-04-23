/*
 * Tux Droid - Spinning
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

/**
 * \file tux_spinning.h
 * \brief Spinning header.
 * \author remi.jocaille@c2me.be
 * \ingroup spinning
 */

#ifndef _TUX_SPINNING_H_
#define _TUX_SPINNING_H_

#include <stdbool.h>

/** \brief Spinning direction none (stopped) */
#define TUX_SPINNING_DIRECTION_NONE                 0
/** \brief Spinning direction left */
#define TUX_SPINNING_DIRECTION_LEFT                 1
/** \brief Spinning direction right */
#define TUX_SPINNING_DIRECTION_RIGHT                2

extern void tux_spinning_update_direction(void);
extern void tux_spinning_update_left_motor(void);
extern void tux_spinning_update_right_motor(void);
extern void tux_spinning_update_movements_remaining(void);

extern bool tux_spinning_cmd_speed(unsigned char speed);
extern bool tux_spinning_cmd_left_on(unsigned char counter);
extern bool tux_spinning_cmd_right_on(unsigned char counter);
extern bool tux_spinning_cmd_left_on_during(float timeout);
extern bool tux_spinning_cmd_right_on_during(float timeout);
extern bool tux_spinning_cmd_off(void);

#endif /* _TUX_SPINNING_H_ */
