/*
 * Tux Droid - ID connection
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

#include <stdio.h>
#include <string.h>

#include "tux_descriptor.h"
#include "tux_hw_status.h"
#include "tux_hw_cmd.h"
#include "tux_misc.h"
#include "tux_types.h"
#include "tux_usb.h"

LIBLOCAL id_descriptor_t id_desc;

/**
 *
 */
LIBLOCAL void
tux_id_init_descriptor(void)
{
    memset(&id_desc, 0, sizeof(id_descriptor_t));
}

/**
 *
 */
LIBLOCAL char *
tux_id_dump_descriptor(char *p)
{
    p = p + sprintf(p, "- ID connection\n  - number : \t[%d]\n", id_desc.number);

    return p;
}

/**
 *
 */
LIBLOCAL void
tux_id_update_number(void)
{
    id_desc.number = (hw_status_table.id.msb_number << 8)
            + hw_status_table.id.lsb_number;
}

/**
 *
 */
LIBLOCAL void
tux_id_get_descriptor(void)
{
    data_frame frame =
                        { USB_DONGLE_CONNECTION_CMD,
                          USB_TUX_CONNECTION_ID_REQUEST,
                          0, 0};

    tux_usb_send_to_dongle(frame);
}

/**
 *
 */
LIBLOCAL bool
tux_id_cmd_disconnect_from_tux(void)
{
    data_frame frame =
                        { USB_DONGLE_CONNECTION_CMD,
                          USB_TUX_CONNECTION_DISCONNECT,
                          0, 0};

    return tux_usb_send_to_dongle(frame);
}
