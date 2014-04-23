/*
 * Tux Droid - High level status
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

/*
This file contains the eventing code for the tux
values stored are the last evented values (so not the current values)
there is no external function that allows retrieving that value so it
was decided not to store the current value as there was no need for it
*/

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef USE_MUTEX
#   include "threading_uniform.h"
#endif

#include "tux_firmware.h"
#include "tux_hw_status.h"
#include "tux_misc.h"
#include "tux_sw_status.h"
#include "version.h"

/*
   notes on the following struct:
   - strvalue is a pointer. it should only be used for static data
   - intvalue is used for bool, uint8, int8
   - event_threshold is the threshold value; if the threshold is exceeded
     the value is evented; for string values 0 means not evented,
					     1 means evented if string changes
*/
typedef struct
{
    int id;
    char *name;
    int value_fmt;
    union {
        int intvalue;
        float floatvalue;
        const char *strvalue;
    };
    int event_threshold;
    const char *value_doc;
    double lu_time;
} sw_status_t;

#ifdef USE_MUTEX
static mutex_t __status_mutex;
#endif
static event_callback_t event_funct;

#define INIT_FLOATID(id, value_fmt, name, value_doc, initval, threshold) \
    { id, name, value_fmt, {.floatvalue = initval}, threshold, value_doc, 0.0 },
#define INIT_INTID(id, value_fmt, name, value_doc, initval, threshold) \
    { id, name, value_fmt, {.intvalue = initval}, threshold, value_doc, 0.0 },
#define INIT_STRINGID(id, value_fmt, name, value_doc, initval, threshold) \
    { id, name, value_fmt, {.strvalue = initval}, threshold, value_doc, 0.0 },

sw_status_t sw_status_table[SW_STATUS_NUMBER] = {
    INIT_STRINGID(SW_ID_FLIPPERS_POSITION, ID_FMT_STRING,
        "flippers_position", "DOWN|UP", STRING_VALUE_DOWN, 1)

    INIT_INTID(SW_ID_FLIPPERS_REMAINING_MVM, ID_FMT_UINT8,
        "flippers_remaining_movements", "range[0..255]", 0, 1)

    INIT_STRINGID(SW_ID_SPINNING_DIRECTION, ID_FMT_STRING,
        "spinning_direction", "NONE|LEFT|RIGHT", STRING_VALUE_NONE, 1)

    INIT_INTID(SW_ID_SPINNING_REMAINING_MVM, ID_FMT_UINT8,
        "spinning_remaining_movements", "range[0..255]", 0, 1)

    INIT_INTID(SW_ID_LEFT_WING_BUTTON, ID_FMT_BOOL,
        "left_wing_button", "False|True", 0, 1)

    INIT_INTID(SW_ID_RIGHT_WING_BUTTON, ID_FMT_BOOL,
        "right_wing_button", "False|True", 0, 1)

    INIT_INTID(SW_ID_HEAD_BUTTON, ID_FMT_BOOL,
        "head_button", "False|True", 0, 1)

    INIT_STRINGID(SW_ID_REMOTE_BUTTON, ID_FMT_STRING,
        "remote_button", "K_<remote button>|RELEASE", STRING_VALUE_RELEASE, 1)

    INIT_STRINGID(SW_ID_MOUTH_POSITION, ID_FMT_STRING,
        "mouth_position", "OPEN|CLOSE|NDEF", STRING_VALUE_CLOSE, 1)

    INIT_INTID(SW_ID_MOUTH_REMAINING_MVM, ID_FMT_UINT8,
        "mouth_remaining_movements", "range[0..255]", 0, 1)

    INIT_STRINGID(SW_ID_EYES_POSITION, ID_FMT_STRING,
        "eyes_position", "OPEN|CLOSE|NDEF", STRING_VALUE_CLOSE, 1)

    INIT_INTID(SW_ID_EYES_REMAINING_MVM, ID_FMT_UINT8,
        "eyes_remaining_movements", "range[0..255]", 0, 1)

    INIT_INTID(SW_ID_DESCRIPTOR_COMPLETE, ID_FMT_BOOL,
        "descriptor_complete", "True", 0, 1)

    INIT_INTID(SW_ID_RF_STATE, ID_FMT_BOOL,
        "radio_state", "False|True", 0, 1)

    INIT_INTID(SW_ID_DONGLE_PLUG, ID_FMT_BOOL,
        "dongle_plug", "False|True", 0, 1)

    INIT_STRINGID(SW_ID_CHARGER_STATE, ID_FMT_STRING,
        "charger_state",
        "UNPLUGGED|CHARGING|PLUGGED_NO_POWER|TRICKLE|INHIBITED",
        STRING_VALUE_UNPLUGGED, 1)

    INIT_INTID(SW_ID_BATTERY_LEVEL, ID_FMT_INT,
        "battery_level", "range[4000..6500] (mV)", 0, 1)

    INIT_STRINGID(SW_ID_BATTERY_STATE, ID_FMT_STRING,
        "battery_state", "EMPTY|LOW|HIGH|FULL", STRING_VALUE_EMPTY, 1)

    INIT_INTID(SW_ID_LIGHT_LEVEL, ID_FMT_FLOAT,
        "light_level", "range[0.0..100.0]", 0.0, 1000)

    INIT_STRINGID(SW_ID_LEFT_LED_STATE, ID_FMT_STRING,
        "left_led_state", "ON|OFF|CHANGING", STRING_VALUE_OFF, 1)

    INIT_STRINGID(SW_ID_RIGHT_LED_STATE, ID_FMT_STRING,
        "right_led_state", "ON|OFF|CHANGING", STRING_VALUE_OFF, 1)

    INIT_INTID(SW_ID_CONNECTION_QUALITY, ID_FMT_INT,
        "connection_quality", "range[0..100]", 0, 1)

    INIT_STRINGID(SW_ID_AUDIO_FLASH_PLAY, ID_FMT_STRING,
        "audio_flash_play", "TRACK_<range[0..255]>|STOP", STRING_VALUE_STOP, 1)

    INIT_INTID(SW_ID_AUDIO_GENERAL_PLAY, ID_FMT_BOOL,
        "audio_general_play", "False|True", 0, 1)

    INIT_INTID(SW_ID_FLASH_PROG_CURR_TRACK, ID_FMT_UINT8,
        "flash_programming_current_track", "range[0..255]", 0, 1)

    INIT_INTID(SW_ID_FLASH_PROG_LAST_TRACK_SIZE, ID_FMT_INT,
        "flash_programming_last_track_size", "<track size>", 0, 1)

    INIT_STRINGID(SW_ID_TUXCORE_SYMBOLIC_VERSION, ID_FMT_STRING,
        "tuxcore_symbolic_version", "<string>",
        knowed_tuxcore_symbolic_version, 1)

    INIT_STRINGID(SW_ID_TUXAUDIO_SYMBOLIC_VERSION, ID_FMT_STRING,
        "tuxaudio_symbolic_version", "<string>",
        knowed_tuxaudio_symbolic_version, 1)

    INIT_STRINGID(SW_ID_FUXUSB_SYMBOLIC_VERSION, ID_FMT_STRING,
        "fuxusb_symbolic_version", "<string>",
        knowed_fuxusb_symbolic_version, 1)

    INIT_STRINGID(SW_ID_FUXRF_SYMBOLIC_VERSION, ID_FMT_STRING,
        "fuxrf_symbolic_version", "<string>",
        knowed_fuxrf_symbolic_version, 1)

    INIT_STRINGID(SW_ID_TUXRF_SYMBOLIC_VERSION, ID_FMT_STRING,
        "tuxrf_symbolic_version", "<string>",
        knowed_tuxrf_symbolic_version, 1)

    INIT_STRINGID(SW_ID_DRIVER_SYMBOLIC_VERSION, ID_FMT_STRING,
        "driver_symbolic_version", "<string>",
        "", 1)

    INIT_FLOATID(SW_ID_SOUND_REFLASH_BEGIN, ID_FMT_FLOAT,
        "sound_reflash_begin", "<seconds>", 0.0, 1000)

    INIT_STRINGID(SW_ID_SOUND_REFLASH_END, ID_FMT_STRING,
        "sound_reflash_end", "NO_ERROR|ERROR_RF_OFFLINE|ERROR_WAV|ERROR_USB",
        STRING_VALUE_NO_ERROR, 1)

    INIT_INTID(SW_ID_SOUND_REFLASH_CURRENT_TRACK, ID_FMT_UINT8,
        "sound_reflash_current_track", "range[0..255]", 0, 1)

    INIT_INTID(SW_ID_EYES_MOTOR_ON, ID_FMT_BOOL,
        "eyes_motor_on", "False|True", 0, 1)

    INIT_INTID(SW_ID_MOUTH_MOTOR_ON, ID_FMT_BOOL,
        "mouth_motor_on", "False|True", 0, 1)

    INIT_INTID(SW_ID_FLIPPERS_MOTOR_ON, ID_FMT_BOOL,
        "flippers_motor_on", "False|True", 0, 1)

    INIT_INTID(SW_ID_SPIN_LEFT_MOTOR_ON, ID_FMT_BOOL,
        "spin_left_motor_on", "False|True", 0, 1)

    INIT_INTID(SW_ID_SPIN_RIGHT_MOTOR_ON, ID_FMT_BOOL,
        "spin_right_motor_on", "False|True", 0, 1)

    INIT_INTID(SW_ID_FLASH_SOUND_COUNT, ID_FMT_UINT8,
        "sound_flash_count", "range[0..255]", 0, 1)
};

/**
 *
 */
LIBLOCAL void
tux_sw_status_init(void)
{
    static char driver_symbolic_version[128] = "";
    int i;

#ifdef USE_MUTEX
    mutex_init(__status_mutex);
#endif

    sprintf(driver_symbolic_version, "libtuxdriver_%d.%d.%d-r%d",
        VER_MAJOR,
        VER_MINOR,
        VER_UPDATE,
        VER_REVISION);

    tux_sw_status_set_strvalue(SW_ID_DRIVER_SYMBOLIC_VERSION,
        driver_symbolic_version, false);

    /* Initialize the "last updated time" value of the statuses */
    for (i = 0; i < SW_STATUS_NUMBER; i++)
    {
        sw_status_table[i].lu_time = get_time();
    }


#ifdef GENERATE_DOC
    tux_sw_status_dump_status_doc();
#endif
}

/**
 *
 */
LIBLOCAL TuxDrvError
tux_sw_status_name_from_id(int id, char *name)
{
    if ((id < 0) || (id >= SW_STATUS_NUMBER))
    {
        strcpy(name, "UNKNOW");
        return E_TUXDRV_INVALIDIDENTIFIER;
    }

    strcpy(name, sw_status_table[id].name);
    return E_TUXDRV_NOERROR;
}

/**
 *
 */
LIBLOCAL TuxDrvError
tux_sw_status_id_from_name(const char *name, int *id)
{
    int i = -1;

#ifdef USE_MUTEX
    mutex_lock(__status_mutex);
#endif
    for (i = 0; i < SW_STATUS_NUMBER; i++)
    {
        if (!(strcmp(name, sw_status_table[i].name)))
        {
            *id = i;
#ifdef USE_MUTEX
            mutex_unlock(__status_mutex);
#endif
            return E_TUXDRV_NOERROR;
        }
    }
#ifdef USE_MUTEX
    mutex_unlock(__status_mutex);
#endif

    return E_TUXDRV_INVALIDNAME;
}

/**
 *
 */
LIBLOCAL const char *
tux_sw_status_value_fmt_from_id(int id)
{
    switch (id) {
    case ID_FMT_BOOL:
        return "bool";
    case ID_FMT_UINT8:
        return "uint8";
    case ID_FMT_INT:
        return "int";
    case ID_FMT_FLOAT:
        return "float";
    case ID_FMT_STRING:
        return "string";
    default:
        return "UNKNOW";
    }
}

#include "log.h"
/**
 *
 */
static void
get_status_value_str(int id, char *str)
{
#ifdef USE_MUTEX
    mutex_lock(__status_mutex);
#endif
    switch (sw_status_table[id].value_fmt) {
    case ID_FMT_BOOL:
        if (sw_status_table[id].intvalue)
        {
            strcpy(str, "True");
        }
        else
        {
            strcpy(str, "False");
        }
        break;
    case ID_FMT_UINT8:
        sprintf(str, "%d", sw_status_table[id].intvalue);
        break;
    case ID_FMT_INT:
        sprintf(str, "%d", sw_status_table[id].intvalue);
        break;
    case ID_FMT_FLOAT:
        sprintf(str, "%f", sw_status_table[id].floatvalue);
        break;
    case ID_FMT_STRING:
        strcpy(str, sw_status_table[id].strvalue);
        break;
    default:
        break;
    }
#ifdef USE_MUTEX
    mutex_unlock(__status_mutex);
#endif

    return;
}

/**
 *
 */
LIBLOCAL TuxDrvError
tux_sw_status_get_state_str(int id, char *state)
{
    const char *fmt_str;
    char name_str[128] = "";
    char value_str[128] = "";
    TuxDrvError err;

#ifdef USE_MUTEX
    mutex_lock(__status_mutex);
#endif
    fmt_str = tux_sw_status_value_fmt_from_id(sw_status_table[id].value_fmt);
#ifdef USE_MUTEX
    mutex_unlock(__status_mutex);
#endif
    err = tux_sw_status_name_from_id(id, name_str);
    if (err != E_TUXDRV_NOERROR)
    {
        return err;
    }

    get_status_value_str(id, value_str);
    sprintf(state, "%s:%s:%s:%.3f",
                sw_status_table[id].name,
                fmt_str,
                value_str,
                (get_time() - sw_status_table[id].lu_time));

    return E_TUXDRV_NOERROR;
}

/**
 *
 */
LIBLOCAL TuxDrvError
tux_sw_status_get_value_str(int id, char *value)
{
    if ((id < 0) || (id >= SW_STATUS_NUMBER))
    {
        strcpy(value, "NULL");
        return E_TUXDRV_INVALIDIDENTIFIER;
    }

    get_status_value_str(id, value);

    return E_TUXDRV_NOERROR;
}

/**
 *
 */
LIBLOCAL void
tux_sw_status_get_all_state_str(char *state)
{
    char *p;

    p = state; /* p always points to the end of the string */
    char tmp_state[256] = "";
    int i;

    for (i = 0; i < SW_STATUS_NUMBER; i++)
    {
        tux_sw_status_get_state_str(i, tmp_state);
        p = p + sprintf(p, "%s\n", tmp_state);
    }
}

/**
 *
 */
LIBLOCAL void
tux_sw_status_set_intvalue(int id, int value, bool make_event)
{
    int delta;
    char state_str[1024];

    if (make_event)
    {
#ifdef USE_MUTEX
        mutex_lock(__status_mutex);
#endif

        delta = sw_status_table[id].intvalue - value;
        if (delta < 0)
        {
            delta = -delta;
        }

#ifdef USE_MUTEX
        mutex_unlock(__status_mutex);
#endif

        if (event_funct)
        {
            if (delta >= sw_status_table[id].event_threshold)
            {
#ifdef USE_MUTEX
                mutex_lock(__status_mutex);
#endif
                sw_status_table[id].intvalue = value;
#ifdef USE_MUTEX
                mutex_unlock(__status_mutex);
#endif
                tux_sw_status_get_state_str(id, state_str);
                event_funct(state_str);
            }
        }
    }
    else
    {
#ifdef USE_MUTEX
        mutex_lock(__status_mutex);
#endif
        sw_status_table[id].intvalue = value;
#ifdef USE_MUTEX
        mutex_unlock(__status_mutex);
#endif
    }

#ifdef USE_MUTEX
    mutex_lock(__status_mutex);
#endif

    sw_status_table[id].lu_time = get_time();

#ifdef USE_MUTEX
    mutex_unlock(__status_mutex);
#endif
}

/**
 *
 */
LIBLOCAL void
tux_sw_status_set_floatvalue(int id, float value, bool make_event)
{
    float delta;
    char state_str[1024];

    if (make_event)
    {
#ifdef USE_MUTEX
        mutex_lock(__status_mutex);
#endif

        delta = sw_status_table[id].floatvalue - value;
        if (delta < 0)
        {
            delta = -delta;
        }

#ifdef USE_MUTEX
        mutex_unlock(__status_mutex);
#endif

        if (event_funct)
        {
            if ((1000*delta) >= sw_status_table[id].event_threshold)
            {
#ifdef USE_MUTEX
                mutex_lock(__status_mutex);
#endif
                sw_status_table[id].floatvalue = value;
#ifdef USE_MUTEX
                mutex_unlock(__status_mutex);
#endif
                tux_sw_status_get_state_str(id, state_str);
                event_funct(state_str);
            }
        }
    }
    else
    {
#ifdef USE_MUTEX
        mutex_lock(__status_mutex);
#endif
        sw_status_table[id].floatvalue = value;
#ifdef USE_MUTEX
        mutex_unlock(__status_mutex);
#endif
    }

#ifdef USE_MUTEX
    mutex_lock(__status_mutex);
#endif

    sw_status_table[id].lu_time = get_time();

#ifdef USE_MUTEX
    mutex_unlock(__status_mutex);
#endif
}

/**
 *
 */
LIBLOCAL void
tux_sw_status_set_strvalue(int id, const char *value, bool make_event)
{
    char state_str[1024];

    if (make_event)
    {
        if (event_funct)
        {
            /*
               the next if statement uses pointer comparison
               this works like a charm under the following two conditions
               - value points to a string constants (and not variables)
               - no duplicate string constants (althought the compiler might
                 pick up this one anyway).
               If the first condition is not met, we need to copy the string
               instead of the pointer (and allocate space for it),
               (or resolve it in the caller, not really a nice solution)
               If the second condtion is not met something like:
			      ((sw_status_table[id].strvalue == NULL) ||
			      strcmp(sw_status_table[id].strvalue,value)))
               could be done instead of the value != line)
            */
            if (sw_status_table[id].event_threshold &&
                 (value != sw_status_table[id].strvalue))
            {
#ifdef USE_MUTEX
                mutex_lock(__status_mutex);
#endif
		        sw_status_table[id].strvalue = value;
#ifdef USE_MUTEX
                mutex_unlock(__status_mutex);
#endif
                tux_sw_status_get_state_str(id, state_str);
                event_funct(state_str);
            }
        }
    }
    else
    {
#ifdef USE_MUTEX
        mutex_lock(__status_mutex);
#endif
		sw_status_table[id].strvalue = value;
#ifdef USE_MUTEX
        mutex_unlock(__status_mutex);
#endif
    }

#ifdef USE_MUTEX
    mutex_lock(__status_mutex);
#endif

    sw_status_table[id].lu_time = get_time();

#ifdef USE_MUTEX
    mutex_unlock(__status_mutex);
#endif
}

/**
 *
 */
LIBLOCAL void
tux_sw_status_set_event_callback(event_callback_t funct)
{
    event_funct = funct;
}

/**
 *
 */
LIBLOCAL void
tux_sw_status_dump_status_doc(void)
{
    int i;
    char status_doc[8192] = "";
    char value_str[256] = "";
    FILE *doc_file;
    char *p;

    p = status_doc; /* point to the end of the string */
    strcpy(p, "Tux status documentation :\n"
              "--------------------------\n\n");

    p = p + strlen(p);

    for (i = 0; i < SW_STATUS_NUMBER; i++)
    {
        tux_sw_status_get_state_str(i, value_str);
        p = p + sprintf(p,
            "Status %.2d:\n"
            "    ID : %d\n"
            "    Name : %s\n"
            "    Value type : %s\n"
            "    Possible values : %s\n"
            "    Default state : [%s]\n\n",
            i,
            i,
            sw_status_table[i].name,
            tux_sw_status_value_fmt_from_id(sw_status_table[i].value_fmt),
            sw_status_table[i].value_doc,
            value_str
        );
    }

    doc_file = fopen(STATUS_DOC_FILE_PATH, "w");
    if (doc_file)
    {
        fprintf(doc_file, "%s\n", status_doc);
        fclose(doc_file);
    }
    else
    {
        printf("file error %s\n", strerror(errno));
    }
}

