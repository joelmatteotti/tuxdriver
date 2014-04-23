# Tux Droid - Driver
# Copyright (C) 2008 C2ME Sa <Acness : remi.jocaille@c2me.be>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.
 
from ctypes import *
import os

E_TUXDRV_BEGIN                      = 256
E_TUXDRV_NOERROR                    = 0
E_TUXDRV_PARSERISDISABLED           = E_TUXDRV_BEGIN
E_TUXDRV_INVALIDCOMMAND             = E_TUXDRV_BEGIN + 1
E_TUXDRV_STACKOVERFLOW              = E_TUXDRV_BEGIN + 2
E_TUXDRV_FILEERROR                  = E_TUXDRV_BEGIN + 3
E_TUXDRV_BADWAVFILE                 = E_TUXDRV_BEGIN + 4
E_TUXDRV_INVALIDIDENTIFIER          = E_TUXDRV_BEGIN + 5
E_TUXDRV_INVALIDNAME                = E_TUXDRV_BEGIN + 6
E_TUXDRV_INVALIDPARAMETER           = E_TUXDRV_BEGIN + 7
E_TUXDRV_BUSY                       = E_TUXDRV_BEGIN + 8
E_TUXDRV_WAVSIZEEXCEDED             = E_TUXDRV_BEGIN + 9

SW_ID_FLIPPERS_POSITION             = 0
SW_ID_FLIPPERS_REMAINING_MVM        = 1
SW_ID_SPINNING_DIRECTION            = 2
SW_ID_SPINNING_REMAINING_MVM        = 3
SW_ID_LEFT_WING_BUTTON              = 4
SW_ID_RIGHT_WING_BUTTON             = 5
SW_ID_HEAD_BUTTON                   = 6
SW_ID_REMOTE_BUTTON                 = 7
SW_ID_MOUTH_POSITION                = 8
SW_ID_MOUTH_REMAINING_MVM           = 9
SW_ID_EYES_POSITION                 = 10
SW_ID_EYES_REMAINING_MVM            = 11
SW_ID_DESCRIPTOR_COMPLETE           = 12
SW_ID_RF_STATE                      = 13
SW_ID_DONGLE_PLUG                   = 14
SW_ID_CHARGER_STATE                 = 15
SW_ID_BATTERY_LEVEL                 = 16
SW_ID_BATTERY_STATE                 = 17
SW_ID_LIGHT_LEVEL                   = 18
SW_ID_LEFT_LED_STATE                = 19
SW_ID_RIGHT_LED_STATE               = 20
SW_ID_CONNECTION_QUALITY            = 21
SW_ID_AUDIO_FLASH_PLAY              = 22
SW_ID_AUDIO_GENERAL_PLAY            = 23
SW_ID_FLASH_PROG_CURR_TRACK         = 24
SW_ID_FLASH_PROG_LAST_TRACK_SIZE    = 25
SW_ID_TUXCORE_SYMBOLIC_VERSION      = 26
SW_ID_TUXAUDIO_SYMBOLIC_VERSION     = 27
SW_ID_FUXUSB_SYMBOLIC_VERSION       = 28
SW_ID_FUXRF_SYMBOLIC_VERSION        = 29
SW_ID_TUXRF_SYMBOLIC_VERSION        = 30
SW_ID_DRIVER_SYMBOLIC_VERSION       = 31
SW_ID_SOUND_REFLASH_BEGIN           = 32
SW_ID_SOUND_REFLASH_END             = 33
SW_ID_SOUND_REFLASH_CURRENT_TRACK   = 34
SW_ID_EYES_MOTOR_ON                 = 35
SW_ID_MOUTH_MOTOR_ON                = 36
SW_ID_FLIPPERS_MOTOR_ON             = 37
SW_ID_SPIN_LEFT_MOTOR_ON            = 38
SW_ID_SPIN_RIGHT_MOTOR_ON           = 39
SW_ID_FLASH_SOUND_COUNT             = 40

SW_NAME_DRIVER = [
    "flippers_position",
    "flippers_remaining_movements",
    "spinning_direction",
    "spinning_remaining_movements",
    "left_wing_button",
    "right_wing_button",
    "head_button",
    "remote_button",
    "mouth_position",
    "mouth_remaining_movements",
    "eyes_position",
    "eyes_remaining_movements",
    "descriptor_complete",
    "radio_state",
    "dongle_plug",
    "charger_state",
    "battery_level",
    "battery_state",
    "light_level",
    "left_led_state",
    "right_led_state",
    "connection_quality",
    "audio_flash_play",
    "audio_general_play",
    "flash_programming_current_track",
    "flash_programming_last_track_size",
    "tuxcore_symbolic_version",
    "tuxaudio_symbolic_version",
    "fuxusb_symbolic_version",
    "fuxrf_symbolic_version",
    "tuxrf_symbolic_version",
    "driver_symbolic_version",
    "sound_reflash_begin",
    "sound_reflash_end",
    "sound_reflash_current_track",
    "eyes_motor_on",
    "mouth_motor_on",
    "flippers_motor_on",
    "spin_left_motor_on",
    "spin_right_motor_on",
    "sound_flash_count"
]

LOG_LEVEL_DEBUG             = 0
LOG_LEVEL_INFO              = 1
LOG_LEVEL_WARNING           = 2
LOG_LEVEL_ERROR             = 3
LOG_LEVEL_NONE              = 4

LOG_TARGET_TUX              = 0
LOG_TARGET_SHELL            = 1

TUX_DRIVER_STATUS_CALLBACK = CFUNCTYPE(None, c_char_p)
TUX_DRIVER_SIMPLE_CALLBACK = CFUNCTYPE(None)

class TuxDrv(object):

    def __init__(self, library_path):
        self.__callback_container = []
        self.tux_driver_lib = None
        if os.path.isfile(library_path):
            try:
                self.tux_driver_lib = CDLL(library_path)
            except:
                self.tux_driver_lib = None
    
    def SetStatusCallback(self, funct = None):
        if self.tux_driver_lib == None:
            return
            
        if funct == None:
            return
    
        cb = TUX_DRIVER_STATUS_CALLBACK(funct)
        self.__callback_container.append(cb)
        self.tux_driver_lib.TuxDrv_SetStatusCallback(cb)
        return
        
    def SetEndCycleCallback(self, funct = None):
        if self.tux_driver_lib == None:
            return
            
        if funct == None:
            return
    
        cb = TUX_DRIVER_SIMPLE_CALLBACK(funct)
        self.__callback_container.append(cb)
        self.tux_driver_lib.TuxDrv_SetEndCycleCallback(cb)
        return
        
    def SetDongleConnectedCallback(self, funct = None):
        if self.tux_driver_lib == None:
            return
            
        if funct == None:
            return
    
        cb = TUX_DRIVER_SIMPLE_CALLBACK(funct)
        self.__callback_container.append(cb)
        self.tux_driver_lib.TuxDrv_SetDongleConnectedCallback(cb)
        return
        
    def SetDongleDisconnectedCallback(self, funct = None):
        if self.tux_driver_lib == None:
            return
            
        if funct == None:
            return
    
        cb = TUX_DRIVER_SIMPLE_CALLBACK(funct)
        self.__callback_container.append(cb)
        self.tux_driver_lib.TuxDrv_SetDongleDisconnectedCallback(cb)
        return
        
    def Start(self):
        if self.tux_driver_lib == None:
            return
            
        self.tux_driver_lib.TuxDrv_Start()
        
    def Stop(self): 
        if self.tux_driver_lib == None:
            return
            
        self.tux_driver_lib.TuxDrv_Stop()

            
    def PerformCommand(self, delay, command):
        if self.tux_driver_lib == None:
            return E_TUXDRV_PARSERISDISABLED
            
        ret = self.tux_driver_lib.TuxDrv_PerformCommand(c_double(delay), 
                c_char_p(command))
        
        return ret
            
    def PerformMacroFile(self, file_path = ""):
        if self.tux_driver_lib == None:
            return E_TUXDRV_PARSERISDISABLED
            
        ret = self.tux_driver_lib.TuxDrv_PerformMacroFile(c_char_p(file_path))
        
        return ret
            
    def PerformMacroText(self, macro = ""):
        if self.tux_driver_lib == None:
            return E_TUXDRV_PARSERISDISABLED
            
        ret = self.tux_driver_lib.TuxDrv_PerformMacroText(c_char_p(macro))
        
        return ret
            
    def ClearCommandStack(self):
        if self.tux_driver_lib == None:
            return
            
        self.tux_driver_lib.TuxDrv_ClearCommandStack()
        
        return
            
    def SoundReflash(self, tracks = ""):
        if self.tux_driver_lib == None:
            return E_TUXDRV_BUSY
            
        ret = self.tux_driver_lib.TuxDrv_SoundReflash(c_char_p(tracks))
        
        return ret
            
    def GetStatusId(self, name = "battery_level"):
        if self.tux_driver_lib == None:
            return -1
            
        idc = c_int(0)
        idcp = pointer(idc)
        ret = self.tux_driver_lib.TuxDrv_GetStatusId(c_char_p(name), idcp)
        
        if ret != E_TUXDRV_NOERROR:
            idc.value = -1
            
        return idc.value
    
    def GetStatusName(self, id = 0):
        if self.tux_driver_lib == None:
            return "UNDEFINED"
            
        result = " " * 256
        ret = self.tux_driver_lib.TuxDrv_GetStatusName(c_int(id), 
            c_char_p(result))
        result = result.replace(" ", "")
        
        if ret == E_TUXDRV_NOERROR:
            return result
        else:
            return "UNDEFINED"
            
    def SetLogLevel(self, level = LOG_LEVEL_INFO):
        if self.tux_driver_lib == None:
            return
            
        self.tux_driver_lib.TuxDrv_SetLogLevel(c_uint8(level))
        
    def SetLogTarget(self, target = LOG_TARGET_SHELL):
        if self.tux_driver_lib == None:
            return
            
        self.tux_driver_lib.TuxDrv_SetLogTarget(c_uint8(target))
        
    def GetStatusState(self, id = 0):
        if self.tux_driver_lib == None:
            return "UNDEFINED"
            
        result = " " * 256
        ret = self.tux_driver_lib.TuxDrv_GetStatusState(c_int(id), 
            c_char_p(result))
        result = result.replace(" ", "")
        
        if ret == E_TUXDRV_NOERROR:
            return result
        else:
            return "UNDEFINED"
            
    def GetStatusValue(self, id = 0):
        if self.tux_driver_lib == None:
            return "UNDEFINED"
            
        result = " " * 256
        ret = self.tux_driver_lib.TuxDrv_GetStatusValue(c_int(id), 
            c_char_p(result))
        result = result.replace(" ", "")
        
        if ret == E_TUXDRV_NOERROR:
            return result
        else:
            return "UNDEFINED"
            
    def GetAllStatusState(self):
        if self.tux_driver_lib == None:
            return ""
            
        result = " " * 8182
        self.tux_driver_lib.TuxDrv_GetAllStatusState(c_char_p(result))
        result = result.replace(" ", "")
        
        return result
            
    def TokenizeStatus(self, status = ""):
        if self.tux_driver_lib == None:
            return []
            
        result = status.split(":")
        if len(result) == 1:
            if result[0] == '':
                result = []
        return result
        
    def ResetPositions(self):
        if self.tux_driver_lib == None:
            return
            
        self.tux_driver_lib.TuxDrv_ResetPositions()
        
        return
        
    def ResetDongle(self):
        if self.tux_driver_lib == None:
            return
            
        self.tux_driver_lib.TuxDrv_ResetDongle()
        
        return
        
    def GetStatusStruct(self, status = ""):
        result = {
            'name' : "None",
            'value' : None,
            'delay' : 0.0,
            'type' : 'string'
        }
        
        if self.tux_driver_lib == None:
            return result
            
        status_s = self.TokenizeStatus(status)
        if len(status_s) == 0:
            return result
            
        result['name'] = status_s[0]
        result['delay'] = status_s[3]
        result['type'] = status_s[1]
        
        if status_s[1] in ['uint8', 'int8', 'int', 'float', 'bool']:
            result['value'] = eval(status_s[2])
        elif status_s[1] == 'string':
            result['value'] = status_s[2]
            
        return result
        
    def StrError(self, error_code):
        if self.tux_driver_lib == None:
            return "Shared library not found"
            
        result = self.tux_driver_lib.TuxDrv_StrError(c_int(error_code))
        
        return c_char_p(result).value

if __name__ == "__main__":
    
    def on_status_event(status):
        status_struct =  tux_drv.GetStatusStruct(status)
        print status_struct
        
    def on_dongle_connected():
        tux_drv.ResetPositions()
        print tux_drv.GetAllStatusState()
        print tux_drv.GetStatusName(0)
        print tux_drv.GetStatusValue(0)
        print tux_drv.GetStatusState(0)

    if os.name == 'nt':
        tux_drv = TuxDrv('../win32/libtuxdriver.dll')
    else:
        tux_drv = TuxDrv('../unix/libtuxdriver.so')
    
    tux_drv.SetLogLevel(LOG_LEVEL_INFO)
    tux_drv.SetStatusCallback(on_status_event)  
    tux_drv.SetDongleConnectedCallback(on_dongle_connected)  
    tux_drv.Start()
