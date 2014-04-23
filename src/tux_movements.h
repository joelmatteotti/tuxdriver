/*
 * Tux Droid - Movements
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

#ifndef _TUX_MOVEMENTS_H_
#define _TUX_MOVEMENTS_H_

#include <stdbool.h>

#define EYES_MOVE_TIME          0.2
#define MOUTH_MOVE_TIME         0.25
#define FLIPPERS_MOVE_TIME      0.3
#define SPIN_MOVE_TIME          0.2

typedef enum
{
    MOVE_EYES = 0,
    MOVE_MOUTH = 1,
    MOVE_FLIPPERS = 2,
    MOVE_SPIN_R = 3,
    MOVE_SPIN_L = 4,
} move_body_part_t;

typedef enum
{
    SPEED_VERYLOW = 1,
    SPEED_LOW = 2,
    SPEED_MEDIUM = 3,
    SPEED_MIDHIGH = 4,
    SPEED_HIGH = 5,
} move_speed_t;

typedef enum
{
    FINAL_ST_UNDEFINED = 0,
    FINAL_ST_OPEN_UP = 1,
    FINAL_ST_CLOSE_DOWN = 2,
    FINAL_ST_STOP = 3,
} move_final_state_t;

extern bool tux_movement_cmd_on(move_body_part_t movement,
    unsigned char counter, unsigned char final_state);
extern bool tux_movement_perform(move_body_part_t movement,
    unsigned char counter, float timeout, move_speed_t speed,
    move_final_state_t final_state, bool refresh);

#endif /* _TUX_MOVEMENTS_H_ */
