# riscv-opcodes (third party)

This directory holds a file generated from [riscv/riscv-opcodes](https://github.com/riscv/riscv-opcodes). It is not part of this repository's own code. Do not edit it by hand.

- `encoding.h`: CSR numbers, bit definitions (`MSTATUS_MIE`, `MIP_MTIP`, ...), cause codes (`IRQ_M_TIMER`, ...) and CSR access macros (`read_csr` / `write_csr` / `set_csr` / `clear_csr`)
- `LICENSE`: BSD-3-Clause license of riscv-opcodes

## Source

- Commit: `5783cf3bea312a48d28e3e2c8a2fcb55603e4bed`
- Generated with:

```sh
git clone https://github.com/riscv/riscv-opcodes.git
cd riscv-opcodes
make RUNNER="PYTHONPATH=src python3 -m" encoding.out.h
cp encoding.out.h <this repo>/third_party/riscv-opcodes/encoding.h
```

`RUNNER` is overridden because the devcontainer only has `python3` (the Makefile's default runner calls `python`, or `uv` if installed).
