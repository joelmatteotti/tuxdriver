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
 * \file tux_spinning.c
 * \brief Spinning functions.
 * \author remi.jocaille@c2me.be
 * \ingroup spinning
 */

#include <string.h>

#include "tux_cmd_parser.h"
#include "tux_hw_status.h"
#include "tux_hw_cmd.h"
#include "tux_movements.h"
#include "tux_spinning.h"
#include "tux_sw_status.h"
#include "tux_types.h"
#include "tux_usb.h"

/** Counter of spinning movements */
static unsigned char mvmt_counter = 0;

/**
 * \brief Update the status of the direction of the spinning.
 */
LIBLOCAL void
tux_spinning_update_direction(void)
{
    char *new_direction = "";

    if ((!hw_status_table.position2.motors.bits.spin_left_on) &&
        (!hw_status_table.position2.motors.bits.spin_right_on))
    {
        new_direction = STRING_VALUE_NONE;
    }
    else
    {
        if (hw_status_table.position2.motors.bits.spin_left_on)
        {
            new_direction = STRING_VALUE_LEFT;
        }
        else
        {
            if (hw_status_table.position2.motors.bits.spin_right_on)
            {
                new_direction = STRING_VALUE_RIGHT;
            }
        }
    }

    tux_sw_status_set_strvalue(SW_ID_SPINNING_DIRECTION, new_direction, true);
}

/**
 * \brief Update the status of the motor state of the left spin.
 */
LIBLOCAL void
tux_spinning_update_left_motor(void)
{
    unsigned char new_state;

    new_state = hw_status_table.position2.motors.bits.spin_left_on;
    tux_sw_status_set_intvalue(SW_ID_SPIN_LEFT_MOTOR_ON, new_state, true);
}

/**
 * \brief Update the status of the motor state of the right spin.
 */
LIBLOCAL void
tux_spinning_update_right_motor(void)
{
    unsigned char new_state;

    new_state = hw_status_table.position2.motors.bits.spin_right_on;
    tux_sw_status_set_intvalue(SW_ID_SPIN_RIGHT_MOTOR_ON, new_state, true);
}

/**
 * \brief Update the status of the spinning movements remaining.
 */
LIBLOCAL void
tux_spinning_update_movements_remaining(void)
{
    unsigned char new_count;

    new_count = hw_status_table.position2.spin_remaining_mvm;

    mvmt_counter = new_count;
    tux_sw_status_set_intvalue(SW_ID_SPINNING_REMAINING_MVM, new_count, true);
}

/**
 * \brief Set the speed of the spinning movements.
 * \param speed Movement speed.
 * \return The command success result.
 */
LIBLOCAL bool
tux_spinning_cmd_speed(unsigned char speed)
{
    return tux_movement_perform(MOVE_SPIN_R, 0, 0.0, speed, FINAL_ST_UNDEFINED,
                                true);
}

/**
 * \brief Execute a spinning on command.
 * \param movement Direction of the movement
 * \param counter Number of spinning movements.
 * \return The command success result.
 */
static bool
tux_spinning_cmd_on(move_body_part_t movement, unsigned char counter)
{
    return tux_movement_perform(movement, counter, 0.0, 5, FINAL_ST_UNDEFINED,
                            false);
}

/**
 * \brief Execute a spinning on command to the left.
 * \param counter Number of spinning movements.
 * \return The command success result.
 */
LIBLOCAL bool
tux_spinning_cmd_left_on(unsigned char counter)
{
    return tux_spinning_cmd_on(MOVE_SPIN_L, counter);
}

/**
 * \brief Execute a spinning on command to the right.
 * \param counter Number of spinning movements.
 * \return The command success result.
 */
LIBLOCAL bool
tux_spinning_cmd_right_on(unsigned char counter)
{
    return tux_spinning_cmd_on(MOVE_SPIN_R, counter);
}

/**
 * \brief Execute a spinning on command.
 * \param timeout Duration of the movement.
 * \param command Command for long movement.
 * \param movement Direction of the movement.
 * \return The command success result.
 */
static bool
tux_spinning_cmd_on_during(float timeout, unsigned char command,
                            move_body_part_t movement)
{
    bool ret;
    data_frame frame = { command, 0, 5, 0};
    delay_cmd_t cmd = { 0.0, TUX_CMD, SPINNING, OFF };

    /* Short movements */
    if (timeout < 0.3)
    {
        return tux_movement_perform(movement, 0, timeout, 5,
                FINAL_ST_UNDEFINED, false);
    }

    /* Long movements */
    ret = tux_usb_send_to_tux(frame);
    if (!ret)
    {
        return false;
    }

    mvmt_counter = 255;
    tux_sw_status_set_intvalue(SW_ID_SPINNING_REMAINING_MVM, mvmt_counter, true);

    ret = tux_cmd_parser_insert_sys_command(timeout, &cmd);

    return ret;
}

/**
 * \brief Execute a spinning on command to the left.
 * \param timeout Duration of the movement.
 * \return The command success result.
 */
LIBLOCAL bool
tux_spinning_cmd_left_on_during(float timeout)
{
    return tux_spinning_cmd_on_during(timeout, SPIN_LEFT_CMD, MOVE_SPIN_L);
}

/**
 * \brief Execute a spinning on command to the right.
 * \param timeout Duration of the movement.
 * \return The command success result.
 */
LIBLOCAL bool
tux_spinning_cmd_right_on_during(float timeout)
{
    return tux_spinning_cmd_on_during(timeout, SPIN_RIGHT_CMD, MOVE_SPIN_R);
}

/**
 * \brief Stop the spinning movement.
 * \return The command success result.
 */
LIBLOCAL bool
tux_spinning_cmd_off(void)
{
    bool ret;

    tux_cmd_parser_clean_sys_command(SPINNING);
    ret = tux_movement_perform(MOVE_SPIN_R, 0, 0, 5, FINAL_ST_STOP, false);
    mvmt_counter = 0;
    tux_sw_status_set_intvalue(SW_ID_SPINNING_REMAINING_MVM, mvmt_counter, true);

    return ret;
}
