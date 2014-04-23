/*
 * Tux Droid - Low level status
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

#include "log.h"
#include "tux_hw_status.h"
#include "tux_misc.h"

LIBLOCAL hw_status_table_t hw_status_table;
LIBLOCAL unsigned char tux_hw_status_header_counter[16] = { 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static int parse_body_ports(const unsigned char *frame);
static int parse_body_sensors1(const unsigned char *frame);
static int parse_body_light(const unsigned char *frame);
static int parse_body_position1(const unsigned char *frame);
static int parse_body_position2(const unsigned char *frame);
static int parse_body_ir(const unsigned char *frame);
static int parse_body_id(const unsigned char *frame);
static int parse_body_battery(const unsigned char *frame);
static int parse_body_version(const unsigned char *frame);
static int parse_body_revision(const unsigned char *frame);
static int parse_body_author(const unsigned char *frame);
static int parse_body_audio(const unsigned char *frame);
static int parse_body_sound_var(const unsigned char *frame);
static int parse_body_flash_prog(const unsigned char *frame);
static int parse_body_led(const unsigned char *frame);
static int parse_body_pong(const unsigned char *frame);

/**
 *
 */
LIBLOCAL void
tux_hw_status_init(void)
{
    memset(&hw_status_table, 0, sizeof(hw_status_table_t));
}

/**
 *
 */
LIBLOCAL int
tux_hw_status_parse_frame(const unsigned char *frame)
{
    switch(frame[0]) {
    case FRAME_HEADER_PORTS:
        tux_hw_status_header_counter[ID_FRAME_HEADER_PORTS]++;
        return parse_body_ports(frame);
    case FRAME_HEADER_SENSORS1:
        tux_hw_status_header_counter[ID_FRAME_HEADER_SENSORS1]++;
        return parse_body_sensors1(frame);
    case FRAME_HEADER_LIGHT:
        tux_hw_status_header_counter[ID_FRAME_HEADER_LIGHT]++;
        return parse_body_light(frame);
    case FRAME_HEADER_POSITION1:
        tux_hw_status_header_counter[ID_FRAME_HEADER_POSITION1]++;
        return parse_body_position1(frame);
    case FRAME_HEADER_POSITION2:
        tux_hw_status_header_counter[ID_FRAME_HEADER_POSITION2]++;
        return parse_body_position2(frame);
    case FRAME_HEADER_IR:
        tux_hw_status_header_counter[ID_FRAME_HEADER_IR]++;
        return parse_body_ir(frame);
    case FRAME_HEADER_ID:
        tux_hw_status_header_counter[ID_FRAME_HEADER_ID]++;
        return parse_body_id(frame);
    case FRAME_HEADER_BATTERY:
        tux_hw_status_header_counter[ID_FRAME_HEADER_BATTERY]++;
        return parse_body_battery(frame);
    case FRAME_HEADER_VERSION:
        tux_hw_status_header_counter[ID_FRAME_HEADER_VERSION]++;
        return parse_body_version(frame);
    case FRAME_HEADER_REVISION:
        tux_hw_status_header_counter[ID_FRAME_HEADER_REVISION]++;
        return parse_body_revision(frame);
    case FRAME_HEADER_AUTHOR:
        tux_hw_status_header_counter[ID_FRAME_HEADER_AUTHOR]++;
        return parse_body_author(frame);
    case FRAME_HEADER_AUDIO:
        tux_hw_status_header_counter[ID_FRAME_HEADER_AUDIO]++;
        return parse_body_audio(frame);
    case FRAME_HEADER_SOUND_VAR:
        tux_hw_status_header_counter[ID_FRAME_HEADER_SOUND_VAR]++;
        return parse_body_sound_var(frame);
    case FRAME_HEADER_FLASH_PROG:
        tux_hw_status_header_counter[ID_FRAME_HEADER_FLASH_PROG]++;
        return parse_body_flash_prog(frame);
    case FRAME_HEADER_LED:
        tux_hw_status_header_counter[ID_FRAME_HEADER_LED]++;
        return parse_body_led(frame);
    case FRAME_HEADER_PONG:
        tux_hw_status_header_counter[ID_FRAME_HEADER_PONG]++;
        return parse_body_pong(frame);
    default:
        return -1;
    }
}

/**
 *
 */
LIBLOCAL char *
tux_hw_status_id_to_str(const unsigned char id)
{
    switch(id) {
    case ID_FRAME_HEADER_PORTS:
        return "PORTS";
    case ID_FRAME_HEADER_SENSORS1:
        return "SENSORS1";
    case ID_FRAME_HEADER_LIGHT:
        return "LIGHT";
    case ID_FRAME_HEADER_POSITION1:
        return "POSITION1";
    case ID_FRAME_HEADER_POSITION2:
        return "POSITION2";
    case ID_FRAME_HEADER_IR:
        return "IR";
    case ID_FRAME_HEADER_ID:
        return "ID";
    case ID_FRAME_HEADER_BATTERY:
        return "BATTERY";
    case ID_FRAME_HEADER_VERSION:
        return "VERSION";
    case ID_FRAME_HEADER_REVISION:
        return "REVISION";
    case ID_FRAME_HEADER_AUTHOR:
        return "AUTHOR";
    case ID_FRAME_HEADER_AUDIO:
        return "AUDIO";
    case ID_FRAME_HEADER_SOUND_VAR:
        return "SOUND_VAR";
    case ID_FRAME_HEADER_FLASH_PROG:
        return "FLASH_PROG";
    case ID_FRAME_HEADER_LED:
        return "LED";
    case ID_FRAME_HEADER_PONG:
        return "PONG";
    default:
        return "NDEF";
    }
}

/**
 *
 */
LIBLOCAL void
tux_hw_status_header_counter_check(void)
{
    int i;
    unsigned char p_count = 0;

    for (i = 0; i < 16; i++)
    {
        p_count += tux_hw_status_header_counter[i];
    }

    /* as there is only debug code in this for loop it might make sense
       to have a guarding ifdef around the for loop to save a few cycles */
    if (p_count >= 15)
    {
        log_debug("Frames counter (%d) :\n\t%s:[%d]\n\t%s:[%d]\n\
                \r\t%s:[%d]\n\t%s:[%d]\n\t%s:[%d]\n\t%s:[%d]\n\t%s:[%d]\n\
                \r\t%s:[%d]\n\t%s:[%d]\n\t%s:[%d]\n\t%s:[%d]\n\t%s:[%d]\n\
                \r\t%s:[%d]\n\t%s:[%d]\n\t%s:[%d]\n\t%s:[%d]\n",
            p_count,
            tux_hw_status_id_to_str(0), tux_hw_status_header_counter[0],
            tux_hw_status_id_to_str(1), tux_hw_status_header_counter[1],
            tux_hw_status_id_to_str(2), tux_hw_status_header_counter[2],
            tux_hw_status_id_to_str(3), tux_hw_status_header_counter[3],
            tux_hw_status_id_to_str(4), tux_hw_status_header_counter[4],
            tux_hw_status_id_to_str(5), tux_hw_status_header_counter[5],
            tux_hw_status_id_to_str(6), tux_hw_status_header_counter[6],
            tux_hw_status_id_to_str(7), tux_hw_status_header_counter[7],
            tux_hw_status_id_to_str(8), tux_hw_status_header_counter[8],
            tux_hw_status_id_to_str(9), tux_hw_status_header_counter[9],
            tux_hw_status_id_to_str(10), tux_hw_status_header_counter[10],
            tux_hw_status_id_to_str(11), tux_hw_status_header_counter[11],
            tux_hw_status_id_to_str(12), tux_hw_status_header_counter[12],
            tux_hw_status_id_to_str(13), tux_hw_status_header_counter[13],
            tux_hw_status_id_to_str(14), tux_hw_status_header_counter[14],
            tux_hw_status_id_to_str(15), tux_hw_status_header_counter[15]);
    }

    // use memset instead ?
    for (i = 0; i < 16; i++)
    {
        tux_hw_status_header_counter[i] = 0;
    }
}

/**
 *
 */
static int
parse_body_ports(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.ports.portb.Byte != frame[1])
    {
        ret = FRAME_HEADER_PORTS;
    }
    if (hw_status_table.ports.portc.Byte != frame[2])
    {
        ret = FRAME_HEADER_PORTS;
    }
    if (hw_status_table.ports.portd.Byte != frame[3])
    {
        ret = FRAME_HEADER_PORTS;
    }

    hw_status_table.ports.portb.Byte = frame[1];
    hw_status_table.ports.portc.Byte = frame[2];
    hw_status_table.ports.portd.Byte = frame[3];

    return ret;
}

/**
 *
 */
static int
parse_body_sensors1(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.sensors1.sensors.Byte != frame[1])
    {
        ret = FRAME_HEADER_SENSORS1;
    }
    if (hw_status_table.sensors1.play_internal_sound != frame[2])
    {
        ret = FRAME_HEADER_SENSORS1;
    }
    if (hw_status_table.sensors1.play_general_sound != frame[3])
    {
        ret = FRAME_HEADER_SENSORS1;
    }

    hw_status_table.sensors1.sensors.Byte = frame[1];
    hw_status_table.sensors1.play_internal_sound = frame[2];
    hw_status_table.sensors1.play_general_sound = frame[3];

    return ret;
}

/**
 *
 */
static int
parse_body_light(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.light.high_level != frame[1])
    {
        ret = FRAME_HEADER_LIGHT;
    }
    if (hw_status_table.light.low_level != frame[2])
    {
        ret = FRAME_HEADER_LIGHT;
    }
    if (hw_status_table.light.mode != frame[3])
    {
        ret = FRAME_HEADER_LIGHT;
    }

    hw_status_table.light.high_level = frame[1];
    hw_status_table.light.low_level = frame[2];
    hw_status_table.light.mode = frame[3];

    return ret;
}

/**
 *
 */
static int
parse_body_position1(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.position1.eyes_remaining_mvm != frame[1])
    {
        ret = FRAME_HEADER_POSITION1;
    }
    if (hw_status_table.position1.mouth_remaining_mvm != frame[2])
    {
        ret = FRAME_HEADER_POSITION1;
    }
    if (hw_status_table.position1.flippers_remaining_mvm != frame[3])
    {
        ret = FRAME_HEADER_POSITION1;
    }

    hw_status_table.position1.eyes_remaining_mvm = frame[1];
    hw_status_table.position1.mouth_remaining_mvm = frame[2];
    hw_status_table.position1.flippers_remaining_mvm = frame[3];

    return ret;
}

/**
 *
 */
static int
parse_body_position2(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.position2.spin_remaining_mvm != frame[1])
    {
        ret = FRAME_HEADER_POSITION2;
    }
    if (hw_status_table.position2.flippers_down != frame[2])
    {
        ret = FRAME_HEADER_POSITION2;
    }
    if (hw_status_table.position2.motors.Byte != frame[3])
    {
        ret = FRAME_HEADER_POSITION2;
    }

    hw_status_table.position2.spin_remaining_mvm = frame[1];
    hw_status_table.position2.flippers_down = frame[2];
    hw_status_table.position2.motors.Byte = frame[3];

    return ret;
}

/**
 *
 */
static int
parse_body_ir(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.ir.rc5_code.Byte != frame[1])
    {
        ret = FRAME_HEADER_IR;
    }

    hw_status_table.ir.rc5_code.Byte = frame[1];

    return ret;
}

/**
 *
 */
static int
parse_body_id(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.id.msb_number != frame[1])
    {
        ret = FRAME_HEADER_ID;
    }
    if (hw_status_table.id.lsb_number != frame[2])
    {
        ret = FRAME_HEADER_ID;
    }

    hw_status_table.id.msb_number = frame[1];
    hw_status_table.id.lsb_number = frame[2];

    return ret;
}

/**
 *
 */
static int
parse_body_battery(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.battery.high_level != frame[1])
    {
        ret = FRAME_HEADER_BATTERY;
    }
    if (hw_status_table.battery.low_level != frame[2])
    {
        ret = FRAME_HEADER_BATTERY;
    }
    if (hw_status_table.battery.motors_state != frame[3])
    {
        ret = FRAME_HEADER_BATTERY;
    }

    hw_status_table.battery.high_level = frame[1];
    hw_status_table.battery.low_level = frame[2];
    hw_status_table.battery.motors_state = frame[3];

    return ret;
}

/**
 *
 */
static int
parse_body_version(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.version.cm.Byte != frame[1])
    {
        ret = FRAME_HEADER_VERSION;
    }
    if (hw_status_table.version.minor != frame[2])
    {
        ret = FRAME_HEADER_VERSION;
    }
    if (hw_status_table.version.update != frame[3])
    {
        ret = FRAME_HEADER_VERSION;
    }

    hw_status_table.version.cm.Byte = frame[1];
    hw_status_table.version.minor = frame[2];
    hw_status_table.version.update = frame[3];

    return ret;
}

/**
 *
 */
LIBLOCAL void
tux_hw_parse_body_version(const unsigned char *frame)
{
    parse_body_version(frame);
}

/**
 *
 */
static int
parse_body_revision(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.revision.lsb_number != frame[1])
    {
        ret = FRAME_HEADER_REVISION;
    }
    if (hw_status_table.revision.msb_number != frame[2])
    {
        ret = FRAME_HEADER_REVISION;
    }
    if (hw_status_table.revision.release_type.Byte != frame[3])
    {
        ret = FRAME_HEADER_REVISION;
    }

    hw_status_table.revision.lsb_number = frame[1];
    hw_status_table.revision.msb_number = frame[2];
    hw_status_table.revision.release_type.Byte = frame[3];

    return ret;
}

/**
 *
 */
LIBLOCAL void
tux_hw_parse_body_revision(const unsigned char *frame)
{
    parse_body_revision(frame);
}

/**
 *
 */
static int
parse_body_author(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.author.lsb_id != frame[1])
    {
        ret = FRAME_HEADER_AUTHOR;
    }
    if (hw_status_table.author.msb_id != frame[2])
    {
        ret = FRAME_HEADER_AUTHOR;
    }
    if (hw_status_table.author.variation_number != frame[3])
    {
        ret = FRAME_HEADER_AUTHOR;
    }

    hw_status_table.author.lsb_id = frame[1];
    hw_status_table.author.msb_id = frame[2];
    hw_status_table.author.variation_number = frame[3];

    return ret;
}

/**
 *
 */
LIBLOCAL void
tux_hw_parse_body_author(const unsigned char *frame)
{
    parse_body_author(frame);
}

/**
 *
 */
static int
parse_body_audio(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.audio.sound_track_played != frame[1])
    {
        ret = FRAME_HEADER_AUDIO;
    }
    if (hw_status_table.audio.programming_steps.Byte != frame[2])
    {
        ret = FRAME_HEADER_AUDIO;
    }
    if (hw_status_table.audio.programmed_sound_track != frame[3])
    {
        ret = FRAME_HEADER_AUDIO;
    }

    hw_status_table.audio.sound_track_played = frame[1];
    hw_status_table.audio.programming_steps.Byte = frame[2];
    hw_status_table.audio.programmed_sound_track = frame[3];

    return ret;
}

/**
 *
 */
static int
parse_body_sound_var(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.sound_var.number_of_sounds != frame[1])
    {
        ret = FRAME_HEADER_SOUND_VAR;
    }
    if (hw_status_table.sound_var.flash_usage != frame[2])
    {
        ret = FRAME_HEADER_SOUND_VAR;
    }

    hw_status_table.sound_var.number_of_sounds = frame[1];
    hw_status_table.sound_var.flash_usage = frame[2];

    return ret;
}

/**
 *
 */
static int
parse_body_flash_prog(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.flash_prog.current_state != frame[1])
    {
        ret = FRAME_HEADER_FLASH_PROG;
    }
    if (hw_status_table.flash_prog.last_sound_size != frame[2])
    {
        ret = FRAME_HEADER_FLASH_PROG;
    }

    hw_status_table.flash_prog.current_state = frame[1];
    hw_status_table.flash_prog.last_sound_size = frame[2];

    return ret;
}

/**
 *
 */
static int
parse_body_led(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.led.left_led_intensity != frame[1])
    {
        ret = FRAME_HEADER_LED;
    }
    if (hw_status_table.led.right_led_intensity != frame[2])
    {
        ret = FRAME_HEADER_LED;
    }
    if (hw_status_table.led.effect_status.Byte != frame[3])
    {
        ret = FRAME_HEADER_LED;
    }

    hw_status_table.led.left_led_intensity = frame[1];
    hw_status_table.led.right_led_intensity = frame[2];
    hw_status_table.led.effect_status.Byte = frame[3];

    return ret;
}

/**
 *
 */
static int
parse_body_pong(const unsigned char *frame)
{
    int ret = 0;

    if (hw_status_table.pong.pongs_pending_number != frame[1])
    {
        ret = FRAME_HEADER_PONG;
    }
    if (hw_status_table.pong.pongs_lost_by_i2c_number != frame[2])
    {
        ret = FRAME_HEADER_PONG;
    }
    if (hw_status_table.pong.pongs_lost_by_rf_number != frame[3])
    {
        ret = FRAME_HEADER_PONG;
    }

    hw_status_table.pong.pongs_pending_number = frame[1];
    hw_status_table.pong.pongs_lost_by_i2c_number = frame[2];
    hw_status_table.pong.pongs_lost_by_rf_number = frame[3];

    return ret;
}
