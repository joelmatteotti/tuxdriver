/*
 * Tux Droid - Driver
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

#ifndef _TUX_DRIVER_H_
#define _TUX_DRIVER_H_

#include <stdbool.h>

/**
 * Id enumeration of high level status.
 */
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
} SW_ID_DRIVER;

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Error codes
 */
#define TUX_ERROR_BEGIN         256
typedef int TuxDrvError;
typedef enum
{
    E_TUXDRV_NOERROR                = 0,
    E_TUXDRV_PARSERISDISABLED       = TUX_ERROR_BEGIN,
    E_TUXDRV_INVALIDCOMMAND,
    E_TUXDRV_STACKOVERFLOW,
    E_TUXDRV_FILEERROR,
    E_TUXDRV_BADWAVFILE,
    E_TUXDRV_INVALIDIDENTIFIER,
    E_TUXDRV_INVALIDNAME,
    E_TUXDRV_INVALIDPARAMETER,
    E_TUXDRV_BUSY,
    E_TUXDRV_WAVSIZEEXCEDED,
} tux_drv_error_t;

/**
 * CPU number enumeration.
 */
typedef enum {
    TUXCORE_CPU_NUM     = 0,
    TUXAUDIO_CPU_NUM    = 1,
    TUXRF_CPU_NUM       = 2,
    FUXRF_CPU_NUM       = 3,
    FUXUSB_CPU_NUM      = 4,
    INVALID_CPU_NUM     = -1,
} CPU_IDENTIFIERS;

/**
 * Descriptor structure of a firmaware.
 */
typedef struct {
    CPU_IDENTIFIERS cpu_id;
    unsigned int    version_major;
    unsigned int    version_minor;
    unsigned int    version_update;
    unsigned int    revision;
    bool            release;
    bool            local_modification;
    bool            mixed_revisions;
    unsigned int    author;
    unsigned int    variation;
    char            version_string[256];
} firmware_descriptor_t;

/**
 * Descriptor structure of sound flash.
 */
typedef struct {
    unsigned int number_of_sounds;
    unsigned int flash_usage;
    unsigned int available_record_time;
} sound_flash_descriptor_t;

/**
 * Descriptor structure of ID connection.
 */
typedef struct {
    unsigned int number;
} id_descriptor_t;

/**
 * Global descriptor structure of tuxdroid.
 */
typedef struct {
    struct firmwares_t {
        firmware_descriptor_t *package;
        firmware_descriptor_t *tuxcore;
        firmware_descriptor_t *tuxaudio;
        firmware_descriptor_t *tuxrf;
        firmware_descriptor_t *fuxrf;
        firmware_descriptor_t *fuxusb;
    } firmwares;
    struct driver_version_t {
        unsigned int version_major;
        unsigned int version_minor;
        unsigned int version_update;
        unsigned int version_build;
        char version_state[100];
        char version_string[100];
    } driver;
    sound_flash_descriptor_t *sound_flash;
    id_descriptor_t *id;
} tux_descriptor_t;

/**
 * Simple callback definition.
 */
typedef void(*drv_simple_callback_t)(void);

/**
 * Status callback definition.
 */
typedef void(*drv_status_callback_t)(char *status);

/**
 * Tokens structure
 */
typedef char drv_token_str_t[1024];
typedef drv_token_str_t drv_tokens_t[256];

/**
 * Logging target
 */
typedef enum log_target
{
    LOG_TARGET_TUX,
    LOG_TARGET_SHELL
} log_target_t;

/**
 * Logging levels, in increasing priorities
 */
typedef enum log_level
{
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_NONE
} log_level_t;

extern void TuxDrv_Start(void);
extern void TuxDrv_Stop(void);

/**
 * 31/08/2012 - Joël Maatteotti <sfuser: joelmatteitti>
 */
extern bool TuxDrv_Eyes_cmd_off(void);
extern bool TuxDrv_Mouth_cmd_off(void);
extern bool TuxDrv_Spinning_cmd_off(void);
extern bool TuxDrv_Flippers_cmd_off(void);
extern char *TuxDrv_SoundFlash_dump_descriptor(char *p);
extern void TuxDrv_LightLevel_update(void);
/** ------------------------- */


extern const char *TuxDrv_StrError(TuxDrvError error_code);
extern void TuxDrv_GetDescriptor(tux_descriptor_t *tux_desc);
extern void TuxDrv_SetStatusCallback(drv_status_callback_t funct);
extern void TuxDrv_SetEndCycleCallback(drv_simple_callback_t funct);
extern void TuxDrv_SetDongleConnectedCallback(drv_simple_callback_t funct);
extern void TuxDrv_SetDongleDisconnectedCallback(drv_simple_callback_t funct);
extern TuxDrvError TuxDrv_PerformCommand(double delay, char *cmd_str);
extern void TuxDrv_ClearCommandStack(void);
extern TuxDrvError TuxDrv_PerformMacroFile(char *file_path);
extern TuxDrvError TuxDrv_PerformMacroText(char *macro);
extern TuxDrvError TuxDrv_SoundReflash(char *tracks);
extern void TuxDrv_SetLogLevel(log_level_t level);
extern void TuxDrv_SetLogTarget(log_target_t target);
extern TuxDrvError TuxDrv_GetStatusName(int id, char* name);
extern TuxDrvError TuxDrv_GetStatusId(char* name, int *id);
extern TuxDrvError TuxDrv_GetStatusState(int id, char *state);
extern TuxDrvError TuxDrv_GetStatusValue(int id, char *value);
extern void TuxDrv_GetAllStatusState(char *state);
extern int TuxDrv_TokenizeStatus(char *status, drv_tokens_t *tokens);
extern void TuxDrv_ResetPositions(void);
extern void TuxDrv_ResetDongle(void);
extern double get_time(void);

#if defined(__cplusplus)
}
#endif

#endif /* _TUX_DRIVER_H_ */
