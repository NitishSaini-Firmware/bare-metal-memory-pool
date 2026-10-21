# Bare-Metal Memory Allocator (C)

A custom, deterministic fixed-block memory allocator written from scratch for 32-bit microcontrollers (ARM Cortex-M / RISC-V). Built as a safer alternative to standard `stdlib.h` `malloc()` for interrupt-heavy, resource-constrained embedded systems.

## Why this exists
Standard dynamic allocation causes heap fragmentation and unpredictable execution time. This allocator uses a statically allocated `.bss` pool with fixed 32-byte blocks to guarantee O(1) allocation time and zero fragmentation.

## Core Architecture
* **Pool Size:** 1024 Bytes (32 blocks of 32 Bytes each).
* **Hardware Alignment:** Uses C11 `_Alignas(4)` to force natural word boundaries, preventing CPU HardFaults on 32-bit architectures.
* **ISR Race-Condition Safety:** Implements atomic critical sections (`ENTER_CRITICAL` / `EXIT_CRITICAL`) and `volatile` metadata registries to safely handle preemption from hardware interrupts.
* **Pointer Arithmetic Guards:** Fails safely when passed NULL, out-of-bounds stack addresses, or unaligned mid-block pointers.

## Running the Verification Test Bench
The `main.c` file includes an automated torture test that verifies boundary exhaustion, fault injection (bad pointer handling), and deterministic block recycling.

Compile using standard GCC:
```bash
gcc main.c -o memory_test
./memory_test
