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
 * \file tux_error.c
 * \brief Error functions.
 * \author remi.jocaille@c2me.be
 * \ingroup error
 */

#include "tux_misc.h"
#include "tux_error.h"

/**
 * \brief Get an explicite message from an error code.
 * \param error_code Error code.
 * \return The explicite message.
 */
LIBLOCAL const char *
tux_error_strerror(TuxDrvError error_code)
{
    switch (error_code) {
    case E_TUXDRV_NOERROR:
        return "No error";
    case E_TUXDRV_PARSERISDISABLED:
        return "The parser of command is disabled";
    case E_TUXDRV_INVALIDCOMMAND:
        return "Invalid command";
    case E_TUXDRV_STACKOVERFLOW:
        return "The stack of commands is full";
    case E_TUXDRV_FILEERROR:
        return "File error";
    case E_TUXDRV_BADWAVFILE:
        return "Wave file error";
    case E_TUXDRV_INVALIDIDENTIFIER:
        return "The identifier is unknow";
    case E_TUXDRV_INVALIDNAME:
        return "The name is unknow";
    case E_TUXDRV_INVALIDPARAMETER:
        return "One or more parameters are invalid";
    case E_TUXDRV_BUSY:
        return "The system is busy";
    case E_TUXDRV_WAVSIZEEXCEDED:
        return "The size of the selection exceeds 127 blocks";
    default:
        return "Unknow error";
    }
}
