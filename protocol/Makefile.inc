# used commands and flags
export CC ?= gcc
export LD ?= gcc
export AR ?= ar
export RM ?= rm
export MKDIR ?= mkdir
export ECHO ?= echo
export SHELL ?= /bin/sh

# riotsensors protocol specific variables
export RS_PROTOCOL_CFLAGS ?= -g -std=gnu99 -Werror -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wcast-align -Wstrict-prototypes \
	-Wstrict-overflow=5 -Wno-write-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum \
	-Wunreachable-code

export RS_PROTOCOL_SRC_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

export RS_PROTOCOL_OBJS_DIR ?= $(abspath $(RS_PROTOCOL_SRC_DIR)/build/rspacketslib)

export RS_PROTOCOL_LIB_FILE ?= $(abspath $(RS_PROTOCOL_SRC_DIR)/build/rspacketslib/rspacketslib.a)

export RS_PROTOCOL_INCLUDES += -I$(RS_PROTOCOL_SRC_DIR)/include

export RS_PROTOCOL_CFILES := \
			ieee754_network.c \
			lambda_registry.c \
			rs_packets.c

export RS_PROTOCOL_HFILES := \
			ieee754_network.h \
			lambda_registry.h \
			rs_packets.h

export RS_PROTOCOL_OBJS = $(addprefix $(RS_PROTOCOL_OBJS_DIR)/, $(RS_PROTOCOL_CFILES:.c=.o))
