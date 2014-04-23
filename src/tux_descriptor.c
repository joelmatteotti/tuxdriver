/*
 * Tux Droid - Descriptor
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
 * \file tux_descriptor.c
 * \brief Descriptor functions.
 * \author remi.jocaille@c2me.be
 * \ingroup descriptor
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "tux_descriptor.h"
#include "tux_usb.h"
#include "tux_sw_status.h"

/**
 * \brief Initialize the descriptor.
 */
LIBLOCAL void
tux_descriptor_init(void)
{
    tux_firmware_init_descriptor();
    tux_sound_flash_init_descriptor();
    tux_id_init_descriptor();
}

/**
 * \brief Command to start the descriptor retrieving (asynchronous)
 */
LIBLOCAL void
tux_descriptor_get(void)
{
    tux_sw_status_set_intvalue(SW_ID_DESCRIPTOR_COMPLETE, false, false);
    tux_id_get_descriptor();
    tux_firmware_get_descriptor();
    tux_sound_flash_get_descripor();
}

/**
 * \brief Command to call when the descriptor was completly retrieved.
 */
LIBLOCAL void
tux_descriptor_update(void)
{
    tux_sw_status_set_intvalue(SW_ID_DESCRIPTOR_COMPLETE, true, false);
}
