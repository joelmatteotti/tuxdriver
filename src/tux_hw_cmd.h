/*
 * Tux Droid - Low level commands
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

#ifndef _TUX_HW_CMD_H_
#define _TUX_HW_CMD_H_

typedef enum
{
    USB_HEADER_TUX              = 0,
    USB_HEADER_DONGLE           = 1,
    USB_HEADER_BOOTLOADER       = 2,
} usb_command_header_t;

typedef enum
{
    USB_DONGLE_CONNECTION_CMD   = 0,
    USB_DONGLE_STATUS_CMD       = 1,
    USB_DONGLE_AUDIO_CMD        = 2,
    USB_DONGLE_VERSION_CMD      = 6,
} usb_dongle_commands_t;

typedef enum
{
    USB_TUX_CONNECTION_DISCONNECT       = 1,
    USB_TUX_CONNECTION_CONNECT          = 2,
    USB_TUX_CONNECTION_ID_REQUEST       = 3,
    USB_TUX_CONNECTION_ID_LOOKUP        = 4,
    USB_TUX_CONNECTION_CHANGE_ID        = 5,
    USB_TUX_CONNECTION_WAKEUP           = 6,
    USB_TUX_CONNECTION_WIRELESS_CHANNEL = 7,
} usb_tux_connection_t;

typedef enum
{
    EYES_OPEN_CMD               = 0x33,
    EYES_CLOSE_CMD              = 0x38,
    EYES_BLINK_CMD              = 0x40,
    EYES_STOP_CMD               = 0x32,

    MOUTH_OPEN_CMD              = 0x34,
    MOUTH_CLOSE_CMD             = 0x35,
    MOUTH_MOVE_CMD              = 0x41,
    MOUTH_STOP_CMD              = 0x36,

    FLIPPERS_RAISE_CMD             = 0x39,
    FLIPPERS_LOWER_CMD             = 0x3A,
    FLIPPERS_WAVE_CMD              = 0x80,
    FLIPPERS_STOP_CMD              = 0x30,

    SPIN_LEFT_CMD               = 0x83,
    SPIN_RIGHT_CMD              = 0x82,
    SPIN_STOP_CMD               = 0x37,

    LED_FADE_SPEED_CMD          = 0xD0,
    LED_SET_CMD                 = 0xD1,
    LED_PULSE_RANGE_CMD         = 0xD2,
    LED_PULSE_CMD               = 0xD3,

    TUX_PONG_PING_CMD           = 0x7F,

    MOTORS_SET_CMD              = 0xD4,
    MOTORS_CONFIG_CMD           = 0x81,

    TURN_IR_ON_CMD              = 0x17,
    TURN_IR_OFF_CMD             = 0x18,
    IR_SEND_RC5_CMD             = 0x91,

    PLAY_SOUND_CMD              = 0x90,
    STORE_SOUND_CMD             = 0x52,
    CONFIRM_STORAGE_CMD         = 0x53,
    ERASE_FLASH_CMD             = 0x54,

    AUDIO_MUTE_CMD              = 0x92,

    WIRELESS_FREQ_BOUNDARIES_CMD = 0x88
} usb_tux_commands_t;


#endif /* _TUX_HW_CMD_H_ */
