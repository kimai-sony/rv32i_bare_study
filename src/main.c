#include <stdint.h>

#include "encoding.h"
#include "timer.h"
#include "uart.h"

static int counter;

int main(void)
{
    if (counter == 0)
        uart_puts("BSS OK\r\n");
    else
        uart_puts("BSS FAIL\r\n");

    /* Set up a 1-second periodic timer interrupt and enable interrupts globally */
    timer_init(TIMER_FREQ_HZ);
    set_csr(mstatus, MSTATUS_MIE);

    uint32_t last = 0;

    while (1) {
        /* Idle the CPU until an interrupt arrives */
        __asm__ volatile ("wfi");

        uint32_t now = timer_get_ticks();
        if (now != last) {
            last = now;
            uart_puts("tick ");
            uart_put_hex(now);
            uart_puts("\r\n");
        }
    }
}
