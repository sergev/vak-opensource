# Spigot π (x86_64 and OpenBSD sparc64)

This repository contains small **position-by-position** (spigot-style) programs that print decimal digits of π. They implement the same core recurrence as the classic C exposition in Ben Lynn’s notes ([Computing Pi in C](https://crypto.stanford.edu/pbc/notes/pi/code.html)), itself based on Dik T. Winter’s tiny program. Three executables are provided:

- **`pi-linux64`** — [FASM](https://flatassembler.net/) source [pi-linux64.asm](pi-linux64.asm), Linux x86_64 ELF64, uses Linux system calls.
- **`pi-macos64`** — [NASM](https://www.nasm.us/) source [pi-macos64.asm](pi-macos64.asm), macOS x86_64 Mach-O, built via [Makefile](Makefile); uses BSD-style system call numbers on macOS.
- **`pi-sparc64-openbsd`** — GNU assembler source [pi-sparc64-openbsd.S](pi-sparc64-openbsd.S), OpenBSD **sparc64** static ELF, OpenBSD system calls via `ta 0`; build on an OpenBSD/sparc64 host with **`make sparc64-openbsd`** (see [Building and running](#building-and-running)).

Full background on the π-spigot benchmark rules and historical results is on litwr’s page: [π spigot benchmark](https://litwr2.github.io/pi/pi-spigot-benchmark.html).

---

## Algorithm (spigot, base 10000)

The algorithm keeps an array of nonnegative integers `r[i]` (residues). One **outer** loop variable `k` steps downward by **14** each time. Because \(2^{14} = 16384 > 10^4\), each outer step is safe to expose **four** new decimal digits by working in base **10000** (four digits per “digit group”). That matches the `k -= 14` and `printf("%.4d", …)` pattern in the usual C code.

For each `k`, set `d = 0` and run an **inner** loop with index `i` running from `k` down to `1`:

1. `d += r[i] * 10000`
2. `b = 2*i - 1`
3. `r[i] = d mod b`
4. `d = floor(d / b)`
5. If `i > 1`, decrement `i` and set `d = d * (i_after_decrement)` — equivalently: after using `b = 2*i-1`, decrement `i`, and if not finished, multiply `d` by the new `i`.

After the inner loop finishes, let `carry` be the running value `c` from the previous outer step. Emit the four decimal digits for `carry + floor(d / 10000)` (modulo the usual leading digit behavior), then set `c = d mod 10000` for the next outer iteration.

A clear reference implementation (with `N = 2800` and `r[i] = 2000`) is spelled out on the Stanford page: [Computing Pi in C](https://crypto.stanford.edu/pbc/notes/pi/code.html).

Correct, minimal C shape (indices and `d` declared; same recurrence as above):

```c
#include <stdio.h>

#define N 2800

int main(void) {
    long r[N + 1];
    long i, k, b, d, c = 0;

    for (i = 1; i <= N; i++)
        r[i] = 2000;

    for (k = N; k > 0; k -= 14) {
        d = 0;
        i = k;
        for (;;) {
            d += r[i] * 10000;
            b = 2 * i - 1;
            r[i] = d % b;
            d /= b;
            i--;
            if (i == 0)
                break;
            d *= i;
        }
        printf("%.4d", (int)(c + d / 10000));
        c = d % 10000;
    }
    return 0;
}
```

This repository uses **larger** `N` and the same fill constant **2000**; see [Parameters and memory](#parameters-and-memory) below.

---

## Complexity and operation count

The cost is **dominated by integer division** and grows **roughly quadratically** in the number of printed digits: if time for `D` digits is `T`, then computing about `2*D` digits takes **about `4*T`** (doubling the problem size scales work by roughly four).

The **total number of inner-loop body executions** (one pass of the `i`-loop) for digit count `D` is:

\[
\frac{7(4 + D)\,D}{16}
\]

Here `D` is the number of decimal digits produced. For this project `D = 9360`, which matches the assembly definitions. That count is the same quantity used in litwr’s benchmark discussions for comparing implementations.

---

## litwr benchmark context

The [π spigot benchmark](https://litwr2.github.io/pi/pi-spigot-benchmark.html) compares programs under constraints that include:

1. **Timing** — the program is measured for how long the calculation takes.
2. **Output** — digits are printed through an **OS routine**, **four digits at a time**, in sync with the calculation (no batching all digits at the end).
3. **Memory** — **code and data together** must stay **below 64 KB**.
4. **No artificial digit cap below the limit** — implementations should use the available RAM under that ceiling to maximize digits rather than imposing a smaller fixed limit.

This codebase targets the **64 KB data** ceiling: the residue array `ra` is sized so that `D = 9360` is the practical limit noted in the sources (`9360` is the limit for a 64 KB `ra`-array in the original comment). Integer division dominates, which is why architectures with fast hardware division are favored in the benchmark tables.

---

## Parameters and memory

| Symbol | Value | Role |
|--------|-------|------|
| `D` | **9360** | Decimal digits printed (four per outer step). |
| `N` | **`D*7/2` = 32760** | Upper range for the spigot index `k` / size of the `r` array in **words**. |

Outer iterations: `k` starts at `N` and decreases by 14 until it stops, so there are **`N/14 = 2340`** outer steps, and **`2340 × 4 = 9360`** decimal digits of output.

The word array **`ra`** holds `N+1` elements of **16 bits** each (`ra` in the sources: one leading word plus `N` words — see the assembly). Together with `cv`, `kv`, and `wb`, the layout stays within the intended memory budget. On the macOS port, **`wb` is only two bytes reserved**, but **`PR0000` stores four ASCII bytes** starting at `wb`; the last two bytes overlap the start of **`ra`**, matching the original packed layout (the process does not rely on those two bytes of `ra` remaining unchanged across that write in a way that breaks the algorithm).

---

## Implementation details (assembly)

### Initialization

`r[1..N]` must start at **2000**, as in the C code. The programs fill memory with **`rep stosq`** using

`2000 * 0x1000100010001`,

so each 64-bit store writes **four** consecutive 16-bit words equal to 2000. The store begins at **`ra+2`** (skip the first 16-bit word), and **`ecx = N/4`** quadwords are written — i.e. **`N` words** filled.

### Outer loop (`kv`, `cv`)

- **`kv`** holds the current outer `k` (starts at `N`, subtracts 14 each time).
- **`cv`** holds the carry **`c`** (mod 10000) between outer steps.

### Inner loop and indexing

- **`esi`** is a **byte** offset into `ra`: it is set from `kv` and doubled so the inner indexing matches **`2*k`** for the first inner step, then `dec esi` tracks decreasing `i` (the code uses **`b = 2*i - 1`** via `dec esi` before `div`).
- **`cx = 10000`** scales residues for base-10000 arithmetic (`mul` / `div`).

### Correction before each inner step after the first

Control flow enters the inner body at **`.l2`** on the **first** `i`-iteration (`jmp .l2` after setup). On later iterations, execution goes **`.l4` → `.l2`**. The **`.l4`** block

`sub edi, edx` / `sub edi, eax` / `shr edi, 1`

adjusts **`edi`** (the running **`d`**) so that the narrow **multiply/divide** path using 16-bit `r[i]` and the chosen register widths stays consistent with the spigot recurrence across steps. (Without this prelude on subsequent iterations, the simplified fixed-width arithmetic would drift from the ideal integer sequence.)

### Output: `PR0000` and `write`

**`PR0000`** converts the 16-bit value in **`cx`** (the four-digit group to print) into **four ASCII digits** at **`wb`** via repeated subtraction (thousands, hundreds, tens, ones). The program then issues a **`write`** of **4** bytes to **standard output** — Linux: syscall **1** (`write`) with `rdi = 1`; macOS: **`0x2000004`** in `rax` with the same argument registers; OpenBSD sparc64: **`SYS_write`** with **`%o0 = 1`**, **`%o1`** pointing at **`wb`**, **`%o2 = 4`**, then **`ta 0`**.

### Linux vs macOS

| | Linux ([pi-linux64.asm](pi-linux64.asm)) | macOS ([pi-macos64.asm](pi-macos64.asm)) |
|---|------------------------------------------|----------------------------------------|
| Assembler | FASM, `format ELF64 executable 3` | NASM, `-f macho64` |
| Entry | FASM `entry $` | `global _start` |
| `ra` addressing | `movzx` / `mov` with `[rsi+ra]` | **`lea r8, [rel ra]`** then `[r8+rsi]` (Mach-O disallows the Linux-style absolute index encoding) |
| `write` | `rax = 1`, `syscall` | `rax = 0x2000004`, `syscall` |
| `exit` | `rax = 60`, `syscall` | `rax = 0x2000001`, `syscall` |

### OpenBSD sparc64 ([pi-sparc64-openbsd.S](pi-sparc64-openbsd.S))

| Topic | Notes |
|-------|--------|
| Toolchain | **GNU `as`** (OpenBSD): preprocess with **`cc -E`**, then assemble from stdin; link with **`ld -static -nopie`** (see [Makefile](Makefile) target **`sparc64-openbsd`**). |
| Syscalls | OpenBSD: load **`SYS_write`** / **`SYS_exit`** into **`%g1`**, arguments in **`%o0`–`%o5`**, trap **`ta 0`**. The object includes **`.note.openbsd.ident`** and **`.openbsd.syscalls`** so the static linker records syscall sites (same pattern as [demo-sparc64-openbsd.S](demo-sparc64-openbsd.S)). |
| Arithmetic | 16-bit residues use **`lduh`** / **`sth`**; **64×32** division uses **`umul`**, **`Y`**, **`udiv`**, and subtract-with-borrow for the remainder. After each multiply-add into the running `d`, the code updates **`%l1`** like x86 **`mov edi, eax`** before the divide so the **`.l4`** correction matches the narrow fixed-width path. |
| Addresses | 64-bit absolute addresses use **`setx sym, temp, reg`** (do not use **`%g0`** as the `setx` scratch register). |

---

## Building and running

### Linux (FASM)

Assemble and link with the Flat Assembler; exact driver flags depend on your FASM version and linker setup. A typical pattern:

```bash
fasm pi-linux64.asm pi-linux64
chmod +x pi-linux64   # if needed
time ./pi-linux64
```

Adjust the command line if your FASM install uses a different output or requires a separate link step.

### macOS (NASM + Clang)

From this directory:

```bash
make
time ./pi-macos64
```

Or **`make run`** (builds if needed and runs **`time ./pi-macos64`**).

On **Apple Silicon**, build an **x86_64** binary under Rosetta:

```bash
make ARCH_PREFIX='arch -x86_64'
arch -x86_64 time ./pi-macos64
```

### OpenBSD (sparc64)

Build and run **on OpenBSD/sparc64** (cross-assembling from another OS is not covered here):

```bash
make sparc64-openbsd
time ./pi-sparc64-openbsd
```

Equivalent steps without `make`:

```bash
cc -E pi-sparc64-openbsd.S | as -o pi-sparc64-openbsd.o -
ld -static -nopie pi-sparc64-openbsd.o -o pi-sparc64-openbsd
```

---

## Credits

- **litwr** — 80x86 work and the π-spigot benchmark ([π spigot benchmark](https://litwr2.github.io/pi/pi-spigot-benchmark.html)).
- **tricky**, **MMS**, **Thorham**, **meynaf** — help and support on the original line of development (as noted in earlier versions of this file).

Further reading: [Computing Pi in C](https://crypto.stanford.edu/pbc/notes/pi/code.html) (algorithm write-up).
