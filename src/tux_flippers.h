/*
 * Tux Droid - Flippers
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
 * \file tux_flippers.h
 * \brief Flippers header.
 * \author remi.jocaille@c2me.be
 * \ingroup flippers
 */

#ifndef _TUX_FLIPPERS_H_
#define _TUX_FLIPPERS_H_

#include <stdbool.h>

/** \brief Flippers position down */
#define TUX_FLIPPERS_POSITION_DOWN                 0
/** \brief Flippers position up */
#define TUX_FLIPPERS_POSITION_UP                   1

extern void tux_flippers_update_position(void);
extern void tux_flippers_update_motor(void);
extern void tux_flippers_update_movements_remaining(void);
extern bool tux_flippers_cmd_speed(unsigned char speed);
extern bool tux_flippers_cmd_on(unsigned char counter, unsigned char final_state);
extern bool tux_flippers_cmd_on_during(float timeout, unsigned char final_state);
extern bool tux_flippers_cmd_up(void);
extern bool tux_flippers_cmd_down(void);
extern bool tux_flippers_cmd_off(void);

#endif /* _TUX_FLIPPERS_H_ */
