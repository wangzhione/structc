#
# Top level Makefile, the real shis is at structc/Makefile
#

default: all

.DEFAULT:
	cd modular && make $@
