/*
 * Tux Droid - Sound flash
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

#ifndef _TUX_SOUND_FLASH_H_
#define _TUX_SOUND_FLASH_H_

#include <stdbool.h>

#include "tux_error.h"

typedef struct
{
    unsigned int number_of_sounds;
    unsigned int flash_usage;
    unsigned int available_record_time;
} sound_flash_descriptor_t;

extern sound_flash_descriptor_t sound_flash_desc;

extern char knowed_track_num[128];

extern void tux_sound_flash_init_descriptor(void);
extern char *tux_sound_flash_dump_descriptor(char *descriptor);
extern void tux_sound_flash_update_flash_play(void);
extern void tux_sound_flash_update_general_play(void);
extern void tux_sound_flash_update_prog_current_track(void);
extern void tux_sound_flash_update_prog_last_track_size(void);
extern void tux_sound_flash_update(void);
extern void tux_sound_flash_get_descripor(void);
extern bool tux_sound_flash_check_new_descriptor(bool save);
extern bool tux_sound_flash_cmd_play(unsigned char track_num, float volume);
extern void tux_sound_flash_state_machine_call(void);
extern TuxDrvError tux_sound_flash_cmd_reflash(const char *tracks);
extern void tux_sound_flash_avoid_tts_default_sound_card(void);

#endif /* _TUX_SOUND_FLASH_H_ */
