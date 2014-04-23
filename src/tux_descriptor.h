/*
 * Tux Droid - Status descriptor
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
 * \file tux_descriptor.h
 * \brief Descriptor header.
 * \author remi.jocaille@c2me.be
 * \ingroup descriptor
 */

#ifndef _TUX_DESCRIPTOR_H_
#define _TUX_DESCRIPTOR_H_

#include "tux_firmware.h"
#include "tux_sound_flash.h"
#include "tux_id.h"

/** \brief Output file path of the descriptor */
#define DESCRIPTOR_FILE_PATH                "./descriptor.txt"

/**
 * \brief Tuxdroid descriptor.
 * The descriptor contains some informations about the Tuxdroid hardware.
 */
typedef struct {
    struct firmwares_t {
        firmware_descriptor_t *package; /**< Package version */
        firmware_descriptor_t *tuxcore; /**< Tuxcore version */
        firmware_descriptor_t *tuxaudio; /**< Tuxaudio version */
        firmware_descriptor_t *tuxrf; /**< Tuxrf version */
        firmware_descriptor_t *fuxrf; /**< Fuxrf version */
        firmware_descriptor_t *fuxusb; /**< Fuxusb version */
    } firmwares; /**< About firmwares */
    struct driver_version_t {
        unsigned int version_major; /**< Major version number */
        unsigned int version_minor; /**< Minor version number */
        unsigned int version_update; /**< Update version number */
        unsigned int version_build; /**< Build version number */
        char version_state[100]; /**< Version state */
        char version_string[100]; /**< Complete version string */
    } driver; /**< About libtuxdriver */
    sound_flash_descriptor_t *sound_flash; /**< About the sound flash */
    id_descriptor_t *id; /**< About Id connection */
} tux_descriptor_t;

extern void tux_descriptor_init(void);
extern void tux_descriptor_get(void);
extern void tux_descriptor_update(void);

#endif /* _TUX_STATUS_DESCRIPTOR_H_ */
