/*
 * Tux Droid - USB interface
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

#include <errno.h>
#include <string.h>

#include "log.h"
#ifdef WIN32
#   include "tux_hid_win32.h"
#else
#   include "tux_hid_unix.h"
#endif
#include "tux_types.h"
#include "tux_usb.h"

#ifdef USE_MUTEX
#   include "threading_uniform.h"
#endif

static bool usb_connected = false;
static frame_callback_t frame_callback_function;
static simple_callback_t dongle_disconnect_function;
static simple_callback_t dongle_connect_function;
static simple_callback_t loop_cycle_complete_function;
static rf_state_callback_t rf_state_callback_function;
static unsigned char last_knowed_rf_state = 0;
static char frame_status_request[5] = {1, 1, 0, 0, 0};
static char frame_reset_dongle[5] = {1, 1, 0, 0, 0xFE};
static char frame_reset_rf[5] = {1, 1, 0, 0, 0xFD};
static char frame_blink_eyes[5] = {0, 0x40, 2, 0, 0};

#ifdef USE_MUTEX
static mutex_t __connected_mutex;
static mutex_t __read_write_mutex;
static mutex_t __callback_mutex;
#endif

static bool read_loop_started = false;

static void set_connected(bool value);

static void set_read_loop_started(bool value);
static bool get_read_loop_started(void);
static void read_usb_loop(void);

#ifdef USB_IDFRAME
static int id_frame_last = 999;
static int freezed_frame_cnt = 0;
#endif
static int empty_frame_cnt = 0;

/**
 *
 */
LIBLOCAL void
tux_usb_set_frame_callback(frame_callback_t funct)
{
#ifdef USE_MUTEX
    mutex_lock(__callback_mutex);
#endif
    frame_callback_function = funct;
#ifdef USE_MUTEX
    mutex_unlock(__callback_mutex);
#endif
}

/**
 *
 */
LIBLOCAL void
tux_usb_set_rf_state_callback(rf_state_callback_t funct)
{
#ifdef USE_MUTEX
    mutex_lock(__callback_mutex);
#endif
    rf_state_callback_function = funct;
#ifdef USE_MUTEX
    mutex_unlock(__callback_mutex);
#endif
}

/**
 *
 */
LIBLOCAL void
tux_usb_set_disconnect_dongle_callback(simple_callback_t funct)
{
#ifdef USE_MUTEX
    mutex_lock(__callback_mutex);
#endif
    dongle_disconnect_function = funct;
#ifdef USE_MUTEX
    mutex_unlock(__callback_mutex);
#endif
}

/**
 *
 */
LIBLOCAL void
tux_usb_set_connect_dongle_callback(simple_callback_t funct)
{
#ifdef USE_MUTEX
    mutex_lock(__callback_mutex);
#endif
    dongle_connect_function = funct;
#ifdef USE_MUTEX
    mutex_unlock(__callback_mutex);
#endif
}

/**
 *
 */
LIBLOCAL void
tux_usb_set_loop_cycle_complete_callback(simple_callback_t funct)
{
#ifdef USE_MUTEX
    mutex_lock(__callback_mutex);
#endif
    loop_cycle_complete_function = funct;
#ifdef USE_MUTEX
    mutex_unlock(__callback_mutex);
#endif
}

/**
 *
 */
LIBLOCAL void
tux_usb_init_module(void)
{
#ifdef USE_MUTEX
    mutex_init(__connected_mutex);
    mutex_init(__read_write_mutex);
    mutex_init(__callback_mutex);
#endif
}

/**
 *
 */
LIBLOCAL void
tux_usb_exit_module(void)
{
#ifdef USE_MUTEX
    mutex_delete(__connected_mutex);
    mutex_delete(__read_write_mutex);
    mutex_delete(__callback_mutex);
#endif
}

/**
 *
 */
static void
set_connected(bool value)
{
#ifdef USE_MUTEX
    mutex_lock(__connected_mutex);
#endif
    usb_connected = value;
#ifdef USE_MUTEX
    mutex_unlock(__connected_mutex);
#endif
    if (value)
    {
        if (dongle_connect_function)
        {
            dongle_connect_function();
        }
    }
    else
    {
        if (dongle_disconnect_function)
        {
            dongle_disconnect_function();
        }
    }
}

/**
 *
 */
LIBLOCAL bool
tux_usb_connected(void)
{
    bool ret = false;

#ifdef USE_MUTEX
    mutex_lock(__connected_mutex);
#endif
    ret = usb_connected;
#ifdef USE_MUTEX
    mutex_unlock(__connected_mutex);
#endif

    return ret;
}

/**
 *
 */
LIBLOCAL TuxUSBError
tux_usb_capture(void)
{
    empty_frame_cnt = 0;
#ifdef USB_IDFRAME
    id_frame_last = 999;
    freezed_frame_cnt = 0;
#endif

    if (!tux_hid_capture(TUX_VID, TUX_PID))
    {
        return TuxUSBFuxNotFound;
    }

    set_connected(true);

    return TuxUSBNoError;
}

/**
 *
 */
LIBLOCAL TuxUSBError
tux_usb_release(void)
{
    tux_hid_release();

    set_connected(false);

    return TuxUSBNoError;
}

/**
 *
 */
LIBLOCAL TuxUSBError
tux_usb_write(const void *buff)
{
    bool ret;

    if (!tux_usb_connected())
    {
        log_error("Fux USB device not connected");
        return TuxUSBNotConnected;
    }

#ifdef USE_MUTEX
    mutex_lock(__read_write_mutex);
#endif

    ret = tux_hid_write(TUX_SEND_LENGTH, (char *)buff);
#ifdef USE_MUTEX
    mutex_unlock(__read_write_mutex);
#endif

    if (!ret)
    {
        set_connected(false);
        tux_usb_release();
        log_error("Fux is disconnected");
        return TuxUSBDisconnected;
    }
    return TuxUSBNoError;
}

/**
 *
 */
static void
process_usb_frame(const char *data)
{
    int i, j;
    int rf_state;
    int packet_count;
    int id_frame;
    char *data_buf;
    char packet_data[4];

    id_frame = data[0];
    rf_state = data[1];
    packet_count = data[3];
    data_buf = (char *)data;
    data_buf += 4;

#ifdef USB_IDFRAME
    /* Check if the frame is newer than the last received one */
    if (id_frame == id_frame_last)
    {
        freezed_frame_cnt++;
        log_warning("The id of USB frame is the same than the previous [%d]",
            freezed_frame_cnt);
#ifndef USB_DEBUG
        if (freezed_frame_cnt >= TUX_USB_FREEZED_FRAMES_LIMIT)
        {
            freezed_frame_cnt = 0;
            id_frame_last = 999;
            log_error("The USB frame retrieving seems to be freezed [%d]",
                TUX_USB_FREEZED_FRAMES_LIMIT);
            log_info("The RF connection will be reinitialized");
            tux_usb_rf_reset();
        }
#endif
        return;
    }
    else
    {
        freezed_frame_cnt = 0;
        id_frame_last = id_frame;
    }
#endif

    /* Having RF state to ON and no status frame is not normal */
    if ((packet_count == 0) && (rf_state == 1))
    {
        empty_frame_cnt++;
#ifndef USB_DEBUG
        if (empty_frame_cnt > 2)
        {
            log_warning("Consecutive frames without status : %d", empty_frame_cnt);
        }
        if (empty_frame_cnt >= TUX_USB_ERROR_LIMIT)
        {
            log_error("DONGLE ERROR : Too many consecutive frames without status [%d], but the RF is online",
                TUX_USB_ERROR_LIMIT);
            empty_frame_cnt = 0;
            log_info("Send a command to the eyes.");
            tux_usb_send_raw((unsigned char *)frame_blink_eyes);
        }
#else
        log_warning("Consecutive frames without status : %d", empty_frame_cnt);
#endif
    }
    else
    {
        empty_frame_cnt = 0;
    }

    if (last_knowed_rf_state != rf_state)
    {
        last_knowed_rf_state = rf_state;
#ifdef USE_MUTEX
        mutex_lock(__callback_mutex);
#endif
        if (rf_state_callback_function)
        {
            rf_state_callback_function(last_knowed_rf_state);
        }
#ifdef USE_MUTEX
        mutex_unlock(__callback_mutex);
#endif
    }

    if (packet_count > 15)
    {
        log_error("DONGLE ERROR : Statuses packets count is wrong (>15)");
        return;
    }

    for (i = 0; i < packet_count; i++)
    {
        for (j = 0; j < 4; j++)
        {
            packet_data[j] = (unsigned char)data_buf[j];
        }
#ifdef USE_MUTEX
        mutex_lock(__callback_mutex);
#endif
        if (frame_callback_function)
        {
            frame_callback_function((unsigned char*)packet_data);
        }
#ifdef USE_MUTEX
        mutex_unlock(__callback_mutex);
#endif

        data_buf += 4;
    }
}

/**
 *
 */
LIBLOCAL TuxUSBError
tux_usb_read(void *buf)
{
    bool ret;

    if (!tux_usb_connected())
    {
        log_warning("Fux USB device not connected");
        return TuxUSBNotConnected;
    }

    memset(buf, 0, TUX_RECEIVE_LENGTH);

#ifdef USE_MUTEX
    mutex_lock(__read_write_mutex);
#endif
    ret = tux_hid_write(TUX_SEND_LENGTH, (char *)frame_status_request);
    if (!ret)
    {
#ifdef USE_MUTEX
        mutex_unlock(__read_write_mutex);
#endif
        set_connected(false);
        tux_usb_release();
        log_error("Fux is disconnected");
        return TuxUSBDisconnected;
    }

#ifndef WIN32
    /* Hid read write are not bocking on linux */
    usleep(10000);
#endif

    ret = tux_hid_read(TUX_RECEIVE_LENGTH, (char *)buf);
#ifdef USE_MUTEX
    mutex_unlock(__read_write_mutex);
#endif

    if (!ret)
    {
        set_connected(false);
        tux_usb_reset();
        tux_usb_release();
        log_error("Fux is disconnected");
        return TuxUSBDisconnected;
    }

    process_usb_frame((char *)buf);

    return TuxUSBNoError;
}

/**
 *
 */
static void
set_read_loop_started(bool value)
{
#ifdef USE_MUTEX
    mutex_lock(__connected_mutex);
#endif
    read_loop_started = value;
#ifdef USE_MUTEX
    mutex_unlock(__connected_mutex);
#endif
}

/**
 *
 */
static bool
get_read_loop_started(void)
{
    bool ret = false;
#ifdef USE_MUTEX
    mutex_lock(__connected_mutex);
#endif
    ret = read_loop_started;
#ifdef USE_MUTEX
    mutex_unlock(__connected_mutex);
#endif

    return ret;
}

/**
 *
 */
static void
read_usb_loop(void)
{
    double initial_timeout = 0.0;
    double current_timeout = 0.0;
    unsigned char data[64] = { [0 ... 63] = 0 };

    initial_timeout = get_time();
    current_timeout = initial_timeout;

    set_read_loop_started(true);

    log_info("Start the read loop");

    while (tux_usb_connected())
    {
        current_timeout += TUX_READ_LOOP_INTERVAL;

        if (!tux_usb_connected())
        {
            break;
        }

        tux_usb_read(data);

        if (loop_cycle_complete_function)
        {
            loop_cycle_complete_function();
        }

        while (get_time() < current_timeout)
        {
            usleep(1000);
        }

        current_timeout = get_time();
    }

    set_read_loop_started(false);

    log_info("Read loop stopped");
}

/**
 *
 */
LIBLOCAL TuxUSBError
tux_usb_start(void)
{
    int ret;

    if (get_read_loop_started())
    {
        return TuxUSBAlreadyStarted;
    }

    ret = tux_usb_capture();
    if (ret != TuxUSBNoError)
    {
        return ret;
    }

    last_knowed_rf_state = 0;

    read_usb_loop();
    usleep(100000);

    return TuxUSBNoError;
}

/**
 *
 */
LIBLOCAL TuxUSBError
tux_usb_stop(void)
{
    int ret;

    if (!tux_usb_connected())
    {
        return TuxUSBNoError;
    }

    set_connected(false);

    usleep(500000);

    ret = tux_usb_release();
    if (ret != TuxUSBNoError)
    {
        return ret;
    }

    return TuxUSBNoError;
}

/**
 *
 */
LIBLOCAL void
tux_usb_reset(void)
{
    tux_hid_write(TUX_SEND_LENGTH, (char *)frame_reset_dongle);
}

/**
 *
 */
LIBLOCAL void
tux_usb_rf_reset(void)
{
    tux_hid_write(TUX_SEND_LENGTH, (char *)frame_reset_rf);
}

/**
 *  Send a raw command
 *  @param data 5 bytes array
 */
LIBLOCAL bool
tux_usb_send_raw(const unsigned char* data)
{
    int ret;

    ret = tux_usb_write(data);

    usleep(10000);

    if (ret != TuxUSBNoError)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 *  Send a command to Tuxdroid.
 *  @param data 4 bytes array
 */
LIBLOCAL bool
tux_usb_send_to_tux(const unsigned char* data)
{
    raw_frame frame = {0, data[0], data[1], data[2], data[3]};

    return tux_usb_send_raw(frame);
}

/**
 *  Send a command to fux dongle.
 *  @param data 4 bytes array
 */
LIBLOCAL bool
tux_usb_send_to_dongle(const unsigned char* data)
{
    raw_frame frame = {1, data[0], data[1], data[2], data[3]};

    return tux_usb_send_raw(frame);
}

/**
 *  Get the rf state
 *  @param data 4 bytes array
 */
LIBLOCAL bool
tux_usb_get_rf_state(void)
{
    return last_knowed_rf_state;
}
