{*
 * Tux Droid - Driver
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
 *}
 
unit tux_driver;

interface

uses
  Windows, classes;
  
const

  DLL_NAME = 'libtuxdriver.dll';

  {**
   * Error codes enumeration.
   *}
  E_TUXDRV_BEGIN                    = 256;
  E_TUXDRV_NOERROR                  = 0;
  E_TUXDRV_PARSERISDISABLED         = E_TUXDRV_BEGIN;
  E_TUXDRV_INVALIDCOMMAND           = E_TUXDRV_BEGIN + 1;
  E_TUXDRV_STACKOVERFLOW            = E_TUXDRV_BEGIN + 2;
  E_TUXDRV_FILEERROR                = E_TUXDRV_BEGIN + 3;
  E_TUXDRV_BADWAVFILE               = E_TUXDRV_BEGIN + 4;
  E_TUXDRV_INVALIDIDENTIFIER        = E_TUXDRV_BEGIN + 5;
  E_TUXDRV_INVALIDNAME              = E_TUXDRV_BEGIN + 6;
  E_TUXDRV_INVALIDPARAMETER         = E_TUXDRV_BEGIN + 7;
  E_TUXDRV_BUSY                     = E_TUXDRV_BEGIN + 8;
  E_TUXDRV_WAVSIZEEXCEDED           = E_TUXDRV_BEGIN + 9;
    
  {** 
   * Id enumeration of high level status.
   *}
  SW_ID_FLIPPERS_POSITION           = 0;
  SW_ID_FLIPPERS_REMAINING_MVM      = 1;
  SW_ID_SPINNING_DIRECTION          = 2;
  SW_ID_SPINNING_REMAINING_MVM      = 3;
  SW_ID_LEFT_WING_BUTTON            = 4;
  SW_ID_RIGHT_WING_BUTTON           = 5;
  SW_ID_HEAD_BUTTON                 = 6;
  SW_ID_REMOTE_BUTTON               = 7;
  SW_ID_MOUTH_POSITION              = 8;
  SW_ID_MOUTH_REMAINING_MVM         = 9;
  SW_ID_EYES_POSITION               = 10;
  SW_ID_EYES_REMAINING_MVM          = 11;
  SW_ID_DESCRIPTOR_COMPLETE         = 12;
  SW_ID_RF_STATE                    = 13;
  SW_ID_DONGLE_PLUG                 = 14;
  SW_ID_CHARGER_STATE               = 15;
  SW_ID_BATTERY_LEVEL               = 16;
  SW_ID_BATTERY_STATE               = 17;
  SW_ID_LIGHT_LEVEL                 = 18;
  SW_ID_LEFT_LED_STATE              = 19;
  SW_ID_RIGHT_LED_STATE             = 20;
  SW_ID_CONNECTION_QUALITY          = 21;
  SW_ID_AUDIO_FLASH_PLAY            = 22;
  SW_ID_AUDIO_GENERAL_PLAY          = 23;
  SW_ID_FLASH_PROG_CURR_TRACK       = 24;
  SW_ID_FLASH_PROG_LAST_TRACK_SIZE  = 25;
  SW_ID_TUXCORE_SYMBOLIC_VERSION    = 26;
  SW_ID_TUXAUDIO_SYMBOLIC_VERSION   = 27;
  SW_ID_FUXUSB_SYMBOLIC_VERSION     = 28;
  SW_ID_FUXRF_SYMBOLIC_VERSION      = 29;
  SW_ID_TUXRF_SYMBOLIC_VERSION      = 30;
  SW_ID_DRIVER_SYMBOLIC_VERSION     = 31;
  SW_ID_SOUND_REFLASH_BEGIN         = 32;
  SW_ID_SOUND_REFLASH_END           = 33;
  SW_ID_SOUND_REFLASH_CURRENT_TRACK = 34;
  SW_ID_EYES_MOTOR_ON               = 35;
  SW_ID_MOUTH_MOTOR_ON              = 36;
  SW_ID_FLIPPERS_MOTOR_ON           = 37;
  SW_ID_SPIN_LEFT_MOTOR_ON          = 38;
  SW_ID_SPIN_RIGHT_MOTOR_ON         = 39;
  SW_ID_FLASH_SOUND_COUNT           = 40;

type

  {** 
   * Simple callback definition.
   *}
  drv_simple_callback_t = procedure;

  {**
   * Status callback definition.
   *}
  drv_status_callback_t = procedure(status:PChar); cdecl;
  
  {** 
   * Logging target
   *}
  log_target_t = (
    LOG_TARGET_TUX = 0,
    LOG_TARGET_SHELL
  );

  {** 
   * Logging levels, in increasing priorities 
   *}
  log_level_t = (
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_NONE
  );
  
  {** 
   * Descriptor structure of a firmaware.
   *}
  p_firmware_descriptor_t = ^firmware_descriptor_t;
  firmware_descriptor_t = packed record
    cpu_id              : integer;
    version_major       : cardinal;
    version_minor       : cardinal;
    version_update      : cardinal;
    revision            : cardinal;
    rlmmr               : cardinal;
    author              : cardinal;
    variation           : cardinal;
    version_string      : packed array[0..255] of char;
  end;

  {** 
   * Descriptor structure of sound flash.
   *}
  p_sound_flash_descriptor_t = ^sound_flash_descriptor_t;
  sound_flash_descriptor_t = packed record
    number_of_sounds      : cardinal;
    flash_usage           : cardinal;
    available_record_time : cardinal;
  end;
  
  {** 
   * Descriptor structure of ID connection.
   *}
  p_id_descriptor_t = ^id_descriptor_t;
  id_descriptor_t = packed record
    number    : cardinal;
  end;
  
  {** 
   * Global descriptor structure of tuxdroid.
   *}
  p_tux_descriptor_t = ^tux_descriptor_t;
  tux_descriptor_t = packed record
    firmwares : packed record
      package         : p_firmware_descriptor_t;
      tuxcore         : p_firmware_descriptor_t;
      tuxaudio        : p_firmware_descriptor_t;
      tuxrf           : p_firmware_descriptor_t;
      fuxrf           : p_firmware_descriptor_t;
      fuxusb          : p_firmware_descriptor_t;
    end;
    driver : packed record
      version_major   : cardinal;
      version_minor   : cardinal;
      version_update  : cardinal;
      version_build   : cardinal;
      version_state   : packed array[0..99] of char;
      version_string  : packed array[0..99] of char;
    end;
    sound_flash       : p_sound_flash_descriptor_t;
    id                : p_id_descriptor_t;
  end;

{**
 * DLL static linkage.
 *}
procedure TuxDrv_Start; cdecl;
  external DLL_NAME name 'TuxDrv_Start';  
procedure TuxDrv_Stop; cdecl;
  external DLL_NAME name 'TuxDrv_Stop';  
procedure TuxDrv_SetStatusCallback(funct:drv_status_callback_t); cdecl;
  external DLL_NAME name 'TuxDrv_SetStatusCallback';   
procedure TuxDrv_SetEndCycleCallback(funct:drv_simple_callback_t); cdecl;
  external DLL_NAME name 'TuxDrv_SetEndCycleCallback';   
procedure TuxDrv_SetDongleConnectedCallback(funct:drv_simple_callback_t); cdecl;
  external DLL_NAME name 'TuxDrv_SetDongleConnectedCallback';   
procedure TuxDrv_SetDongleDisconnectedCallback(funct:drv_simple_callback_t); cdecl;
  external DLL_NAME name 'TuxDrv_SetDongleDisconnectedCallback';   
function TuxDrv_PerformCommand(delay:real; cmd_str:PChar):integer; cdecl;
  external DLL_NAME name 'TuxDrv_PerformCommand';  
function TuxDrv_PerformMacroFile(file_path:PChar):integer; cdecl;
  external DLL_NAME name 'TuxDrv_PerformMacroFile';  
function TuxDrv_PerformMacroText(macro:PChar):integer; cdecl;
  external DLL_NAME name 'TuxDrv_PerformMacroText';  
function TuxDrv_GetStatusName(id:integer; name:PChar):integer; cdecl;
  external DLL_NAME name 'TuxDrv_GetStatusName';  
function TuxDrv_GetStatusValue(id:integer; value:PChar):integer; cdecl;
  external DLL_NAME name 'TuxDrv_GetStatusValue';  
function TuxDrv_GetStatusId(name:PChar; var id:integer):integer; cdecl;
  external DLL_NAME name 'TuxDrv_GetStatusId';  
procedure TuxDrv_ClearCommandStack; cdecl;
  external DLL_NAME name 'TuxDrv_ClearCommandStack';  
function TuxDrv_GetStatusState(id:integer; state:PChar):integer; cdecl;
  external DLL_NAME name 'TuxDrv_GetStatusState';  
procedure TuxDrv_GetAllStatusState(state:PChar); cdecl;
  external DLL_NAME name 'TuxDrv_GetAllStatusState';  
function TuxDrv_SoundReflash(tracks:PChar):integer; cdecl;
  external DLL_NAME name 'TuxDrv_SoundReflash';    
procedure TuxDrv_ResetPositions; cdecl;
  external DLL_NAME name 'TuxDrv_ResetPositions';  
procedure TuxDrv_ResetDongle; cdecl;
  external DLL_NAME name 'TuxDrv_ResetDongle';  
procedure TuxDrv_SetLogLevel(level:integer); cdecl;
  external DLL_NAME name 'TuxDrv_SetLogLevel';  
procedure TuxDrv_SetLogTarget(target:integer); cdecl;
  external DLL_NAME name 'TuxDrv_SetLogTarget'; 
procedure TuxDrv_GetDescriptor(tux_desc:p_tux_descriptor_t); cdecl;  
  external DLL_NAME name 'TuxDrv_GetDescriptor';  
function TuxDrv_StrError(error_code:integer):PChar; cdecl;
  external DLL_NAME name 'TuxDrv_StrError';
  
implementation

end.
