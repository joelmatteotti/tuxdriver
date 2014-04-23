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
 * \file tux_firmware.c
 * \brief Firmware functions.
 * \author remi.jocaille@c2me.be
 * \ingroup firmware
 * \todo Remove deprecated functions.
 * \todo Implement the new way to retrieve the RF versions.
 */

#include <stdio.h>
#include <string.h>

#include "tux_descriptor.h"
#include "tux_firmware.h"
#include "tux_hw_status.h"
#include "tux_misc.h"
#include "tux_sw_status.h"
#include "tux_usb.h"
#include "log.h"

/** State of the versioning state machine. */
static versioning_state_t versioning_state = STDBY;
/** Used to hold the CPU number that is currently sending its information as it
 * needs multiple commands to send it all. */
static int current_cpu = INVALID_CPU_NUM;
/** Number of cpu */
static int cpu_num;
/** Retries counter of firmware versions */
static int retries;
static int global_retries;

/** Descriptor of the firmwares */
LIBLOCAL firmwares_descriptor_t firmwares_desc;
/** Descriptor of the firmwares package */
LIBLOCAL firmware_descriptor_t firmware_release_desc;

LIBLOCAL char knowed_tuxcore_symbolic_version[256] = "Tuxcore 0.0.0";
LIBLOCAL char knowed_tuxaudio_symbolic_version[256] = "Tuxaudio 0.0.0";
LIBLOCAL char knowed_fuxusb_symbolic_version[256] = "FuxUSB 0.0.0";
LIBLOCAL char knowed_fuxrf_symbolic_version[256] = "FuxRF 0.0.0";
LIBLOCAL char knowed_tuxrf_symbolic_version[256] = "TuxRF 0.0.0";

/**
 * \brief Get the name of a tux cpu.
 * \param id cpu identifier.
 * \return a string.
 */
static const char *
cpu_id_to_name(int id)
{
    switch (id)
    {
    case TUXCORE_CPU_NUM:
        return CPU_STR_NAME_TUXCORE;
    case TUXAUDIO_CPU_NUM:
        return CPU_STR_NAME_TUXAUDIO;
    case TUXRF_CPU_NUM:
        return CPU_STR_NAME_TUXRF;
    case FUXRF_CPU_NUM:
        return CPU_STR_NAME_FUXRF;
    case FUXUSB_CPU_NUM:
        return CPU_STR_NAME_FUXUSB;
    default:
        return "CPU_UNKNOW";
    }
}

/**
 * \deprecated
 */
static void
load_knowed_symbolic_versions(void)
{
#ifdef LOCK_TUX
    FILE *f;
    char ret_str[] = "\n";
    char *ret_c;

    f = fopen("./symbolic_versions", "r");

    if (f) {
        fgets(knowed_tuxcore_symbolic_version,
            sizeof(knowed_tuxcore_symbolic_version)-2, f);
        ret_c = strstr(knowed_tuxcore_symbolic_version, ret_str);
        *ret_c = '\0';
        fgets(knowed_tuxaudio_symbolic_version,
            sizeof(knowed_tuxaudio_symbolic_version)-2, f);
        ret_c = strstr(knowed_tuxaudio_symbolic_version, ret_str);
        *ret_c = '\0';
        fgets(knowed_fuxusb_symbolic_version,
            sizeof(knowed_fuxusb_symbolic_version)-2, f);
        ret_c = strstr(knowed_fuxusb_symbolic_version, ret_str);
        *ret_c = '\0';
        fgets(knowed_fuxrf_symbolic_version,
            sizeof(knowed_fuxrf_symbolic_version)-2, f);
        ret_c = strstr(knowed_fuxrf_symbolic_version, ret_str);
        *ret_c = '\0';
        fgets(knowed_tuxrf_symbolic_version,
            sizeof(knowed_tuxrf_symbolic_version)-2, f);
        ret_c = strstr(knowed_tuxrf_symbolic_version, ret_str);
        *ret_c = '\0';
        fclose(f);
    }
#endif
}

/**
 * \deprecated
 */
#ifdef LOCK_TUX
static void
save_knowed_symbolic_versions(void)
{
    FILE *f;

    f = fopen("./symbolic_versions", "w");

    if (f)
    {
        fprintf(f, "%s\n", firmwares_desc[TUXCORE_CPU_NUM].version_string);
        fprintf(f, "%s\n", firmwares_desc[TUXAUDIO_CPU_NUM].version_string);
        fprintf(f, "%s\n", firmwares_desc[FUXUSB_CPU_NUM].version_string);
        fprintf(f, "%s\n", firmwares_desc[FUXRF_CPU_NUM].version_string);
        fprintf(f, "%s\n", firmwares_desc[TUXRF_CPU_NUM].version_string);
        fclose(f);
    }
}
#endif

/**
 * \deprecated
 */
LIBLOCAL bool
tux_firmware_check_new_descriptor(bool save)
{
#ifdef LOCK_TUX
    bool ret = false;

    if (strcmp(knowed_tuxcore_symbolic_version,
        firmwares_desc[TUXCORE_CPU_NUM].version_string))
    {
        ret = true;
    }
    if (strcmp(knowed_tuxaudio_symbolic_version,
        firmwares_desc[TUXAUDIO_CPU_NUM].version_string))
    {
        ret = true;
    }
    if (strcmp(knowed_tuxrf_symbolic_version,
        firmwares_desc[TUXRF_CPU_NUM].version_string))
    {
        ret = true;
    }

    if (save)
    {
        save_knowed_symbolic_versions();
    }

    return ret;
#else
    return false;
#endif
}

/**
 * \brief Initilize the firmware part of the descriptor.
 */
LIBLOCAL void
tux_firmware_init_descriptor(void)
{
    memset(&firmwares_desc, 0, sizeof(firmwares_descriptor_t));
    memset(&firmware_release_desc, 0, sizeof(firmware_descriptor_t));
    load_knowed_symbolic_versions();
    versioning_state = STDBY;
    global_retries = TUX_FIRMWARE_GLOBAL_RETRY_COUNT;
}

/**
 * \deprecated
 */
static char *
dump_descriptor_of_cpu(char *descriptor, const firmware_descriptor_t *desc)
{
    char *p = descriptor + strlen(descriptor);

    p = p + sprintf(p,
        "    CPU id : \t\t\t%d\n"
        "    CPU name : \t\t\t%s\n"
        "    Version major : \t\t%d\n"
        "    Version minor : \t\t%d\n"
        "    Version update : \t\t%d\n"
        "    Revision : \t\t\t%d\n"
        "    Released : \t\t\t%s\n"
        "    Local modification : \t%s\n"
        "    Mixed revisions : \t\t%s\n"
        "    Author id : \t\t%d\n"
        "    Variation : \t\t%d\n"
        "    Symbolic version : \t\t[%s]\n",
        desc->cpu_id,
        cpu_id_to_name(desc->cpu_id),
        desc->version_major,
        desc->version_minor,
        desc->version_update,
        desc->revision,
        desc->release ? "True" : "False",
        desc->local_modification ? "True" : "False",
        desc->mixed_revisions ? "True" : "False",
        desc->author,
        desc->variation,
        desc->version_string
    );
    return p;
}



/**
 * \brief Update the status of the version of a cpu.
 */
LIBLOCAL void
tux_firmware_update_version(void)
{
    firmware_descriptor_t *f_desc;

    current_cpu = hw_status_table.version.cm.bits.cpu_number;
    f_desc = &firmwares_desc[current_cpu];
    f_desc->cpu_id = current_cpu;
    f_desc->version_major = hw_status_table.version.cm.bits.major;
    f_desc->version_minor = hw_status_table.version.minor;
    f_desc->version_update = hw_status_table.version.update;

    if ((current_cpu == TUXRF_CPU_NUM) || (current_cpu == FUXRF_CPU_NUM))
    {
        sprintf(f_desc->version_string, "%s_%d.%d.%d",
            cpu_id_to_name(current_cpu),
            f_desc->version_major,
            f_desc->version_minor,
            f_desc->version_update);
    }
}

/**
 * \brief Update the status of the revision of a cpu.
 */
LIBLOCAL void
tux_firmware_update_revision(void)
{
    char revision_string[40];
    firmware_descriptor_t *f_desc;

    if (current_cpu == INVALID_CPU_NUM)
    {
        return;
    }

    f_desc = &firmwares_desc[current_cpu];
    f_desc->revision = hw_status_table.revision.msb_number << 8;
    f_desc->revision += hw_status_table.revision.lsb_number;
    f_desc->release = hw_status_table.revision.release_type.bits.original_release;
    f_desc->local_modification = hw_status_table.revision.release_type.bits.local_modification;
    f_desc->mixed_revisions = hw_status_table.revision.release_type.bits.mixed_update;

    if ((f_desc->local_modification || f_desc->mixed_revisions)
        && f_desc->release)
    {
        f_desc->release = 0;
        /* Log warning */
    }

    sprintf(revision_string, " - r%d (SVN/UNRELEASED)", f_desc->revision);
    sprintf(f_desc->version_string, "%s_%d.%d.%d%s%s%s",
            cpu_id_to_name(current_cpu),
            f_desc->version_major, f_desc->version_minor,
            f_desc->version_update,
            f_desc->release ? "" : revision_string,
            f_desc->local_modification ? "(modified locally)" : "",
            f_desc->mixed_revisions ? "(mixed revisions)" : "");
}

/**
 * \brief Update the status of the author of a cpu.
 */
LIBLOCAL void
tux_firmware_update_author(void)
{
    firmware_descriptor_t *f_desc;

    if (current_cpu == INVALID_CPU_NUM)
    {
        return;
    }

    f_desc = &firmwares_desc[current_cpu];
    f_desc->author = hw_status_table.author.msb_id << 8;
    f_desc->author += hw_status_table.author.lsb_id;
    f_desc->variation = hw_status_table.author.variation_number;
}

/**
 * \brief Request the versionning of a cpu.
 * \param cpu_id cpu identifier.
 * \return 0 or 1
 */
static int
send_firmware_versionning_request(const int cpu_id)
{
    int ret;
    unsigned char data[4] = {(cpu_id + 2), 0, 0, 0};

#ifdef USB_DEBUG
    printf("USB debug : Send firmware versionning request (%s)\n",
            cpu_id_to_name(cpu_id));
#endif

    if (cpu_id == FUXUSB_CPU_NUM)
    {
        ret = tux_usb_send_to_dongle(data);
    }
    else
    {
        ret = tux_usb_send_to_tux(data);
    }
    if (ret)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/**
 * \brief Test the version of a firmware.
 * \param firmware Firmware descriptor.
 * \param major Major number to match.
 * \param minor Minor number to match.
 * \param update Update number to match.
 * \return The firmware version match result.
 */
static bool
test_fw_ver(const firmware_descriptor_t *firmware, int major, int minor,
        int update)
{
    if ((firmware->version_major != major) ||
        (firmware->version_minor != minor) ||
        (firmware->version_update != update))
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * \brief Function to determine the firmwares package version.
 */
static void
determine_release_package(void)
{
    int cpu_num = LOWEST_CPU_NUM;
    int major = 0, minor = 0, update = 0;
    int t_major, t_minor, t_update;
    bool perfect_package = true;
    bool ret;
    bool first_rf_ver;
    firmware_descriptor_t *release, *firmware;

/*
    firmwares_desc[TUXCORE_CPU_NUM].version_major = 0;
    firmwares_desc[TUXCORE_CPU_NUM].version_minor = 4;
    firmwares_desc[TUXCORE_CPU_NUM].version_update = 0;
    firmwares_desc[TUXCORE_CPU_NUM].release = true;
    firmwares_desc[TUXAUDIO_CPU_NUM].version_major = 0;
    firmwares_desc[TUXAUDIO_CPU_NUM].version_minor = 4;
    firmwares_desc[TUXAUDIO_CPU_NUM].version_update = 0;
    firmwares_desc[TUXAUDIO_CPU_NUM].release = true;
    firmwares_desc[TUXRF_CPU_NUM].version_major = 0;
    firmwares_desc[TUXRF_CPU_NUM].version_minor = 3;
    firmwares_desc[TUXRF_CPU_NUM].version_update = 0;
    firmwares_desc[TUXRF_CPU_NUM].release = true;
    firmwares_desc[FUXRF_CPU_NUM].version_major = 0;
    firmwares_desc[FUXRF_CPU_NUM].version_minor = 3;
    firmwares_desc[FUXRF_CPU_NUM].version_update = 0;
    firmwares_desc[FUXRF_CPU_NUM].release = true;
    firmwares_desc[FUXUSB_CPU_NUM].version_major = 0;
    firmwares_desc[FUXUSB_CPU_NUM].version_minor = 4;
    firmwares_desc[FUXUSB_CPU_NUM].version_update = 0;
    firmwares_desc[FUXUSB_CPU_NUM].release = true;
*/

    release = &firmware_release_desc;

    strcpy(release->version_string, UNOFFICIAL_RELEASE_STR);

    /* Get the version of the first firmware */
    t_major = firmwares_desc[LOWEST_CPU_NUM].version_major;
    t_minor = firmwares_desc[LOWEST_CPU_NUM].version_minor;
    t_update = firmwares_desc[LOWEST_CPU_NUM].version_update;

    /* Check if both RF versions is 0.3.0 (first revision) */
    first_rf_ver = test_fw_ver(&firmwares_desc[FUXRF_CPU_NUM], 0, 3, 0);
    first_rf_ver &= test_fw_ver(&firmwares_desc[TUXRF_CPU_NUM], 0, 3, 0);

    /* Check if all firmwares is released and it have the same
     * version */
    while (cpu_num <= HIGHEST_CPU_NUM)
    {
        if (first_rf_ver && ((cpu_num == FUXRF_CPU_NUM) ||
            (cpu_num == TUXRF_CPU_NUM)))
        {
            cpu_num ++;
        }
        else
        {
            firmware = &firmwares_desc[cpu_num];
            if (!firmware->release)
            {
                /* Firmware is not released -> FAIL */
                return;
            }
            if (!test_fw_ver(firmware, t_major, t_minor, t_update))
            {
                /* Firmware have not the same version than the first one */
                perfect_package = false;
            }
            cpu_num ++;
        }
    }

    /* If the firmware set is not a perfect package */
    if (!perfect_package)
    {
        /* Determine the general version major */
        cpu_num = LOWEST_CPU_NUM;
        while (cpu_num <= HIGHEST_CPU_NUM)
        {
            firmware = &firmwares_desc[cpu_num];
            if (firmware->version_major > major)
            {
                major = firmware->version_major;
            }
            cpu_num ++;
        }
        /* Determine the general version minor */
        cpu_num = LOWEST_CPU_NUM;
        while (cpu_num <= HIGHEST_CPU_NUM)
        {
            firmware = &firmwares_desc[cpu_num];
            if ((firmware->version_major == major) &&
                (firmware->version_minor > minor))
            {
                minor = firmware->version_minor;
            }
            cpu_num ++;
        }
        /* Determine the general version update */
        cpu_num = LOWEST_CPU_NUM;
        while (cpu_num <= HIGHEST_CPU_NUM)
        {
            firmware = &firmwares_desc[cpu_num];
            if ((firmware->version_major == major) &&
                (firmware->version_minor == minor) &&
                (firmware->version_update > update))
            {
                update = firmware->version_update;
            }
            cpu_num ++;
        }

        /* Test if the firmware set is an official package */

        /* release 0.3.1, tuxcore and tuxaudio updated to 0.3.1 */
        if ((major == 0) && (minor == 3) && (update == 1))
        {
            ret = test_fw_ver(&firmwares_desc[TUXCORE_CPU_NUM], 0, 3, 1);
            ret &= test_fw_ver(&firmwares_desc[TUXAUDIO_CPU_NUM], 0, 3, 1);
            ret &= test_fw_ver(&firmwares_desc[FUXUSB_CPU_NUM], 0, 3, 0);
            if (!ret)
            {
                /* Not official package -> FAIL */
                return;
            }
        }
        else
        {
            /* Not other special case -> FAIL */
            return;
        }
    }
    else
    {
        major = t_major;
        minor = t_minor;
        update = t_update;
    }

    /* It's a release. */
    release->version_major = major;
    release->version_minor = minor;
    release->version_update = update;
    /* Author and variation are taken from tuxcore as that's the CPU most */
    /* likely to be customized. */
    release->author = firmwares_desc[TUXCORE_CPU_NUM].author;
    release->variation = firmwares_desc[TUXCORE_CPU_NUM].variation;
    sprintf(release->version_string, "tuxdroid firmware release %d.%d.%d",
            major, minor, update);
    printf("%s\n", release->version_string);
}

/**
 * \brief State machine task to retrieving the firmwares versions.
 */
LIBLOCAL void
tux_firmware_state_machine_call(void)
{
    if (!tux_usb_connected())
    {
        return;
    }

    switch (versioning_state) {
    case STDBY:
        break;
    case INIT:
        tux_firmware_init_descriptor();
        cpu_num = LOWEST_CPU_NUM;
        versioning_state = INFO_REQ;
        retries = TUX_FIRMWARE_RETRY_COUNT;
        break;
    case INFO_REQ:
        if (cpu_num > HIGHEST_CPU_NUM)
        {
            versioning_state = SPECIAL;
            break;
        }

        if (firmwares_desc[cpu_num].version_string[0] == '\0')
        {
            if (!retries)
            {
                cpu_num++;
                break;
            }

            if (!send_firmware_versionning_request(cpu_num))
            {
                retries--;
                break;
            }

        }
        retries = TUX_FIRMWARE_RETRY_COUNT;
        versioning_state = INFO_GET;
        break;
    case INFO_GET:
        if (!retries)
        {
            cpu_num++;
            retries = TUX_FIRMWARE_RETRY_COUNT;
            versioning_state = INFO_REQ;
            break;
        }

        if (firmwares_desc[cpu_num].version_string[0] != '\0')
        {
            cpu_num++;
            retries = TUX_FIRMWARE_RETRY_COUNT;
            versioning_state = INFO_REQ;
            break;
        }
        retries--;
        break;
    case SPECIAL:
        if ((firmwares_desc[TUXCORE_CPU_NUM].version_string[0] == '\0') ||
            (firmwares_desc[TUXAUDIO_CPU_NUM].version_string[0] == '\0') ||
            (firmwares_desc[TUXRF_CPU_NUM].version_string[0] == '\0') ||
            (firmwares_desc[FUXRF_CPU_NUM].version_string[0] == '\0'))
        {
            if (global_retries)
            {
                versioning_state = INIT;
                global_retries--;
            }
        }
        else
        {
            versioning_state = RELEASE;
        }
        break;
    case RELEASE:
        determine_release_package();
        versioning_state = FINALIZE;
        break;
    case FINALIZE:
        tux_sw_status_set_strvalue(SW_ID_TUXCORE_SYMBOLIC_VERSION,
            firmwares_desc[TUXCORE_CPU_NUM].version_string, true);
        tux_sw_status_set_strvalue(SW_ID_TUXAUDIO_SYMBOLIC_VERSION,
            firmwares_desc[TUXAUDIO_CPU_NUM].version_string, true);
        tux_sw_status_set_strvalue(SW_ID_FUXUSB_SYMBOLIC_VERSION,
            firmwares_desc[FUXUSB_CPU_NUM].version_string, true);
        tux_sw_status_set_strvalue(SW_ID_FUXRF_SYMBOLIC_VERSION,
            firmwares_desc[FUXRF_CPU_NUM].version_string, true);
        tux_sw_status_set_strvalue(SW_ID_TUXRF_SYMBOLIC_VERSION,
            firmwares_desc[TUXRF_CPU_NUM].version_string, true);
        tux_descriptor_update();
        /* Check version of tuxcore and tuxaudio */
        versioning_state = STDBY;
        break;
    default:
        break;
    }
}

/**
 * \brief Start the firmware versions retrieving.
 */
LIBLOCAL void
tux_firmware_get_descriptor(void)
{
    if (versioning_state == STDBY)
    {
        tux_firmware_init_descriptor();
        versioning_state = INIT;
    }
}
