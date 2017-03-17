RS_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

# variables for
RS_BUILD_DIR ?= $(RS_DIR)/build
RS_PROTOCOL_DIR ?= $(RS_DIR)/../protocol
LIBSPT_DIR ?= $(RS_BUILD_DIR)/libspt

ifdef GITLAB_USE_SSH
LIBSPT_GIT_URL ?= git@gitlab.com:patgrosse/libspt.git
else
LIBSPT_GIT_URL ?= https://gitlab.com/patgrosse/libspt.git
endif

# get libspt specific variables
include $(LIBSPT_DIR)/Makefile.inc

# get riotsensors protocol specific variables
include $(RS_PROTOCOL_DIR)/Makefile.inc

# RIOT specific variables
APPDEPS += $(LIBSPT_LIB_FILE) $(RS_PROTOCOL_LIB_FILE)
EXTERNAL_MODULE_DIRS += $(RS_DIR)
USEMODULE += riotsensors_module
INCLUDES += -I$(RS_DIR)/include -I$(LIBSPT_SRC_DIR)/include -I$(RS_PROTOCOL_DIR)/include
ADDITIONAL_INCLUDES = $(addprefix -I, $(USEMODULE_INCLUDES)) $(INCLUDES)

# variables passed to libspt
LIBSPT_CFLAGS := -DNO_LIBEVENT=1 -DNO_TERMIOS=1 $(CFLAGS)
LIBSPT_INCLUDES = $(ADDITIONAL_INCLUDES)
RS_PROTOCOL_CFLAGS := $(CFLAGS)
RS_PROTOCOL_INCLUDES = $(ADDITIONAL_INCLUDES)

# get libspt from git
$(LIBSPT_DIR)/Makefile.inc:
	mkdir -p "$(RS_BUILD_DIR)" && cd $(RS_BUILD_DIR)
	git clone $(LIBSPT_GIT_URL) "$(LIBSPT_DIR)"

# target for libspt static library
$(LIBSPT_LIB_FILE):
	$(MAKE) -C "$(LIBSPT_DIR)"

# target for riotsensors protocol static library
$(RS_PROTOCOL_LIB_FILE):
	$(MAKE) -C "$(RS_PROTOCOL_DIR)"
