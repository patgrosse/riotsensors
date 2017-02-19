RS_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

# variables for
RS_BUILD_DIR ?= $(RS_DIR)/build
LIBSPT_DIR ?= $(RS_BUILD_DIR)/libspt
LIBSPT_GIT_URL ?= https://gitlab.com/patgrosse/libspt.git

# get libspt specific variables
include $(LIBSPT_DIR)/Makefile.inc

# RIOT specific variables
APPDEPS += $(LIBSPT_LIB_FILE)
EXTERNAL_MODULE_DIRS += $(RS_DIR)
USEMODULE += riotsensors
INCLUDES += -I$(RS_DIR)/include -I$(LIBSPT_SRC_DIR)/include
ADDITIONAL_INCLUDES = $(addprefix -I, $(USEMODULE_INCLUDES)) $(INCLUDES)

# variables passed to libspt
LIBSPT_CFLAGS := -DNO_LIBEVENT=1 -DNO_TERMIOS=1 $(CFLAGS)
LIBSPT_INCLUDES = $(ADDITIONAL_INCLUDES)

# get libspt from git
$(LIBSPT_DIR)/Makefile.inc:
	mkdir -p "$(BUILD_DIR)" && cd $(BUILD_DIR)
	git clone $(LIBSPT_GIT_URL) "$(LIBSPT_DIR)"

# target for libspt static library
$(LIBSPT_LIB_FILE):
	$(MAKE) -C "$(LIBSPT_DIR)"