# Configuration de la carte
BOARD_TAG         = mega
#BOARD_TAG         = uno
BOARD_SUB         = atmega2560
MONITOR_BAUDRATE  = 115200

# On peut écraser cette valeur en utilisant
# make MONITOR_PORT=/dev/ttyACM1 upload
# (par exemple)
MONITOR_PORT      = /dev/ttyACM*

# Flags du compilateur
CFLAGS_STD        = -std=gnu11
CXXFLAGS_STD      = -std=gnu++11

CXXFLAGS         = -pedantic -Wall -Wextra
CXXFLAGS        += -fdiagnostics-color

# Fichires de sortie
TARGET=main
OBJDIR=build

USER_LIB_PATH    := libraries
ARDUINO_QUIET=true
include Arduino-Makefile/Arduino.mk

serial:
	minicom -D $(MONITOR_PORT) -b 9600
