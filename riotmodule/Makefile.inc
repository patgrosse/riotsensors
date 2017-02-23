RS_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

# variables for
RS_BUILD_DIR ?= $(RS_DIR)/build
RS_PACKETS_DIR ?= $(RS_DIR)/../packets
LIBSPT_DIR ?= $(RS_BUILD_DIR)/libspt
LIBSPT_GIT_URL ?= https://gitlab.com/patgrosse/libspt.git

# get libspt specific variables
include $(LIBSPT_DIR)/Makefile.inc

# get riotsensors packets specific variables
include $(RS_PACKETS_DIR)/Makefile.inc

# RIOT specific variables
APPDEPS += $(LIBSPT_LIB_FILE) $(RS_PACKETS_LIB_FILE)
EXTERNAL_MODULE_DIRS += $(RS_DIR)
USEMODULE += riotsensors_module
INCLUDES += -I$(RS_DIR)/include -I$(LIBSPT_SRC_DIR)/include -I$(RS_PACKETS_DIR)/include
ADDITIONAL_INCLUDES = $(addprefix -I, $(USEMODULE_INCLUDES)) $(INCLUDES)

# variables passed to libspt
LIBSPT_CFLAGS := -DNO_LIBEVENT=1 -DNO_TERMIOS=1 $(CFLAGS)
LIBSPT_INCLUDES = $(ADDITIONAL_INCLUDES)
RS_PACKETS_CFLAGS = $(CFLAGS)
RS_PACKETS_INCLUDES = $(ADDITIONAL_INCLUDES)

# get libspt from git
$(LIBSPT_DIR)/Makefile.inc:
	mkdir -p "$(RS_BUILD_DIR)" && cd $(RS_BUILD_DIR)
	git clone $(LIBSPT_GIT_URL) "$(LIBSPT_DIR)"

# target for libspt static library
$(LIBSPT_LIB_FILE):
	$(MAKE) -C "$(LIBSPT_DIR)"

# target for riotsensors packets static library
$(RS_PACKETS_LIB_FILE):
	$(MAKE) -C "$(RS_PACKETS_DIR)"