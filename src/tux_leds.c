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

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "tux_hw_cmd.h"
#include "tux_hw_status.h"
#include "tux_leds.h"
#include "tux_sw_status.h"
#include "tux_types.h"
#include "tux_usb.h"

typedef enum
{
    LED_ON,
    LED_OFF,
    LED_CHANGING,
} led_state_t;

static led_state_t left_led_state = LED_OFF;
static led_state_t right_led_state = LED_OFF;

static bool led_set(leds_t leds, int intensity, led_effect_t *effect);
static bool led_pulse(leds_t leds, int min_intensity, int max_intensity,
    int toggle_count, float pulse_period, led_effect_t *effect);
static int led_configure_effects(leds_t leds, int left_intensity_delta,
    int right_intensity_delta, led_effect_t *effect);

/**
 *
 */
static int
bound_to_range(int val, int min, int max)
{
    if (val < min)
    {
        return(min);
    }
    if (val > max)
    {
        return(max);
    }
    return(val);
}

/**
 *
 */
LIBLOCAL void
tux_leds_update_left(void)
{
    char *new_left_state = "";

    /* Get on / off state */
    if (hw_status_table.led.left_led_intensity < 50)
    {
        left_led_state = LED_OFF;
    }
    else
    {
        left_led_state = LED_ON;
    }

    /* Get changing state */
    if (hw_status_table.led.effect_status.bits.left_led_fading ||
        hw_status_table.led.effect_status.bits.left_led_pulsing)
    {
        left_led_state = LED_CHANGING;
    }

    /* State to string */
    switch (left_led_state) {
    case LED_ON:
        new_left_state = STRING_VALUE_ON;
        break;
    case LED_OFF:
        new_left_state = STRING_VALUE_OFF;
        break;
    case LED_CHANGING:
        new_left_state = STRING_VALUE_CHANGING;
        break;
    }

    tux_sw_status_set_strvalue(SW_ID_LEFT_LED_STATE, new_left_state, true);
}

/**
 *
 */
LIBLOCAL void
tux_leds_update_right(void)
{
    char *new_right_state = "";

    /* Get on / off state */
    if (hw_status_table.led.right_led_intensity < 50)
    {
        right_led_state = LED_OFF;
    }
    else
    {
        right_led_state = LED_ON;
    }

    /* Get changing state */
    if (hw_status_table.led.effect_status.bits.right_led_fading ||
        hw_status_table.led.effect_status.bits.right_led_pulsing)
    {
        right_led_state = LED_CHANGING;
    }

    /* State to string */
    switch (right_led_state) {
    case LED_ON:
        new_right_state = STRING_VALUE_ON;
        break;
    case LED_OFF:
        new_right_state = STRING_VALUE_OFF;
        break;
    case LED_CHANGING:
        new_right_state = STRING_VALUE_CHANGING;
        break;
    }

    tux_sw_status_set_strvalue(SW_ID_RIGHT_LED_STATE, new_right_state, true);
}

/** Default settings for the fading effect. */
#define DEFAULT_STEP 1
/** Default settings for the fading effect. */
#define DEFAULT_DELAY 1

/** Values corresponding to the firmware implementation of the fading effect.
 * 'delay' is the delay (4ms time base) before which 'step' is applied. */
static int delay = DEFAULT_DELAY;
static int step = DEFAULT_STEP;

/**
 * \brief Configure the led effect parameters for a fading effect.
 * \param leds Which LEDs should be configured.
 * \param fading_delay Delay (in seconds) between 2 increments of intensity
 * when fading.
 * \return ack of tux command.
 *
 * One problem is that around 1sec (delay = 1, step = 1, 255 steps to fade from
 * off to on), the resolution is quite coarse if we increment step or delay but
 * not both simultaneously. We have approximately 0.25s, 0.5s, 1s, 2s, 3s, etc.
 * when delay or step is increased.  XXX If necessary we can add intermediate
 * values by using 3/4 or 2,3,4/5 or even 2,3,7,9/10 and get a 100ms
 * resolution, but the gradient (steps) effect starts to be noticeable.
 */
static bool
config_fading(leds_t leds, float fading_delay)
{
    int loops = fading_delay / FW_MAIN_LOOP_DELAY;
    data_frame frame = {0, 0, 0, 0};

    /* Can't go infinitely fast. */
    if (loops == 0)
    {
        step = 0xFF;
        delay = 1;
    }
    /* XXX We only handle the firmware function for now,
     * when the delay will be over 255, we should split in 2
     * commands while tracking the time. */
    else
    {
        if (loops > 255)
        {
            step = 1;
            delay = 255;
        }
        /* Faster speed, the delay is set to minimum and we need to
         * increase the step. */
        else
        {
            if (loops < 1)
            {
                delay = 1;
                step = (unsigned char)roundf(1/loops);
            }
            else
            {
                /* (loops >= 1) */
                step = 1;
                delay = (unsigned char)roundf(loops);
            }
        }
    }

    frame[0] = LED_FADE_SPEED_CMD;
    frame[1] = leds;
    frame[2] = delay;
    frame[3] = step;

    return tux_usb_send_to_tux(frame);
}

/**
 * \brief Configure the led effect parameters for a gradient effect.
 * \param leds Which LEDs should be configured.
 * \param delta Intensity increment to apply at each step.
 * \param gradient_delay Delay (in seconds) between 2 increments of intensity.
 * \return ack of tux command.
 */
static int
config_gradient(leds_t leds, int delta, float gradient_delay)
{
    data_frame frame = {0, 0, 0, 0};

    /* Preconditions. */
    delta = bound_to_range(delta, 1, 255);

    delay = (unsigned char)roundf(gradient_delay / FW_MAIN_LOOP_DELAY);
    /* Can't go infinitely fast, so must be > 0. */
    /* Hardware doesn't support longer delays.
     * We should do them with multiple commands if necessary.
     * XXX Not supported for now. */
    delay = bound_to_range(delay, 1, 255);

    frame[0] = LED_FADE_SPEED_CMD;
    frame[1] = leds;
    frame[2] = delay;
    frame[3] = delta;

    return tux_usb_send_to_tux(frame);
}

/**
 * \brief Configure the hardware to the desired LED effect.
 * \param leds Which LEDs are affected by the command
 * \param left_intensity_delta Variation of intensity the left LED will have,
 * necessary to handle timings.
 * \param right_intensity_delta Variation of intensity the right LED will have,
 * necessary to handle timings.
 * \param effect Fading or gradient effect applied when changing the intensity.
 *
 * \sa The effect types and parameters are described in the documentation of
 * effect_type_t.
 */
static int
led_configure_effects(leds_t leds, int left_intensity_delta,
        int right_intensity_delta, led_effect_t *effect)
{
    bool ret = false;
    data_frame frame = {0, 0, 0, 0};

    switch(effect->type)
    {
    case UNAFFECTED:
        /* Just change the intensity in this case. */
        break;
    case LAST:
        /* Don't update the parameters but send them in case the standalone
         * changed them in the meantime. */
        frame[0] = LED_FADE_SPEED_CMD;
        frame[1] = leds;
        frame[2] = delay;
        frame[3] = step;

        ret =tux_usb_send_to_tux(frame);
        break;
    case NONE:
        /* Emulate on/off. */
        step = 0xFF;
        delay = 1;
        frame[0] = LED_FADE_SPEED_CMD;
        frame[1] = leds;
        frame[2] = delay;
        frame[3] = step;

        ret =tux_usb_send_to_tux(frame);
        break;
    case DEFAULT:
        /* Use default settings. */
        step = DEFAULT_STEP;
        delay = DEFAULT_DELAY;
        frame[0] = LED_FADE_SPEED_CMD;
        frame[1] = leds;
        frame[2] = delay;
        frame[3] = step;

        ret =tux_usb_send_to_tux(frame);
        break;
    case FADE_DURATION:
        {
            /* Intensity is changed by 'step' each 'delay'*4ms.
             * So the duration from I1 to I2 is:
             *   duration = abs(I2 - I1) * 4ms * delay / step
             * The fading delay in seconds is
             *   fading_delay = delay * 4ms / step
             *                = duration / abs(I2 - I1)
             *
             * If both leds are not at the same initial level, we can only
             * have the same duration by assigning different parameters to
             * each eye. */
            /* Flag that indicates if nothing needs to be done. */
            bool skip = true;

            /* We should use effect NONE if we don't want fading. */
            if (effect->speed <= 0)
                effect->speed = 0.1;
            /* Don't divide by zero if there's nothing to do ;-). */
            if (leds & LED_LEFT && left_intensity_delta)
            {
                skip = false;
                ret = config_fading(LED_LEFT, effect->speed / \
                                    left_intensity_delta);
            }
            if (leds & LED_RIGHT && right_intensity_delta)
            {
                skip = false;
                ret = config_fading(LED_RIGHT, effect->speed / \
                                    right_intensity_delta);
            }
            if (skip)
            {
                /* Nothing to do then. */
                return ret;
            }
        }
        break;
    case FADE_RATE:
        /* Intensity is changed by 'step' each 'delay'*4ms, and it takes
         * 255 steps to go from off to on.
         * So the duration from off to on is:
         *   duration = 255 * 4ms * delay / step
         * The fading delay in seconds is
         *   fading_delay = delay * 4ms / step
         *                = duration / 255
         * Now the delay should be rounded to the closest int or, if
         * inferior to 1, we should play with the step in order to
         * achieve faster effects.
         */
        ret = config_fading(leds, effect->speed / 255);
        break;
    case GRADIENT_NBR:
        {
            int delta;
            /* Flag indicating if nothing needs to be done. */
            bool skip = true;

            /* Preconditions */
            effect->step = bound_to_range(effect->step, 1, 255);

            /* We should use effect NONE if we don't want gradient. */
            if (effect->speed <= 0)
            {
                effect->speed = 0.1;
            }

            /* Don't divide by zero if there's nothing to do ;-). */
            if (leds & LED_LEFT && left_intensity_delta)
            {
                skip = false;
                delta = (int)roundf(left_intensity_delta / effect->step);
                /* If we can't have as many steps as required. */
                if (delta == 0)
                {
                    delta = 1;
                }
                ret = config_gradient(LED_LEFT, delta,
                                      effect->speed/effect->step);
            }
            if (leds & LED_RIGHT && right_intensity_delta)
            {
                skip = false;
                delta = (int)roundf(right_intensity_delta / effect->step);
                /* If we can't have as many steps as required. */
                if (delta == 0)
                {
                    delta = 1;
                }
                ret = config_gradient(LED_RIGHT, delta,
                                      effect->speed/effect->step);
            }
            if (skip)
            {
                /* Nothing to do then. */
                return ret;
            }
        }
        break;
    case GRADIENT_DELTA:
        {
            float gradient_delay;
            /* Flag indicating if nothing needs to be done. */
            bool skip = true;

            /* Preconditions */
            effect->step = bound_to_range(effect->step, 1, 255);

            /* We should use effect NONE if we don't want gradient. */
            if (effect->speed <= 0)
            {
                effect->speed = 0.1;
            }

            /* Don't divide by zero if there's nothing to do ;-). */
            if (leds & LED_LEFT && left_intensity_delta)
            {
                skip = false;
                gradient_delay = effect->speed * effect->step /
                                 left_intensity_delta;
                ret = config_gradient(LED_LEFT, effect->step, gradient_delay);
            }
            if (leds & LED_RIGHT && right_intensity_delta)
            {
                skip = false;
                gradient_delay = effect->speed * effect->step /
                                 right_intensity_delta;
                ret = config_gradient(LED_RIGHT, effect->step, gradient_delay);
            }
            if (skip)
            {
                /* Nothing to do then. */
                return ret;
            }
        }
        break;
    default:
        /* Not a correct type. */
        ret = false;
        break;
    }
    return ret;
}

/**
 * \brief Set the intensity of the LEDs.
 * \param leds Which LEDs are affected by the command
 * \param intensity Value of the intensity the LEDs should be set to.
 * \param effect Fading or gradient effect applied when changing the intensity.
 *
 * \sa The effect types and parameters are described in the documentation of
 * effect_type_t.
 */
static bool led_set(leds_t leds, int intensity, led_effect_t *effect)
{
    data_frame frame = {0, 0, 0, 0};
    int left_intensity_delta, right_intensity_delta;
    bool ret;

    /* Preconditions */
    if (leds < LED_NONE)
    {
        leds = LED_NONE;
    }
    else
    {
        if (leds > LED_BOTH)
        {
            leds = LED_BOTH;
        }
    }

    intensity = bound_to_range(intensity, 0, 255);

    left_intensity_delta = abs(intensity - hw_status_table.led.left_led_intensity);
    right_intensity_delta = abs(intensity - hw_status_table.led.right_led_intensity);
    ret = led_configure_effects(leds, left_intensity_delta,
                                right_intensity_delta, effect);
    frame[0] = LED_SET_CMD;
    frame[1] = leds;
    frame[2] = intensity;

    ret =tux_usb_send_to_tux(frame);
    return ret;
}

/**
 * \brief Pulse the LEDs.
 * \param leds Which LEDs are affected by the command.
 * \param min_intensity Value of the minimum intensity when pulsing.
 * \param max_intensity Value of the maximum intensity when pulsing.
 * \param toggle_count Number of toggles before pusling stops.
 * \param pulse_period Period between 2 pulses, in seconds.
 * \param effect Fading or gradient effect applied when changing the intensity.
 *
 * \sa The effect types and parameters are described in the documentation of
 * effect_type_t.
 *
 * The effect duration has priority on the pulse period. If you set the pulse
 * period to 0.2s but the fading effect to 0.5s, then you will have 2 effects
 * per period (or per pulse) and the pulse period will spend 1s and not 0.2s.
 */
static bool
led_pulse(leds_t leds, int min_intensity, int max_intensity,
        int toggle_count, float pulse_period, led_effect_t *effect)
{
    data_frame frame = {0, 0, 0, 0};
    bool ret;
    /* Pulse width or duration of the pulse, in hardware loops. The pulse
     * period is twice that number. */
    int pulse_width;
    int delta;

    /* Preconditions */
    if (leds < LED_NONE)
    {
        leds = LED_NONE;
    }
    else
    {
        if (leds > LED_BOTH)
        {
            leds = LED_BOTH;
        }
    }

    min_intensity = bound_to_range(min_intensity, 0, 255);
    max_intensity = bound_to_range(max_intensity, 0, 255);

    if (min_intensity > max_intensity)
    {
        min_intensity = max_intensity;
    }

    /* TODO right now the limitation is the firmware limitation of 255, if we
     * want to overcome this limitation, this libary should split the user
     * command into multiple commands sent over time in order to achieve the
     * required effect. i.e. 500 toggles could be split into 2 commands of
     * 200 toggles and one of 100, each command sent when the previous one is
     * completed. */
    toggle_count = bound_to_range(toggle_count, 1, 255);

    pulse_width = (int)roundf(pulse_period/FW_MAIN_LOOP_DELAY/2);

    /* TODO right now the limitation is the firmware limitation of 255, if we
     * want to overcome this limitation, this libary should split the user
     * command into multiple commands sent over time in order to achieve the
     * required effect. i.e. 500 could be split into 2 commands of
     * 200 and one of 100, each command sent when the previous one is
     * completed. */
    pulse_width = bound_to_range(pulse_width, 1, 255);

    delta = max_intensity - min_intensity;
    ret = led_configure_effects(leds, delta, delta, effect);

    frame[0] = LED_PULSE_RANGE_CMD;
    frame[1] = leds;
    frame[2] = max_intensity;
    frame[3] = min_intensity;

    ret =tux_usb_send_to_tux(frame);

    frame[0] = LED_PULSE_CMD;
    frame[1] = leds;
    frame[2] = toggle_count;
    frame[3] = pulse_width;

    ret = tux_usb_send_to_tux(frame);
    return ret;
}

/**
 *
 */
LIBLOCAL bool
tux_leds_cmd_set(leds_t leds, float intensity, unsigned char effect_type,
        float effect_speed, unsigned char effect_step)
{
    led_effect_t effect;

    unsigned char intsty2 = 255;

    effect.type = effect_type;
    effect.speed = effect_speed;
    effect.step = effect_step;

    intsty2 = bound_to_range((int)(255 * intensity), 0, 255);

    return led_set(leds, intsty2, &effect);
}

/**
 *
 */
LIBLOCAL bool
tux_leds_cmd_pulse(leds_t leds, float min_intensity, float max_intensity,
        unsigned char pulse_count, float pulse_period,
        unsigned char effect_type, float effect_speed,
        unsigned char effect_step)
{
    led_effect_t effect;

    unsigned char min_intsty2 = 255;
    unsigned char max_intsty2 = 255;

    effect.type = effect_type;
    effect.speed = effect_speed;
    effect.step = effect_step;

    min_intsty2 = bound_to_range((int)(255 * min_intensity), 0, 255);
    max_intsty2 = bound_to_range((int)(255 * max_intensity), 0, 255);

    return led_pulse(leds, min_intsty2, max_intsty2, pulse_count,
        pulse_period, &effect);
}
