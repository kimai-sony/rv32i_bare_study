#include <stdint.h>

#include "encoding.h"
#include "timer.h"

/*
 * CLINT (Core Local Interruptor) registers (QEMU virt, hart 0)
 *   mtime    : 64-bit counter that increments at a fixed rate from power-on
 *   mtimecmp : a timer interrupt fires when mtime >= mtimecmp
 * RV32 cannot read or write 64 bits at once, so access the upper and lower 32 bits separately.
 */
#define CLINT_BASE      0x02000000UL
#define MTIMECMP_LO     (*(volatile uint32_t*)(CLINT_BASE + 0x4000))
#define MTIMECMP_HI     (*(volatile uint32_t*)(CLINT_BASE + 0x4004))
#define MTIME_LO        (*(volatile uint32_t*)(CLINT_BASE + 0xBFF8))
#define MTIME_HI        (*(volatile uint32_t*)(CLINT_BASE + 0xBFFC))

static uint32_t timer_interval;
static uint64_t next_compare;
static volatile uint32_t ticks;    /* volatile because it is shared between the interrupt handler and main */

static uint64_t mtime_read(void)
{
    uint32_t hi, lo;

    /* Re-read until the upper half is stable, in case a carry into it occurs while reading the lower half */
    do {
        hi = MTIME_HI;
        lo = MTIME_LO;
    } while (hi != MTIME_HI);

    return ((uint64_t)hi << 32) | lo;
}

static void mtimecmp_write(uint64_t value)
{
    /*
     * To avoid a spurious interrupt from a partially written value,
     * first set the upper half to the maximum, then write lower, then upper.
     */
    MTIMECMP_HI = 0xFFFFFFFFu;
    MTIMECMP_LO = (uint32_t)value;
    MTIMECMP_HI = (uint32_t)(value >> 32);
}

void timer_init(uint32_t interval)
{
    timer_interval = interval;
    next_compare = mtime_read() + interval;
    mtimecmp_write(next_compare);

    /*
     * Enable the timer interrupt individually (the caller sets the global enable mstatus.MIE).
     * mie and mip share bit positions, so encoding.h uses the MIP_* names for both.
     */
    set_csr(mie, MIP_MTIP);
}

void timer_handler(void)
{
    ticks++;

    /*
     * Set the next interrupt time. This also clears the pending interrupt (mip.MTIP).
     * Using "previous compare value + period" rather than "current mtime + period"
     * keeps handler entry latency from accumulating and drifting the period.
     */
    next_compare += timer_interval;
    mtimecmp_write(next_compare);
}

uint32_t timer_get_ticks(void)
{
    return ticks;
}
