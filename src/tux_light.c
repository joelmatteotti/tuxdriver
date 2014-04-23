/*
 * Tux Droid - Light
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

#include <math.h>
#include <string.h>

#include "tux_hw_status.h"
#include "tux_light.h"
#include "tux_misc.h"
#include "tux_sw_status.h"

static float last_level_for_event = 0.0;

/**
 *
 */
LIBLOCAL void
tux_light_update_level(void)
{
    float new_level = 0.0;

    int light_value;

    light_value = (hw_status_table.light.high_level << 8);
    light_value += hw_status_table.light.low_level;

    if (hw_status_table.light.mode == 0)
    {
        light_value = light_value / 8 + 1000;
    }

    light_value = 1128 - light_value;

    new_level = (light_value * 100.0) / 1128.0;

    if (fabs(new_level - last_level_for_event) > 1.0)
    {
        last_level_for_event = new_level;
        tux_sw_status_set_floatvalue(SW_ID_LIGHT_LEVEL, new_level, true);
    }
    else
    {
        tux_sw_status_set_floatvalue(SW_ID_LIGHT_LEVEL, new_level, false);
    }
}
