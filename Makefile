#
# 编译的目录结构
# Release : make 
# Debug   : make D=-D_DEBUG
# Clean   : make clean
#
ROOT        ?= structc

DMAIN       ?= main
DTEST       ?= test

DSYSTEM     ?= system
DSTRUCT	    ?= struct
DBASE 	    ?= base

OUTS        ?= Outs
DOBJ        ?= Obj

#
# DIRS      : 所有可变编译文件目录
# IINC      : -I 需要导入的 include 目录
# SRCC      : 所有 .c 文件
#
DIRS    =   $(DSYSTEM) $(DSTRUCT) $(DBASE)
IINC    =   $(foreach v, $(DIRS),-I$(ROOT)/$(v))
SRCC    =   $(wildcard $(foreach v, $(DMAIN) $(DIRS) $(DTEST),$(ROOT)/$(v)/*.c))

OBJC    =   $(wildcard $(foreach v, $(DIRS),$(ROOT)/$(v)/*.c))
OBJO    =   $(foreach v, $(OBJC), $(notdir $(basename $(v))).o)
OBJP    =   $(OUTS)/$(DOBJ)/

TESTC   =   $(wildcard $(ROOT)/$(DTEST)/*.c)
TESTO   =   $(foreach v, $(TESTC), $(notdir $(basename $(v))).o)

#
# 全局编译的设置
#
CC      = gcc
CFLAGS  = -g -O2 -Wall -Wno-unused-result
LIB     = -lm -lpthread -luv

RHAD    = $(CC) $(CFLAGS) $(D)
RTAL    = $(foreach v, $^,$(OBJP)$(v)) $(LIB)
RUNO    = $(RHAD) $(IINC) -c -o $(OBJP)$@ $<
RUN     = $(RHAD) $(IINC) -o $(OUTS)/$@ $(RTAL)

#
# 具体的产品生产
#
.PHONY : all clean

all : main.exe

#
# 主运行程序main
#
main.exe : main.o main_init.o main_test.o main_run.o structc.a
	$(RUN)

#
# 循环产生 -> 所有 - 链接文件 *.o
#
define CALL_RUNO
$(notdir $(basename $(1))).o : $(1) | $$(OUTS)
	$$(RUNO)
endef

$(foreach v, $(SRCC), $(eval $(call CALL_RUNO, $(v))))

#
# 生成 structc.a 静态库, 方便处理所有问题
#
structc.a : $(OBJO) $(TESTO)
	$(RHAD) -c -o $(OBJP)stdext.o $(ROOT)/$(DSYSTEM)/stdext.c -DJEMALLOC_NO_DEMANGLE -l:libjemalloc.a
	ar cr $(OBJP)$@ $(foreach v,$^,$(OBJP)$(v))

#
# 程序的收尾工作,清除,目录构建
#
$(OUTS):
	mkdir -p $(OBJP)
	mkdir -p $@/$(DTEST)
	-cp -r $(ROOT)/conf $@
	-cp -r $(ROOT)/$(DTEST)/conf $@/$(DTEST)

# 清除操作
clean :
	-rm -rf *core*
	-rm -rf $(OUTS)
	-rm -rf logs $(ROOT)/logs
	-rm -rf Debug Release x64
	-rm -rf $(ROOT)/Debug $(ROOT)/Release $(ROOT)/x64
