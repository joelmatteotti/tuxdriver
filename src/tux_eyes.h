/*
 * Tux Droid - Eyes
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
 * \file tux_eyes.h
 * \brief Eyes header.
 * \author remi.jocaille@c2me.be
 * \ingroup eyes
 */

#ifndef _TUX_EYES_H_
#define _TUX_EYES_H_

/** \brief Eyes position not defined */
#define TUX_EYES_POSITION_NDEF          0
/** \brief Eyes position open */
#define TUX_EYES_POSITION_OPEN          1
/** \brief Eyes position close */
#define TUX_EYES_POSITION_CLOSE         2

extern void tux_eyes_update_position(void);
extern void tux_eyes_update_motor(void);
extern void tux_eyes_update_movements_remaining(void);
extern bool tux_eyes_cmd_on(unsigned char counter, unsigned char final_state);
extern bool tux_eyes_cmd_on_during(float timeout, unsigned char final_state);
extern bool tux_eyes_cmd_open(void);
extern bool tux_eyes_cmd_close(void);
extern bool tux_eyes_cmd_off(void);

#endif /* _TUX_EYES_H_ */
