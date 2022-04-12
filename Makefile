#
# Top level Makefile, the real shis is at structc/Makefile
#
# Debug   : make
# Clean   : make clean
# Release : make D=-DNDEBUG
#

default: all

.DEFAULT:
	cd modular && make $@
