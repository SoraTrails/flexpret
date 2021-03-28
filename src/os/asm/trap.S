
.text
.align 4

.global trap_entry
trap_entry:
# save context
save_context:
    addi sp, sp, -32 * 4
    sw x1,   1 * 4(sp)
    sw sp,   2 * 4(sp)
    sw x3,   3 * 4(sp)
    sw x4,   4 * 4(sp)
    sw x5,   5 * 4(sp)
    sw x6,   6 * 4(sp)
    sw x7,   7 * 4(sp)
    sw x8,   8 * 4(sp)
    sw x9,   9 * 4(sp)
    sw x10, 10 * 4(sp)
    sw x11, 11 * 4(sp)
    sw x12, 12 * 4(sp)
    sw x13, 13 * 4(sp)
    sw x14, 14 * 4(sp)
    sw x15, 15 * 4(sp)
    sw x16, 16 * 4(sp)
    sw x17, 17 * 4(sp)
    sw x18, 18 * 4(sp)
    sw x19, 19 * 4(sp)
    sw x20, 20 * 4(sp)
    sw x21, 21 * 4(sp)
    sw x22, 22 * 4(sp)
    sw x23, 23 * 4(sp)
    sw x24, 24 * 4(sp)
    sw x25, 25 * 4(sp)
    sw x26, 26 * 4(sp)
    sw x27, 27 * 4(sp)
    sw x28, 28 * 4(sp)
    sw x29, 29 * 4(sp)
    sw x30, 30 * 4(sp)
    sw x31, 31 * 4(sp)

    # load cause and call corresponding handler
    csrr a0, cause
    csrr a1, epc
    csrr a2, hartid
    slli a6, a2, 2
    # la a4, trap_stack
    # add a4, a4, a6
    la a7, epc_vector
    add a7, a7, a6
    la a5, trap_handler_vector
    slli a2, a2, 4
    sw a1, 0(a7)

    li a3, 0x8000000d # CAUSE_IE
    beq a0, a3, ie
    li a3, 0xd # CAUSE_EE
    beq a0, a3, ee
    li a3, 0x8000000e # CAUSE_EXTERNAL_INT
    beq a0, a3, int
    j fault
ie:
    addi a2, a2, 0
    addi s0, x0, 0
    j call_handler
ee:
    addi a2, a2, 4
    addi s0, x0, 0
    j call_handler
int:
    addi a2, a2, 8
    addi s0, x0, 0
    j call_handler
fault:
    addi a2, a2, 12
    addi s0, x0, 4
    # j call_handler
call_handler:
    add a5, a5, a2
    lw a5, 0(a5)    # a5 = func_entry
    # lw sp, 0(a4)    # a4 = stack

    jalr ra, a5, 0

    # NOTE: because flexpret doesn't implement standard sret semantics,
    # using tp(x4) to restore epc and jump to it, just like sret does.
restore_epc:
    csrr a2, hartid
    slli a6, a2, 2
    la a7, epc_vector
    add a7, a7, a6
    lw tp, 0(a7)
    add tp, tp, s0 # when fault, goto pc + 4; else goto pc

    # restore context except tp(x4)
    # NOTE: sp should be equal to the value when it was saved
load_context:
    lw x31, 31 * 4(sp)
    lw x30, 30 * 4(sp)
    lw x29, 29 * 4(sp)
    lw x28, 28 * 4(sp)
    lw x27, 27 * 4(sp)
    lw x26, 26 * 4(sp)
    lw x25, 25 * 4(sp)
    lw x24, 24 * 4(sp)
    lw x23, 23 * 4(sp)
    lw x22, 22 * 4(sp)
    lw x21, 21 * 4(sp)
    lw x20, 20 * 4(sp)
    lw x19, 19 * 4(sp)
    lw x18, 18 * 4(sp)
    lw x17, 17 * 4(sp)
    lw x16, 16 * 4(sp)
    lw x15, 15 * 4(sp)
    lw x14, 14 * 4(sp)
    lw x13, 13 * 4(sp)
    lw x12, 12 * 4(sp)
    lw x11, 11 * 4(sp)
    lw x10, 10 * 4(sp)
    lw x9,   9 * 4(sp)
    lw x8,   8 * 4(sp)
    lw x7,   7 * 4(sp)
    lw x6,   6 * 4(sp)
    lw x5,   5 * 4(sp)
    # lw x4,   4 * 4(sp)
    lw x3,   3 * 4(sp)
    # lw x2,   2 * 4(sp)
    lw x1,   1 * 4(sp)
    addi sp, sp, 32 * 4

trap_ret:
    # enable interrupts, because interrupts was disabled automatically by hardware when trap occurred.
    csrrsi x0, status, 16 
    jalr x0, tp, 0

    # unreachable code
1:
    j 1b


.data
# format: t0: ie_handler, ee_handler, int_handler, fault_handler, t1:...
.global trap_handler_vector
trap_handler_vector:
.word default_ie_handler
.word default_ee_handler
.word default_int_handler
.word default_fault_handler

.word default_ie_handler
.word default_ee_handler
.word default_int_handler
.word default_fault_handler

.word default_ie_handler
.word default_ee_handler
.word default_int_handler
.word default_fault_handler

.word default_ie_handler
.word default_ee_handler
.word default_int_handler
.word default_fault_handler

epc_vector:
.word 0
.word 0
.word 0
.word 0

.global startup_timer_state
startup_timer_state:
.word 0
.word 0
.word 0
.word 0

.word 0
.word 0
.word 0
.word 0

.word 0
.word 0
.word 0
.word 0

.word 0
.word 0
.word 0
.word 0
