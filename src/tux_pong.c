/*
 * Tux Droid - Pong
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

#include "tux_hw_status.h"
#include "tux_hw_cmd.h"
#include "tux_pong.h"
#include "tux_sw_status.h"
#include "tux_types.h"
#include "tux_usb.h"

#define STACK_SIZE 10

static int average_stack[STACK_SIZE];
static unsigned char stack_idx = 0;
static unsigned char stack_fill_count = 0;

/**
 *
 */
static void
stack_insert(int value)
{
    if (value > 100)
    {
        value = 100;
    }

    average_stack[stack_idx] = value;
    stack_idx++;

    if (stack_fill_count < STACK_SIZE)
    {
        stack_fill_count++;
    }

    stack_idx %= STACK_SIZE;
}

/**
 *
 */
static int
stack_average(void)
{
    unsigned char i;
    int average = 0;

    for (i = 0; i < stack_fill_count; i++)
    {
        average += average_stack[i];
    }

    return (average / stack_fill_count);
}

/**
 *
 */
LIBLOCAL void
tux_pong_update(void)
{
    static unsigned char received_pong = 0;
    int average;

    received_pong++;

    if (hw_status_table.pong.pongs_pending_number <= 190)
    {
        if (received_pong > 1)
        {
            stack_insert(received_pong * 10);
            average = stack_average();
            tux_sw_status_set_intvalue(SW_ID_CONNECTION_QUALITY, average, true);
        }
        received_pong = 0;
    }
}

/**
 *
 */
LIBLOCAL void
tux_pong_get(void)
{
    static unsigned char get_count = 0;
    data_frame frame = { TUX_PONG_PING_CMD, 200, 0, 0};

    get_count++;

    if (get_count >= 40)
    {
        get_count = 0;
        tux_usb_send_to_tux(frame);
    }
}
