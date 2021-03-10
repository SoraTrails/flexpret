# Build flexpret compatible simple operation system
RISCV_CC=riscv64-unknown-elf-gcc

FLEXPRET_OS_SRC := $(wildcard src/*.c)
FLEXPRET_OS_INCLUDE := $(wildcard include/*.h)
FLEXPRET_OS_ASM := $(wildcard asm/*.S)
FLEXPRET_OS_COBJ := $(patsubst %.c,%.o,$(FLEXPRET_OS_SRC))
FLEXPRET_OS_SOBJ := $(patsubst %.S,%.o,$(FLEXPRET_OS_ASM))
INCLUDE := include

all: os_all

$(FLEXPRET_OS_COBJ): %.o: %.c $(FLEXPRET_OS_INCLUDE)
	$(RISCV_CC) -m32 -I$(INCLUDE) -o $@ -c $<

$(FLEXPRET_OS_SOBJ): %.o: %.S $(FLEXPRET_OS_INCLUDE)
	$(RISCV_CC) -m32 -I$(INCLUDE) -o $@ -c $<

flexpret_os: $(FLEXPRET_OS_COBJ) $(FLEXPRET_OS_SOBJ)
	${RISCV_CC} -m32 -I$(INCLUDE) -nostdlib -Xlinker -defsym -Xlinker TEXT_START_ADDR=0x00000000 -Xlinker -defsym -Xlinker DATA_START_ADDR=0x20000000 -T layout.ld $^ -o $@

os.inst.mem.ins: flexpret_os
	riscv64-unknown-elf-objdump -d --section=.text flexpret_os > os.inst.mem.ins
	riscv64-unknown-elf-objdump --disassemble-all --section=.text --section=.data --section=.bss flexpret_os > os.dump

os.inst.mem: flexpret_os
	riscv64-unknown-elf-objcopy --only-section .text -O binary flexpret_os os.inst
	hexdump -v -e '1/4 "%08X" "\n"' os.inst > os.inst.mem

os.data.mem: flexpret_os
	riscv64-unknown-elf-objcopy --only-section .data --only-section .bss -O binary flexpret_os os.data
	hexdump -v -e '1/4 "%08X" "\n"' os.data > os.data.mem

os_all: os.inst.mem os.data.mem os.inst.mem.ins

# emulator and testbench
CORE=/opt/flexpret/emulator/generated-src/4tf-16i-16d-ti
TESTBENCH=/opt/flexpret/emulator/testbench
Core.o:${CORE}/Core.cpp ${CORE}/*.h
	g++ -g -O2 -c ${CORE}/Core.cpp -o Core.o
Core-os.o:${TESTBENCH}/Core-tb-os.cpp ${CORE}/*.h
	g++ -g -O2 -I${CORE} -c ${TESTBENCH}/Core-tb-os-gen.cpp -o Core-os.o
Core:Core.o Core-os.o
	g++ -g -O2 -o Core Core.o Core-os.o