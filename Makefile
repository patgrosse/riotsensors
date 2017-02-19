# ===================================================
# Makefile that is called by RIOT OS's make structure
# do not execute make here manually
# ===================================================
MODULE = riotsensors

include $(RIOTBASE)/Makefile.base

# not called by RIOT, only for packages
# clean::
#	for cfile in $(LIBSPT_CFILES) ; do \
#		rm -f $$cfile ; \
#	done
#	for hfile in $(LIBSPT_HFILES) ; do \
#		rm -f include/$$hfile ; \
#	done
