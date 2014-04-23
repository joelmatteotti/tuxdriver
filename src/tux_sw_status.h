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

#ifndef _TUX_SW_STATUS_H_
#define _TUX_SW_STATUS_H_

#include <stdbool.h>

#include "tux_error.h"

#define STRING_VALUE_ON                     "ON"
#define STRING_VALUE_OFF                    "OFF"
#define STRING_VALUE_CHANGING               "CHANGING"
#define STRING_VALUE_FULL                   "FULL"
#define STRING_VALUE_HIGH                   "HIGH"
#define STRING_VALUE_LOW                    "LOW"
#define STRING_VALUE_EMPTY                  "EMPTY"
#define STRING_VALUE_OPEN                   "OPEN"
#define STRING_VALUE_CLOSE                  "CLOSE"
#define STRING_VALUE_NDEF                   "NDEF"
#define STRING_VALUE_NONE                   "NONE"
#define STRING_VALUE_LEFT                   "LEFT"
#define STRING_VALUE_RIGHT                  "RIGHT"
#define STRING_VALUE_DOWN                   "DOWN"
#define STRING_VALUE_UP                     "UP"
#define STRING_VALUE_UNPLUGGED              "UNPLUGGED"
#define STRING_VALUE_CHARGING               "CHARGING"
#define STRING_VALUE_PLUGGED_NO_POWER       "PLUGGED_NO_POWER"
#define STRING_VALUE_TRICKLE                "TRICKLE"
#define STRING_VALUE_INHIBITED              "INHIBITED"
#define STRING_VALUE_STOP                   "STOP"
#define STRING_VALUE_TRACK                  "TRACK"
#define STRING_VALUE_RELEASE                "RELEASE"
#define STRING_VALUE_NO_ERROR               "NO_ERROR"
#define STRING_VALUE_ERROR_RF_OFFLINE       "ERROR_RF_OFFLINE"
#define STRING_VALUE_ERROR_WAV_ERROR        "ERROR_WAV"
#define STRING_VALUE_ERROR_USB              "ERROR_USB"

typedef enum {
    SW_ID_FLIPPERS_POSITION = 0,
    SW_ID_FLIPPERS_REMAINING_MVM,
    SW_ID_SPINNING_DIRECTION,
    SW_ID_SPINNING_REMAINING_MVM,
    SW_ID_LEFT_WING_BUTTON,
    SW_ID_RIGHT_WING_BUTTON,
    SW_ID_HEAD_BUTTON,
    SW_ID_REMOTE_BUTTON,
    SW_ID_MOUTH_POSITION,
    SW_ID_MOUTH_REMAINING_MVM,
    SW_ID_EYES_POSITION,
    SW_ID_EYES_REMAINING_MVM,
    SW_ID_DESCRIPTOR_COMPLETE,
    SW_ID_RF_STATE,
    SW_ID_DONGLE_PLUG,
    SW_ID_CHARGER_STATE,
    SW_ID_BATTERY_LEVEL,
    SW_ID_BATTERY_STATE,
    SW_ID_LIGHT_LEVEL,
    SW_ID_LEFT_LED_STATE,
    SW_ID_RIGHT_LED_STATE,
    SW_ID_CONNECTION_QUALITY,
    SW_ID_AUDIO_FLASH_PLAY,
    SW_ID_AUDIO_GENERAL_PLAY,
    SW_ID_FLASH_PROG_CURR_TRACK,
    SW_ID_FLASH_PROG_LAST_TRACK_SIZE,
    SW_ID_TUXCORE_SYMBOLIC_VERSION,
    SW_ID_TUXAUDIO_SYMBOLIC_VERSION,
    SW_ID_FUXUSB_SYMBOLIC_VERSION,
    SW_ID_FUXRF_SYMBOLIC_VERSION,
    SW_ID_TUXRF_SYMBOLIC_VERSION,
    SW_ID_DRIVER_SYMBOLIC_VERSION,
    SW_ID_SOUND_REFLASH_BEGIN,
    SW_ID_SOUND_REFLASH_END,
    SW_ID_SOUND_REFLASH_CURRENT_TRACK,
    SW_ID_EYES_MOTOR_ON,
    SW_ID_MOUTH_MOTOR_ON,
    SW_ID_FLIPPERS_MOTOR_ON,
    SW_ID_SPIN_LEFT_MOTOR_ON,
    SW_ID_SPIN_RIGHT_MOTOR_ON,
    SW_ID_FLASH_SOUND_COUNT,
    SW_STATUS_NUMBER // SW_STATUS_NUMBER must be last and may not be removed !!
} SW_ID;


typedef enum {
    ID_FMT_BOOL = 0,
    ID_FMT_UINT8,
    ID_FMT_INT,
    ID_FMT_FLOAT,
    ID_FMT_STRING,
} ID_FMT;

#define STATUS_DOC_FILE_PATH                "./status_doc.txt"

typedef void(*event_callback_t)(char *event);

extern void tux_sw_status_init(void);
extern void tux_sw_status_set_intvalue(int id, int value, bool make_event);
extern void tux_sw_status_set_strvalue(int id, const char *value, bool make_event);
extern void tux_sw_status_set_floatvalue(int id, float value, bool make_event);
extern TuxDrvError tux_sw_status_name_from_id(int id, char* name);
extern TuxDrvError tux_sw_status_id_from_name(const char* name, int *id);
extern const char *tux_sw_status_value_fmt_from_id(int id);
extern TuxDrvError tux_sw_status_get_state_str(int id, char *state);
extern TuxDrvError tux_sw_status_get_value_str(int id, char *value);
extern void tux_sw_status_get_all_state_str(char *state);
extern void tux_sw_status_set_event_callback(event_callback_t funct);
extern void tux_sw_status_dump_status_doc(void);

#endif /* _TUX_SW_STATUS_H_ */
