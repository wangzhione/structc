#
# 编译的目录结构
# Debug   : make
# Clean   : make clean
# Release : make D=-DNDEBUG
#

ROOT      ?= ..

#
# DMAIN	  : main 启动主目录
# DTEST	  : test 单元测试目录
# DSRC	  : 基础源代码目录
# DMODULE : module 拓展模块目录集
# DIRS    : 所有待编译库文件目录集
#
DMAIN     := main
DTEST     := test
DSRC      := system struct module
DMODULE   := module/conf
DIRS      := $(DSRC) $(DMODULE) $(DMAIN)

#
# DOUT    : 项目输出目录
# DOBJ    : obj 二进制对象输出文件目录
#
DOUT      := $(ROOT)/bin
DOBJ      := $(DOUT)/.o

#
# IINC    : -I 需要导入的 include 目录
# SRCC    : 所有 *.c 文件
# OBJO    : 所有 *.o 文件
#
IINC      = $(foreach v, $(DSRC),-I$(v))
SRCC      = $(wildcard $(foreach v, $(DIRS),$(v)/*.c))
OBJO      = $(foreach v, $(SRCC),$(DOBJ)/$(notdir $(basename $(v))).o)

# src c source filter out main/main.c main/exec.c
# 
ALLSRCC   = $(wildcard $(foreach v, $(DIRS) $(DTEST),$(v)/*.c))
TESTSRCC  = $(filter-out main/main.c main/exec.c, $(ALLSRCC))
TESTOBJO  = $(foreach v, $(TESTSRCC),$(DOBJ)/$(notdir $(basename $(v))).o)

#
# 全局编译的设置
#
CC        ?= cc
#
# -Werror                           : 使所有警告变为错误
# -Wall -Wextra                     : 启用一些很有用的警告选项
# -Wimplicit-fallthrough=0          : 忽略 switch case missing break warning
# -Wno-missing-field-initializers   : 忽略 initializer for field warning
#
CFLAGS    := -g -Wall -Wextra -Wno-missing-field-initializers -D_GNU_SOURCE
LIB       := -lm -lpthread

ifneq ( , $(D))
	PREFIX=$(CC) -O3 $(CFLAGS) $(D)
else
	PREFIX=$(CC) -O2 $(CFLAGS)
endif

RUNO      = $(PREFIX) $(IINC) -c -o $@ $^
RUN       = $(PREFIX) $(IINC) -o $(DOUT)/$@ $^ $(LIB)

#
# 具体的产品生产
#
.PHONY : all clean test

all : main.out

test : test.out

#
# 主运行程序 main
#
main.out : $(OBJO)
	$(RUN)

test.out : $(TESTOBJO)
	$(RUN)

#
# 循环产生 -> 所有 - 链接文件 *.o
#
define CALL_RUNO
$(DOBJ)/$(notdir $(basename $(1))).o : $(1) | $$(DOUT)
	$$(RUNO)
endef

$(foreach v, $(ALLSRCC), $(eval $(call CALL_RUNO, $(v))))

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
