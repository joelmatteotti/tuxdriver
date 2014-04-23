/*
 * Tux Droid - Audio
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
 * \file tux_audio.c
 * \brief Audio speaker functions.
 * \author remi.jocaille@c2me.be
 * \ingroup audio
 */

#include "tux_audio.h"
#include "tux_hw_cmd.h"
#include "tux_types.h"
#include "tux_usb.h"

/**
 * \brief Send a command to switch the audio stream to Tuxdroid-Audio channel.
 * \return The sending success.
 */
LIBLOCAL bool
tux_audio_cmd_channel_general(void)
{
    data_frame frame = {USB_DONGLE_AUDIO_CMD, 0, 0, 0};
    return tux_usb_send_to_dongle(frame);
}

/**
 * \brief Send a command to switch the audio stream to Tuxdroid-TTS channel.
 * \return The sending success.
 */
LIBLOCAL bool
tux_audio_cmd_channel_tts(void)
{
    data_frame frame = {USB_DONGLE_AUDIO_CMD, 1, 0, 0};
    return tux_usb_send_to_dongle(frame);
}

/**
 * \brief Send a command to mute/unmute the speaker.
 * \param value Mute value.
 * \return The sending success.
 */
LIBLOCAL bool
tux_audio_cmd_mute(bool value)
{
    data_frame frame = {AUDIO_MUTE_CMD, 0, 0, 0};

    if (value)
    {
        frame[1] = 1;
    }
    else
    {
        frame[1] = 0;
    }

    return tux_usb_send_to_tux(frame);
}

