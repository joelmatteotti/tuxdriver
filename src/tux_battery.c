/*
 * Tux Droid - Battery
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
 * \file tux_battery.c
 * \brief Battery level functions.
 * \author remi.jocaille@c2me.be
 * \ingroup power
 */

#include <string.h>

#include "tux_battery.h"
#include "tux_misc.h"
#include "tux_hw_status.h"
#include "tux_sw_status.h"

/** \brief Battery states */
typedef enum
{
    FULL, /**< Battery level full */
    HIGH, /**< Battery level high */
    LOW, /**< Battery level low */
    EMPTY /**< Battery level empty */
} battery_state_t;

/**
 * REPORTING_DELTA is the treshold value for changes;
 * changes smaller than this value are not reported
 * might become a controllable parameter in the future
 */
#define REPORTING_DELTA 100

/** Current battery state */
static battery_state_t battery_state = EMPTY;
/** Current battery state for event */
static int last_level_for_event = 0;

/**
 * \brief Update the status of the battery voltage.
 */
LIBLOCAL void
tux_battery_update_level(void)
{
    int new_level = 0;
    int adc_value;
    char *new_state_str = "";
    int delta;

    /* Get the current battery level from adc and convert it to mV */
    adc_value = (hw_status_table.battery.high_level << 8);
    adc_value += hw_status_table.battery.low_level;
    new_level = adc_value * 7.467;
    /* 7.467 = 0.00322 * 2.319 * 1000 no idea where the first two are from
     * 1000 is to go to mV */

    /* Get the difference between last and new level */
    delta = new_level - last_level_for_event;

    /* If the delta threshold is reached */
    if ((delta > REPORTING_DELTA) || (delta < -REPORTING_DELTA))
    {
        last_level_for_event = new_level;

        /* Update directly the SW_ID_BATTERY_LEVEL status if motors off */
        if (!hw_status_table.battery.motors_state)
        {
            tux_sw_status_set_intvalue(SW_ID_BATTERY_LEVEL, new_level, true);
        }

        /* Battery level full */
        if (new_level >= TUX_BATTERY_FULL_VALUE)
        {
            new_state_str = STRING_VALUE_FULL;
            battery_state = FULL;
        }
        else
        {
            /* Battery level high */
            if ((new_level < TUX_BATTERY_FULL_VALUE) &&
                (new_level >= TUX_BATTERY_HIGH_VALUE))
            {
                new_state_str = STRING_VALUE_HIGH;
                battery_state = HIGH;
            }
            else
            {
                /* Battery level low */
                if ((new_level < TUX_BATTERY_HIGH_VALUE) &&
                    (new_level >= TUX_BATTERY_LOW_VALUE))
                {
                    new_state_str = STRING_VALUE_LOW;
                    battery_state = LOW;
                }
                else
                {
                    /* Battery level empty */
                    new_state_str = STRING_VALUE_EMPTY;
                    battery_state = EMPTY;
                }
            }
        }
        /* Update the SW_ID_BATTERY_STATE status */
        tux_sw_status_set_strvalue(SW_ID_BATTERY_STATE, new_state_str, true);
    }
    else
    {
        /* Update the SW_ID_BATTERY_LEVEL status */
        tux_sw_status_set_intvalue(SW_ID_BATTERY_LEVEL, new_level, false);
    }
}
