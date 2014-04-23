/*
 * Tux Droid - tux types
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

#ifndef _TUX_TYPES_H_
#define _TUX_TYPES_H_

#include "tux_leds.h"
#include "tux_movements.h"

typedef unsigned char data_frame[4];
typedef unsigned char raw_frame[5];

#define CMDSIZE 1024
#define MACROSIZE 16384

/* command groups */
typedef enum {
    NO_CMD = 0,
    TUX_CMD,
    RAW_CMD
} tux_command_group_t;

/* commands */
typedef enum {
    AUDIO,
    EYES,
    IR,
    LED,
    MOUTH,
    SOUND_FLASH,
    SPINNING,
    FLIPPERS
} tux_command_t;

/* subcommands */
typedef enum {
    BLINK,
    CHANNEL_GENERAL,
    CHANNEL_TTS,
    CLOSE,
    DOWN,
    LEFT_ON,
    LEFT_ON_DURING,
    MUTE,
    OFF,
    ON,
    ON_DURING,
    OPEN,
    PLAY,
    PULSE,
    RIGHT_ON,
    RIGHT_ON_DURING,
    SEND,
    SET,
    SPEED,
    UP
} tux_sub_command_t;

/*
   The data structures for the invididual commands.
   It was decided to make separate structures for each subcommand
   (instead of e.g. clubbing all parameters together)
   Rationale:
     separate structures make it more straigthforward to add a new subcommand.
     separate structures make it simpler to create a C api.
     depending on the compiler this could also result in stricter type
        checking
   For now the types are not used to communicate between parser and
   individual functions.
   Note that storage waste by duplicate data fields is recouped
     as all data is in a union
*/

/* audio */
typedef struct {
    bool muteflag;
} audio_mute_parameters_t;

/* eyes */
typedef struct {
    move_final_state_t state;
    unsigned char nr_movements;
} eyes_on_parameters_t;

typedef struct {
    move_final_state_t state;
    float duration;
} eyes_on_during_parameters_t;

/* ir */
typedef struct {
    unsigned char address;
    unsigned char command;
} ir_send_parameters_t;

/* leds */
typedef struct {
    leds_t leds;
    float intensity;
} led_on_parameters_t;

typedef struct {
    leds_t leds;
} led_off_parameters_t;

typedef struct {
    leds_t leds;
    float min_intensity;
    float max_intensity;
    unsigned char pulse_count;
    float pulse_period;
    effect_type_t effect_type;
    float effect_speed;
    unsigned char effect_step;
} led_pulse_parameters_t;

typedef struct {
    leds_t leds;
    unsigned char pulse_count;
    float pulse_period;
} led_blink_parameters_t;

typedef struct {
    leds_t leds;
    float intensity;
    effect_type_t effect_type;
    float effect_speed;
    unsigned char effect_step;
} led_set_parameters_t;

/* mouth */
typedef struct {
    move_final_state_t state;
    unsigned char nr_movements;
} mouth_on_parameters_t;

typedef struct {
    move_final_state_t state;
    float duration;
} mouth_on_during_parameters_t;

/* sound flash */
typedef struct {
    unsigned char track;
    float volume;
} sound_flash_play_parameters_t;

/* spinning */
typedef struct {
    unsigned char nr_qturns;
} spinning_on_parameters_t;

typedef struct {
    float duration;
} spinning_on_during_parameters_t;

typedef struct {
    unsigned char speed;
} spinning_speed_parameters_t;

/* flippers */
typedef struct {
    move_final_state_t state;
    unsigned char nr_movements;
} flippers_on_parameters_t;

typedef struct {
    move_final_state_t state;
    float duration;
} flippers_on_during_parameters_t;

typedef struct {
    unsigned char speed;
} flippers_speed_parameters_t;

/* wifi avoidance */
typedef struct {
    unsigned char channel;
} wifi_avoid_channel_parameters_t;

/* raw */
typedef struct {
    unsigned char raw[5];
} raw_parameters_t;

/*
   this is the struct which contains all commands and arguments
   the union is there to save storage
   based upon command/sub_command we exactly know which union field we need
*/
typedef struct {
    double timeout;
    tux_command_group_t command_group;
    tux_command_t command;
    tux_sub_command_t sub_command;
    union {
        audio_mute_parameters_t         audio_mute_parameters;
        eyes_on_parameters_t            eyes_on_parameters;
        eyes_on_during_parameters_t     eyes_on_during_parameters;
        ir_send_parameters_t            ir_send_parameters;
        led_on_parameters_t             led_on_parameters;
        led_off_parameters_t            led_off_parameters;
        led_pulse_parameters_t          led_pulse_parameters;
        led_blink_parameters_t          led_blink_parameters;
        led_set_parameters_t            led_set_parameters;
        mouth_on_parameters_t           mouth_on_parameters;
        mouth_on_during_parameters_t    mouth_on_during_parameters;
        sound_flash_play_parameters_t   sound_flash_play_parameters;
        spinning_on_parameters_t        spinning_on_parameters;
        spinning_on_during_parameters_t spinning_on_during_parameters;
        spinning_speed_parameters_t     spinning_speed_parameters;
        flippers_on_parameters_t        flippers_on_parameters;
        flippers_on_during_parameters_t flippers_on_during_parameters;
        flippers_speed_parameters_t     flippers_speed_parameters;
        wifi_avoid_channel_parameters_t wifi_avoid_channel_parameters;
        raw_parameters_t                raw_parameters;
    };
    float inserted_at_time;
} delay_cmd_t;

#endif /* _TUX_TYPES_H_ */
