#
# Trap (interrupt / exception) entry point
#
#   1. Save general-purpose registers x1-x31 and mepc / mstatus on the stack
#   2. Call trap_handler(struct trap_frame* tf) (C code identifies and handles the cause)
#   3. Restore the saved registers and return to the interrupted location with mret
#
# The stack layout must match struct trap_frame in trap.h.
#   x(i)    : 4 * i
#   mepc    : 128
#   mstatus : 132
# The frame size is rounded up from 136 to 144 to meet the ABI requirement (sp is 16-byte aligned).
#

.equ FRAME_SIZE,  144
.equ OFF_MEPC,    128
.equ OFF_MSTATUS, 132

.section .text

.global trap_entry
.extern trap_handler

# The address set in mtvec (Direct mode) must be 4-byte aligned
.align 2
trap_entry:

    #
    # 1. Save registers
    #
    addi sp, sp, -FRAME_SIZE

    sw x1,   4(sp)
    # x2 (sp) is saved later, after computing its pre-trap value
    sw x3,  12(sp)
    sw x4,  16(sp)
    sw x5,  20(sp)
    sw x6,  24(sp)
    sw x7,  28(sp)
    sw x8,  32(sp)
    sw x9,  36(sp)
    sw x10, 40(sp)
    sw x11, 44(sp)
    sw x12, 48(sp)
    sw x13, 52(sp)
    sw x14, 56(sp)
    sw x15, 60(sp)
    sw x16, 64(sp)
    sw x17, 68(sp)
    sw x18, 72(sp)
    sw x19, 76(sp)
    sw x20, 80(sp)
    sw x21, 84(sp)
    sw x22, 88(sp)
    sw x23, 92(sp)
    sw x24, 96(sp)
    sw x25, 100(sp)
    sw x26, 104(sp)
    sw x27, 108(sp)
    sw x28, 112(sp)
    sw x29, 116(sp)
    sw x30, 120(sp)
    sw x31, 124(sp)

    # Pre-trap sp (= current sp + FRAME_SIZE). t0 is already saved, so it is free to use
    addi t0, sp, FRAME_SIZE
    sw t0,   8(sp)

    # mepc: address of the interrupted (or faulting) instruction
    csrr t0, mepc
    sw t0, OFF_MEPC(sp)

    # mstatus: interrupt-enable state, etc.
    csrr t0, mstatus
    sw t0, OFF_MSTATUS(sp)

    #
    # 2. Call the C handler (first argument a0 = start of the trap frame)
    #
    mv a0, sp
    call trap_handler

    #
    # 3. Restore registers
    #
    # Restore from the frame so values modified by the handler (e.g. mepc += 4 after ecall) take effect
    lw t0, OFF_MEPC(sp)
    csrw mepc, t0

    lw t0, OFF_MSTATUS(sp)
    csrw mstatus, t0

    lw x1,   4(sp)
    # x2 (sp) is restored last with addi
    lw x3,  12(sp)
    lw x4,  16(sp)
    lw x5,  20(sp)
    lw x6,  24(sp)
    lw x7,  28(sp)
    lw x8,  32(sp)
    lw x9,  36(sp)
    lw x10, 40(sp)
    lw x11, 44(sp)
    lw x12, 48(sp)
    lw x13, 52(sp)
    lw x14, 56(sp)
    lw x15, 60(sp)
    lw x16, 64(sp)
    lw x17, 68(sp)
    lw x18, 72(sp)
    lw x19, 76(sp)
    lw x20, 80(sp)
    lw x21, 84(sp)
    lw x22, 88(sp)
    lw x23, 92(sp)
    lw x24, 96(sp)
    lw x25, 100(sp)
    lw x26, 104(sp)
    lw x27, 108(sp)
    lw x28, 112(sp)
    lw x29, 116(sp)
    lw x30, 120(sp)
    lw x31, 124(sp)

    addi sp, sp, FRAME_SIZE

    # Jump to mepc and copy MPIE back into mstatus.MIE (= restore the interrupt-enable state)
    mret
