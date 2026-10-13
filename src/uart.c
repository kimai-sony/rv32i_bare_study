#include <stdint.h>

#include "uart.h"

#define UART0_BASE 0x10000000UL

static volatile uint8_t* const uart =
    (volatile uint8_t*)UART0_BASE;

void uart_putc(char c)
{
    *uart = (uint8_t)c;
}

void uart_puts(const char* s)
{
    while (*s) {
        uart_putc(*s++);
    }
}

/*
 * Print a 32-bit value as 8 hexadecimal digits.
 * RV32I has no divide instruction (it needs the M extension), so use hex, which needs only shifts, rather than decimal.
 */
void uart_put_hex(uint32_t v)
{
    static const char hex[] = "0123456789ABCDEF";

    for (int i = 7; i >= 0; --i) {
        uart_putc(hex[(v >> (i * 4)) & 0xF]);
    }
}
