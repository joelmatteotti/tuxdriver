/*
 * Tux Droid - Command parser
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
 * \file tux_cmd_parser.c
 * \brief Command parser functions.
 * \author remi.jocaille@c2me.be
 * \ingroup command_parser
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef USE_MUTEX
#   include "threading_uniform.h"
#endif

#include "log.h"
#include "tux_audio.h"
#include "tux_cmd_parser.h"
#include "tux_error.h"
#include "tux_eyes.h"
#include "tux_leds.h"
#include "tux_mouth.h"
#include "tux_sound_flash.h"
#include "tux_spinning.h"
#include "tux_sw_status.h"
#include "tux_types.h"
#include "tux_usb.h"
#include "tux_user_inputs.h"
#include "tux_flippers.h"

#define NRCMDS 512

/** \brief Cmd stack structure */
typedef struct {
    delay_cmd_t cmd_list[NRCMDS]; /**< Stack */
    int cmd_count; /**< Number of commands in stack */
} cmd_stack_t;

/** \brief Cmd stack for user */
static cmd_stack_t user_cmd_stack;
/** \brief Cmd stack for internal system */
static cmd_stack_t sys_cmd_stack;
#ifdef USE_MUTEX
static mutex_t __stack_mutex;
static mutex_t __macro_mutex;
#endif
/** \brief Flag which indicates if the parser is enabled */
static bool cmd_parser_enable = true;

/**
 * \brief Initialize the parser.
 */
LIBLOCAL void
tux_cmd_parser_init(void)
{
    memset(&user_cmd_stack, 0, sizeof(cmd_stack_t));
    memset(&sys_cmd_stack, 0, sizeof(cmd_stack_t));
#ifdef USE_MUTEX
    mutex_init(__stack_mutex);
    mutex_init(__macro_mutex);
#endif
}

/**
 * \brief Enabling/disabling the parser.
 * \param value Flag value.
 */
LIBLOCAL void
tux_cmd_parser_set_enable(bool value)
{
    cmd_parser_enable = value;
}

/**
 * \brief Get tokens from a string line.
 * \param src_str Line to parse.
 * \param toks Output tokens.
 * \param max_tokens Maximum tokens to retrieve.
 * \param delimiters Delimiters chars.
 * \return The number of retrieved tokens.
 */
LIBLOCAL int
tux_cmd_parser_get_tokens(const char *src_str, tokens_t *toks,
        int max_tokens, const char *delimiters)
{
    const char *p;      /* pointer to the next token */
    const char *pnext;  /* pointer to the next delimiter */
    int len;            /* length of the next token */
    int cnt = 0;        /* nr of tokens processed */

    p = src_str;

    /* the implementation below is build upon strncpy
       it might be more efficient/cleaner to use strtok
    */

    if (p)
    {
        while (1)
        {
            /* get the next delimiter */
            pnext = strpbrk(p, delimiters);
            if (pnext)
            {
                len = pnext - p;
                strncpy((*toks)[cnt], p, len);
                ((*toks)[cnt])[len] = 0;
                cnt++;
                if (cnt >= max_tokens)
                {
                    break;
                }
            }
            else
            {
                /* no next delimiter, so copy all the remaining strings */
                strcpy((*toks)[cnt], p);
                cnt++;
                break;
            }
            p = pnext+1;
        }
    }
    return(cnt);
}

/**
 * \brief Convert a string to a final movement state value.
 * \brief conststr String to convert.
 * \brief state Output final movement state.
 * \return The convertion success.
 */
static bool
str_to_state_t(const char *conststr, move_final_state_t *state)
{
    if (!strcmp(conststr, "NDEF"))
    {
        *state = FINAL_ST_UNDEFINED;
        return true;
    }
    if (!strcmp(conststr, "UNDEFINED"))
    {
        *state = FINAL_ST_UNDEFINED;
        return true;
    }
    if (!strcmp(conststr, "OPEN"))
    {
        *state = FINAL_ST_OPEN_UP;
        return true;
    }
    if (!strcmp(conststr, "UP"))
    {
        *state = FINAL_ST_OPEN_UP;
        return true;
    }
    if (!strcmp(conststr, "CLOSE"))
    {
        *state = FINAL_ST_CLOSE_DOWN;
        return true;
    }
    if (!strcmp(conststr, "DOWN"))
    {
        *state = FINAL_ST_CLOSE_DOWN;
        return true;
    }
    if (!strcmp(conststr, "STOP"))
    {
        *state = FINAL_ST_STOP;
        return true;
    }
    return false;
}

/**
 * \brief Convert a string to a led type.
 * \brief conststr String to convert.
 * \brief leds Output led type.
 * \return The convertion success.
 */
static bool
str_to_leds_t(const char *conststr, leds_t *leds)
{
    if (!strcmp(conststr, "LED_NONE"))
    {
        *leds = LED_NONE;
        return true;
    }
    if (!strcmp(conststr, "LED_LEFT"))
    {
        *leds = LED_LEFT;
        return true;
    }
    if (!strcmp(conststr, "LED_RIGHT"))
    {
        *leds = LED_RIGHT;
        return true;
    }
    if (!strcmp(conststr, "LED_BOTH"))
    {
        *leds = LED_BOTH;
        return true;
    }
    return false;
}

/**
 * \brief Convert a string to a led effect type.
 * \brief conststr String to convert.
 * \brief effect_type Output led effect type.
 * \return The convertion success.
 */
static bool
str_to_effect_type(const char *conststr, effect_type_t *effect_type)
{
    if (!strcmp(conststr, "UNAFFECTED"))
    {
        *effect_type = UNAFFECTED;
        return true;
    }
    if (!strcmp(conststr, "LAST"))
    {
        *effect_type = LAST;
        return true;
    }
    if (!strcmp(conststr, "NONE"))
    {
        *effect_type = NONE;
        return true;
    }
    if (!strcmp(conststr, "DEFAULT"))
    {
        *effect_type = DEFAULT;
        return true;
    }
    if (!strcmp(conststr, "FADE_DURATION"))
    {
        *effect_type = FADE_DURATION;
        return true;
    }
    if (!strcmp(conststr, "FADE_RATE"))
    {
        *effect_type = FADE_RATE;
        return true;
    }
    if (!strcmp(conststr, "GRADIENT_NBR"))
    {
        *effect_type = GRADIENT_NBR;
        return true;
    }
    if (!strcmp(conststr, "GRADIENT_DELTA"))
    {
        *effect_type = GRADIENT_DELTA;
        return true;
    }
    return false;
}

/**
 * \brief Parse an audio command [Level 2]
 * \param tokens Command tokens.
 * \param cmd Cmd structure.
 * \return The error result.
 */
static TuxDrvError
parse_tux_audio_command(tokens_t tokens, delay_cmd_t *cmd)
{
    TuxDrvError ret =  E_TUXDRV_INVALIDCOMMAND;

    if (strcmp(tokens[2], "CHANNEL_GENERAL") == 0)
    {
        cmd->sub_command = CHANNEL_GENERAL;
        ret = E_TUXDRV_NOERROR;
    }
    else if (strcmp(tokens[2], "CHANNEL_TTS") == 0)
    {
        cmd->sub_command = CHANNEL_TTS;
        ret = E_TUXDRV_NOERROR;
    }
    else if (strcmp(tokens[2], "MUTE") == 0)
    {
        cmd->sub_command = MUTE;
        if (str_to_bool(tokens[3], &cmd->audio_mute_parameters.muteflag))
        {
            // write to struct
            ret = E_TUXDRV_NOERROR;
        }
    }
    return ret;
}

/**
 * \brief Parse an eyes command [Level 2]
 * \param tokens Command tokens.
 * \param cmd Cmd structure.
 * \return The error result.
 */
static TuxDrvError
parse_tux_eyes_command(tokens_t tokens, delay_cmd_t *cmd)
{
    TuxDrvError ret =  E_TUXDRV_INVALIDCOMMAND;

    if (strcmp(tokens[2], "CLOSE") == 0)
    {
        cmd->sub_command = CLOSE;
        ret = E_TUXDRV_NOERROR;
    }
    else if (strcmp(tokens[2], "OFF") == 0)
    {
        cmd->sub_command = OFF;
        ret = E_TUXDRV_NOERROR;
    }
    else if (strcmp(tokens[2], "ON") == 0)
    {
        cmd->sub_command = ON;
        if (str_to_uint8(tokens[3], &cmd->eyes_on_parameters.nr_movements) &&
            str_to_state_t(tokens[4], &cmd->eyes_on_parameters.state))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    else if (strcmp(tokens[2], "ON_DURING") == 0)
    {
        cmd->sub_command = ON_DURING;
        if (str_to_float(tokens[3], &cmd->eyes_on_during_parameters.duration) &&
            str_to_state_t(tokens[4], &cmd->eyes_on_during_parameters.state))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    else if (strcmp(tokens[2], "OPEN") == 0)
    {
        cmd->sub_command = OPEN;
        ret = E_TUXDRV_NOERROR;
    }
    return ret;
}

/**
 * \brief Parse an IR command [Level 2]
 * \param tokens Command tokens.
 * \param cmd Cmd structure.
 * \return The error result.
 */
static TuxDrvError
parse_tux_ir_command(tokens_t tokens, delay_cmd_t *cmd)
{
    TuxDrvError ret =  E_TUXDRV_INVALIDCOMMAND;

    if (strcmp(tokens[2], "ON") == 0)
    {
        cmd->sub_command = ON;
        ret = E_TUXDRV_NOERROR;
    }
    else if (strcmp(tokens[2], "OFF") == 0)
    {
        cmd->sub_command = OFF;
        ret = E_TUXDRV_NOERROR;
    }
    else if (strcmp(tokens[2], "SEND") == 0)
    {
        cmd->sub_command = SEND;
        if (str_to_uint8(tokens[3], &cmd->ir_send_parameters.address) &&
            str_to_uint8(tokens[4], &cmd->ir_send_parameters.command))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }

    return ret;
}

/**
 * \brief Parse a led command [Level 2]
 * \param tokens Command tokens.
 * \param cmd Cmd structure.
 * \return The error result.
 */
static TuxDrvError
parse_tux_led_command(tokens_t tokens, delay_cmd_t *cmd)
{
    TuxDrvError ret =  E_TUXDRV_INVALIDCOMMAND;
    if (strcmp(tokens[2], "BLINK") == 0)
    {
        cmd->sub_command = BLINK;
        if (str_to_leds_t(tokens[3], &cmd->led_blink_parameters.leds) &&
            str_to_uint8(tokens[4], &cmd->led_blink_parameters.pulse_count) &&
            str_to_float(tokens[5], &cmd->led_blink_parameters.pulse_period))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    else if (strcmp(tokens[2], "OFF") == 0)
    {
        cmd->sub_command = OFF;
        if (str_to_leds_t(tokens[3], &cmd->led_off_parameters.leds))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    else if (strcmp(tokens[2], "ON") == 0)
    {
        cmd->sub_command = ON;
        if (str_to_leds_t(tokens[3], &cmd->led_on_parameters.leds) &&
            str_to_float(tokens[4], &cmd->led_on_parameters.intensity))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    else if (strcmp(tokens[2], "PULSE") == 0)
    {
        cmd->sub_command = PULSE;
        if (str_to_leds_t(tokens[3], &cmd->led_pulse_parameters.leds) &&
            str_to_float(tokens[4], &cmd->led_pulse_parameters.min_intensity) &&
            str_to_float(tokens[5], &cmd->led_pulse_parameters.max_intensity) &&
            str_to_uint8(tokens[6], &cmd->led_pulse_parameters.pulse_count) &&
            str_to_float(tokens[7], &cmd->led_pulse_parameters.pulse_period) &&
            str_to_effect_type(tokens[8], &cmd->led_pulse_parameters.effect_type) &&
            str_to_float(tokens[9], &cmd->led_pulse_parameters.effect_speed) &&
            str_to_uint8(tokens[10], &cmd->led_pulse_parameters.effect_step))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    else if (strcmp(tokens[2], "SET") == 0)
    {
        cmd->sub_command = SET;
        if (str_to_leds_t(tokens[3], &cmd->led_set_parameters.leds) &&
            str_to_float(tokens[4], &cmd->led_set_parameters.intensity) &&
            str_to_effect_type(tokens[5], &cmd->led_set_parameters.effect_type) &&
            str_to_float(tokens[6], &cmd->led_set_parameters.effect_speed) &&
            str_to_uint8(tokens[7], &cmd->led_set_parameters.effect_step))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    return ret;
}

/**
 * \brief Parse a mouth command [Level 2]
 * \param tokens Command tokens.
 * \param cmd Cmd structure.
 * \return The error result.
 */
static TuxDrvError
parse_tux_mouth_command(tokens_t tokens, delay_cmd_t *cmd)
{
    TuxDrvError ret =  E_TUXDRV_INVALIDCOMMAND;

    if (strcmp(tokens[2], "CLOSE") == 0)
    {
        cmd->sub_command = CLOSE;
        ret = E_TUXDRV_NOERROR;
    }
    else if (strcmp(tokens[2], "OFF") == 0)
    {
        cmd->sub_command = OFF;
        ret = E_TUXDRV_NOERROR;
    }
    else if (strcmp(tokens[2], "ON") == 0)
    {
        cmd->sub_command = ON;
        if (str_to_uint8(tokens[3], &cmd->mouth_on_parameters.nr_movements) &&
            str_to_state_t(tokens[4], &cmd->mouth_on_parameters.state))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    else if (strcmp(tokens[2], "ON_DURING") == 0)
    {
        cmd->sub_command = ON_DURING;
        if (str_to_float(tokens[3], &cmd->mouth_on_during_parameters.duration) &&
            str_to_state_t(tokens[4], &cmd->mouth_on_during_parameters.state))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    else if (strcmp(tokens[2], "OPEN") == 0)
    {
        cmd->sub_command = OPEN;
        ret = E_TUXDRV_NOERROR;
    }
    return ret;
}

/**
 * \brief Parse a sound flash command [Level 2]
 * \param tokens Command tokens.
 * \param cmd Cmd structure.
 * \return The error result.
 */
static TuxDrvError
parse_tux_sound_flash_command(tokens_t tokens, delay_cmd_t *cmd)
{
    TuxDrvError ret =  E_TUXDRV_INVALIDCOMMAND;

    if (strcmp(tokens[2], "PLAY") == 0)
    {
        cmd->sub_command = PLAY;
        if (str_to_uint8(tokens[3], &cmd->sound_flash_play_parameters.track) &&
            str_to_float(tokens[4], &cmd->sound_flash_play_parameters.volume))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    return ret;
}

/**
 * \brief Parse a spinning command [Level 2]
 * \param tokens Command tokens.
 * \param cmd Cmd structure.
 * \return The error result.
 */
static TuxDrvError
parse_tux_spinning_command(tokens_t tokens, delay_cmd_t *cmd)
{
    TuxDrvError ret =  E_TUXDRV_INVALIDCOMMAND;

    if (strcmp(tokens[2], "LEFT_ON") == 0)
    {
        cmd->sub_command = LEFT_ON;
        if (str_to_uint8(tokens[3], &cmd->spinning_on_parameters.nr_qturns))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    else if (strcmp(tokens[2], "LEFT_ON_DURING") == 0)
    {
        cmd->sub_command = LEFT_ON_DURING;
        if (str_to_float(tokens[3], &cmd->spinning_on_during_parameters.duration))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    else if (strcmp(tokens[2], "RIGHT_ON") == 0)
    {
        cmd->sub_command = RIGHT_ON;
        if (str_to_uint8(tokens[3], &cmd->spinning_on_parameters.nr_qturns))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    else if (strcmp(tokens[2], "RIGHT_ON_DURING") == 0)
    {
        cmd->sub_command = RIGHT_ON_DURING;
        if (str_to_float(tokens[3], &cmd->spinning_on_during_parameters.duration))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    else if (strcmp(tokens[2], "OFF") == 0)
    {
        cmd->sub_command = OFF;
        ret = E_TUXDRV_NOERROR;
    }
    else if (strcmp(tokens[2], "SPEED") == 0)
    {
        cmd->sub_command = SPEED;
        if (str_to_uint8(tokens[3], &cmd->spinning_speed_parameters.speed))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    return ret;
}

/**
 * \brief Parse a flippers command [Level 2]
 * \param tokens Command tokens.
 * \param cmd Cmd structure.
 * \return The error result.
 */
static TuxDrvError
parse_tux_flippers_command(tokens_t tokens, delay_cmd_t *cmd)
{
    TuxDrvError ret =  E_TUXDRV_INVALIDCOMMAND;

    if (strcmp(tokens[2], "DOWN") == 0)
    {
        cmd->sub_command = DOWN;
        ret = E_TUXDRV_NOERROR;
    }
    else if (strcmp(tokens[2], "OFF") == 0)
    {
        cmd->sub_command = OFF;
        ret = E_TUXDRV_NOERROR;
    }
    else if (strcmp(tokens[2], "ON") == 0)
    {
        cmd->sub_command = ON;
        if (str_to_uint8(tokens[3], &cmd->flippers_on_parameters.nr_movements) &&
            str_to_state_t(tokens[4], &cmd->flippers_on_parameters.state))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    else if (strcmp(tokens[2], "ON_DURING") == 0)
    {
        cmd->sub_command = ON_DURING;
        if (str_to_float(tokens[3], &cmd->flippers_on_during_parameters.duration) &&
            str_to_state_t(tokens[4], &cmd->flippers_on_during_parameters.state))
        {
            ret = E_TUXDRV_NOERROR;
        }
        cmd->sub_command = ON_DURING;
        ret = E_TUXDRV_NOERROR;
    }
    else if (strcmp(tokens[2], "UP") == 0)
    {
        cmd->sub_command = UP;
        ret = E_TUXDRV_NOERROR;
    }
    else if (strcmp(tokens[2], "SPEED") == 0)
    {
        cmd->sub_command = SPEED;
        if (str_to_uint8(tokens[3], &cmd->flippers_speed_parameters.speed))
        {
            ret = E_TUXDRV_NOERROR;
        }
    }
    return ret;
}

/**
 * \brief Parse a Tux command [Level 1]
 * \param tokens Command tokens.
 * \param cmd Cmd structure.
 * \return The error result.
 */
static TuxDrvError
parse_tux_command(tokens_t tokens, delay_cmd_t *cmd)
{
    TuxDrvError ret =  E_TUXDRV_INVALIDCOMMAND;

    if (strcmp(tokens[1], "AUDIO") == 0)
    {
        cmd->command = AUDIO;
        ret = parse_tux_audio_command(tokens, cmd);
    }
    else if (strcmp(tokens[1], "EYES") == 0)
    {
        cmd->command = EYES;
        ret = parse_tux_eyes_command(tokens, cmd);
    }
    else if (strcmp(tokens[1], "IR") == 0)
    {
        cmd->command = IR;
        ret = parse_tux_ir_command(tokens, cmd);
    }
    else if (strcmp(tokens[1], "LED") == 0)
    {
        cmd->command = LED;
        ret = parse_tux_led_command(tokens, cmd);
    }
    else if (strcmp(tokens[1], "MOUTH") == 0)
    {
        cmd->command = MOUTH;
        ret = parse_tux_mouth_command(tokens, cmd);
    }
    else if (strcmp(tokens[1], "SOUND_FLASH") == 0)
    {
        cmd->command = SOUND_FLASH;
        ret = parse_tux_sound_flash_command(tokens, cmd);
    }
    else if (strcmp(tokens[1], "SPINNING") == 0)
    {
        cmd->command = SPINNING;
        ret = parse_tux_spinning_command(tokens, cmd);
    }
    else if (strcmp(tokens[1], "FLIPPERS") == 0)
    {
        cmd->command = FLIPPERS;
        ret = parse_tux_flippers_command(tokens, cmd);
    }
    return ret;
}

/**
 * \brief Parse a RAW command [Level 1]
 * \param tokens Command tokens.
 * \param cmd Cmd structure.
 * \return The error result.
 */
static TuxDrvError
parse_raw_command(tokens_t tokens, delay_cmd_t *cmd)
{
    TuxDrvError ret =  E_TUXDRV_INVALIDCOMMAND;
    int r = 0;
    int i;

    for (i = 0; i < TUX_SEND_LENGTH; i++)
    {
        if (hex_to_uint8(tokens[i + 1], &cmd->raw_parameters.raw[i]))
        {
            r++;
        }
    }

    if (r == TUX_SEND_LENGTH)
    {
        ret = E_TUXDRV_NOERROR;
    }
    return ret;
}

/**
 * \brief Parse a command [Level 0]
 * \param tokens Command tokens.
 * \param cmd Cmd structure.
 * \return The error result.
 */
static TuxDrvError
parse_command(const char *cmd_str, delay_cmd_t *cmd)
{
    TuxDrvError ret =  E_TUXDRV_INVALIDCOMMAND;
    tokens_t tokens;
    /*int nr_tokens;*/

    /* If the parser is not enabled then fail */
    if (!cmd_parser_enable)
    {
        return E_TUXDRV_PARSERISDISABLED;
    }

    log_debug("parse_command : [%s]", cmd_str);
    memset(&tokens, 0, sizeof(tokens_t));
    /*nr_tokens = tux_cmd_parser_get_tokens(cmd_str, &tokens, 32, ":,");
	*/
	
	tux_cmd_parser_get_tokens(cmd_str, &tokens, 32, ":,");
	
    if (strcmp(tokens[0], "TUX_CMD") == 0)
    {
        cmd->command_group = TUX_CMD;
        ret = parse_tux_command(tokens, cmd);
    }
    else if (strcmp(tokens[0], "RAW_CMD") == 0)
    {
        cmd->command_group = RAW_CMD;
        ret = parse_raw_command(tokens, cmd);
    }
    return ret;
}

/**
 * \brief Execute an audio command.
 * \param cmd Command to execute.
 */
static void
execute_audio_command (delay_cmd_t *cmd)
{
    switch (cmd->sub_command) {
        case CHANNEL_GENERAL:
            tux_audio_cmd_channel_general();
            break;
        case CHANNEL_TTS:
            tux_audio_cmd_channel_tts();
            break;
        case MUTE:
            tux_audio_cmd_mute(cmd->audio_mute_parameters.muteflag);
            break;
        default: /* should not occur */
            log_error("execute invalid audio command");
    }
}

/**
 * \brief Execute an eyes command.
 * \param cmd Command to execute.
 */
static void
execute_eyes_command (delay_cmd_t *cmd)
{
    switch (cmd->sub_command) {
        case ON:
            tux_eyes_cmd_on(
                cmd->eyes_on_parameters.nr_movements,
                cmd->eyes_on_parameters.state);
            break;
        case ON_DURING:
            tux_eyes_cmd_on_during(
                cmd->eyes_on_during_parameters.duration,
                cmd->eyes_on_during_parameters.state);
            break;
        case OPEN:
            tux_eyes_cmd_open();
            break;
        case CLOSE:
            tux_eyes_cmd_close();
            break;
        case OFF:
            tux_eyes_cmd_off();
            break;
        default: /* should not occur */
            log_error("execute invalid eyes command");
    }
}

/**
 * \brief Execute an IR command.
 * \param cmd Command to execute.
 */
static void
execute_ir_command (delay_cmd_t *cmd)
{
    switch (cmd->sub_command) {
        case ON:
            tux_user_inputs_cmd_ir_on();
            break;
        case OFF:
            tux_user_inputs_cmd_ir_off();
        case SEND:
            tux_user_inputs_cmd_ir_send(
                cmd->ir_send_parameters.address,
                cmd->ir_send_parameters.command);
            break;
        default: /* should not occur */
            log_error("execute invalid ir command");
    }
}

/**
 * \brief Execute a led command.
 * \param cmd Command to execute.
 */
static void
execute_led_command (delay_cmd_t *cmd)
{
    switch (cmd->sub_command) {
        case ON:
            tux_leds_cmd_set(
                cmd->led_on_parameters.leds,
                cmd->led_on_parameters.intensity,
                NONE,
                0,
                0);
            break;
        case OFF:
            tux_leds_cmd_set(
                cmd->led_off_parameters.leds,
                0.0,
                NONE,
                0,
                0);
            break;
        case PULSE:
            tux_leds_cmd_pulse(
                cmd->led_pulse_parameters.leds,
                cmd->led_pulse_parameters.min_intensity,
                cmd->led_pulse_parameters.max_intensity,
                cmd->led_pulse_parameters.pulse_count,
                cmd->led_pulse_parameters.pulse_period,
                cmd->led_pulse_parameters.effect_type,
                cmd->led_pulse_parameters.effect_speed,
                cmd->led_pulse_parameters.effect_step);
            break;
        case BLINK:
            tux_leds_cmd_pulse(
                cmd->led_blink_parameters.leds,
                0.0,
                1.0,
                cmd->led_blink_parameters.pulse_count,
                cmd->led_blink_parameters.pulse_period,
                NONE,
                0,
                0);
            break;
        case SET:
            tux_leds_cmd_set(
                cmd->led_set_parameters.leds,
                cmd->led_set_parameters.intensity,
                cmd->led_set_parameters.effect_type,
                cmd->led_set_parameters.effect_speed,
                cmd->led_set_parameters.effect_step);
            break;
        default: /* should not occur */
            log_error("execute invalid led command");
    }
}

/**
 * \brief Execute a mouth command.
 * \param cmd Command to execute.
 */
static void
execute_mouth_command (delay_cmd_t *cmd)
{
    switch (cmd->sub_command) {
        case ON:
            tux_mouth_cmd_on(
                cmd->mouth_on_parameters.nr_movements,
                cmd->mouth_on_parameters.state);
            break;
        case ON_DURING:
            tux_mouth_cmd_on_during(
                cmd->mouth_on_during_parameters.duration,
                cmd->mouth_on_during_parameters.state);
            break;
        case OPEN:
            tux_mouth_cmd_open();
            break;
        case CLOSE:
            tux_mouth_cmd_close();
            break;
        case OFF:
            tux_mouth_cmd_off();
            break;
        default: /* should not occur */
            log_error("execute invalid mouth command");
    }
}

/**
 * \brief Execute a sound flash command.
 * \param cmd Command to execute.
 */
static void
execute_sound_flash_command (delay_cmd_t *cmd)
{
    switch (cmd->sub_command) {
        case PLAY:
            tux_sound_flash_cmd_play(
                cmd->sound_flash_play_parameters.track,
                cmd->sound_flash_play_parameters.volume);
            break;
        default: /* should not occur */
            log_error("execute invalid sound flash command");
    }
}

/**
 * \brief Execute a spinning command.
 * \param cmd Command to execute.
 */
static void
execute_spinning_command (delay_cmd_t *cmd)
{
    switch (cmd->sub_command) {
        case LEFT_ON:
            tux_spinning_cmd_left_on(cmd->spinning_on_parameters.nr_qturns);
            break;
        case RIGHT_ON:
            tux_spinning_cmd_right_on(cmd->spinning_on_parameters.nr_qturns);
            break;
        case LEFT_ON_DURING:
            tux_spinning_cmd_left_on_during(cmd->spinning_on_during_parameters.duration);
            break;
        case RIGHT_ON_DURING:
            tux_spinning_cmd_right_on_during(cmd->spinning_on_during_parameters.duration);
            break;
        case OFF:
            tux_spinning_cmd_off();
            break;
        case SPEED:
            tux_spinning_cmd_speed(cmd->spinning_speed_parameters.speed);
            break;
        default: /* should not occur */
            log_error("execute invalid spinning command");
    }
}

/**
 * \brief Execute a flippers command.
 * \param cmd Command to execute.
 */
static void
execute_flippers_command (delay_cmd_t *cmd)
{
    switch (cmd->sub_command) {
        case ON:
            tux_flippers_cmd_on(
                cmd->flippers_on_parameters.nr_movements,
                cmd->flippers_on_parameters.state);
            break;
        case ON_DURING:
            tux_flippers_cmd_on_during(
                cmd->flippers_on_during_parameters.duration,
                cmd->flippers_on_during_parameters.state);
            break;
        case OFF:
            tux_flippers_cmd_off();
            break;
        case UP:
            tux_flippers_cmd_up();
            break;
        case DOWN:
            tux_flippers_cmd_down();
            break;
        case SPEED:
            tux_flippers_cmd_speed(cmd->flippers_speed_parameters.speed);
            break;
        default: /* should not occur */
            log_error("execute invalid flippers command");
    }
}

/**
 * \brief Execute a RAW command.
 * \param cmd Command to execute.
 */
static void
execute_raw_command (delay_cmd_t *cmd)
{
    tux_usb_send_raw(cmd->raw_parameters.raw);
}

/**
 * \brief Execute a command.
 * \param cmd Command to execute.
 */
static void
execute_command (delay_cmd_t *cmd)
{
    if(cmd->command_group == TUX_CMD)
    {
        switch(cmd->command) {
            case AUDIO:
                execute_audio_command(cmd);
                break;
            case EYES:
                execute_eyes_command(cmd);
                break;
            case IR:
                execute_ir_command(cmd);
                break;
            case LED:
                execute_led_command(cmd);
                break;
            case MOUTH:
                execute_mouth_command(cmd);
                break;
            case SOUND_FLASH:
                execute_sound_flash_command(cmd);
                break;
            case SPINNING:
                execute_spinning_command(cmd);
                break;
            case FLIPPERS:
                execute_flippers_command(cmd);
                break;
        }
    }
    else
    {
        if(cmd->command_group == RAW_CMD)
        {
            execute_raw_command(cmd);
        }
    }
    cmd->command_group = NO_CMD;
}

/**
 * \brief Cleanup of the system commands in order to keep the consistency of the
 * system stack.
 * \param command Command to check.
 */
LIBLOCAL void
tux_cmd_parser_clean_sys_command(tux_command_t command)
{
    int i, j;
    bool have_parent = true;

    /* For all sys commands */
    for (i = 0; i < NRCMDS; i++)
    {
        /* System command match */
        if ((sys_cmd_stack.cmd_list[i].command == command) &&
            (sys_cmd_stack.cmd_list[i].command_group != NO_CMD))
        {
            /* Find the user command (on_during) which have insert this
             * system command.
             */
            have_parent = false;

            for (j = 0; j < NRCMDS; j++)
            {
                /* Possible user command*/
                if ((user_cmd_stack.cmd_list[j].command == command) &&
                    (user_cmd_stack.cmd_list[j].command_group != NO_CMD))
                {
                    /* Ok : system command have a parent user command */
                    if (user_cmd_stack.cmd_list[j].inserted_at_time ==\
                        sys_cmd_stack.cmd_list[i].inserted_at_time)
                    {
                        have_parent = true;
                        break;
                    }
                }
            }

            /* The system command is orphan */
            if (!have_parent)
            {
                /* The system command must be deleted */
                sys_cmd_stack.cmd_list[i].command_group = NO_CMD;
                sys_cmd_stack.cmd_list[i].timeout = 0.;
                sys_cmd_stack.cmd_list[i].inserted_at_time = 0.;
            }
        }
    }
}

/**
 * \brief Insert a command in a command stack.
 * \param delay Delay before the execution of the command.
 * \param cmd Command to execute.
 * \param stack Command stack how to insert the command.
 */
static TuxDrvError
insert_command(float delay, delay_cmd_t *cmd, cmd_stack_t *stack)
{
    TuxDrvError ret = E_TUXDRV_STACKOVERFLOW;
    int i;
    double curtime = get_time();

    for (i = 0; i < NRCMDS; i++)
    {
        if (stack->cmd_list[i].command_group == NO_CMD)
        {
            stack->cmd_list[i] = *cmd;
            stack->cmd_list[i].timeout = delay + curtime;
            stack->cmd_list[i].inserted_at_time = (float)(int)(curtime * 100) / 100.0;
            ret = E_TUXDRV_NOERROR;
            break;
        }
    }
    return(ret);
}

/**
 * \brief Insert a command in the system stack.
 * \param delay Delay before the execution of the command.
 * \param cmd Command to execute.
 */
LIBLOCAL TuxDrvError
tux_cmd_parser_insert_sys_command(float delay, delay_cmd_t *cmd)
{
    TuxDrvError ret;

#ifdef USE_MUTEX
    mutex_lock(__stack_mutex);
#endif

    ret = insert_command(delay, cmd, &sys_cmd_stack);

#ifdef USE_MUTEX
    mutex_unlock(__stack_mutex);
#endif
    return ret;
}

/**
 * \brief Insert a command in the user stack.
 * \param delay Delay before the execution of the command.
 * \param cmd Command to execute.
 */
LIBLOCAL TuxDrvError
tux_cmd_parser_insert_user_command(float delay, const char *cmd_str)
{
    TuxDrvError ret;
    delay_cmd_t cmd;

#ifdef USE_MUTEX
    mutex_lock(__stack_mutex);
#endif
    ret = parse_command(cmd_str, &cmd);
    if (ret == E_TUXDRV_NOERROR)
    {
        ret = insert_command(delay, &cmd, &user_cmd_stack);
    }

#ifdef USE_MUTEX
    mutex_unlock(__stack_mutex);
#endif
    return ret;
}

/**
 * \brief Clear the delayed commands from the system stack.
 * \return The result success.
 */
LIBLOCAL bool
tux_cmd_parser_clear_delay_commands(void)
{
    int i;

#ifdef USE_MUTEX
    mutex_lock(__stack_mutex);
#endif

    /* Clear user cmd */
    memset(&user_cmd_stack, 0, sizeof(cmd_stack_t));

    /* process all pending system commands */
    for (i = 0; i < NRCMDS; i++)
    {
        if (sys_cmd_stack.cmd_list[i].command_group != NO_CMD)
        {
            execute_command(&sys_cmd_stack.cmd_list[i]);
            /* no need to execute the following command as we are going
               to clear the complete stack after this for loop
               memset(&sys_cmd_stack.cmd_list[i], 0, sizeof(delay_cmd_t));
            */
        }
    }

    /* Clear system cmd */
    memset(&sys_cmd_stack, 0, sizeof(cmd_stack_t));

#ifdef USE_MUTEX
    mutex_unlock(__stack_mutex);
#endif

    return true;
}

/**
 * \brief Execute the expired commands from the stacks.
 */
LIBLOCAL void
tux_cmd_parser_delay_stack_perform(void)
{
    int i;
    double curtime = get_time();

#ifdef USE_MUTEX
    mutex_lock(__stack_mutex);
#endif

    for (i = 0; i < NRCMDS; i++)
    {
        if (user_cmd_stack.cmd_list[i].command_group != NO_CMD)
        {
            if (curtime >= user_cmd_stack.cmd_list[i].timeout)
            {
                execute_command(&user_cmd_stack.cmd_list[i]);
                /* next two commands are faster than a memset
                   writing a null byte to the first char of cmd is sufficient
                   to make it an empty string
                */
                user_cmd_stack.cmd_list[i].timeout = 0;
                user_cmd_stack.cmd_list[i].command_group = NO_CMD;
            }
        }
        if (sys_cmd_stack.cmd_list[i].command_group != NO_CMD)
        {
            if (curtime >= sys_cmd_stack.cmd_list[i].timeout)
            {
                execute_command(&sys_cmd_stack.cmd_list[i]);
                sys_cmd_stack.cmd_list[i].timeout = 0;
                sys_cmd_stack.cmd_list[i].command_group = NO_CMD;
            }
        }
    }

#ifdef USE_MUTEX
    mutex_unlock(__stack_mutex);
#endif
}

/**
 * \brief Parse a command line string.
 * \param line_str Line to parse.
 * \return The error result (always E_TUXDRV_NOERROR).
 */
static TuxDrvError
parse_line(const char *line_str)
{
    float delay= 0.0;
    char cmd_str[CMDSIZE] = "";
    int i;

    i = sscanf(line_str, "%f:%[^\n]", &delay, cmd_str);

    if (i == 2)
    {
        return tux_cmd_parser_insert_user_command(delay, cmd_str);
    }

    return E_TUXDRV_NOERROR;
}


/**
 * \brief Parse a macro string of commands.
 * \param macro_str Macro string.
 * \return The success result.
 */
LIBLOCAL TuxDrvError
tux_cmd_parser_parse_macro(const char *macro_str)
{
    const char lex_ret[] = "\n";
    char *line_tmp;
    char macro[MACROSIZE];
    TuxDrvError ret = E_TUXDRV_NOERROR;

#ifdef USE_MUTEX
    mutex_lock(__macro_mutex);
#endif

    /*
        strtok is used, this modifies the string, hence the copy
        if it is ok to write 0 bytes into the argument no copy is needed
    */
    strcpy(macro, macro_str);
    if ((line_tmp = strtok(macro, lex_ret)) != NULL)
    {
        ret = parse_line(line_tmp);
        if (ret != E_TUXDRV_NOERROR)
        {
            if (ret != E_TUXDRV_INVALIDCOMMAND)
            {
#ifdef USE_MUTEX
                mutex_unlock(__macro_mutex);
#endif
                return(ret);
            }
        }

        while ((line_tmp = strtok(NULL, lex_ret)) != NULL)
        {
            ret = parse_line(line_tmp);
            if (ret != E_TUXDRV_NOERROR)
            {
                if (ret != E_TUXDRV_INVALIDCOMMAND)
                {
#ifdef USE_MUTEX
                    mutex_unlock(__macro_mutex);
#endif
                    return(ret);
                }
            }
        }
    }

#ifdef USE_MUTEX
    mutex_unlock(__macro_mutex);
#endif

    return ret;
}

/**
 * \brief Parse a macro file of commands.
 * \param file_path Macro file path.
 * \return The success result.
 */
LIBLOCAL TuxDrvError
tux_cmd_parser_parse_file(const char *file_path)
{
    char line[CMDSIZE] = "";
    FILE *macro_file;
    TuxDrvError ret = E_TUXDRV_NOERROR;

#ifdef USE_MUTEX
    mutex_lock(__macro_mutex);
#endif
    macro_file = fopen(file_path, "r");

    if (macro_file)
    {
        while (fgets(line, sizeof(line)-2, macro_file) != NULL)
        {
            ret = parse_line(line);
            if (ret != E_TUXDRV_NOERROR)
            {
                if (ret != E_TUXDRV_INVALIDCOMMAND)
                {
#ifdef USE_MUTEX
                    mutex_unlock(__macro_mutex);
#endif
                    return(ret);
                }
            }
        }
        fclose(macro_file);
    }
    else
    {
        ret = E_TUXDRV_FILEERROR;
    }

#ifdef USE_MUTEX
    mutex_unlock(__macro_mutex);
#endif

    return ret;
}

/**
 * \brief Parse a command string.
 * \param cmd_str Command string.
 * \return The result success.
 */
LIBLOCAL TuxDrvError
tux_cmd_parser_parse_command(const char *cmd_str)
{
    TuxDrvError ret;
    delay_cmd_t cmd;

    /* If the parser is not enabled then fail */
    if (!cmd_parser_enable)
    {
        return E_TUXDRV_PARSERISDISABLED;
    }
    ret = parse_command(cmd_str, &cmd);
    if (ret == E_TUXDRV_NOERROR)
    {
        execute_command(&cmd);
    }
    return ret;
}
