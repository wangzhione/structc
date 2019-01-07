#
# 编译的目录结构
# Release : make 
# Debug   : make D=-D_DEBUG
# Clean   : make clean
#
ROOT        ?= structc

DMAIN       ?= $(ROOT)/main
DTEST       ?= $(ROOT)/test

DSYSTEM     ?= $(ROOT)/system
DSTRUCT	    ?= $(ROOT)/struct
DBASE 	    ?= $(ROOT)/base

DOUT        ?= Out
DOBJ        ?= $(DOUT)/Obj

#
# DIRS      : 所有可变编译文件目录
# IINC      : -I 需要导入的 include 目录
# SRCC      : 所有 .c 文件
#
DIRS      = $(DSYSTEM) $(DSTRUCT) $(DBASE)
IINC      = $(foreach v, $(DIRS),-I$(v))
SRCC      = $(wildcard $(foreach v, $(DMAIN) $(DIRS) $(DTEST),$(v)/*.c))

OBJC      = $(wildcard $(foreach v, $(DIRS),$(v)/*.c))
OBJO      = $(foreach v, $(OBJC), $(notdir $(basename $(v))).o)

TESTC     = $(wildcard $(DTEST)/*.c)
TESTO     = $(foreach v, $(TESTC), $(notdir $(basename $(v))).o)

#
# 全局编译的设置
#
CC       = gcc
CFLAGS   = -g -O2 -Wall -Wno-unused-result
LIB      = -lm -lpthread -luv

RHAD     = $(CC) $(CFLAGS) $(D)
RTAL     = $(foreach v, $^,$(DOBJ)/$(v)) $(LIB)
RUNO     = $(RHAD) $(IINC) -c -o $(DOBJ)/$@ $<
RUN      = $(RHAD) $(IINC) -o $(DOUT)/$@ $(RTAL)

#
# 具体的产品生产
#
.PHONY : all clean

all : main.exe

#
# 主运行程序main
#
main.exe : structc.a
	$(RUN)

#
# 循环产生 -> 所有 - 链接文件 *.o
#
define CALL_RUNO
$(notdir $(basename $(1))).o : $(1) | $$(DOUT)
	$$(RUNO)
endef

$(foreach v, $(SRCC), $(eval $(call CALL_RUNO, $(v))))

#
# 生成 structc.a 静态库, 方便处理所有问题
#
structc.a : $(OBJO) $(TESTO) main.o main_init.o main_run.o main_test.o
	ar cr $(DOBJ)/$@ $(DOBJ)/*.o

#
# 程序的收尾工作,清除,目录构建
#
$(DOUT):
	mkdir -p $(DOBJ)
	mkdir -p $@/test
	-cp -r $(ROOT)/conf $@
	-if [ -d $(DTEST)/conf ] ; then cp -r $(DTEST)/conf $@/test ; fi

# 清除操作
clean :
	-rm -rf *core*
	-rm -rf $(DOUT)
	-rm -rf logs $(ROOT)/logs
	-rm -rf Debug Release x64
	-rm -rf $(ROOT)/Debug $(ROOT)/Release $(ROOT)/x64
