.DEFAULT_GOAL = app

# Add necessary options if the target is a shared library
ifeq ($(SHARE),1)
SO = -so
CFLAGS  += -fPIC
LDFLAGS += -rdynamic -shared -fPIC
endif

WORK_DIR  = $(shell pwd)
BUILD_DIR = $(WORK_DIR)/build

INC_PATH := $(WORK_DIR)/include $(INC_PATH)
#目标文件的路径 /build/obj-riscv32-nemu-interpreter
OBJ_DIR  = $(BUILD_DIR)/obj-$(NAME)$(SO)
BINARY   = $(BUILD_DIR)/$(NAME)$(SO)

# Compilation flags
ifeq ($(CC),clang)
CXX := clang++
else
CXX := g++
endif
#LD = g++
LD := $(CXX)
INCLUDES = $(addprefix -I, $(INC_PATH))
CFLAGS  := -O2 -MMD -Wall $(INCLUDES) $(CFLAGS)
#删掉了werror
LDFLAGS := -O2 $(LDFLAGS)

OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o) $(CXXSRC:%.cc=$(OBJ_DIR)/%.o)

# Compilation patterns
#echo + CC src/nemu-main.c
#mkdir -p /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/
#gcc -O2 -MMD -Wall -I/home/lcx/ics2022/nemu/include -I/home/lcx/ics2022/nemu/src/isa/riscv32/include -I/home/lcx/ics2022/nemu/src/engine/interpreter -O2  -Og -ggdb3 -fsanitize=address -DITRACE_COND=true -D__GUEST_ISA__=riscv32 -c -o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o src/nemu-main.c
#/home/lcx/ics2022/nemu/tools/fixdep/build/fixdep  /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.d  /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o unused >  /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.d.tmp
#mv  /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.d.tmp  /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.d

$(OBJ_DIR)/%.o: %.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<
	$(call call_fixdep, $(@:.o=.d), $@)

$(OBJ_DIR)/%.o: %.cc
	@echo + CXX $<
	@mkdir -p $(dir $@)
	@$(CXX) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<
	$(call call_fixdep, $(@:.o=.d), $@)

# Depencies
-include $(OBJS:.o=.d)

# Some convenient rules

.PHONY: app clean

app: $(BINARY)

#echo + LD /home/lcx/ics2022/nemu/build/riscv32-nemu-interpreter
#g++ -o /home/lcx/ics2022/nemu/build/riscv32-nemu-interpreter /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/device/device.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/device/alarm.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/device/intr.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/device/serial.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/device/timer.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/device/keyboard.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/device/vga.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/device/audio.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/device/disk.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/isa/riscv32/init.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/isa/riscv32/reg.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/isa/riscv32/system/mmu.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/isa/riscv32/system/intr.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/isa/riscv32/inst.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/isa/riscv32/logo.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/isa/riscv32/difftest/dut.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/cpu/cpu-exec.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/cpu/difftest/ref.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/cpu/difftest/dut.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/sdb/expr.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/sdb/sdb.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/sdb/watchpoint.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/monitor.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/utils/state.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/utils/rand.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/utils/timer.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/utils/log.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/memory/vaddr.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/memory/paddr.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/engine/interpreter/init.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/engine/interpreter/hostcall.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/device/io/mmio.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/device/io/port-io.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/device/io/map.o /home/lcx/ics2022/nemu/build/obj-riscv32-nemu-interpreter/src/utils/disasm.o -O2 -O2  -Og -ggdb3 -fsanitize=address  -lLLVM-14 -lreadline -ldl -pie -lSDL2

$(BINARY): $(OBJS) $(ARCHIVES)
	@echo + LD $@
	@$(LD) -o $@ $(OBJS) $(LDFLAGS) $(ARCHIVES) $(LIBS)

clean:
	-rm -rf $(BUILD_DIR)
