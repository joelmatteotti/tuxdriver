/*
 * Tux Droid - Misc
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

#ifndef _TUX_MISC_H_
#define _TUX_MISC_H_

#include <stdbool.h>
#include <stdint.h>

#define FW_MAIN_LOOP_DELAY          0.004

#ifdef WIN32
#   include <windows.h>
#   include <mmsystem.h>
#   define sleep(sec)   Sleep(sec * 1000)
#   define usleep(usec) Sleep(usec / 1000)
#   define LIBEXPORT    __declspec(dllexport)
#   define LIBLOCAL
#else
#   define LIBEXPORT    __attribute__ ((visibility ("default")))
#   define LIBLOCAL     __attribute__ ((visibility ("hidden")))
#endif


/**
 *      Callback function prototype for simple event
 */
typedef void(*simple_callback_t)(void);

extern double get_time(void);
extern bool str_to_uint8(const char *str, unsigned char *dest);
extern bool str_to_int8(const char *str, char *dest);
extern bool str_to_int(const char *str, int *dest);
extern bool str_to_bool(const char *str, bool *dest);
extern bool str_to_float(const char *str, float *dest);
extern bool hex_to_uint8(const char *str, unsigned char *dest);

#endif /* _TUX_MISC_H_ */
