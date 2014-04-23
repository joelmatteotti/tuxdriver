/*
 * Tux Droid - Test driver
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#ifdef WIN32
#   include <windows.h>
#endif

#include "../include/tux_driver.h"

static int print_descriptor = 0;
static int print_status = 0;

/**
 *
 */
static void
on_status_event(status)
char *status;
{
    drv_tokens_t tokens;
    tux_descriptor_t tux_desc;
    char status_state[1024];
    char status_name[256];
    int status_id;
    TuxDrvError err;
    char *macro_str =
        "0.0:TUX_CMD:MOUTH:OPEN\n"
        "2.0:TUX_CMD:MOUTH:CLOSE\n";
    char *tracks =
        "tada.wav|tada.wav|tada.wav|tada.wav|tada.wav|"
        "tada.wav|tada.wav|tada.wav|tada.wav|tada.wav";

    /* Split the status string */
    TuxDrv_TokenizeStatus(status, &tokens);

    /* Head button */
    if (!strcmp(tokens[0], "head_button"))
        /* TuxDrv_PerformMacroFile */
        if (!strcmp(tokens[2], "True")) {
            err = TuxDrv_PerformMacroFile("test_macro.txt");
            if (err != E_TUXDRV_NOERROR)
                printf("%s\n", TuxDrv_StrError(err));
        }

    /* Left wing button */
    if (!strcmp(tokens[0], "left_wing_button"))
        /* TuxDrv_PerformMacroText */
        if (!strcmp(tokens[2], "True")) {
            err = TuxDrv_PerformMacroText(macro_str);
            if (err != E_TUXDRV_NOERROR)
                printf("%s\n", TuxDrv_StrError(err));
        }

     /* Right wing button */
    if (!strcmp(tokens[0], "right_wing_button"))
        /* TuxDrv_PerformCommand */
        if (!strcmp(tokens[2], "True")) {
            err = TuxDrv_PerformCommand(0.0, "TUX_CMD:MOUTH:ON:1,NDEF");
            if (err != E_TUXDRV_NOERROR)
                printf("%s\n", TuxDrv_StrError(err));
        }

    /* Remote controller button */
    if (!strcmp(tokens[0], "remote_button")) {
        /* K_0 */
        /* TuxDrv_SoundReflash */
        if (!strcmp(tokens[2], "K_0")) {
            err = TuxDrv_SoundReflash(tracks);
            if (err != E_TUXDRV_NOERROR)
                printf("%s\n", TuxDrv_StrError(err));
        /* K_1 */
        /* TuxDrv_GetStatusName */
        }else if (!strcmp(tokens[2], "K_1")) {
            err = TuxDrv_GetStatusName(SW_ID_MOUTH_POSITION, status_name);
            if (err != E_TUXDRV_NOERROR)
                printf("%s\n", TuxDrv_StrError(err));
            else
                printf("The name of the status number %d is : %s\n",
                    SW_ID_MOUTH_POSITION, status_name);
        /* K_2 */
        /* TuxDrv_GetStatusId */
        } else if (!strcmp(tokens[2], "K_2")) {
            err = TuxDrv_GetStatusId("mouth_position", &status_id);
            if (err != E_TUXDRV_NOERROR)
                printf("%s\n", TuxDrv_StrError(err));
            else
                printf("The number of the status (mouth_position) is : %d\n",
                    status_id);
        /* K_3 */
        /* TuxDrv_GetStatusState */
        } else if (!strcmp(tokens[2], "K_3")) {
            err = TuxDrv_GetStatusState(SW_ID_MOUTH_POSITION, status_state);
            if (err != E_TUXDRV_NOERROR)
                printf("%s\n", TuxDrv_StrError(err));
            else
                printf("The status state of (mouth_position) is : %s\n",
                    status_state);
        /* K_4 */
        /* TuxDrv_PerformCommand */
        } else if (!strcmp(tokens[2], "K_4")) {
            err = TuxDrv_PerformCommand(0.0, "TUX_CMD:AUDIO:MUTE:True");
            if (err != E_TUXDRV_NOERROR)
                printf("%s\n", TuxDrv_StrError(err));
        /* K_5 */
        /* TuxDrv_PerformCommand */
        } else if (!strcmp(tokens[2], "K_5")) {
            err = TuxDrv_PerformCommand(0.0, "TUX_CMD:AUDIO:MUTE:False");
            if (err != E_TUXDRV_NOERROR)
                printf("%s\n", TuxDrv_StrError(err));
        }
    }

    if (print_descriptor)
    {
        /* Tuxdroid descriptor is complete */
        if (!strcmp(tokens[0], "descriptor_complete"))
        {
            /* TuxDrv_GetDescriptor */
            if (!strcmp(tokens[2], "True"))
            {
                TuxDrv_GetDescriptor(&tux_desc);
                printf("%s\n", tux_desc.firmwares.package->version_string);
                printf("%s\n", tux_desc.firmwares.tuxcore->version_string);
                printf("%s\n", tux_desc.firmwares.tuxaudio->version_string);
                printf("%s\n", tux_desc.firmwares.tuxrf->version_string);
                printf("%s\n", tux_desc.firmwares.fuxrf->version_string);
                printf("%s\n", tux_desc.firmwares.fuxusb->version_string);
                printf("%s\n", tux_desc.driver.version_string);
            }
        }
    }

    if (print_status)
    {

        /* Print status string */
        if ((print_status > 1) || (strcmp(tokens[0], "light_level")))
        {
            printf("%s\n", status);
        }
    }
}

/**
 *
 */
static void
on_end_cycle()
{
}

static void
usage(char *progname)
{
    fprintf(stderr, "Usage: %s [-d -s -S]\n", progname);
    fprintf(stderr, "    -d: prints descriptor\n");
    fprintf(stderr, "    -s: prints status messages except light_sensor\n");
    fprintf(stderr, "    -S: prints all status messages\n");
}

/**
 *  Main function.
 */
int
main(argc, argv)
int argc;
char* argv[];
{
    char ch;
    char *progname = argv[0];

    // option names are to be reconsidered
    while ((ch = getopt(argc, argv, "dsS")) != -1)
    {
        switch (ch) {
        case 'd':
            print_descriptor = 1;
            break;
        case 's':
            print_status = 1;
            break;
        case 'S':
            print_status = 2;
            break;
        case '?':
        default:
            usage(progname);
        }
    }
    argc -= optind;
    argv += optind;

    TuxDrv_SetLogLevel(LOG_LEVEL_DEBUG);
    TuxDrv_SetLogTarget(LOG_TARGET_SHELL);
    TuxDrv_SetStatusCallback(on_status_event);
    TuxDrv_SetEndCycleCallback(on_end_cycle);
    TuxDrv_Start();

    return 0;
}
