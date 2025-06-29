#
# Top level Makefile, the real shis is at structc/Makefile
#
# Debug   : make
# Clean   : make clean
# Release : make D=-DNDEBUG
#

.PHONY: all env

all: env
	cd modular && $(MAKE)

env:
	@echo "Operating System: $(shell uname -s)"            # 操作系统
	@echo "CPU Architecture: $(shell uname -m)"            # CPU 架构
	@echo "GCC Path: $(shell which gcc)"                   # gcc 编译器的路径
	@echo -n "GCC Version: " && gcc --version | head -n 1  # gcc 版本信息
	@echo

default: all
