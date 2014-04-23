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
 * \file tux_cmd_parser.h
 * \brief Command parser header.
 * \author remi.jocaille@c2me.be
 * \ingroup command_parser
 */

#ifndef _TUX_CMD_PARSER_H_
#define _TUX_CMD_PARSER_H_

#include <stdbool.h>

#include "tux_error.h"
#include "tux_types.h"

/** \brief Maximal size of a token */
#define TOKENSIZE 1024
/** \brief Maximal tokens count in a command */
#define MAXNRTOKENS 256

/** \brief Token string */
typedef char token_str_t[TOKENSIZE];
/** \brief Token string array */
typedef token_str_t tokens_t[MAXNRTOKENS];

extern void tux_cmd_parser_init(void);
extern void tux_cmd_parser_set_enable(bool value);
extern int tux_cmd_parser_get_tokens(const char *src_str, tokens_t *toks,
    int max_tokens, const char *delimiters);
extern TuxDrvError tux_cmd_parser_parse_command(const char *cmd_str);
extern bool tux_cmd_parser_clear_delay_commands(void);
extern TuxDrvError tux_cmd_parser_insert_sys_command(float delay,
    delay_cmd_t *cmd);
extern TuxDrvError tux_cmd_parser_insert_user_command(float delay,
    const char *cmd_str);
extern void tux_cmd_parser_clean_sys_command(tux_command_t command);
extern void tux_cmd_parser_delay_stack_perform(void);
extern TuxDrvError tux_cmd_parser_parse_macro(const char *macro_str);
extern TuxDrvError tux_cmd_parser_parse_file(const char *file_path);

#endif /* _TUX_CMD_PARSER_H_ */
