# Startup code for multithreaded application, assuming hardware thread 0 is the
# master thread. Thread 0 will run set the stack pointer and run the 'main'
# function. Currently, a hardware thread cannot modify the PC of another
# hardware thread, so all other hardware threads monitor a shared data memory 
# location. When a non-zero value (desired PC) is set, the stack address is set
# from the subsequent word address, and the hardware thread jumps to this value.
# On completion, the thread will reset this value and sleep.
# Assumptions: 
#   HW: 
#       - 4 hardware threads, 16kB data
#       - thread 0 scheduled
#       - All PC on boot/reset is 'reset'
#   SW: 
#       - 'main' function
#
# Michael Zimmer (mzimmer@eecs.berkeley.edu)


.text
.align 4

# code that all threads will run on reset
reset:
# read thread ID
csrr a0, hartid
# t0 (thread ID 0) will run initialization code (init)
beqz a0, init
# all other threads will watch memory location for non-zero value
# compute address (using thread ID)
la a2, startup_state
# a0 = a0 * sizeof(hwthread_state)
slli a0, a0, 2
addi a3, a0, 0
slli a0, a0, 1
add a0, a0, a3

add a2, a2, a0
# check data at address and just loop if 0
wait: 
lw a3, 0(a2)
beqz a3, wait
# desired PC set, load stack address and run startup code
# NOTE(zxj): set tid and move arg to a3 
lw a1, 8(a2) # sp
lw a0, 4(a2) # arg
addi a2, a3, 0
j startup

# executed only by thread 0
init:

# thread 0 will run main
la a1, startup_state
#lw a0, 0(a1)
la a0, main
lw a1, 8(a1)
addi a2, a0, 0
# j startup

# startup thread, assumes a0=function, a1=stack address
startup:
# Ignored: 0 to all registers
# Ignored: 0 .bss (would require 2 more arguments)

# intialize trap entry
la a7, trap_entry
csrw evec, a7
csrrsi x0, status, 16 # enable interrupts for timer.

# initialize the stack
mv sp, a1 
# run C program 
# NOTE(zxj) a0 is the arg, a1 & sp is stack pointer, a2 is func_entry
jalr ra, a2, 0
# when done, behavior depends on thread
csrr a1, hartid
# for all threads except thread 0, sleep and back to reset on wake
beqz a1, done
#### la a2, startup_state
#### slli a1, a1, 2
#### addi a3, a1, 0
#### slli a1, a1, 1
#### add a1, a1, a3
#### add a2, a2, a1

#### 当线程退出时清空func和arg
#### TODO: stack设为默认值。。
#### sw zero, 0(a2)
#### sw zero, 4(a2)
la a0, thread_after_return_handler
jalr ra, a0, 0

#TODO: thread sleep
j reset
# for thread 0, indicate pass/fail by setting tohost to return value from
# program (if 0, set to 15; ends simulation)

# NOTE(zxj 3.10): when thread 0 returns a, write to tohost (a + 1) to end simulation
# If a equals 0, simulation is ended correctly, otherwise bad return value will be shown by 
# simulator (should minus 1).
done:
addi a0, a0, 1
csrw tohost, a0
1: j 1b


# Format is t0_PC, t0_stack, t1_PC, t1_stack, ...
# NOTE(zxj):should make sure bss_end do NOT exceed 0x20001BFC
# Format: t0_PC, t0_arg, t0_stack, t1_PC, t1_arg, t1_stack, ...
# hw thread 0 stack size: 0x1C00
# hw thread 1,2,3 stack size: 0x0C00
# sw thread should reside in thread0
.data
.global startup_state
.global thread_after_return
startup_state:
.word main
.word 0
.word startup_state+0x1BFC
.word 0
.word 0
.word startup_state+0x27FC
.word 0
.word 0
.word startup_state+0x33FC
.word 0
.word 0
.word startup_state+0x3FFC

thread_after_return:
.word thread_after_return_handler
