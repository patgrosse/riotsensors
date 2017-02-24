# used commands and flags
export CC ?= gcc
export LD ?= gcc
export AR ?= ar
export RM ?= rm
export MKDIR ?= mkdir
export ECHO ?= echo
export SHELL ?= /bin/sh

# riotsensors packets specific variables
export RS_PACKETS_CFLAGS ?= -g -std=gnu99 -Werror -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wcast-align -Wstrict-prototypes \
	-Wstrict-overflow=5 -Wno-write-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum \
	-Wunreachable-code

export RS_PACKETS_SRC_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

export RS_PACKETS_OBJS_DIR ?= $(abspath $(RS_PACKETS_SRC_DIR)/build/rspacketslib)

export RS_PACKETS_LIB_FILE ?= $(abspath $(RS_PACKETS_SRC_DIR)/build/rspacketslib/rspacketslib.a)

export RS_PACKETS_INCLUDES += -I$(RS_PACKETS_SRC_DIR)/include

export RS_PACKETS_CFILES := \
			ieee754_network.c \
			rs_packets.c

export RS_PACKETS_HFILES := \
			ieee754_network.h \
			rs_packets.h

export RS_PACKETS_OBJS = $(addprefix $(RS_PACKETS_OBJS_DIR)/, $(RS_PACKETS_CFILES:.c=.o))
