/*
 * Tux Droid - Error
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
 * \file tux_error.h
 * \brief Error header.
 * \author remi.jocaille@c2me.be
 * \ingroup error
 */

#ifndef _TUX_ERROR_H_
#define _TUX_ERROR_H_

/** \brief Errors code begin index */
#define TUX_ERROR_BEGIN                 256

/** \brief TuxDrvError type */
typedef int TuxDrvError;
/** \brief Error codes enumeration */
typedef enum
{
    E_TUXDRV_NOERROR = 0, /**< No error */
    E_TUXDRV_PARSERISDISABLED = TUX_ERROR_BEGIN, /**< The parser is disabled*/
    E_TUXDRV_INVALIDCOMMAND, /**< The command is invalid */
    E_TUXDRV_STACKOVERFLOW, /**< Command stack overflow */
    E_TUXDRV_FILEERROR, /**< File IO error */
    E_TUXDRV_BADWAVFILE, /**< Bad wave file */
    E_TUXDRV_INVALIDIDENTIFIER, /**< Invalid status identifier */
    E_TUXDRV_INVALIDNAME, /**< Invalid status name */
    E_TUXDRV_INVALIDPARAMETER, /**< Invalid command parameter */
    E_TUXDRV_BUSY, /**< Tuxdriver is busy */
    E_TUXDRV_WAVSIZEEXCEDED, /**< Wave size exceded (for sound flash) */
} tux_drv_error_t;

extern const char *tux_error_strerror(TuxDrvError error_code);

#endif /* _TUX_ERROR_H_ */

