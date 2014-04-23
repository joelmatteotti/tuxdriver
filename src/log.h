/*
 * Tux Droid - Log
 * Copyright (C) 2007 C2ME S.A. <tuxdroid@c2me.be>
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
 * \file log.h
 * \brief Logger header.
 * \ingroup logger
 */

#ifndef __LOG_H__
#define __LOG_H__

#include <stdbool.h>

/** \brief Logging target. */
typedef enum log_target
{
    LOG_TARGET_TUX, /**< Target file */
    LOG_TARGET_SHELL /**< Target shell */
} log_target_t;

extern bool log_open(log_target_t target);
extern void log_close(void);

/** \brief Logging levels, in increasing priorities */
typedef enum log_level
{
    LOG_LEVEL_DEBUG, /**< Level Debug */
    LOG_LEVEL_INFO, /**< Level Info */
    LOG_LEVEL_WARNING, /**< Level Warning */
    LOG_LEVEL_ERROR, /**< Level Error */
    LOG_LEVEL_NONE /**< Level None */
} log_level_t;

extern void log_set_level(log_level_t new_level);
extern log_level_t log_get_level(void);

extern bool log_text(log_level_t at_level, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));

#define log_debug(fmt, ...)  log_text(LOG_LEVEL_DEBUG, (fmt), ## __VA_ARGS__)
#define log_info(fmt, ...)  log_text(LOG_LEVEL_INFO, (fmt), ## __VA_ARGS__)
#define log_warning(fmt, ...)  log_text(LOG_LEVEL_WARNING, (fmt), ## __VA_ARGS__)
#define log_error(fmt, ...)  log_text(LOG_LEVEL_ERROR, (fmt), ## __VA_ARGS__)

#endif /* __LOG_H__ */
