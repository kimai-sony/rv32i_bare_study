#ifndef TRAP_H
#define TRAP_H

#include <stdint.h>

/*
 * Register save area (trap frame) that trap.S builds on the stack.
 * regs[i] corresponds to x(i) (regs[0] is unused because x0 is always 0).
 * When changing the layout, update the offsets in trap.S to match.
 */
struct trap_frame {
    uint32_t regs[32];  /* offset   0 - 124 */
    uint32_t mepc;      /* offset 128 */
    uint32_t mstatus;   /* offset 132 */
};

void trap_handler(struct trap_frame* tf);

#endif
