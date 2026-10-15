# Educational Mini RTOS (RV32I / QEMU virt)

## Purpose
A learning RTOS for junior members. Keep it small enough to read in full.

## Design Principles
- No dynamic memory allocation (TCBs and stacks are static arrays)
- Separate the kernel core from the port layer (arch-dependent), with a future Cortex-M port in mind
- Fixed-priority preemptive scheduling + round-robin among equal priorities
- Task states: Ready / Running / Blocked; the idle task executes WFI

## Port Layer (RV32I)
- Tick: CLINT mtime/mtimecmp
- Context switch: timer interrupt / MSIP / ecall; x1-x31 + mepc + mstatus saved in software
- Critical sections: mstatus.MIE

## Basic API
os_init / task_create / os_start / task_yield / task_delay

## Roadmap
0 Basics (UART) → 1 Periodic timer → 2 Cooperative switching → 3 Preemption
→ 4 delay/Blocked → 5 Priorities → 6 Semaphores/mutexes/queues → 7 Advanced topics

## Notes
- This is teaching material: prioritize readability above all, and commit in units that can be verified at each step
- Current progress: the trap handler skeleton is in place
