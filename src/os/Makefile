# Build flexpret compatible simple operation system
# RISCV_CC=/data/xpack-riscv-none-embed-gcc-8.3.0-2.3/bin/riscv-none-embed-gcc
# RISCV_OBJDUMP=/data/xpack-riscv-none-embed-gcc-8.3.0-2.3/bin/riscv-none-embed-objdump
# RISCV_OBJCOPY=/data/xpack-riscv-none-embed-gcc-8.3.0-2.3/bin/riscv-none-embed-objcopy
# FLEXPRET_CFLAGS=-march=rv32i -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -L. -nostartfiles -lc -O0

# before modify
# RISCV_CC=riscv64-unknown-elf-gcc
# RISCV_OBJDUMP=riscv64-unknown-elf-objdump
# RISCV_OBJCOPY=riscv64-unknown-elf-objcopy
# FLEXPRET_CFLAGS=-m32 -msoft-float

# after modify
RISCV_CC=/data/riscv/bin/riscv64-unknown-elf-gcc
RISCV_OBJDUMP=/data/riscv/bin/riscv64-unknown-elf-objdump
RISCV_OBJCOPY=/data/riscv/bin/riscv64-unknown-elf-objcopy
FLEXPRET_CFLAGS=-m32 -O2

FLEXPRET_OS_SRC := $(wildcard src/*.c)
FLEXPRET_OS_APP_SRC := $(wildcard app/*.c)
FLEXPRET_OS_INCLUDE := $(wildcard include/*.h)
FLEXPRET_OS_ASM := $(wildcard asm/*.S)
FLEXPRET_OS_COBJ := $(patsubst %.c,%.o,$(FLEXPRET_OS_SRC))
FLEXPRET_OS_APP_COBJ := $(patsubst %.c,%.o,$(FLEXPRET_OS_APP_SRC))
FLEXPRET_OS_SOBJ := $(patsubst %.S,%.o,$(FLEXPRET_OS_ASM))
INCLUDE := include

all: os_all

$(FLEXPRET_OS_APP_COBJ): %.o: %.c $(FLEXPRET_OS_INCLUDE)
	$(RISCV_CC) $(FLEXPRET_CFLAGS) -I$(INCLUDE) -o $@ -c $<

$(FLEXPRET_OS_COBJ): %.o: %.c $(FLEXPRET_OS_INCLUDE)
	$(RISCV_CC) $(FLEXPRET_CFLAGS) -I$(INCLUDE) -o $@ -c $<

$(FLEXPRET_OS_SOBJ): %.o: %.S $(FLEXPRET_OS_INCLUDE)
	$(RISCV_CC) $(FLEXPRET_CFLAGS) -I$(INCLUDE) -o $@ -c $<

# layout.ld is the arg of `-Xlinker -T`
flexpret_os: layout.ld $(FLEXPRET_OS_APP_COBJ) $(FLEXPRET_OS_COBJ) $(FLEXPRET_OS_SOBJ)
	$(RISCV_CC) $(FLEXPRET_CFLAGS) -I$(INCLUDE) -nostdlib -Xlinker -defsym -Xlinker TEXT_START_ADDR=0x00000000 -Xlinker -defsym -Xlinker DATA_START_ADDR=0x20000000 -T $^ -o $@

os.inst.mem.ins: flexpret_os
	$(RISCV_OBJDUMP) -d --section=.text flexpret_os > os.inst.mem.ins
	$(RISCV_OBJDUMP) --disassemble-all --section=.text --section=.data --section=.bss flexpret_os > os.dump

os.inst.mem: flexpret_os
	$(RISCV_OBJCOPY) --only-section .text -O binary flexpret_os os.inst
	hexdump -v -e '1/4 "%08X" "\n"' os.inst > os.inst.mem

os.data.mem: flexpret_os
	$(RISCV_OBJCOPY) --only-section .data --only-section .bss -O binary flexpret_os os.data
	hexdump -v -e '1/4 "%08X" "\n"' os.data > os.data.mem

os_all: os.inst.mem os.data.mem os.inst.mem.ins

# emulator and testbench
# CORE=/opt/flexpret/emulator/generated-src/4tf-16i-16d-ti
CORE=/opt/flexpret/emulator/generated-src/8tf-64i-64d-ti
TESTBENCH=/opt/flexpret/emulator/testbench
Core.o:${CORE}/Core.cpp ${CORE}/*.h
	g++ -g -O2 -c ${CORE}/Core.cpp -o Core.o
Core-os.o:${TESTBENCH}/Core-tb-os.cpp ${CORE}/*.h
	g++ -g -O2 -I${CORE} -c ${TESTBENCH}/Core-tb-os-gen.cpp -o Core-os.o
Core:Core.o Core-os.o
	g++ -g -O2 -o Core Core.o Core-os.o

# build test source file
test: test.inst.mem.ins test.inst.mem test.data.mem

FLEXPRET_OS_TEST_SRC := $(wildcard test/*.c)
FLEXPRET_OS_TEST_COBJ := $(patsubst %.c,%.o,$(FLEXPRET_OS_TEST_SRC))

$(FLEXPRET_OS_TEST_COBJ): %.o: %.c $(FLEXPRET_OS_INCLUDE)
	$(RISCV_CC) $(FLEXPRET_CFLAGS) -I$(INCLUDE) -o $@ -c $<

flexpret_test_os: layout.ld $(FLEXPRET_OS_COBJ) $(FLEXPRET_OS_SOBJ) $(FLEXPRET_OS_TEST_COBJ)
	$(RISCV_CC) $(FLEXPRET_CFLAGS) -I$(INCLUDE) -nostdlib -Xlinker -defsym -Xlinker TEXT_START_ADDR=0x00000000 -Xlinker -defsym -Xlinker DATA_START_ADDR=0x20000000 -T $^ -o $@

test.inst.mem.ins: flexpret_test_os
	$(RISCV_OBJDUMP) -d --section=.text flexpret_test_os > test.inst.mem.ins
	$(RISCV_OBJDUMP) --disassemble-all --section=.text --section=.data --section=.bss flexpret_test_os > test.dump

test.inst.mem: flexpret_test_os
	$(RISCV_OBJCOPY) --only-section .text -O binary flexpret_test_os test.inst
	hexdump -v -e '1/4 "%08X" "\n"' test.inst > test.inst.mem

test.data.mem: flexpret_test_os
	$(RISCV_OBJCOPY) --only-section .data --only-section .bss -O binary flexpret_test_os test.data
	hexdump -v -e '1/4 "%08X" "\n"' test.data > test.data.mem

FLEXPRET_WCET_SRC := $(wildcard test/malardalen_wcet/*.c)
FLEXPRET_WCET_COBJ := $(patsubst %.c,%.o,$(FLEXPRET_WCET_SRC))
FLEXPRET_WCET_ELF := $(patsubst %.c,%_wcet.out,$(FLEXPRET_WCET_SRC))
FLEXPRET_WCET_INST := $(patsubst %.c,%_wcet.inst.mem,$(FLEXPRET_WCET_SRC))
FLEXPRET_WCET_DATA := $(patsubst %.c,%_wcet.data.mem,$(FLEXPRET_WCET_SRC))

wcet: $(FLEXPRET_WCET_INST) $(FLEXPRET_WCET_DATA)

$(FLEXPRET_WCET_COBJ): %.o: %.c $(FLEXPRET_OS_INCLUDE)
	$(RISCV_CC) $(FLEXPRET_CFLAGS) -I$(INCLUDE) -o $@ -c $<

$(FLEXPRET_WCET_ELF): %_wcet.out: layout.ld %.o $(FLEXPRET_OS_COBJ) $(FLEXPRET_OS_SOBJ)
	$(RISCV_CC) $(FLEXPRET_CFLAGS) -I$(INCLUDE) -nostdlib -Xlinker -defsym -Xlinker TEXT_START_ADDR=0x00000000 -Xlinker -defsym -Xlinker DATA_START_ADDR=0x20000000 -T $^ -o $@

$(FLEXPRET_WCET_INST): %_wcet.inst.mem: %_wcet.out
	$(RISCV_OBJCOPY) --only-section .text -O binary $< $(patsubst %_wcet.inst.mem,%.inst.tmp,$@)
	hexdump -v -e '1/4 "%08X" "\n"' $(patsubst %_wcet.inst.mem,%.inst.tmp,$@) > $@
	rm $(patsubst %_wcet.inst.mem,%.inst.tmp,$@)

$(FLEXPRET_WCET_DATA): %_wcet.data.mem: %_wcet.out
	$(RISCV_OBJCOPY) --only-section .data --only-section .bss -O binary $< $(patsubst %_wcet.data.mem,%.data.tmp,$@)
	hexdump -v -e '1/4 "%08X" "\n"' $(patsubst %_wcet.data.mem,%.data.tmp,$@) > $@
	rm $(patsubst %_wcet.data.mem,%.data.tmp,$@)

FLEXPRET_CMSIS_INCLUDE := test/cmsis_validation/include
FLEXPRET_CMSIS_SRC := $(wildcard test/cmsis_validation/*.c)
FLEXPRET_CMSIS_COBJ := $(patsubst %.c,%.o,$(FLEXPRET_CMSIS_SRC))

$(FLEXPRET_CMSIS_COBJ): %.o: %.c $(FLEXPRET_OS_INCLUDE)
	$(RISCV_CC) $(FLEXPRET_CFLAGS) -I$(INCLUDE) -I$(FLEXPRET_CMSIS_INCLUDE) -o $@ -c $<

cmsis.out: layout.ld $(FLEXPRET_CMSIS_COBJ) $(FLEXPRET_OS_COBJ) $(FLEXPRET_OS_SOBJ)
	$(RISCV_CC) $(FLEXPRET_CFLAGS) -I$(INCLUDE) -I$(FLEXPRET_CMSIS_INCLUDE) -nostdlib -Xlinker -defsym -Xlinker TEXT_START_ADDR=0x00000000 -Xlinker -defsym -Xlinker DATA_START_ADDR=0x20000000 -T $^ -o $@

cmsis.inst.mem.ins: cmsis.out
	$(RISCV_OBJDUMP) -d --section=.text cmsis.out > cmsis.inst.mem.ins
	$(RISCV_OBJDUMP) --disassemble-all --section=.text --section=.data --section=.bss cmsis.out > cmsis.dump

cmsis.inst.mem: cmsis.out
	$(RISCV_OBJCOPY) --only-section .text -O binary cmsis.out cmsis.inst
	hexdump -v -e '1/4 "%08X" "\n"' cmsis.inst > cmsis.inst.mem

cmsis.data.mem: cmsis.out
	$(RISCV_OBJCOPY) --only-section .data --only-section .bss -O binary cmsis.out cmsis.data
	hexdump -v -e '1/4 "%08X" "\n"' cmsis.data > cmsis.data.mem

cmsis: cmsis.inst.mem.ins cmsis.inst.mem cmsis.data.mem

clean:
	rm -f app/*.o asm/*.o src/*.o os.inst.mem.ins os.inst.mem os.data.mem os.data os.inst flexpret_os

cleantest:
	rm -f test/*.o asm/*.o src/*.o test.inst.mem.ins test.inst.mem test.data.mem test.data test.inst flexpret_test_os

cleanwcet:
	rm -f test/malardalen_wcet/*.o test/malardalen_wcet/*.out test/malardalen_wcet/*.mem

cleancmsis:
	rm -f test/cmsis_validation/*.o cmsis.inst.mem.ins cmsis.inst.mem cmsis.data.mem cmsis.data cmsis.inst cmsis.out

cleanall: clean cleantest cleanwcet cleancmsis
