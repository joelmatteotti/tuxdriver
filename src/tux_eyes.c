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
 * \file tux_eyes.c
 * \brief Eyes functions.
 * \author remi.jocaille@c2me.be
 * \ingroup eyes
 */

#include <string.h>

#include "tux_cmd_parser.h"
#include "tux_eyes.h"
#include "tux_hw_cmd.h"
#include "tux_hw_status.h"
#include "tux_misc.h"
#include "tux_movements.h"
#include "tux_sw_status.h"
#include "tux_types.h"
#include "tux_usb.h"

/** Counter of eyes movements */
static unsigned char mvmt_counter = 0;

/**
 * \brief Update the status of the position of the eyes.
 */
LIBLOCAL void
tux_eyes_update_position(void)
{
    char *new_position = "";

    if (!hw_status_table.ports.portd.bits.eyes_open_switch)
    {
        new_position = STRING_VALUE_OPEN;
    }
    else
    {
        if (!hw_status_table.ports.portd.bits.eyes_closed_switch)
        {
            new_position = STRING_VALUE_CLOSE;
        }
        else
        {
            new_position = STRING_VALUE_NDEF;
        }
    }

    tux_sw_status_set_strvalue(SW_ID_EYES_POSITION, new_position, true);
}

/**
 * \brief Update the status of the motor state of the eyes.
 */
LIBLOCAL void
tux_eyes_update_motor(void)
{
    unsigned char new_state;

    new_state = hw_status_table.position2.motors.bits.eyes_on;
    tux_sw_status_set_intvalue(SW_ID_EYES_MOTOR_ON, new_state, true);
}

/**
 * \brief Update the status of the eyes movements remaining.
 */
LIBLOCAL void
tux_eyes_update_movements_remaining(void)
{
    unsigned char new_count;

    new_count = hw_status_table.position1.eyes_remaining_mvm;

    mvmt_counter = new_count;
    tux_sw_status_set_intvalue(SW_ID_EYES_REMAINING_MVM, new_count, true);
}

/**
 * \brief Execute an eyes on command.
 * \param counter Number of eyes movements.
 * \param final_state Desired final state of the eyes.
 * \return The command success result.
 */
LIBLOCAL bool
tux_eyes_cmd_on(unsigned char counter, unsigned char final_state)
{
    return tux_movement_cmd_on(MOVE_EYES, counter, final_state);
}

/**
 * \brief Execute an eyes on command.
 * \param timeout Duration of the movement.
 * \param final_state Desired final state of the eyes.
 * \return The command success result.
 */
LIBLOCAL bool
tux_eyes_cmd_on_during(float timeout, unsigned char final_state)
{
    bool ret;
    data_frame frame = {EYES_BLINK_CMD, 0, 0, 0};
    delay_cmd_t cmd = { 0.0, TUX_CMD, EYES };

    /* Short movements */
    if (timeout < 0.3)
    {
        return tux_movement_perform(MOVE_EYES, 0, timeout, 5, final_state,
                                    false);
    }

    /* Long movements */
    ret = tux_usb_send_to_tux(frame);
    if (!ret)
    {
        return false;
    }

    mvmt_counter = 255;
    tux_sw_status_set_intvalue(SW_ID_EYES_REMAINING_MVM, mvmt_counter, true);

    switch (final_state) {
    case FINAL_ST_UNDEFINED:
        cmd.sub_command = OFF;
        break;
    case FINAL_ST_OPEN_UP:
        cmd.sub_command = OPEN;
        break;
    case FINAL_ST_CLOSE_DOWN:
        cmd.sub_command = CLOSE;
        break;
    case FINAL_ST_STOP:
        cmd.sub_command = OFF;
        break;
    }
    ret = tux_cmd_parser_insert_sys_command(timeout, &cmd);

    return ret;
}

/**
 * \brief Open the eyes.
 * \return The command success result.
 */
LIBLOCAL bool
tux_eyes_cmd_open(void)
{
    return tux_movement_perform(MOVE_EYES, 0, 0, 5, FINAL_ST_OPEN_UP, false);
}

/**
 * \brief Close the eyes.
 * \return The command success result.
 */
LIBLOCAL bool
tux_eyes_cmd_close(void)
{
    return tux_movement_perform(MOVE_EYES, 0, 0, 5, FINAL_ST_CLOSE_DOWN, false);
}

/**
 * \brief Stop the eyes movement.
 * \return The command success result.
 */
LIBLOCAL bool
tux_eyes_cmd_off(void)
{
    bool ret;

    tux_cmd_parser_clean_sys_command(EYES);
    ret = tux_movement_perform(MOVE_EYES, 0, 0, 5, FINAL_ST_STOP, false);
    mvmt_counter = 0;
    tux_sw_status_set_intvalue(SW_ID_EYES_REMAINING_MVM, mvmt_counter, true);

    return ret;
}
