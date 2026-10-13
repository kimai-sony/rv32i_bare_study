# rv32i_bare_study

A study repository for a bare-metal RISC-V RV32I implementation running on QEMU.
Starting from an OS-less, library-less state (`-ffreestanding` / `-nostdlib`), it builds up startup code, UART output, trap handling, and more, one piece at a time.

## Layout

```
.
├── cmake/riscv32-clang.cmake  # Toolchain file for cross-building for RISC-V with clang
├── CMakeLists.txt
├── linker.ld                  # Linker script targeting the QEMU virt machine's RAM (0x80000000)
├── src/
│   ├── start.S                # Entry point (_start). Sets up the stack, clears .bss, sets mtvec
│   ├── trap.S / .c / .h       # Trap entry (register save/restore) and interrupt/exception dispatch
│   ├── timer.c / timer.h      # Periodic timer interrupt using CLINT mtime/mtimecmp
│   ├── uart.c / uart.h        # Output to the QEMU virt machine's UART0 (0x10000000)
│   └── main.c
├── third_party/
│   └── riscv-opcodes/         # encoding.h generated from riscv/riscv-opcodes (CSR definitions, BSD-3-Clause)
└── .vscode/                   # Tasks/launch configuration for build, run, and debug
```

The target is QEMU's `virt` machine (`-machine virt -bios none`). Real hardware is not supported.

## Prerequisites

This repository assumes the use of a devcontainer (`.devcontainer/`). Using "Reopen in Container" in VSCode gives you an environment with the following tools already set up:

- clang / lld / llvm (cross-compilation for RISC-V)
- cmake / ninja
- qemu-system-misc (`qemu-system-riscv32`)
- gdb-multiarch

If you're not using the devcontainer, please set these up yourself.

## Build

```sh
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-clang.cmake
ninja -C build
```

In VSCode, you can do the same with `Ctrl+Shift+B` (the default build task).

The build output is `build/hello` (an ELF, loaded starting at `0x80000000`).

## Run (without debugging)

```sh
qemu-system-riscv32 -machine virt -bios none -kernel build/hello -nographic
```

In VSCode, you can also run this via the command palette: `Tasks: Run Task` → `run` (it depends on `build` internally).

UART output is streamed directly to the terminal. To quit, press `Ctrl-A` followed by `x` (`Ctrl-C` is sent to the guest rather than to QEMU, so it won't quit).

## Debugging (VSCode)

Pressing `F5` (Run and Debug → `Attach QEMU RV32`) automatically does the following:

1. Builds via the `build` task
2. The `qemu-gdb` task starts QEMU with `-S` (halt on startup) and opens a GDB server on `localhost:1234`
3. `gdb-multiarch` attaches to it and stops just before `main` (`stopAtEntry`)
4. When the debug session ends, the `stop-qemu-gdb` task terminates QEMU

Breakpoints, stepping, and inspecting registers/variables all work as in normal GDB debugging.

### Debugging manually

To do the same thing from a terminal without VSCode:

```sh
# 1. Start QEMU halted, opening a GDB server
qemu-system-riscv32 -machine virt -bios none -kernel build/hello -nographic -S -gdb tcp::1234

# 2. In another terminal, connect with GDB
gdb-multiarch -ex "set architecture riscv:rv32" -ex "file build/hello" -ex "target remote localhost:1234"
```

### Note: about the QEMU start/stop tasks

The `qemu-gdb` / `stop-qemu-gdb` tasks track the QEMU process by recording its PID in `build/qemu.pid`. Matching by process name or `pkill -f` is deliberately avoided, since such patterns can also match the command string itself when run via `bash -c`, which can end up killing the entire running shell.
