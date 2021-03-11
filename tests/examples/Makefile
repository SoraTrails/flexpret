CORE=/opt/flexpret/emulator/generated-src/4tf-16i-16d-ti
TESTBENCH=/opt/flexpret/emulator/testbench
RISCV_OBJCOPY=riscv64-unknown-elf-objcopy
RISCV_CC=riscv64-unknown-elf-gcc
ELF_NAME=sboth

all:Core Core-my sboth.elf

# run: all
# 	riscv64-unknown-elf-objdump -d --section=.text sboth.elf > sboth.inst.mem.ins
# 	./Core --maxcycles=150000 --ispm=sboth.inst.mem --dspm=sboth.data.mem --trace > sboth.out

# emulator and testbench
Core.o:${CORE}/Core.cpp ${CORE}/*.h
	g++ -g -O2 -c ${CORE}/Core.cpp -o Core.o
Core-tb.o:${TESTBENCH}/Core-tb-sboth.cpp ${CORE}/*.h
	g++ -g -O2 -I${CORE} -c ${TESTBENCH}/Core-tb-sboth.cpp -o Core-tb.o
Core-my.o:${TESTBENCH}/Core-tb-sboth.cpp ${CORE}/*.h
	g++ -g -O2 -I${CORE} -c ${TESTBENCH}/Core-tb-sboth-gen.cpp -o Core-sboth.o
Core-my:Core.o Core-my.o
	g++ -g -O2 -o Core-my Core.o Core-my.o
Core:Core.o Core-tb.o Core-my.o
	g++ -g -O2 -o Core Core.o Core-tb.o
	g++ -g -O2 -o Core-my Core.o Core-my.o

# elf gen
sboth.o:both_comm.c 
	${RISCV_CC} -m32 -I../include -c ./both_comm.c -o sboth.o
startup.o:../include/startup.S
	${RISCV_CC} -m32 -I../include -c ../include/startup.S -o startup.o
sboth.elf:sboth.o startup.o ../include/layout.ld
	${RISCV_CC} -m32 -nostdlib -Itests/include -Xlinker -defsym -Xlinker TEXT_START_ADDR=0x00000000 -Xlinker -defsym -Xlinker DATA_START_ADDR=0x20000000 -T ../include/layout.ld sboth.o startup.o -o sboth.elf

sboth.inst.mem.ins: sboth.elf
	riscv64-unknown-elf-objdump -d --section=.text sboth.elf > sboth.inst.mem.ins
	riscv64-unknown-elf-objdump --disassemble-all --section=.text --section=.data --section=.bss sboth.elf > sboth.dump

sboth.inst.mem: sboth.elf
	${RISCV_OBJCOPY} --only-section .text -O binary sboth.elf sboth.inst
	hexdump -v -e '1/4 "%08X" "\n"' sboth.inst > sboth.inst.mem

sboth.data.mem: sboth.elf
	${RISCV_OBJCOPY} --only-section .data --only-section .bss -O binary sboth.elf sboth.data
	hexdump -v -e '1/4 "%08X" "\n"' sboth.data > sboth.data.mem

sboth_all: sboth.inst.mem sboth.data.mem sboth.inst.mem.ins

clean:
	rm -f *.o sboth.*
