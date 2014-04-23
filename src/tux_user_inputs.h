/*
 * Tux Droid - User inputs
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

#ifndef _TUX_USER_INPUTS_H_
#define _TUX_USER_INPUTS_H_

#include <stdbool.h>

#define RC5_TIMEOUT         2

#define K_0                 0x00
#define K_1                 0x01
#define K_2                 0x02
#define K_3                 0x03
#define K_4                 0x04
#define K_5                 0x05
#define K_6                 0x06
#define K_7                 0x07
#define K_8                 0x08
#define K_9                 0x09
#define K_STANDBY           0X0C
#define K_MUTE              0X0D
#define K_VOLUMEPLUS        0X10
#define K_VOLUMEMINUS       0X11
#define K_ESCAPE            0X12
#define K_YES               0X13
#define K_NO                0X14
#define K_BACKSPACE         0X15
#define K_STARTVOIP         0X16
#define K_RECEIVECALL       0X17
#define K_HANGUP            0X18
#define K_STAR              0X19
#define K_SHARP             0X1A
#define K_RED               0X1B
#define K_GREEN             0X1C
#define K_BLUE              0X1D
#define K_YELLOW            0X1E
#define K_CHANNELPLUS       0X20
#define K_CHANNELMINUS      0X21
#define K_UP                0X22
#define K_DOWN              0X23
#define K_LEFT              0X24
#define K_RIGHT             0X25
#define K_OK                0X26
#define K_FASTREWIND        0X32
#define K_FASTFORWARD       0X34
#define K_PLAYPAUSE         0X35
#define K_STOP              0X36
#define K_RECORDING         0X37
#define K_PREVIOUS          0X38
#define K_NEXT              0X39
#define K_MENU              0X3A
#define K_MOUSE             0X3B
#define K_ALT               0X3C

#define LAST_VALID_K        K_ALT
#define K_DUMMY_RELEASE     -1

extern void tux_user_inputs_init(void);
extern void tux_user_inputs_update_left_wing_button(void);
extern void tux_user_inputs_update_right_wing_button(void);
extern void tux_user_inputs_update_head_button(void);
extern void tux_user_inputs_update_RC5(void);
extern void tux_user_inputs_init_time_RC5(void);
extern void tux_user_inputs_update_charger_state(void);
extern bool tux_user_inputs_cmd_ir_on(void);
extern bool tux_user_inputs_cmd_ir_off(void);
extern bool tux_user_inputs_cmd_ir_send(unsigned char address, unsigned char command);

#endif /* _TUX_USER_INPUTS_H_ */
