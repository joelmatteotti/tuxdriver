PROJECT = libtuxdriver
CC = gcc
OBJ_DIR = ../obj
SRC_DIR = ../src
OUTPUT_DIR = ./
TARGET = libtuxdriver.so
C_INCLUDE_DIRS = -I"/usr/local/include"
C_PREPROC =
CFLAGS = -pipe -std=gnu99 -DUSE_MUTEX -Wall -g0 -O2 -fPIC
RC_INCLUDE_DIRS =
RC_PREPROC =
RCFLAGS =
LIB_DIRS =
LIBS = -lpthread -lm
LDFLAGS = -pipe -shared

SRC_OBJS = \
  $(OBJ_DIR)/tux_battery.o	\
  $(OBJ_DIR)/tux_cmd_parser.o	\
  $(OBJ_DIR)/tux_driver.o	\
  $(OBJ_DIR)/tux_error.o	\
  $(OBJ_DIR)/tux_eyes.o	\
  $(OBJ_DIR)/tux_firmware.o	\
  $(OBJ_DIR)/tux_hid_unix.o	\
  $(OBJ_DIR)/tux_hw_status.o	\
  $(OBJ_DIR)/tux_id.o	\
  $(OBJ_DIR)/tux_leds.o	\
  $(OBJ_DIR)/tux_light.o	\
  $(OBJ_DIR)/tux_misc.o	\
  $(OBJ_DIR)/tux_mouth.o	\
  $(OBJ_DIR)/tux_movements.o	\
  $(OBJ_DIR)/tux_pong.o	\
  $(OBJ_DIR)/tux_sound_flash.o	\
  $(OBJ_DIR)/tux_audio.o	\
  $(OBJ_DIR)/tux_spinning.o	\
  $(OBJ_DIR)/tux_descriptor.o	\
  $(OBJ_DIR)/tux_sw_status.o	\
  $(OBJ_DIR)/tux_usb.o	\
  $(OBJ_DIR)/tux_user_inputs.o	\
  $(OBJ_DIR)/tux_flippers.o	\
  $(OBJ_DIR)/log.o

.PHONY: make clean

make:
	-@if [ ! -d "$(OBJ_DIR)" ]; then mkdir "$(OBJ_DIR)"; fi
	-@if [ ! -d "$(OUTPUT_DIR)" ]; then mkdir "$(OUTPUT_DIR)"; fi
	-@svnwcrev $(SRC_DIR) $(SRC_DIR)/svnrev.tmpl.h $(SRC_DIR)/svnrev.h
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_battery.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_battery.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_cmd_parser.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_cmd_parser.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_driver.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_driver.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_error.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_error.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_eyes.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_eyes.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_firmware.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_firmware.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_hid_unix.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_hid_unix.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_hw_status.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_hw_status.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_id.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_id.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_leds.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_leds.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_light.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_light.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_misc.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_misc.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_mouth.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_mouth.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_movements.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_movements.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_pong.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_pong.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_sound_flash.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_sound_flash.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_audio.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_audio.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_spinning.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_spinning.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_descriptor.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_descriptor.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_sw_status.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_sw_status.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_usb.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_usb.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_user_inputs.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_user_inputs.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tux_flippers.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/tux_flippers.o
	$(CC) -c $(CFLAGS) $(SRC_DIR)/log.c $(C_INCLUDE_DIRS) -o $(OBJ_DIR)/log.o
	$(CC) -o "$(OUTPUT_DIR)$(TARGET)" $(SRC_OBJS) $(LIB_DIRS) $(LIBS) $(LDFLAGS)
	-@ar rcs $(OUTPUT_DIR)/libtuxdriver.a $(SRC_OBJS)
	-@rm -fR $(OBJ_DIR)/*.o

clean:
	-@rm -fR $(OBJ_DIR)/*.o
