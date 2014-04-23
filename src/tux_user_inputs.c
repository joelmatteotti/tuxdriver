/*
 * Tux Droid - User inputs
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
#include "tux_sw_status.h"
#include "tux_types.h"
#include "tux_usb.h"
#include "tux_user_inputs.h"

static bool ir_received = false;
static bool ir_has_not_yet_been_used = true;
static unsigned char rc5_timeout_counter = 0;
static bool rc5_on_receiving = false;
static unsigned char rc5_last_toggle = 0;

/**
 *
 */
LIBLOCAL void
tux_user_inputs_init(void)
{
    ir_received = false;
    rc5_on_receiving = false;
    rc5_timeout_counter = 0;
    rc5_last_toggle = 0;
    ir_has_not_yet_been_used = true;
}

/**
 *
 */
LIBLOCAL void
tux_user_inputs_update_left_wing_button(void)
{
    unsigned char new_state;

    new_state = hw_status_table.sensors1.sensors.bits.left_wing_push_button;

    tux_sw_status_set_intvalue(SW_ID_LEFT_WING_BUTTON, new_state, true);
}

/**
 *
 */
LIBLOCAL void
tux_user_inputs_update_right_wing_button(void)
{
    unsigned char new_state;

    new_state = hw_status_table.sensors1.sensors.bits.right_wing_push_button;

    tux_sw_status_set_intvalue(SW_ID_RIGHT_WING_BUTTON, new_state, true);
}

/**
 *
 */
LIBLOCAL void
tux_user_inputs_update_head_button(void)
{
    unsigned char new_state;

    new_state = hw_status_table.sensors1.sensors.bits.head_push_button;

    tux_sw_status_set_intvalue(SW_ID_HEAD_BUTTON, new_state, true);
}

/**
 *
 */
static const char *
RC5_code_to_str(int code)
{
    switch (code) {
    case K_DUMMY_RELEASE:
        return "RELEASE";
    case K_0:
        return "K_0";
    case K_1:
        return "K_1";
    case K_2:
        return "K_2";
    case K_3:
        return "K_3";
    case K_4:
        return "K_4";
    case K_5:
        return "K_5";
    case K_6:
        return "K_6";
    case K_7:
        return "K_7";
    case K_8:
        return "K_8";
    case K_9:
        return "K_9";
    case K_STANDBY:
        return "K_STANDBY";
    case K_MUTE:
        return "K_MUTE";
    case K_VOLUMEPLUS:
        return "K_VOLUMEPLUS";
    case K_VOLUMEMINUS:
        return "K_VOLUMEMINUS";
    case K_ESCAPE:
        return "K_ESCAPE";
    case K_YES:
        return "K_YES";
    case K_NO:
        return "K_NO";
    case K_BACKSPACE:
        return "K_BACKSPACE";
    case K_STARTVOIP:
        return "K_STARTVOIP";
    case K_RECEIVECALL:
        return "K_RECEIVECALL";
    case K_HANGUP:
        return "K_HANGUP";
    case K_STAR:
        return "K_STAR";
    case K_SHARP:
        return "K_SHARP";
    case K_RED:
        return "K_RED";
    case K_GREEN:
        return "K_GREEN";
    case K_BLUE:
        return "K_BLUE";
    case K_YELLOW:
        return "K_YELLOW";
    case K_CHANNELPLUS:
        return "K_CHANNELPLUS";
    case K_CHANNELMINUS:
        return "K_CHANNELMINUS";
    case K_UP:
        return "K_UP";
    case K_DOWN:
        return "K_DOWN";
    case K_LEFT:
        return "K_LEFT";
    case K_RIGHT:
        return "K_RIGHT";
    case K_OK:
        return "K_OK";
    case K_FASTREWIND:
        return "K_FASTREWIND";
    case K_FASTFORWARD:
        return "K_FASTFORWARD";
    case K_PLAYPAUSE:
        return "K_PLAYPAUSE";
    case K_STOP:
        return "K_STOP";
    case K_RECORDING:
        return "K_RECORDING";
    case K_PREVIOUS:
        return "K_PREVIOUS";
    case K_NEXT:
        return "K_NEXT";
    case K_MENU:
        return "K_MENU";
    case K_MOUSE:
        return "K_MOUSE";
    case K_ALT:
        return "K_ALT";
    default:
        return "NULL";
    }
}

/**
 *
 */
LIBLOCAL void
tux_user_inputs_update_RC5(void)
{
    const char *code_str = "";

    if (rc5_on_receiving)
    {
        if (!ir_received)
        {
            rc5_timeout_counter++;
            if (rc5_timeout_counter >= RC5_TIMEOUT)
            {
                /* remote button is released */
                code_str = RC5_code_to_str(K_DUMMY_RELEASE);
                tux_sw_status_set_strvalue(SW_ID_REMOTE_BUTTON, code_str, true);

                rc5_timeout_counter = 0;
                rc5_on_receiving = false;
            }
        }
        else
        {
            rc5_timeout_counter = 0;
        }

        if (rc5_last_toggle != hw_status_table.ir.rc5_code.bits.toggle)
        {
            /* remote button is released */
            code_str = RC5_code_to_str(K_DUMMY_RELEASE);
            tux_sw_status_set_strvalue(SW_ID_REMOTE_BUTTON, code_str, true);

            /* remote button is pressed */
            if (hw_status_table.ir.rc5_code.bits.command <= LAST_VALID_K)
            {
                code_str = RC5_code_to_str(hw_status_table.ir.rc5_code.bits.command);
                tux_sw_status_set_strvalue(SW_ID_REMOTE_BUTTON, code_str, true);
            }
            else
            {
                rc5_on_receiving = 0;
            }
        }
    }
    else
    {   /* not in receiving */
        if ((ir_received) &&
            ((rc5_last_toggle != hw_status_table.ir.rc5_code.bits.toggle) ||
            ir_has_not_yet_been_used))
        {
            ir_has_not_yet_been_used = false;
            rc5_timeout_counter = 0;
            rc5_on_receiving = true;

            /* remote button is pressed */
            if (hw_status_table.ir.rc5_code.bits.command <= LAST_VALID_K)
            {
                code_str = RC5_code_to_str(hw_status_table.ir.rc5_code.bits.command);
                tux_sw_status_set_strvalue(SW_ID_REMOTE_BUTTON, code_str, true);
            }
            else
            {
                rc5_on_receiving = false;
            }
        }
    }

    rc5_last_toggle = hw_status_table.ir.rc5_code.bits.toggle;

    ir_received = false;
}

/**
 *
 */
LIBLOCAL void
tux_user_inputs_init_time_RC5(void)
{
    ir_received = true;
}

/**
 *
 */
LIBLOCAL void
tux_user_inputs_update_charger_state(void)
{
    char *new_state = "";

    if (!hw_status_table.sensors1.sensors.bits.power_plug_insertion_switch)
    {
        new_state = STRING_VALUE_UNPLUGGED;
    }
    else
    {
        if (hw_status_table.sensors1.sensors.bits.charger_led_status)
        {
            new_state = STRING_VALUE_CHARGING;
        }
        else
        {
            if (hw_status_table.ports.portb.bits.charger_inhibit_signal)
            {
                new_state = STRING_VALUE_INHIBITED;
            }
            else
            {
                new_state = STRING_VALUE_TRICKLE;
            }
        }
    }

    tux_sw_status_set_strvalue(SW_ID_CHARGER_STATE, new_state, true);
}

/**
 *
 */
LIBLOCAL bool
tux_user_inputs_cmd_ir_on(void)
{
    data_frame frame = {TURN_IR_ON_CMD, 0, 0, 0};

    return tux_usb_send_to_tux(frame);
}

/**
 *
 */
LIBLOCAL bool
tux_user_inputs_cmd_ir_off(void)
{
    data_frame frame = {TURN_IR_OFF_CMD, 0, 0, 0};

    return tux_usb_send_to_tux(frame);
}

/**
 *
 */
LIBLOCAL bool
tux_user_inputs_cmd_ir_send(unsigned char address, unsigned char command)
{
    data_frame frame = {IR_SEND_RC5_CMD, 0, 0, 0};

    frame[1] = address;
    frame[2] = command;

    return tux_usb_send_to_tux(frame);
}
