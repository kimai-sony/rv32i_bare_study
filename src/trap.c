#include <stdint.h>

#include "encoding.h"
#include "timer.h"
#include "trap.h"
#include "uart.h"

static void handle_interrupt(uint32_t code)
{
    switch (code) {
    case IRQ_M_TIMER:
        timer_handler();
        break;

    default:
        uart_puts("\r\nUNKNOWN INTERRUPT code=");
        uart_put_hex(code);
        uart_puts("\r\n");
        while (1) {
        }
    }
}

static void handle_exception(uint32_t code, struct trap_frame* tf)
{
    /* There is no recovery for exceptions yet, so print the details and halt */
    uart_puts("\r\nEXCEPTION\r\n");

    uart_puts("mcause=");
    uart_put_hex(code);
    uart_puts("\r\n");

    uart_puts("mepc=");
    uart_put_hex(tf->mepc);
    uart_puts("\r\n");

    uart_puts("mtval=");
    uart_put_hex(read_csr(mtval));
    uart_puts("\r\n");

    while (1) {
    }
}

void trap_handler(struct trap_frame* tf)
{
    /* Identify the cause: the MSB of mcause distinguishes interrupts from exceptions */
    uint32_t mcause = read_csr(mcause);
    uint32_t code = mcause & MCAUSE_EXCCODE;

    if (mcause & MCAUSE_INT) {
        handle_interrupt(code);
    } else {
        handle_exception(code, tf);
    }
}
