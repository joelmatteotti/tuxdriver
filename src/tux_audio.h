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
 * \file tux_audio.h
 * \brief Audio speaker header.
 * \author remi.jocaille@c2me.be
 * \ingroup audio
 */

#ifndef _TUX_AUDIO_H_
#define _TUX_AUDIO_H_

#include <stdbool.h>

extern bool tux_audio_cmd_channel_general(void);
extern bool tux_audio_cmd_channel_tts(void);
extern bool tux_audio_cmd_mute(bool value);

#endif /* _TUX_AUDIO_H_ */
