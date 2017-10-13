#
# 前期编译一些目录结构使用宏
#
ROOT		?= ./structc

MAIN_DIR	?= main

STRUCT_DIR	?= struct
SYSTEM_DIR	?= system
BASE_DIR 	?= base

TEST_DIR	?= test

OUTS		?= ./Output
OBJ_DIR		?= obj

EXE			?= out

#
# DIRS		: 所有可变编译文件目录
# IINC		: -I 需要导入的include 目录
# SRCC		: 所有 .c 文件
#
DIRS	=	$(STRUCT_DIR) $(SYSTEM_DIR) $(BASE_DIR)

IINC	=	$(foreach v, $(DIRS), -I$(ROOT)/$(v))
SRCC	=	$(wildcard $(foreach v, $(MAIN_DIR) $(DIRS) $(TEST_DIR), $(ROOT)/$(v)/*.c))

OBJC	=	$(wildcard $(foreach v, $(DIRS), $(ROOT)/$(v)/*.c))
OBJO	=	$(foreach v, $(OBJC), $(notdir $(basename $(v))).o)
OBJP	=	$(OUTS)/$(OBJ_DIR)/

TESTC	=	$(wildcard $(ROOT)/$(TEST_DIR)/*.c)
TESTE	=	$(foreach v, $(TESTC), $(notdir $(basename $(v))).$(EXE))

#
# 全局编译的设置
#
CC		= gcc
LIB 	= -lpthread -lm
CFLAGS 	= -g -O2 -Wall -Wno-unused-result -std=gnu11

RHAD	= $(CC) $(CFLAGS) $(IINC)
RTAL	= $(foreach v, $^, $(OBJP)$(v)) $(LIB)
RUNO	= $(RHAD) -c -o $(OBJP)$@ $<
RUN		= $(RHAD) -o $(OUTS)/$@ $(RTAL)

# 单元测试使用, 生成指定主函数的运行程序, 替换回main操作
RUNT	= $(RHAD) -o $(OUTS)/$(TEST_DIR)/$@ $(RTAL)
COPT	= objcopy --redefine-sym $(basename $@)=main $(OBJP)$(basename $@).o

#
# 具体的产品生产								
#
.PHONY : all clean

all : main.$(EXE) $(TESTE)

#
# 主运行程序main
#
main.$(EXE) : main.o main_run.o main_test.o librunc.a
	$(RUN)

#
# -> 单元测试程序生成
#
define CALL_TEST
$(1) : $$(notdir $$(basename $(1))).o librunc.a | $$(OUTS)
	$$(COPT)
	$$(RUNT)
endef

$(foreach v, $(TESTE), $(eval $(call CALL_TEST, $(v))))

#
# 循环产生 -> 所有 - 链接文件 *.o
#
define CALL_RUNO
$(notdir $(basename $(1))).o : $(1) | $$(OUTS)
	$$(RUNO)
endef

$(foreach v, $(SRCC), $(eval $(call CALL_RUNO, $(v))))

#
# 生成 libsimplec.a 静态库, 方便处理所有问题
#
librunc.a : $(OBJO)
	ar cr $(OBJP)$@ $(foreach v, $^, $(OBJP)$(v))

#
# 程序的收尾工作,清除,目录构建						
#
$(OUTS):
	-mkdir -p $(OBJP)
	-mkdir -p $@/$(TEST_DIR)
	

# 清除操作
clean :
	-rm -rf $(OUTS)
	-rm -rf Debug Release logs x64
	-rm -rf $(ROOT)/Debug $(ROOT)/logs $(ROOT)/Release $(ROOT)/x64
