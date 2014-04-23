/*
 * Tux Droid - ID
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

#ifndef _TUX_ID_H_
#define _TUX_ID_H_

typedef struct
{
    unsigned int number;
} id_descriptor_t;

extern id_descriptor_t id_desc;

extern void tux_id_init_descriptor(void);
extern char *tux_id_dump_descriptor(char *descriptor);
extern void tux_id_update_number(void);
extern void tux_id_get_descriptor(void);
extern bool tux_id_cmd_disconnect_from_tux(void);

#endif /* _TUX_ID_H_ */
