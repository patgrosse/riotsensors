RS_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
RS_APP_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

# directories for library builds
RS_BUILD_DIR ?= $(RS_APP_DIR)/bin/riotsensors/$(BOARD)
RS_PROTOCOL_DIR ?= $(RS_DIR)/../protocol
RS_PROTOCOL_OBJS_DIR ?= $(RS_BUILD_DIR)/protocol
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
LIBSPT_CFLAGS = -DNO_LIBEVENT=1 -DNO_TERMIOS=1 $(CFLAGS)
LIBSPT_INCLUDES = $(ADDITIONAL_INCLUDES)

# variables passed to riotsensors protocol
RS_PROTOCOL_CFLAGS = $(CFLAGS)
RS_PROTOCOL_INCLUDES = $(ADDITIONAL_INCLUDES)


# ==== BEGIN CoAP includes and flags
ifndef RS_NO_COAP
  # Include packages that pull up and auto-init the link layer.
  # NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
  USEMODULE += gnrc_netdev_default
  USEMODULE += auto_init_gnrc_netif
  # Specify the mandatory networking modules for IPv6 and UDP
  USEMODULE += gnrc_ipv6_default
  USEMODULE += gnrc_udp
  # Additional networking modules that can be dropped if not needed
  USEMODULE += gnrc_icmpv6_echo

  USEMODULE += gnrc_sock_udp

  USEPKG += microcoap
  CFLAGS += -DMICROCOAP_DEBUG

  # include for printing IP addresses
  USEMODULE += shell_commands

  # Set a custom channel if needed
  ifneq (,$(filter cc110x,$(USEMODULE)))          # radio is cc110x sub-GHz
    DEFAULT_CHANNEL ?= 0
    CFLAGS += -DCC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  else
    ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
      DEFAULT_CHANNEL ?= 5
      CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(DEFAULT_CHANNEL)
    else                                          # radio is IEEE 802.15.4 2.4 GHz
      DEFAULT_CHANNEL ?= 26
      CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
    endif
  endif
else
  CFLAGS += -DRS_NO_COAP
endif
# ==== END CoAP includes and flags

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
