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

#include <stdio.h>
#include <string.h>

#include "log.h"
#include "tux_battery.h"
#include "tux_cmd_parser.h"
#include "tux_descriptor.h"
#include "tux_error.h"
#include "tux_eyes.h"
#include "tux_firmware.h"
#include "tux_hw_status.h"
#include "tux_id.h"
#include "tux_leds.h"
#include "tux_light.h"
#include "tux_mouth.h"
#include "tux_pong.h"
#include "tux_sound_flash.h"
#include "tux_sw_status.h"
#include "tux_user_inputs.h"
#include "tux_spinning.h"
#include "tux_usb.h"
#include "tux_flippers.h"
#include "tux_types.h"
#include "version.h"

static bool driver_started = false;
static simple_callback_t end_cycle_funct;
static simple_callback_t dongle_connected_funct;
static simple_callback_t dongle_disconnected_funct;

static void on_frame(const unsigned char *data);
static void on_rf_state(unsigned char state);
static void on_usb_connect(void);
static void on_usb_disconnect(void);
static void on_read_loop_cycle_complete(void);

void TuxDrv_ResetPositions(void);

/**
 * 31/08/2012 - Joël Maatteotti <sfuser: joelmatteitti>
 */
LIBEXPORT bool TuxDrv_Eyes_cmd_off()
{
	return tux_eyes_cmd_off();
}

/**
 * 31/08/2012 - Joël Maatteotti <sfuser: joelmatteitti>
 */
LIBEXPORT bool TuxDrv_Mouth_cmd_off()
{
	return tux_mouth_cmd_off();
}

/**
 * 31/08/2012 - Joël Maatteotti <sfuser: joelmatteitti>
 */
LIBEXPORT bool TuxDrv_Spinning_cmd_off()
{
	return tux_spinning_cmd_off();
}

/**
 * 31/08/2012 - Joël Maatteotti <sfuser: joelmatteitti>
 */
LIBEXPORT bool TuxDrv_Flippers_cmd_off()
{
	return tux_flippers_cmd_off();
}

/**
 * 31/08/2012 - Joël Maatteotti <sfuser: joelmatteitti>
 */
LIBEXPORT char *TuxDrv_SoundFlash_dump_descriptor(char *p)
{
	return tux_sound_flash_dump_descriptor(p);
}

/**
 * 31/08/2012 - Joël Maatteotti <sfuser: joelmatteitti>
 */
LIBEXPORT void TuxDrv_LightLevel_update()
{
	tux_light_update_level();
}


/**
 *
 */
LIBEXPORT void
TuxDrv_SetStatusCallback(event_callback_t funct)
{
    tux_sw_status_set_event_callback(funct);
}

/**
 *
 */
LIBEXPORT void
TuxDrv_SetEndCycleCallback(simple_callback_t funct)
{
    end_cycle_funct = funct;
}

/**
 *
 */
LIBEXPORT void
TuxDrv_SetDongleConnectedCallback(simple_callback_t funct)
{
    dongle_connected_funct = funct;
}

/**
 *
 */
LIBEXPORT void
TuxDrv_SetDongleDisconnectedCallback(simple_callback_t funct)
{
    dongle_disconnected_funct = funct;
}

/**
 *  Callback function on frame receiving.
 *  @param data 4 bytes array of status.
 */
static void
on_frame(const unsigned char *data)
{
    unsigned char header;
    int ret;

    header = data[0];
    ret = tux_hw_status_parse_frame(data);

    if (ret == -1)
    {
        log_warning("STATUS FRAME : %.2x %.2x %.2x %.2x",
            data[0],
            data[1],
            data[2],
            data[3]);
    }

    switch (header)
    {
    /* Descriptor frames */
    case FRAME_HEADER_VERSION:
        tux_firmware_update_version();
        break;
    case FRAME_HEADER_REVISION:
        tux_firmware_update_revision();
        break;
    case FRAME_HEADER_AUTHOR:
        tux_firmware_update_author();
        break;
    case FRAME_HEADER_SOUND_VAR:
        tux_sound_flash_update();
        break;
    /* Status frames */
    case FRAME_HEADER_PORTS:
        if (header == ret)
        {
            tux_spinning_update_direction();
            tux_mouth_update_position();
            tux_eyes_update_position();
        }
        break;
    case FRAME_HEADER_POSITION1:
        if (header == ret)
        {
            tux_flippers_update_movements_remaining();
            tux_mouth_update_movements_remaining();
            tux_eyes_update_movements_remaining();
        }
        break;
    case FRAME_HEADER_POSITION2:
        if (header == ret)
        {
            tux_flippers_update_position();
            tux_spinning_update_movements_remaining();
            tux_eyes_update_motor();
            tux_mouth_update_motor();
            tux_flippers_update_motor();
            tux_spinning_update_left_motor();
            tux_spinning_update_right_motor();
        }
        break;
    case FRAME_HEADER_SENSORS1:
        if (header == ret)
        {
            tux_sound_flash_update_general_play();
            tux_user_inputs_update_head_button();
            tux_user_inputs_update_left_wing_button();
            tux_user_inputs_update_right_wing_button();
        }
        break;
    case FRAME_HEADER_ID:
        tux_id_update_number();
        break;
    case FRAME_HEADER_IR:
        tux_user_inputs_init_time_RC5();
        break;
    case FRAME_HEADER_BATTERY:
        tux_user_inputs_update_charger_state();
        if (header == ret)
        {
            tux_battery_update_level();
        }
        break;
    case FRAME_HEADER_LIGHT:
        if (header == ret)
        {
            tux_light_update_level();
        }
        break;
    case FRAME_HEADER_LED:
        if (header == ret)
        {
            tux_leds_update_left();
            tux_leds_update_right();
        }
        break;
    /* Don't work ...
    case FRAME_HEADER_FLASH_PROG:
        if (header == ret)
        {
            tux_sound_flash_update_prog_current_track();
            tux_sound_flash_update_prog_last_track_size();
        }
        break;
    */
    case FRAME_HEADER_AUDIO:
        tux_sound_flash_update_flash_play();
        break;
    /*case FRAME_HEADER_PONG:
        tux_pong_update();
        break;*/
    }
}

/**
 *  Callback function on radio state changed.
 *  @param state state of the radio connection.
 */
static void
on_rf_state(unsigned char state)
{
    tux_sw_status_set_intvalue(SW_ID_RF_STATE, state, true);
    if (state)
    {
        tux_descriptor_get();
        /* For reinitilizing the remote control task */
        tux_user_inputs_init();
        /* Reset the body state */
        TuxDrv_ResetPositions();
    }
}

/**
 *  Callback function on fux dongle plugging.
 */
static void
on_usb_connect(void)
{
    data_frame wakeup_frame = {0xB6, 0xFF, 0x01, 0x00};

    tux_descriptor_init();
    tux_hw_status_init();
    tux_sw_status_init();
    tux_user_inputs_init();
    tux_cmd_parser_init();
    tux_sw_status_set_intvalue(SW_ID_DONGLE_PLUG, true, true);
    /* Waking up Tux Droid */
    tux_usb_send_to_tux(wakeup_frame);
    if (dongle_connected_funct)
    {
        dongle_connected_funct();
    }
    /* If default windows sound card is Tuxdroid-TTS then set
     * the default card to TuxDroid-Audio */
#ifdef WIN32
    tux_sound_flash_avoid_tts_default_sound_card();
#endif
}

/**
 *  Callback function on fux dongle unplugging.
 */
static void
on_usb_disconnect(void)
{
    tux_sw_status_set_intvalue(SW_ID_RF_STATE, false, true);
    tux_sw_status_set_intvalue(SW_ID_DONGLE_PLUG, false, true);
    if (dongle_disconnected_funct)
    {
        dongle_disconnected_funct();
    }
}

/**
 *  Callback function on end of a cycle of usb read.
 */
static void
on_read_loop_cycle_complete(void)
{
    tux_user_inputs_update_RC5();
    /* tux_pong_get(); */
    /* tux_firmware_state_machine_call(); */
    tux_sound_flash_state_machine_call();
    tux_hw_status_header_counter_check();

    if (end_cycle_funct)
    {
        end_cycle_funct();
    }

    tux_cmd_parser_delay_stack_perform();
}

/**
 *
 */
LIBEXPORT TuxDrvError
TuxDrv_PerformCommand(double delay, const char *cmd_str)
{
    if (delay == 0.0)
    {
        log_debug("Perform an instant command : [%s]", cmd_str);
        return tux_cmd_parser_parse_command(cmd_str);
    }
    else
    {
        log_debug("Perform a delayed command : [%s], %f", cmd_str, delay);
        return tux_cmd_parser_insert_user_command(delay, cmd_str);
    }
}

/**
 *
 */
LIBEXPORT void
TuxDrv_ClearCommandStack(void)
{
    tux_cmd_parser_clear_delay_commands();
}

/**
 *
 */
LIBEXPORT TuxDrvError
TuxDrv_PerformMacroFile(const char *file_path)
{
    return tux_cmd_parser_parse_file(file_path);
}

/**
 *
 */
LIBEXPORT TuxDrvError
TuxDrv_PerformMacroText(const char *macro)
{
    return tux_cmd_parser_parse_macro(macro);
}

/**
 *
 */
LIBEXPORT TuxDrvError
TuxDrv_SoundReflash(const char *tracks)
{
    return tux_sound_flash_cmd_reflash(tracks);
}

/**
 *
 */
LIBEXPORT void
TuxDrv_SetLogLevel(log_level_t level)
{
    log_set_level(level);
}

/**
 *
 */
LIBEXPORT void
TuxDrv_SetLogTarget(log_target_t target)
{
    log_open(target);
}

/**
 *
 */
LIBEXPORT int
TuxDrv_TokenizeStatus(char *status, tokens_t *tokens)
{
    return tux_cmd_parser_get_tokens(status, tokens, 32, ":,");
}

/**
 *
 */
LIBEXPORT TuxDrvError
TuxDrv_GetStatusName(int id, char *name)
{
    return tux_sw_status_name_from_id(id, name);
}

/**
 *
 */
LIBEXPORT TuxDrvError
TuxDrv_GetStatusId(const char *name, int *id)
{
    return tux_sw_status_id_from_name(name, id);
}

/**
 *
 */
LIBEXPORT TuxDrvError
TuxDrv_GetStatusState(int id, char *state)
{
    return tux_sw_status_get_state_str(id, state);
}

/**
 *
 */
LIBEXPORT void
TuxDrv_GetAllStatusState(char *state)
{
    tux_sw_status_get_all_state_str(state);
}

/**
 *
 */
LIBEXPORT TuxDrvError
TuxDrv_GetStatusValue(int id, char *value)
{
    return tux_sw_status_get_value_str(id, value);
}

/**
 *
 */
LIBEXPORT void
TuxDrv_ResetDongle(void)
{
    tux_usb_reset();
}

/**
 *
 */
LIBEXPORT void
TuxDrv_ResetPositions(void)
{
    TuxDrv_PerformCommand(0.0, "TUX_CMD:EYES:OPEN");
    TuxDrv_PerformCommand(0.3, "TUX_CMD:MOUTH:CLOSE");
    TuxDrv_PerformCommand(0.0, "TUX_CMD:FLIPPERS:DOWN");
    TuxDrv_PerformCommand(0.0, "TUX_CMD:SPINNING:OFF");
    TuxDrv_PerformCommand(0.0, "TUX_CMD:LED:ON:LED_BOTH,1.0");
}

/**
 *
 */
LIBEXPORT void
TuxDrv_GetDescriptor(tux_descriptor_t *tux_desc)
{
    tux_desc->firmwares.package = &firmware_release_desc;
    tux_desc->firmwares.tuxcore = &firmwares_desc[TUXCORE_CPU_NUM];
    tux_desc->firmwares.tuxaudio = &firmwares_desc[TUXAUDIO_CPU_NUM];
    tux_desc->firmwares.tuxrf = &firmwares_desc[TUXRF_CPU_NUM];
    tux_desc->firmwares.fuxrf = &firmwares_desc[FUXRF_CPU_NUM];
    tux_desc->firmwares.fuxusb = &firmwares_desc[FUXUSB_CPU_NUM];
    tux_desc->sound_flash = &sound_flash_desc;
    tux_desc->id = &id_desc;
    tux_desc->driver.version_major = VER_MAJOR;
    tux_desc->driver.version_minor = VER_MINOR;
    tux_desc->driver.version_update = VER_UPDATE;
    tux_desc->driver.version_build = VER_REVISION;
    strcpy(tux_desc->driver.version_state, VER_STATE);
    sprintf(tux_desc->driver.version_string,
        "libtuxdriver_%d.%d.%d-r%d",
        VER_MAJOR,
        VER_MINOR,
        VER_UPDATE,
        VER_REVISION);
}

/**
 * Get the description of an error code.
 */
LIBEXPORT const char *
TuxDrv_StrError(TuxDrvError error_code)
{
    return tux_error_strerror(error_code);
}

/**
 *  Start tux driver.
 */
LIBEXPORT void
TuxDrv_Start(void)
{
    printf("libtuxdriver_%d.%d.%d-r%d\n\n",
        VER_MAJOR,
        VER_MINOR,
        VER_UPDATE,
        VER_REVISION);

    tux_usb_init_module();
    tux_usb_set_frame_callback(on_frame);
    tux_usb_set_rf_state_callback(on_rf_state);
    tux_usb_set_connect_dongle_callback(on_usb_connect);
    tux_usb_set_disconnect_dongle_callback(on_usb_disconnect);
    tux_usb_set_loop_cycle_complete_callback(on_read_loop_cycle_complete);
    tux_descriptor_init();
    tux_hw_status_init();
    tux_sw_status_init();
    tux_user_inputs_init();
    tux_cmd_parser_init();
    driver_started = true;

    while (driver_started)
    {
        tux_usb_start();
        sleep(1);
    }

    tux_usb_exit_module();
}

/**
 * Stop tux driver.
 */
LIBEXPORT void
TuxDrv_Stop(void)
{
    driver_started = false;
    tux_usb_stop();
}
