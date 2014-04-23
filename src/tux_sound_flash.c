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

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "tux_cmd_parser.h"
#include "tux_hw_status.h"
#include "tux_hw_cmd.h"
#include "tux_leds.h"
#include "tux_sw_status.h"
#include "tux_sound_flash.h"
#include "tux_types.h"
#include "tux_usb.h"

LIBLOCAL sound_flash_descriptor_t sound_flash_desc;

LIBLOCAL char knowed_track_num[128] = "0";

#ifdef unix

       /* Patch pour les processeurs 64 bits <sfuser: joelmatteotti >*/
       #if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
       LIBLOCAL char hw_audio_name[12] = "";
       #else
       LIBLOCAL char hw_audio_name[7] = "";
       #endif

static void get_hw_audio_device_name(void);
#endif

typedef enum {
    SRS_STDBY,
    SRS_INIT,
    SRS_ERASE,
    SRS_WRITE,
    SRS_FINISH,
} sound_reflash_state_t;

typedef enum {
    SRE_NO_ERROR,
    SRE_RF_OFFLINE,
    SRE_WAV_ERROR,
    SRE_USB_ERROR,
} sound_reflash_errors_t;

typedef struct {
    tokens_t wav_path;
    unsigned char wav_count;
    unsigned char current_wav;
    unsigned long full_size;
    unsigned long wav_size[256];
    sound_reflash_errors_t error;
    sound_reflash_state_t current_state;
} sound_reflash_info_t;

static sound_reflash_info_t reflash_info;

static void load_knowed_track_num(void);
static void init_reflash_info(void);
static bool play_wav(const char *wav_path);

/**
 * Init the sound flash descriptor part.
 */
LIBLOCAL void
tux_sound_flash_init_descriptor(void)
{
    memset(&sound_flash_desc, 0, sizeof(sound_flash_descriptor_t));
    load_knowed_track_num();
#ifdef unix
    hw_audio_name[0] = '\0';
    get_hw_audio_device_name();
#endif
}

/**
 * Dump the sound flash descriptor part.
 */
LIBLOCAL char *
tux_sound_flash_dump_descriptor(char *p)
{
    p = p + sprintf(p,
        "- Sound flash\n"
        "    Number of sounds : %d\n"
        "    Last used block : %d\n"
        "    Available record time (sec) : %d\n",
        sound_flash_desc.number_of_sounds,
        sound_flash_desc.flash_usage,
        sound_flash_desc.available_record_time
    );

    return p;
}

/**
 * Update the sound flash descriptor part with the low level status.
 */
LIBLOCAL void
tux_sound_flash_update(void)
{
    sound_flash_descriptor_t *sf_desc;

    sf_desc = &sound_flash_desc;
    sf_desc->number_of_sounds = hw_status_table.sound_var.number_of_sounds;
    sf_desc->flash_usage = hw_status_table.sound_var.flash_usage;
    sf_desc->available_record_time = (int)((128 - sf_desc->flash_usage) * 0.5);

    tux_sw_status_set_intvalue(SW_ID_FLASH_SOUND_COUNT, sf_desc->number_of_sounds, true);
}

/**
 * Retry the sound flash infos from the dongle.
 */
LIBLOCAL void
tux_sound_flash_get_descripor(void)
{
    /*  Currently, the status are sent by tuxaudio when then
     *  the versionning is requested */
}

/**
 * Update the flash play state with the low level status.
 */
LIBLOCAL void
tux_sound_flash_update_flash_play(void)
{
    char *new_track = "";
    static char track[2][10];
    static int tracktoggle = 0;

    /*
       the thing with track is a little bit wicked,
       set_strvalue will compare on pointer so the array must be static
       also we need two arrays and toggle between them otherwise we
       will not get the event.
    */
    tracktoggle = !tracktoggle;
    if (!hw_status_table.audio.sound_track_played)
    {
        new_track = STRING_VALUE_STOP;
    }
    else
    {
        sprintf(track[tracktoggle], "TRACK_%.3d",
            hw_status_table.audio.sound_track_played);
        new_track = track[tracktoggle];
    }

    tux_sw_status_set_strvalue(SW_ID_AUDIO_FLASH_PLAY, new_track, true);
}

/**
 * Update the general sound play with the low level status.
 */
LIBLOCAL void
tux_sound_flash_update_general_play(void)
{
    unsigned char new_state;

    new_state = hw_status_table.sensors1.play_general_sound;
    new_state |= hw_status_table.sensors1.play_internal_sound;

    tux_sw_status_set_intvalue(SW_ID_AUDIO_GENERAL_PLAY, new_state, true);
}

/**
 *
 */
LIBLOCAL void
tux_sound_flash_update_prog_current_track(void)
{
    unsigned char new_track;

    new_track = hw_status_table.flash_prog.current_state;

    tux_sw_status_set_intvalue(SW_ID_FLASH_PROG_CURR_TRACK, new_track, true);
}

/**
 *
 */
LIBLOCAL void
tux_sound_flash_update_prog_last_track_size(void)
{
    int new_size;

    new_size = hw_status_table.flash_prog.last_sound_size;

    tux_sw_status_set_intvalue(SW_ID_FLASH_PROG_LAST_TRACK_SIZE, new_size, true);
}

/**
 * Load the sound flash track number from a file.
 */
static void
load_knowed_track_num(void)
{
#ifdef LOCK_TUX
    FILE *f;
    char *ret_c;

    f = fopen("./track_num", "r");

    if (f)
    {
        fgets(knowed_track_num, sizeof(knowed_track_num)-2, f);
        ret_c = strchr(knowed_track_num, '\n');
        *ret_c = '\0';
        fclose(f);
    }
#endif
}

/**
 * Store the sound flash track number in a file.
 */
#ifdef LOCK_TUX
static void
save_knowed_track_num(void)
{
    FILE *f;

    f = fopen("./track_num", "w");

    if (f)
    {
        fprintf(f, "%d\n", sound_flash_desc.number_of_sounds);
        fclose(f);
    }
}
#endif

/**
 * Check the new descriptor.
 * Determine if the sound flash track number have changed.
 */
LIBLOCAL bool
tux_sound_flash_check_new_descriptor(bool save)
{
#ifdef LOCK_TUX
    bool ret = false;
    char track_num_str[8] = "";

    sprintf(track_num_str, "%d", sound_flash_desc.number_of_sounds);

    if (strcmp(knowed_track_num, track_num_str))
    {
        ret = true;
    }

    if (save)
    {
        save_knowed_track_num();
    }

    return ret;
#else
    return false;
#endif
}

/**
 * Send a command to tuxdroid to play a sound flash.
 */
LIBLOCAL bool
tux_sound_flash_cmd_play(unsigned char track_num, float vol)
{
    data_frame frame = {PLAY_SOUND_CMD, 0, 0, 0};
    unsigned char vol2 = 0;

    if (vol < 0.0)
    {
        vol2 = 7;
    }
    else
    {
        if (vol > 100.0)
        {
            vol2 = 0;
        }
        else
        {
            vol2 = 7 - (int)(vol * 7.0 / 100.0);
        }
    }

    frame[1] = track_num;
    frame[2] = vol2;

    return tux_usb_send_to_tux(frame);
}

#ifdef WIN32
#define DRVM_MAPPER                                 0x2000
#define DRVM_MAPPER_PREFERRED_SET                   DRVM_MAPPER + 22
#define DRVM_MAPPER_PREFERRED_GET                   DRVM_MAPPER + 21
/**
 *
 */
static bool
set_tux_as_default_playback(void)
{
    int i;
    WAVEOUTCAPSA wa_device;

    if (!waveOutGetNumDevs())
    {
        return false;
    }

    for (i = 0; i < waveOutGetNumDevs(); i++)
    {
        memset(&wa_device, 0, sizeof(wa_device));
        if (waveOutGetDevCapsA(i, &wa_device, sizeof(wa_device)) == 0)
        {
            if (strstr(wa_device.szPname, "TuxDroid-Audio") != NULL)
            {
                if (waveOutMessage((HWAVEOUT)WAVE_MAPPER,
                    DRVM_MAPPER_PREFERRED_SET, i, 0) == 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}


/**
 *
 */
static bool
get_default_playback_idx(int *idx)
{
    int err;
    DWORD po = -1;
    DWORD pf = 0;

    err = waveOutMessage((HWAVEOUT)WAVE_MAPPER, DRVM_MAPPER_PREFERRED_GET,
            (DWORD)&po, (DWORD)&pf);
    if (err == 0)
    {
        *idx = po;
        return true;
    }

    return true;
}

/**
 *
 */
static bool
get_default_playback_name(char *name)
{
    WAVEOUTCAPSA wa_device;
    int def_idx = -1;

    if (!get_default_playback_idx(&def_idx))
    {
        return false;
    }

    if (waveOutGetDevCapsA(def_idx, &wa_device, sizeof(wa_device)) == 0)
    {
        strcpy(name, wa_device.szPname);
        return true;
    }

    return false;
}

/**
 *
 */
static bool
get_idx_playback_from_name(int *idx, const char *name)
{
    int i;
    WAVEOUTCAPSA wa_device;

    if (!waveOutGetNumDevs())
    {
        return false;
    }

    for (i = 0; i < waveOutGetNumDevs(); i++)
    {
        memset(&wa_device, 0, sizeof(wa_device));
        if (waveOutGetDevCapsA(i, &wa_device, sizeof(wa_device)) == 0)
        {
            if (!strcmp(wa_device.szPname, name))
            {
                *idx = i;
                return true;
            }
        }
    }

    return false;
}

/**
 *
 */
static bool
set_default_playback(int idx)
{
    if (waveOutMessage((HWAVEOUT)WAVE_MAPPER, DRVM_MAPPER_PREFERRED_SET,
        idx, 0) == 0)
    {
        return true;
    }

    return false;
}

/**
 *
 */
LIBLOCAL void
tux_sound_flash_avoid_tts_default_sound_card(void)
{
    char def_dev_name[256] = "";

    get_default_playback_name(def_dev_name);

    if (strstr(def_dev_name, "TuxDroid-TTS") != NULL)
    {
        set_tux_as_default_playback();
    }
}

#else /* UNIX */

/**
 *
 */
static void
get_hw_audio_device_name(void)
{
    FILE *fp;
    int card;
    char filename[29];
    char line[81];
    unsigned int vid, pid;

    for (card = 0; card < 255; card++)
    {
        sprintf(filename, "/proc/asound/card%d/usbid", card);
        fp = fopen((const char *)filename, "r");
        if(fp != NULL)
        {
            fgets(line, 81, fp);
            sscanf( line, "%4x:%4x", &vid, &pid);
            fclose(fp);
            if((vid == TUX_VID) && (pid == TUX_PID))
            {
                /* Todo log this */
                sprintf(hw_audio_name, "default:%d,0", card);
                return;
            }
        }
    }
}
#endif /* WIN32 || UNIX */

/**
 *
 */
static bool
play_wav(const char *wav_path)
{
    bool ret = true;

#ifdef WIN32
    char def_dev_name[256] = "";
    int def_idx = -1;

    get_default_playback_name(def_dev_name);

    if (!set_tux_as_default_playback())
    {
        return false;
    }

    if (sndPlaySound(wav_path, SND_SYNC) != true)
    {
        ret = false;
    }

    if (!get_idx_playback_from_name(&def_idx, def_dev_name))
    {
        return false;
    }

    if (!set_default_playback(def_idx))
    {
        return false;
    }

#else /* UNIX */
    int r;
    char cmd[256] = "";

    ret = false;

    if (hw_audio_name[0] != '\0')
    {
        sprintf(cmd, "aplay -D plughw:TuxDroid %s", wav_path);
        r = system(cmd);
        if (r == 0)
            ret = true;
    }
#endif

    return ret;
}

/**
 *
 */
static void
init_reflash_info(void)
{
    memset(&reflash_info, 0, sizeof(sound_reflash_info_t));
    reflash_info.error = SRE_NO_ERROR;
    reflash_info.current_state = SRS_STDBY;
}

/**
 *
 */
static int
parse_wavs(const char *src_str, tokens_t *toks)
{
    return tux_cmd_parser_get_tokens(src_str, toks, 256, "|");
}

/**
 *
 */
LIBLOCAL TuxDrvError
tux_sound_flash_cmd_reflash(const char *tracks)
{
    FILE *fp;
    int i;
    int block_count = 0;
    int tmp_bc = 0;

    if (reflash_info.current_state != SRS_STDBY)
    {
        return E_TUXDRV_BUSY;
    }

    init_reflash_info();
    reflash_info.wav_count = parse_wavs(tracks, &reflash_info.wav_path);

    if (!reflash_info.wav_count)
    {
        return E_TUXDRV_BADWAVFILE;
    }

    if (reflash_info.wav_count > 0)
    {
        for (i = 0; i < reflash_info.wav_count; i++)
        {
            /* open the wav file */
            fp = fopen((char *)reflash_info.wav_path[i], "rb");
            /* if wav_path not accessible then exit */
            if (fp == NULL)
            {
                return E_TUXDRV_BADWAVFILE;
            }
            /* Get the size of file */
            fseek(fp, 0, SEEK_END);
            reflash_info.wav_size[i] = ftell(fp) - 44;
            fclose(fp);
            reflash_info.full_size += reflash_info.wav_size[i];
            /* Get the needed blocks number for this sound */
            tmp_bc = (int)(reflash_info.wav_size[i] / 4000);
            if ((int)(reflash_info.wav_size[i] % 4000) > 0)
            {
                tmp_bc++;
            }
            /* Add to global blocks count */
            block_count += tmp_bc;
        }
    }

    /* If needed blocks exceeds 127 then fail */
    if (block_count > 127)
    {
        return E_TUXDRV_WAVSIZEEXCEDED;
    }

    reflash_info.current_state = SRS_INIT;

    tux_cmd_parser_clear_delay_commands();

    return E_TUXDRV_NOERROR;
}

/**
 *
 */
LIBLOCAL void
tux_sound_flash_state_machine_call(void)
{
    bool rf_state = false;
    float full_time_sec = 0.0;
    data_frame frame = {0, 0, 0, 0};
    unsigned char curr_track_for_event = 0;

    /* Check fux connection and radio connection */
    if (reflash_info.current_state != SRS_STDBY)
    {
        rf_state = tux_usb_get_rf_state();
        if ((!tux_usb_connected()) || (!rf_state))
        {
            reflash_info.error = SRE_RF_OFFLINE;
            reflash_info.current_state = SRS_FINISH;
        }
    }

    switch (reflash_info.current_state) {
    case SRS_STDBY:
        break;
    case SRS_INIT:
        /* Disabling commands parsing */
        tux_cmd_parser_set_enable(false);
        /* Send begin flashing event with the fulltime of processing */
        full_time_sec = 10.0 + (reflash_info.full_size / 8000.0);
        full_time_sec += reflash_info.wav_count * 0.97;
        tux_sw_status_set_floatvalue(SW_ID_SOUND_REFLASH_BEGIN,
            full_time_sec, true);
        tux_sw_status_set_strvalue(SW_ID_SOUND_REFLASH_END, STRING_VALUE_NDEF,
            true);
        /* Goto SRS_ERASE state */
        reflash_info.current_state = SRS_ERASE;
        break;
    case SRS_ERASE:
        /* Leds pulsing */
        tux_leds_cmd_set(LED_LEFT, 0.0, NONE, 0, 0);
        tux_leds_cmd_set(LED_RIGHT, 1.0, NONE, 0, 0);
        tux_leds_cmd_pulse(LED_BOTH, 0.0, 1.0, 255, 1.0,
            FADE_DURATION, 0.5, 0);
        /* Send erase cmd */
        log_info("Sound reflash: Erasing");
        frame[0] = ERASE_FLASH_CMD;
        if (!tux_usb_send_to_tux(frame))
        {
            reflash_info.error = SRE_USB_ERROR;
            reflash_info.current_state = SRS_FINISH;
            break;
        }
        /* Wait that the flash has been erased */
        sleep(10);
        /* Set first track to write */
        reflash_info.current_wav = 0;
        /* Goto SRS_WRITE state */
        reflash_info.current_state = SRS_WRITE;
        break;
    case SRS_WRITE:
        /* Send store sound command */
        log_info("Sound reflash: Store track (%d of %d)",
            reflash_info.current_wav + 1,
            reflash_info.wav_count);
        frame[0] = STORE_SOUND_CMD;
        if (!tux_usb_send_to_tux(frame))
        {
            reflash_info.error = SRE_USB_ERROR;
            reflash_info.current_state = SRS_FINISH;
            break;
        }
        curr_track_for_event = reflash_info.current_wav + 1;
        tux_sw_status_set_intvalue(SW_ID_SOUND_REFLASH_CURRENT_TRACK,
            curr_track_for_event, true);
        usleep(200000);
        /* Play current wav track */
        if (!play_wav(reflash_info.wav_path[reflash_info.current_wav]))
        {
            frame[0] = CONFIRM_STORAGE_CMD;
            frame[1] = 0;
            if (!tux_usb_send_to_tux(frame))
            {
                reflash_info.error = SRE_USB_ERROR;
                reflash_info.current_state = SRS_FINISH;
                break;
            }
            reflash_info.error = SRE_WAV_ERROR;
            reflash_info.current_state = SRS_FINISH;
            break;
        }
        /* Send confirm track command */
        usleep(200000);
        frame[0] = CONFIRM_STORAGE_CMD;
        frame[1] = 1;
        if (!tux_usb_send_to_tux(frame))
        {
            reflash_info.error = SRE_USB_ERROR;
            reflash_info.current_state = SRS_FINISH;
            break;
        }
        usleep(100000);
        /* Set next track to write */
        reflash_info.current_wav += 1;
        /* If the next track is out of limit, Goto SRS_FINISH state */
        if (reflash_info.current_wav >= reflash_info.wav_count)
        {
            reflash_info.current_state = SRS_FINISH;
        }
        break;
    case SRS_FINISH:
        /* Enabling commands parsing */
        tux_cmd_parser_set_enable(true);
        /* Leds stop */
        tux_leds_cmd_set(LED_BOTH, 1.0, NONE, 0, 0);
        /* Send end of flashing event */
        full_time_sec = 0.0;
        tux_sw_status_set_floatvalue(SW_ID_SOUND_REFLASH_BEGIN,
            full_time_sec, false);
        switch (reflash_info.error) {
        case SRE_NO_ERROR:
            tux_sw_status_set_strvalue(SW_ID_SOUND_REFLASH_END,
                STRING_VALUE_NO_ERROR, true);
            log_info("Sound reflash: Finish (%s)",
                STRING_VALUE_NO_ERROR);
            /* Play first track */
            tux_sound_flash_cmd_play(1, 100.0);
            break;
        case SRE_RF_OFFLINE:
            tux_sw_status_set_strvalue(SW_ID_SOUND_REFLASH_END,
                STRING_VALUE_ERROR_RF_OFFLINE, true);
            log_info("Sound reflash: Finish (%s)",
                STRING_VALUE_ERROR_RF_OFFLINE);
            break;
        case SRE_WAV_ERROR:
            tux_sw_status_set_strvalue(SW_ID_SOUND_REFLASH_END,
                STRING_VALUE_ERROR_WAV_ERROR, true);
            log_info("Sound reflash: Finish (%s)",
                STRING_VALUE_ERROR_WAV_ERROR);
            break;
        case SRE_USB_ERROR:
            tux_sw_status_set_strvalue(SW_ID_SOUND_REFLASH_END,
                STRING_VALUE_ERROR_USB, true);
            log_info("Sound reflash: Finish (%s)",
                STRING_VALUE_ERROR_USB);
            break;
        }
        /* Goto SRS_STDBY state */
        reflash_info.current_state = SRS_STDBY;
        break;
    }
}
