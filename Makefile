
# Data
TARGET = main
V=@
C_SRC_EXC = testbench.c
C_SRC = $(filter-out $(C_SRC_EXC), $(wildcard *.c))
C_OBJ_PATH = obj
C_OBJ = $(addprefix $(C_OBJ_PATH)/,$(patsubst %.c, %.o, $(C_SRC)))

# Configs

# PES buffer length
CFG_SUB_DEC_PES_BUF_LEN = 65536

CC_OPT += -D'SUB_DEC_PES_BUF_LEN=$(CFG_SUB_DEC_PES_BUF_LEN)'

DBG_OPT = -g

# Rules
.PHONY: all clean setup test

all: setup $(TARGET)
	@echo "build finish."

clean:
	$(V)rm -f *.o
	$(V)rm -rf $(C_OBJ_PATH)
	$(V)rm -f main

setup:
	$(V)mkdir -p obj

$(TARGET):$(C_OBJ)
	@echo [LD] $<
	$(V)gcc -o $(TARGET) $(C_OBJ)
	
$(C_OBJ_PATH)/%.o:%.c
	@echo [CC] $<
	$(V)gcc $(DBG_OPT) -c $(CC_OPT) -o $@ $<

test:
	@echo "C_SRC_EXC=$(C_SRC_EXC)"
	@echo "C_SRC=$(C_SRC)"
	@echo "C_OBJ_PATH=$(C_OBJ_PATH)"
	@echo "C_OBJ=$(C_OBJ)"
