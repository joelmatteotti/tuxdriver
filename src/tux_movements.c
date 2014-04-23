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

#include <string.h>

#include "tux_hw_cmd.h"
#include "tux_hw_status.h"
#include "tux_movements.h"
#include "tux_sw_status.h"
#include "tux_types.h"
#include "tux_usb.h"

/*
 * Prototypes
 */
static int movement_status_control(char final_state, int value, char condition);
static int control_final_state(char motor, char final_state, int value);
static int compute_value(char motor, float timeout);
static void single_movement(char motor, char final_state);


/**
 * \brief Convert the timeout on a number of movements.
 * \param motor Which motor should be configured.
 * \param timeout The time (sec.) to convert.
 * \return The number of movement to execute.
 *
 * The firmware use a timebase at 4ms, and is configured with a 8bit
 * variable.
 *
 * The maximum timeout for the firmware is 4ms * 256. When the timeout
 * exceed 1s, the time is converted on a number of movements.
 *
 * The time of each movement has been measured, and defined in movements.h.
 */
static int
compute_value(char motor, float timeout)
{
    int value = 0;
    switch (motor)
    {
    case (MOVE_EYES):
        value = (int)(timeout / EYES_MOVE_TIME);
        break;
    case (MOVE_MOUTH):
        value = (int)(timeout / MOUTH_MOVE_TIME);
        break;
    case (MOVE_FLIPPERS):
        value = (int)(timeout / FLIPPERS_MOVE_TIME);
        break;
    case (MOVE_SPIN_L):
        value = (int)(timeout / SPIN_MOVE_TIME);
        break;
    case (MOVE_SPIN_R):
        value = (int)(timeout / SPIN_MOVE_TIME);
        break;
    }

    if (value > 255)
    {
        value = 255;
    }
    return value;
}

/**
 * \brief Control the final state.
 * \param motor Which motor should be configured.
 * \param final_state The wanted final state.
 * \param value The actual number of movements value.
 * \return The number of movements with the final_state.
 *
 * This function verify if the number of mouvements is OK to reach the desired
 * final state.
 *
 * To do this, the actual motor state is analyzed to determine if the 'value'
 * variable must be incremented or not.
 *
 * If the value is null and a final_state has been determined, a single movement
 * is executed.
 */
int static
control_final_state(char motor, char final_state, int value)
{
    int condition;
    switch (motor)
    {
    case (MOVE_EYES):
        {
            condition = hw_status_table.ports.portd.bits.eyes_open_switch;
            if (value)
            {
                value = movement_status_control(final_state, value, condition);
            }
            else
            {
                if (final_state != FINAL_ST_UNDEFINED)
                {
                    single_movement(motor, final_state);
                }
            }
            break;
        }
    case (MOVE_MOUTH):
        {
            condition = hw_status_table.ports.portb.bits.mouth_open_switch;
            if (value)
            {
                value = movement_status_control(final_state, value, condition);
            }
            else
            {
                if (final_state != FINAL_ST_UNDEFINED)
                {
                    single_movement(motor, final_state);
                }
            }
            break;
        }
    case (MOVE_FLIPPERS):
        {
            condition = !hw_status_table.position2.flippers_down;
            if (value)
            {
                value = movement_status_control(final_state, value, condition);
            }
            else
            {
                if (final_state != FINAL_ST_UNDEFINED)
                {
                    single_movement(motor, final_state);
                }
            }
            break;
        }
    case (MOVE_SPIN_R):
        {
            if (!(value) && final_state == FINAL_ST_STOP)
            {
                single_movement(motor, final_state);
            }
            break;
        }
    case (MOVE_SPIN_L):
        {
            if (!(value) && final_state == FINAL_ST_STOP)
            {
                single_movement(motor, final_state);
            }
            break;
        }
    }
    return value;
}

/**
 * \brief Control the number of mouvements.
 * \param final_state The wanted final state.
 * \param value The actual number of movements.
 * \param condition Status to determine the actual state.
 *
 *  This function determines if the number of movements must be incremented or
 *  not.
 *
 *  The 'condition' parameter must be equal to 1 when the position is
 *  closed or lower.
 */
int static
movement_status_control(char final_state, int value, char condition)
{
    if ((condition == 2 && final_state == FINAL_ST_OPEN_UP && !(value % 2)) ||
        (condition == 1 && final_state == FINAL_ST_CLOSE_DOWN && value % 2) ||
        (condition == 0 && final_state == FINAL_ST_CLOSE_DOWN && !(value % 2)) ||
        (condition == 0 && final_state == FINAL_ST_OPEN_UP && value % 2)
        )
    {
        value ++;
    }
    return value;
}

/**
 * \brief Determine the command to send.
 * \param motor Which motor should be configured.
 * \param final_state The wanted final state.
 *
 * Each single movements has its own command.
 *
 * This function determine which command must to be sent to Tux.
 */
LIBLOCAL void
single_movement(char motor, char final_state)
{
    data_frame frame = {0, 0, 0, 0};

    switch (motor)
    {
    case (MOVE_EYES):
        if (final_state == FINAL_ST_OPEN_UP)
        {
            frame[0] = EYES_OPEN_CMD;
        }
        else
        {
            if (final_state == FINAL_ST_CLOSE_DOWN)
            {
                frame[0] = EYES_CLOSE_CMD;
            }
            else
            {
                if (final_state == FINAL_ST_STOP)
                {
                    frame[0] = EYES_STOP_CMD;
                }
            }
        }
        break;

    case (MOVE_MOUTH):
        if (final_state == FINAL_ST_OPEN_UP)
        {
            frame[0] = MOUTH_OPEN_CMD;
        }
        else
        {
            if (final_state == FINAL_ST_CLOSE_DOWN)
            {
                frame[0] = MOUTH_CLOSE_CMD;
            }
            else
            {
                if (final_state == FINAL_ST_STOP)
                {
                    frame[0] = MOUTH_STOP_CMD;
                }
            }
        }
        break;

    case (MOVE_FLIPPERS):
        if (final_state == FINAL_ST_OPEN_UP)
        {
            frame[0] = FLIPPERS_RAISE_CMD;
        }
        else
        {
            if (final_state == FINAL_ST_CLOSE_DOWN)
            {
                frame[0] = FLIPPERS_LOWER_CMD;
            }
            else
            {
                if (final_state == FINAL_ST_STOP)
                {
                    frame[0] = FLIPPERS_STOP_CMD;
                }
            }
        }
        break;
    case (MOVE_SPIN_L):
        if (final_state == FINAL_ST_STOP)
        {
            frame[0] = SPIN_STOP_CMD;
        }
        break;
    case (MOVE_SPIN_R):
        if (final_state == FINAL_ST_STOP)
        {
            frame[0] = SPIN_STOP_CMD;
        }
        break;
    default:
        return;
    }

    tux_usb_send_to_tux(frame);
}

LIBLOCAL bool
tux_movement_cmd_on(move_body_part_t movement, unsigned char counter,
   unsigned char final_state)
{
    return tux_movement_perform(movement, counter, 0.0, 5, final_state, false);
}

/**
 * \brief Control et configure the movements.
 * \param motor Which motor should be configured
 * \param counter The number of movements to execute
 * \param timeout The duration of the movement (if counter = 0)
 * \param speed The PWM value. Only applicable for the flippers and the spinning.
 * \param final_state The final state to reach
 * \param refresh Flag indicate if the entire command must be sent, or just the
 * new PWM value.
 * \return ack of tux command.
 *
 * This function analyzes the command received from the API.
 * - 'Refresh' has the maximum priority. If this flag is set, only the command
 *   to change the PWM value is sent.
 * - Timeout has priority on counter.
 *   - If timeout's value is defined, the movement will be executed for the
 *   specified time.
 *   - If timeout is lower than 300ms, the timeout will be controlled by the
 *   Tux's firmwares. In this case, the final state will be ignored.
 *   - Else, if timeout's value is greater than 300ms, the value is converted on
 *   a number of movements, and the final state is considered.
 * - If 'counter' has been specified, a command is sent to Tux with this value.
 *   - To do an infinite movement, counter's and timeout's values must be null,
 *   and final_state must be undefined (0).
 *   - If 'counter' is equal to 0 and a final state has been specified, a single
 *   movement is executed if needed.*
 */
LIBLOCAL bool
tux_movement_perform(move_body_part_t movement, unsigned char counter,
        float timeout, move_speed_t speed,
        move_final_state_t final_state, bool refresh)
{
    data_frame frame = {0, 0, 0, 0};
    int value;
    bool ret = false;
    char type = 0;

    if (movement == (MOVE_SPIN_L) || movement == (MOVE_SPIN_R))
    {
        if (final_state != FINAL_ST_STOP)
        {
            final_state = FINAL_ST_UNDEFINED;
        }
    }

    /*
     * Refresh has the maximum priority. If it's set, only the command to
     * refresh the PWM value is sent to Tux
     */
    if (refresh)
    {
        frame[0] = MOTORS_CONFIG_CMD;
        frame[1] = movement;
        frame[2] = speed;
        ret = tux_usb_send_to_tux(frame);
    }
    else
    {
        /*
         * Timeout has priority on counter.
         */
        if (timeout == 0)
        {
            value = counter;
            value = control_final_state(movement, final_state, value);
        }
        else
        {
            /*
             * If timeout is lower than 300ms, the timeout value is sent to Tux
             */
            if (timeout < 0.3)
            {
                type = 1;
                value = timeout / 0.004;
            }
            /*
             * Else, the timeout's value is converted on a number of movements.
             * The final state is considered
             */
            else
            {
                value = compute_value(movement, timeout);
                value = control_final_state(movement, final_state, value);
            }
        }
        /*
         * If a normal movement must be sent, the value is not null, or if value
         * is null, the final state must be also null (infinite movement).
         *
         * If the value is null and a final state has been specified, this means
         * that a single move has already been sent. No command must be sent.
         */
        if (value || !(final_state))
        {
            frame[0] = MOTORS_CONFIG_CMD;
            frame[1] = movement;
            frame[2] = speed;
            ret = tux_usb_send_to_tux(frame);

            frame[0] = MOTORS_SET_CMD;
            frame[1] = movement;
            frame[2] = value;
            frame[3] = type;
            ret = tux_usb_send_to_tux(frame);
        }
    }
    return ret;
}
