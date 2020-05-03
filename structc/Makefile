#
# 编译的目录结构
# Debug   : make
# Clean   : make clean
# Release : make D=-DNDEBUG
#
ROOT      ?= ..

DMAIN     := main
DTEST     := test

#
# DIRS    : 所有待编译库文件目录集
#
DIRS      := system struct base

DOUT      := $(ROOT)/bin
DOBJ      := $(DOUT)/.o

#
# IINC    : -I 需要导入的 include 目录
# SRCC    : 所有 .c 文件
#
IINC      = $(foreach v, $(DIRS),-I$(v))
SRCC      = $(wildcard $(foreach v, $(DMAIN) $(DIRS) $(DTEST),$(v)/*.c))

OBJC      = $(wildcard $(foreach v, $(DIRS),$(v)/*.c))
OBJO      = $(foreach v, $(OBJC), $(notdir $(basename $(v))).o)

TESTC     = $(wildcard $(DTEST)/*.c)
TESTO     = $(foreach v, $(TESTC), $(notdir $(basename $(v))).o)

#
# 全局编译的设置
#
CC        ?= gcc
CFLAGS    := -g -O2 -Wall -Wextra -Wimplicit-fallthrough=0 -Wno-sign-compare -D_GNU_SOURCE
LIB       := -lm -lpthread

RHAD    := $(CC) $(CFLAGS)
ifneq ( , $(D))
	RHAD+=$(D)
endif

RTAL      = $(foreach v, $^,$(DOBJ)/$(v)) $(LIB)
RUNO      = $(RHAD) $(IINC) -c -o $(DOBJ)/$@ $<
RUN       = $(RHAD) $(IINC) -o $(DOUT)/$@ $(RTAL)

#
# 具体的产品生产
#
.PHONY : all clean

all : main

#
# 主运行程序main
#
main : libstructc.a
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
libstructc.a : $(OBJO) $(TESTO) main.o init.o exec.o test.o
	ar cr $(DOBJ)/$@ $(DOBJ)/*.o

#
# 程序的收尾工作,清除,目录构建
#
$(DOUT):
	mkdir -p $(DOBJ)
	-if [ -d conf ] ; then cp -r conf $@ ; fi
	-if [ -d $(DTEST)/conf ] ; then mkdir -p $@/test ; cp -r $(DTEST)/conf $@/test ; fi

# 清除操作
clean :
	-rm -rf *core*
	-rm -rf $(DOUT)
	-rm -rf logs $(ROOT)/logs
	-rm -rf Debug Release x64
	-rm -rf $(ROOT)/Debug $(ROOT)/Release $(ROOT)/x64
