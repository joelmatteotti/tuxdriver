/*
 * Tux Droid - Firmware
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
 * \file tux_firmware.h
 * \brief Firmware header.
 * \author remi.jocaille@c2me.be
 * \ingroup firmware
 */

#ifndef _TUX_FIRMWARE_H_
#define _TUX_FIRMWARE_H_

#include <stdbool.h>

/** \brief First CPU index */
#define LOWEST_CPU_NUM                              0
/** \brief Last CPU index */
#define HIGHEST_CPU_NUM                             4
/** \brief Number of CPU */
#define NUMBER_OF_CPU                               (HIGHEST_CPU_NUM + 1)
/** \brief Maximal length of a version string */
#define VERSION_STRING_LENGTH                       256
/** \brief Tuxcore CPU name */
#define CPU_STR_NAME_TUXCORE                        "Tuxcore"
/** \brief Tuxaudio CPU name */
#define CPU_STR_NAME_TUXAUDIO                       "Tuxaudio"
/** \brief Tuxrf CPU name */
#define CPU_STR_NAME_TUXRF                          "TuxRF"
/** \brief Fuxrf CPU name */
#define CPU_STR_NAME_FUXRF                          "FuxRF"
/** \brief Fuxusb CPU name */
#define CPU_STR_NAME_FUXUSB                         "FuxUSB"
/** \brief Package release name */
#define RELEASE_STR_NAME                            "Global release package"
/** \brief Unofficial package name */
#define UNOFFICIAL_RELEASE_STR                      "Unofficial package"
/** \brief Number of retry count for the firmware versions retrieving */
#define TUX_FIRMWARE_RETRY_COUNT                    4
#define TUX_FIRMWARE_GLOBAL_RETRY_COUNT             4

/** \brief CPU identifiers */
typedef enum
{
    INVALID_CPU_NUM = -1, /**< Invalid CPU id */
    TUXCORE_CPU_NUM = LOWEST_CPU_NUM, /**< Tuxcore CPU id */
    TUXAUDIO_CPU_NUM, /**< Tuxaudio CPU id */
    TUXRF_CPU_NUM, /**< Tuxrf CPU id */
    FUXRF_CPU_NUM, /**< Fuxrf CPU id */
    FUXUSB_CPU_NUM, /**< Fuxusb CPU id */
} CPU_IDENTIFIERS;

/** \brief Versioning states. */
typedef enum
{
    STDBY, /**< State machine STANDBY */
    INIT, /**< State machine Initialization */
    INFO_REQ, /**< State machine Request a firmware version*/
    INFO_GET, /**< State machine Get a firmware version */
    SPECIAL, /**< State machine Treats the special cases */
    RELEASE, /**< State machine Determine the package release version */
    FINALIZE, /**< State machine Finalization */
} versioning_state_t;

/** \brief Firmware descriptor structure */
typedef struct
{
    CPU_IDENTIFIERS cpu_id; /**< CPU identifiant */
    unsigned int version_major; /**< Major version */
    unsigned int version_minor; /**< Minor version */
    unsigned int version_update; /**< Update version */
    unsigned int revision; /**< SVN revision */
    bool release; /**< Is released */
    bool local_modification; /**< Is a local modification */
    bool mixed_revisions; /**< Is a mixed modification */
    unsigned int author; /**< Author Id */
    unsigned int variation; /**< Variation Id */
    char version_string[VERSION_STRING_LENGTH];
    /**< String representation of a firmware version */
} firmware_descriptor_t;

/** \brief Array structure of firmware descriptors */
typedef firmware_descriptor_t firmwares_descriptor_t[NUMBER_OF_CPU];

/** \brief Shared : Firmware descriptors */
extern firmwares_descriptor_t firmwares_desc;
/** \brief Shared : Package release descriptor */
extern firmware_descriptor_t firmware_release_desc;

extern char knowed_tuxcore_symbolic_version[VERSION_STRING_LENGTH];
extern char knowed_tuxaudio_symbolic_version[VERSION_STRING_LENGTH];
extern char knowed_fuxusb_symbolic_version[VERSION_STRING_LENGTH];
extern char knowed_fuxrf_symbolic_version[VERSION_STRING_LENGTH];
extern char knowed_tuxrf_symbolic_version[VERSION_STRING_LENGTH];

extern void tux_firmware_init_descriptor(void);
extern void tux_firmware_update_version(void);
extern void tux_firmware_update_revision(void);
extern void tux_firmware_update_author(void);
extern void tux_firmware_state_machine_call(void);
extern void tux_firmware_get_descriptor(void);
extern bool tux_firmware_check_new_descriptor(bool save);

#endif /* _TUX_FIRMWARE_H_ */
