/*
 * Tux Droid - Mouth
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
 * \file tux_mouth.h
 * \brief Mouth header.
 * \author remi.jocaille@c2me.be
 * \ingroup mouth
 */

#ifndef _TUX_MOUTH_H_
#define _TUX_MOUTH_H_

#include <stdbool.h>

/** \brief Mouth position not defined */
#define TUX_MOUTH_POSITION_NDEF                 0
/** \brief Mouth position open */
#define TUX_MOUTH_POSITION_OPEN                 1
/** \brief Mouth position close */
#define TUX_MOUTH_POSITION_CLOSE                2

extern void tux_mouth_update_position(void);
extern void tux_mouth_update_motor(void);
extern void tux_mouth_update_movements_remaining(void);
extern bool tux_mouth_cmd_on(unsigned char counter, unsigned char final_state);
extern bool tux_mouth_cmd_on_during(float timeout, unsigned char final_state);
extern bool tux_mouth_cmd_open(void);
extern bool tux_mouth_cmd_close(void);
extern bool tux_mouth_cmd_off(void);

#endif /* _TUX_MOUTH_H_ */
