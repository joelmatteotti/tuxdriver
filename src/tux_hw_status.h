/*
 * Tux Droid - Low level status
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

#ifndef _TUX_HW_STATUS_H_
#define _TUX_HW_STATUS_H_

#define FRAME_HEADER_PORTS              0xC0
#define FRAME_HEADER_SENSORS1           0xC1
#define FRAME_HEADER_LIGHT              0xC2
#define FRAME_HEADER_POSITION1          0xC3
#define FRAME_HEADER_POSITION2          0xC4
#define FRAME_HEADER_IR                 0xC5
#define FRAME_HEADER_ID                 0xC6
#define FRAME_HEADER_BATTERY            0xC7
#define FRAME_HEADER_VERSION            0xC8
#define FRAME_HEADER_REVISION           0xC9
#define FRAME_HEADER_AUTHOR             0xCA
#define FRAME_HEADER_SOUND_VAR          0xCB
#define FRAME_HEADER_AUDIO              0xCC
#define FRAME_HEADER_FLASH_PROG         0xCD
#define FRAME_HEADER_LED                0xCE
#define FRAME_HEADER_PONG               0xFF

typedef enum
{
    ID_FRAME_HEADER_PORTS = 0,
    ID_FRAME_HEADER_SENSORS1 = 1,
    ID_FRAME_HEADER_LIGHT = 2,
    ID_FRAME_HEADER_POSITION1 = 3,
    ID_FRAME_HEADER_POSITION2 = 4,
    ID_FRAME_HEADER_IR = 5,
    ID_FRAME_HEADER_ID = 6,
    ID_FRAME_HEADER_BATTERY = 7,
    ID_FRAME_HEADER_VERSION = 8,
    ID_FRAME_HEADER_REVISION = 9,
    ID_FRAME_HEADER_AUTHOR = 10,
    ID_FRAME_HEADER_SOUND_VAR = 11,
    ID_FRAME_HEADER_AUDIO = 12,
    ID_FRAME_HEADER_FLASH_PROG = 13,
    ID_FRAME_HEADER_LED = 14,
    ID_FRAME_HEADER_PONG = 15,
} headers_id_t;

typedef unsigned char _BIT_;

typedef struct
{
    _BIT_ flippers_motor_backward:1;
    _BIT_ spin_motor_forward:1;
    _BIT_ spin_motor_backward:1;
    _BIT_ mouth_open_switch:1;
    _BIT_ mouth_closed_switch:1;
    _BIT_ head_push_switch:1;
    _BIT_ charger_inhibit_signal:1;
    _BIT_ external_io:1;
} _PORTB_BITS_;

typedef union
{
    unsigned char   Byte;
    _PORTB_BITS_    bits;
} _PORTB_BYTE_;

typedef struct
{
    _BIT_ photo_transistor_pull_up:1;
    _BIT_ flippers_position_switch:1;
    _BIT_ right_blue_led:1;
    _BIT_ left_blue_led:1;
    _BIT_ i2c_sda_line:1;
    _BIT_ i2c_scl_line:1;
    _BIT_ reset:1;
    _BIT_ ndef:1;
} _PORTC_BITS_;

typedef union
{
    unsigned char   Byte;
    _PORTC_BITS_    bits;
} _PORTC_BYTE_;

typedef struct
{
    _BIT_ head_motor_for_mouth:1;
    _BIT_ head_motor_for_eyes:1;
    _BIT_ ir_receiver_signal:1;
    _BIT_ spin_position_switch:1;
    _BIT_ flippers_motor_forward:1;
    _BIT_ ir_led:1;
    _BIT_ eyes_open_switch:1;
    _BIT_ eyes_closed_switch:1;
} _PORTD_BITS_;

typedef union
{
    unsigned char   Byte;
    _PORTD_BITS_    bits;
} _PORTD_BYTE_;

typedef struct
{
    _BIT_ left_wing_push_button:1;
    _BIT_ right_wing_push_button:1;
    _BIT_ power_plug_insertion_switch:1;
    _BIT_ head_push_button:1;
    _BIT_ charger_led_status:1;
    _BIT_ rf_connection_status:1;
    _BIT_ internal_power_switch:1;
    _BIT_ mute_status:1;
} _SENSORS_BITS_;

typedef union
{
    unsigned char   Byte;
    _SENSORS_BITS_    bits;
} _SENSORS_BYTE_;

typedef struct
{
    _BIT_ command:6;
    _BIT_ toggle:1;
    _BIT_ received_flag:1;
} _RC5_BITS_;

typedef union
{
    unsigned char Byte;
    _RC5_BITS_   bits;
} _RC5_BYTE_;

typedef struct
{
    _BIT_ cpu_number:3;
    _BIT_ major:5;
} _VERSION_FIRST_BITS_;

typedef union
{
    unsigned char           Byte;
    _VERSION_FIRST_BITS_    bits;
} _VERSION_FIRST_BYTE_;

typedef struct
{
    _BIT_ local_modification:1;
    _BIT_ mixed_update:1;
    _BIT_ original_release:1;
    _BIT_ ndef:5;
} _REVISION_THIRD_BITS_;

typedef union
{
    unsigned char           Byte;
    _REVISION_THIRD_BITS_   bits;
} _REVISION_THIRD_BYTE_;

typedef struct
{
    _BIT_ no_programming:1;
    _BIT_ flash_erased:1;
    _BIT_ toc:1;
    _BIT_ sounds_track:5;

} _AUDIO_BITS_;

typedef union
{
    unsigned char           Byte;
    _AUDIO_BITS_   bits;
} _AUDIO_BYTE_;

typedef struct
{
    _BIT_ left_led_fading:1;
    _BIT_ left_led_pulsing:1;
    _BIT_ right_led_fading:1;
    _BIT_ right_led_pulsing:1;
    _BIT_ led_mask:1;
    _BIT_ ndef:3;
} _LED_EFFECT_STATUS_BITS_;

typedef union
{
    unsigned char              Byte;
    _LED_EFFECT_STATUS_BITS_   bits;
} _LED_EFFECT_STATUS_BYTE_;

typedef struct
{
    _BIT_ spin_right_on:1;
    _BIT_ spin_left_on:1;
    _BIT_ eyes_on:1;
    _BIT_ mouth_on:1;
    _BIT_ flippers_on:1;
    _BIT_ ndef:3;
} _MOTORS_STATUS_BITS_;

typedef union
{
    unsigned char              Byte;
    _MOTORS_STATUS_BITS_       bits;
} _MOTORS_STATUS_BYTE_;

typedef struct
{
    _PORTB_BYTE_ portb;
    _PORTC_BYTE_ portc;
    _PORTD_BYTE_ portd;
} frame_body_ports_t;

typedef struct
{
    _SENSORS_BYTE_  sensors;
    unsigned char   play_internal_sound;
    unsigned char   play_general_sound;
} frame_body_sensors1_t;

typedef struct
{
    unsigned char   high_level;
    unsigned char   low_level;
    unsigned char   mode;
} frame_body_light_t;

typedef struct
{
    unsigned char   eyes_remaining_mvm;
    unsigned char   mouth_remaining_mvm;
    unsigned char   flippers_remaining_mvm;
} frame_body_position1_t;

typedef struct
{
    unsigned char           spin_remaining_mvm;
    unsigned char           flippers_down;
    _MOTORS_STATUS_BYTE_    motors;
} frame_body_position2_t;

typedef struct
{
    _RC5_BYTE_     rc5_code;
    /*unsigned char   ??;                       NDEF */
    /*unsigned char   ??;                       NDEF */
} frame_body_ir_t;

typedef struct
{
    unsigned char   msb_number;
    unsigned char   lsb_number;
    /*unsigned char   ??;                       NDEF */
} frame_body_id_t;

typedef struct
{
    unsigned char   high_level;
    unsigned char   low_level;
    unsigned char   motors_state;
} frame_body_battery_t;

typedef struct
{
    _VERSION_FIRST_BYTE_     cm;
    unsigned char            minor;
    unsigned char            update;
} frame_body_version_t;

typedef struct
{
    unsigned char            lsb_number;
    unsigned char            msb_number;
    _REVISION_THIRD_BYTE_    release_type;
} frame_body_revision_t;

typedef struct
{
    unsigned char   lsb_id;
    unsigned char   msb_id;
    unsigned char   variation_number;
} frame_body_author_t;

typedef struct
{
    unsigned char   sound_track_played;
    _AUDIO_BYTE_    programming_steps;
    unsigned char   programmed_sound_track;
} frame_body_audio_t;

typedef struct
{
    unsigned char   number_of_sounds;
    unsigned char   flash_usage;
    /*unsigned char   ??;                       NDEF */
} frame_body_sound_var_t;

typedef struct
{
    unsigned char   current_state;
    unsigned char   last_sound_size;
    /*unsigned char   ??;                       NDEF */
} frame_body_flash_prog_t;

typedef struct
{
    unsigned char               left_led_intensity;
    unsigned char               right_led_intensity;
    _LED_EFFECT_STATUS_BYTE_    effect_status;
} frame_body_led_t;

typedef struct
{
    unsigned char               pongs_pending_number;
    unsigned char               pongs_lost_by_i2c_number;
    unsigned char               pongs_lost_by_rf_number;
} frame_body_pong_t;

typedef struct
{
    frame_body_ports_t          ports;
    frame_body_sensors1_t       sensors1;
    frame_body_light_t          light;
    frame_body_position1_t      position1;
    frame_body_position2_t      position2;
    frame_body_ir_t             ir;
    frame_body_id_t             id;
    frame_body_battery_t        battery;
    frame_body_version_t        version;
    frame_body_revision_t       revision;
    frame_body_author_t         author;
    frame_body_audio_t          audio;
    frame_body_sound_var_t      sound_var;
    frame_body_flash_prog_t     flash_prog;
    frame_body_led_t            led;
    frame_body_pong_t           pong;
} hw_status_table_t;

extern hw_status_table_t hw_status_table;
extern unsigned char tux_hw_status_header_counter[16];

extern void tux_hw_status_init(void);
extern int tux_hw_status_parse_frame(const unsigned char *frame);
extern char *tux_hw_status_id_to_str(const unsigned char id);
extern void tux_hw_status_header_counter_check(void);

extern void tux_hw_parse_body_version(const unsigned char *frame);
extern void tux_hw_parse_body_revision(const unsigned char *frame);
extern void tux_hw_parse_body_author(const unsigned char *frame);

#endif /* _TUX_HW_STATUS_H_ */
