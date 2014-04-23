/*
 * Tux Droid - Hid interface (only for windows)
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
 * \file tux_hid_win32.c
 * \brief Tux HID functions.
 * \author remi.jocaille@c2me.be
 * \ingroup hid_interface
 */

#ifdef WIN32

#include <windows.h>
#include <hidsdi.h>
#include <setupapi.h>
#include <dbt.h>
#include <stdbool.h>
#include <stdio.h>

#include "tux_hid_win32.h"
#include "tux_misc.h"

static char device_symbolic_name[256] = "";
static HANDLE tux_device_hdl = NULL;
static COMMTIMEOUTS timeout;

/**
 * \brief Capture the HID dongle.
 * \param vendor_id Dongle vendor ID.
 * \param product_id Dongle product ID.
 * \return true or false.
 */
bool LIBLOCAL
tux_hid_capture(int vendor_id, int product_id)
{
    GUID hid_guid;
    HANDLE h_dev_info;
    SP_DEVICE_INTERFACE_DATA dev_info_data;
    PSP_DEVICE_INTERFACE_DETAIL_DATA detail_data = NULL;
    int member_index = 0;
    bool last_device = false;
    long result;
    unsigned long length = 0;
    ULONG required;
    HANDLE device_hdl = NULL;
    HIDD_ATTRIBUTES attributes;
    bool tux_found = false;;

    if (tux_device_hdl != NULL)
    {
        return false;
    }

	HidD_GetHidGuid(&hid_guid);

    h_dev_info = SetupDiGetClassDevs(&hid_guid,
        NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);

    dev_info_data.cbSize = sizeof(dev_info_data);

    member_index = 0;

    do
    {
        result = SetupDiEnumDeviceInterfaces(h_dev_info, 0,
            &hid_guid, member_index, &dev_info_data);

        if (result != 0)
        {
            result = SetupDiGetDeviceInterfaceDetail(h_dev_info,
                &dev_info_data, NULL, 0, &length, NULL);

            detail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(length);
            detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            result = SetupDiGetDeviceInterfaceDetail(h_dev_info,
				&dev_info_data, detail_data, length, &required, NULL);

            device_hdl = CreateFile(detail_data->DevicePath, 0,
				FILE_SHARE_READ|FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING, 0, NULL);

            attributes.Size = sizeof(attributes);
			result = HidD_GetAttributes(device_hdl, &attributes);

            if ((attributes.VendorID == vendor_id) &&
                (attributes.ProductID == product_id))
            {
                sprintf(device_symbolic_name, "%s", detail_data->DevicePath);

                CloseHandle(device_hdl);

                tux_device_hdl = CreateFile(detail_data->DevicePath,
                    GENERIC_WRITE|GENERIC_READ,
                    FILE_SHARE_READ|FILE_SHARE_WRITE,
                    (LPSECURITY_ATTRIBUTES)NULL,
                    OPEN_EXISTING, 0, NULL);

                timeout.ReadTotalTimeoutConstant = HID_RW_TIMEOUT;
                timeout.WriteTotalTimeoutConstant = HID_RW_TIMEOUT;
                SetCommTimeouts(tux_device_hdl, &timeout);

				tux_found = true;

                break;
            }

            CloseHandle(device_hdl);
            free(detail_data);
        }
        else
        {
            last_device = true;
        }

        member_index++;

    }
    while (last_device == false);

    SetupDiDestroyDeviceInfoList(h_dev_info);

    if (tux_found)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * \brief Release the access to the HID dongle.
 */
void LIBLOCAL
tux_hid_release(void)
{
    if (tux_device_hdl != NULL)
    {
        CloseHandle(tux_device_hdl);
        tux_device_hdl = NULL;
    }
}

/**
 * \brief Write data to the HID dongle.
 * \param size Data size.
 * \param buffer Data to write.
 * \return The write success.
 */
bool LIBLOCAL
tux_hid_write(int size, const char *buffer)
{
    long wrt_count;
    
    char report[REPORT_SIZE_OUT + 1] = { [0 ... REPORT_SIZE_OUT] = 0 };
    long result;

    if (size > REPORT_SIZE_OUT)
    {
        return false;
    }

    if (tux_device_hdl == NULL)
    {
        return false;
    }

    report[0] = 0;
    memcpy(&report[1], buffer, size);
    
    result = WriteFile(tux_device_hdl, report, REPORT_SIZE_OUT + 1, &wrt_count, NULL);

    if (!result)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * \brief Read data from the HID dongle.
 * \param size Data size.
 * \param buffer Data buffer.
 * \return The read success.
 */
bool LIBLOCAL
tux_hid_read(int size, char *buffer)
{
    long rd_count;
    char report[REPORT_SIZE_IN + 1];
    long result;

    if (size > REPORT_SIZE_IN)
    {
        return false;
    }

    if (tux_device_hdl == NULL)
    {
        return false;
    }

    result = ReadFile(tux_device_hdl, report, REPORT_SIZE_IN + 1, &rd_count,
        NULL);

    memcpy(buffer, &report[1], size);

    if (!result)
    {
        return false;
    }
    else
    {
        return true;
    }
}

#endif /* WIN32 */
