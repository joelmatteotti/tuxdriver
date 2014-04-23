/*
 * Tux Droid - Leds
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

#ifndef _TUX_LEDS_H_
#define _TUX_LEDS_H_

#include <stdbool.h>

/*
 * Structure definitions, constants and custom types.
 */

/**
 * Type indicating which led should be affected by the command.
 * The left LED is affected to bit0 and the right LED is at bit1. This
 * simplifies comparisons. Assigning an hex value helps keep in mind the bit
 * relation.
 */
typedef enum
{
    LED_NONE = 0,
    LED_LEFT = 0x01,
    LED_RIGHT = 0x02,
    LED_BOTH = 0x03,
} leds_t;

/** Types of effects applied when changing the intensity of the LEDs. */
typedef enum
{
    UNAFFECTED,     /**< Don't update the effect parameters. This can either be
                      the last effect set by software, or by firmware in the
                      autonomous mode. This is probably not what you want. */
    LAST,           /**< Last effect requested by software. */
    NONE,           /**< Don't use effects, equivalent to on/off mode. */
    DEFAULT,        /**< Default effect which is a short fading effect. */
    FADE_DURATION,  /**< Fading effect, 'effect.speed' sets the duration (in
                      seconds) the effect will last. */
    FADE_RATE,      /**< Fading effect, 'effect.speed' sets the rate of the
                      effect. Its value represents the number of seconds it
                      takes to apply the effect from off to on. So the actual
                      effect duration will take less time than specified if the
                      intensity starts or ends at intermediate values.
                      Therefore this parameter guarantees a constant rate of
                      the effect, not the duration.
                      */
    GRADIENT_NBR,   /**< Gradient effect, the intensity changes gradually by a
                      number of steps given by 'effect.step'. 'effect.speed'
                      represents the number of seconds it should take to apply
                      the effect. */
    GRADIENT_DELTA, /**< Gradient effect, the intensity changes by a delta
                      value of 'effect.step'. 'effect.speed' represents the
                      number of seconds it should take to apply the effect. */
} effect_type_t;

/** Fading or gradient effect. This structure holds the type of effect and the
 * corresponding parameters.
 * \sa The effect types and parameters are described in the documentation of
 * effect_type_t. */
typedef struct
{
    effect_type_t type;         /**< Type of effect. */
    float speed;                /**< Speed of the effect, used in both
                                  gradients and fading effects. */
    int step;                   /**< Intensity step of the gradient effect. Not
                                  used for the fading effect. */
} led_effect_t;

extern void tux_leds_update_left(void);
extern void tux_leds_update_right(void);
extern bool tux_leds_cmd_set(leds_t leds, float intensity,
        unsigned char effect_type, float effect_speed,
        unsigned char effect_step);
extern bool tux_leds_cmd_pulse(leds_t leds, float min_intensity,
        float max_intensity, unsigned char pulse_count, float pulse_period,
        unsigned char effect_type, float effect_speed,
        unsigned char effect_step);

#endif /* _TUX_LEDS_H_ */
