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

#ifndef _TUX_USB_H_
#define _TUX_USB_H_

#include <stdbool.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include "tux_misc.h"

#define TUX_PID                         0xFF07
#define TUX_VID                         0x03EB
#define TUX_SEND_LENGTH                 5
#define TUX_RECEIVE_LENGTH              64
#define TUX_READ_LOOP_INTERVAL          0.1
#define TUX_USB_ERROR_LIMIT             20
#define TUX_USB_FREEZED_FRAMES_LIMIT    10

#ifdef WIN32
#   define usb_busses               usb_get_busses()
#endif

typedef int TuxUSBError;
typedef enum
{
    TuxUSBNoError                  = 0,
    TuxUSBFuxNotFound,
    TuxUSBCantClaimInterface,
    TuxUSBHandleNotOpen,
    TuxUSBCantReleaseInterface,
    TuxUSBCantCloseDevice,
    TuxUSBNotConnected,
    TuxUSBWriteError,
    TuxUSBWriteErrorRFDisconnected,
    TuxUSBReadError,
    TuxUSBDisconnected,
    TuxUSBAlreadyStarted,
    TuxUSBFirmwareTooOld,
} tux_usb_error_code_t;

/**
 *      Callback function for the frame receiving event
 *      @param data A pointer to the frame
 */
typedef void(*frame_callback_t)(const unsigned char *data);

/**
 *      Callback function prototype for the RF state event
 *      @param state state of rf connection
 */
typedef void(*rf_state_callback_t)(unsigned char state);

/** Initialization of the module
        Some mutex are initialized in this function.
*/
extern void tux_usb_init_module(void);

/**
 *      Finalization of the module
 *      Some mutex are finalizaed in this function.
 */
extern void tux_usb_exit_module(void);

/**
 *  Capture of the usb handle
 *  @return An error code indicating the success of the operation.
 *  (TuxUSBNoError | TuxUSBFuxNotFound | TuxUSBFirmwareTooOld |
 *  TuxUSBHandleNotOpen)
 */
extern TuxUSBError tux_usb_capture(void);

/** Release of the usb handle
        @return An error code indicating the success of the operation.
    (TuxUSBNoError | TuxUSBCantReleaseInterface | TuxUSBCantCloseDevice)
*/
extern TuxUSBError tux_usb_release(void);

/** Set the callback function for the frame event.
        @param funct The function will be linked
*/
extern void tux_usb_set_frame_callback(frame_callback_t funct);

/**
 *  Set the callback function for the RF state event.
 *  @param funct The function will be linked
 */
extern void tux_usb_set_rf_state_callback(rf_state_callback_t funct);

/**
 *  Set the callback function for the dongle disconnect event.
 *  @param funct The function will be linked
 */
extern void tux_usb_set_disconnect_dongle_callback(simple_callback_t funct);

/**
 *  Set the callback function for the dongle connect event.
 *  @param funct The function will be linked
 */
extern void tux_usb_set_connect_dongle_callback(simple_callback_t funct);

/**
 *  Set the callback function for a cycle has complete event.
 *  @param funct The function will be linked
 */
extern void tux_usb_set_loop_cycle_complete_callback(simple_callback_t funct);

/**
 *  Write data on usb dongle
 *  @param buff Data to write
 *  @return An error code indicating the success of the operation
 *  (TuxUSBNoError | TuxUSBNotConnected | TuxUSBWriteError)
 */
extern TuxUSBError tux_usb_write(const void *buff);

/** Read data from usb dongle
 *  @param buf Data pointer
 *  @return An error code indicating the success of the operation
 *  (TuxUSBNoError | TuxUSBNotConnected | TuxUSBWriteError | TuxUSBReadError
 *  | TuxUSBWriteErrorRFDisconnected)
 */
extern TuxUSBError tux_usb_read(void *buf);

/**
 *  Start a loop that read the data on the usb dongle.
 *  The loop run in a thread. The duration of a cycle is 100msec.
 *  A frame event occuring at the end of each cycle.
 *  The function detect the dongle disconnection.
 *  @return An error code indicating the success of the operation
 *  (TuxUSBNoError | TuxUSBAlreadyStarted | TuxUSBFuxNotFound |
 *  TuxUSBCantClaimInterface | TuxUSBHandleNotOpen)
 */
extern TuxUSBError tux_usb_start(void);

/**
 *  Stop the loop that read the data on the usb dongle.
 *  @return An error code indicating the success of the operation
 *  (TuxUSBNoError | TuxUSBCantReleaseInterface | TuxUSBCantCloseDevice)
 */
extern TuxUSBError tux_usb_stop(void);

/**
 *  Get the usb connected state.
 *  @return An integer which indicate if the usb interface is captured.
 */
extern bool tux_usb_connected(void);

/**
 *  Reset the usb dongle
 */
extern void tux_usb_reset(void);

/**
 *  Reset the RF
 */
extern void tux_usb_rf_reset(void);

/**
 *  Send a command to Tuxdroid.
 *  @param data 4 bytes array
 */
extern bool tux_usb_send_to_tux(const unsigned char* data);

/**
 *  Send a command to fux dongle.
 *  @param data 4 bytes array
 */
extern bool tux_usb_send_to_dongle(const unsigned char* data);

/**
 *  Send a raw command to fux dongle.
 *  @param data 4 bytes array
 */
extern bool tux_usb_send_raw(const unsigned char *data);

/**
 *  Get the rf state
 *  @param data 4 bytes array
 */
extern bool tux_usb_get_rf_state(void);

#endif /* _TUX_USB_H_ */
