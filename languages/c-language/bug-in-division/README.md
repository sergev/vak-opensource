# The Algorithm D bug

Markdown copy of Novak Kaluđerović's **[A long division story](A-long-division-story.md)**
([original](https://kolja.rs/algorithm-d)) — a bug in Knuth's *TAOCP* Algorithm 4.3.1D,
long division, live from 1995 to 2026.

## In one paragraph

Algorithm D divides huge integers digit by digit. It *guesses* each quotient digit from
the top digits only, then repairs the guess at most twice. The book proves the guess can
overshoot by 3. Two repairs are not always enough — and on those inputs the algorithm
silently returns a wrong answer.

## How it works

A big number is stored as a list of machine words: digits in base `b = 2^64`. Division is
schoolbook long division, one digit at a time. Guessing the digit is the hard part, so
Algorithm D estimates it with a single hardware divide of the top digits:

```
q̂ = ⌊(top two digits of u) / (top digit of v)⌋
```

Two theorems bound the error, after *normalising* (scaling both operands so the divisor's
top digit is ≥ `b/2`):

- **Theorem A** — never too small: `q ≤ q̂`
- **Theorem B** — never too large by more than 3: `q̂ ≤ q + 3`

Step **D3** then tests the guess against the divisor's second digit and decrements it —
**at most twice**, per the book's wording. That is claimed to leave the guess (a) off by
at most 1, and (b) **one digit wide**, so step D4 can multiply it back with a cheap
one-digit multiply.

## The gap

D3 repairs 2, Theorem B allows 3.

Worst case `q = b − 1` gives `q̂ = b + 2`, two digits. Two decrements leave `b` — written
`(1,0)` in base `b`, still two digits. D4 multiplies by the *low* digit, which is `0`:
subtracts nothing, and everything downstream is garbage.

Smallest example, in base 3:

```
b = 3
u = (1,2,0,0)₃ = 45     true quotient  q = ⌊45/16⌋ = 2
v =   (1,2,1)₃ = 16     guess          q̂ = ⌊5/1⌋ = 5 = q + 3 = (1,2)₃
```

D3 fires twice: `5 → 4 → 3 = (1,0)₃`. D4 multiplies by `0`. Wrong. A third decrement
would have given the correct `2`.

## Demo

[`divbug.c`](divbug.c) implements Algorithm D over an arbitrary base and runs it both
ways — as printed (D3 corrects at most twice, D4 uses `q̂ mod b`) and with D3 as a `while`
loop — against a schoolbook reference:

```
make && ./divbug          # base 3, the smallest failing input
./divbug 65               # any odd base, up to 2^31
```

```
    D3: q^ = 5, r^ = 0
        correction 1 -> q^ = 4, r^ = 1
        correction 2 -> q^ = 3, r^ = 2
        (as printed: the test is not repeated a third time)
        q^ = 3 = (1,0)_3 does not fit in one limb; D4 multiplies by its low limb 0
    -> q = 0, r = 18    *** WRONG ***
       q*v + r = 18, but u = 45
```

It then enumerates **every** legal 4-by-3-limb division for bases 3–9. Odd bases fail on
exactly the predicted inputs, even bases never fail, and the `while` loop is always right:

```
  b = 3   (odd)         945 divisions   as printed:     9 wrong   while loop: 0 wrong
  b = 4   (even)       6080 divisions   as printed:     0 wrong   while loop: 0 wrong
  b = 5   (odd)       32625 divisions   as printed:    50 wrong   while loop: 0 wrong
  b = 7   (odd)      335454 divisions   as printed:   147 wrong   while loop: 0 wrong
  b = 8   (even)     785408 divisions   as printed:     0 wrong   while loop: 0 wrong
  b = 9   (odd)     1917270 divisions   as printed:   324 wrong   while loop: 0 wrong
```

## Why it hid for 30 years

- **It used to be right.** Editions 1–2 clamped the guess to `b − 1` (what x86 and Knuth's
  MIX `div` do); for the clamped guess `q̂ ≤ q + 2` and two repairs suffice. The 1995
  MMIX-era rewrite switched D3 to the unclamped two-digit guess (ARM-style `div`) without
  updating the theorems. Knuth's published MIX program stayed correct — only the printed
  algorithm broke.
- **No hardware can hit it.** It needs **odd** `b`, exactly `v_{n-1} = u_n = (b−1)/2` and
  `u_{n-1} = b−1`. Real machines use `b = 2^32` or `2^64`.
- **Implementers wrote a loop.** Knuth's *"repeat this test if r̂ < b"* reads as *once
  more*; almost everyone coded *while*, which does the third repair and hides the bug.

## LLVM

`APInt.cpp` follows the text literally (two `if`s) and still tests `q̂ == b` — a TAOCP typo
fixed in 2005, where the text now says `q̂ ≥ b`. With even `b` the guess only reaches
`b + 1`, and that case happens to be caught by D3's other test, so LLVM is correct — but
for a reason proved neither in the book nor in the code. Kaluđerović filed an NFC PR to
match the corrected text.

## Outcome

Filed 2026-05-14, in Knuth's errata 2026-06-09, in print with the 53rd printing of
Volume II. The repaired bound is **Theorem B (N. Kaluđerović, 2026)**. Reward: one
hexadecimal dollar, `0x$1.00` = $2.56, on the Bank of San Serriffe.

> *"I'm especially glad to have this correction, because I think the readers of TAOCP
> Vol 2 look at Algorithm 4.3.1 D more than any other algorithm!"* — Knuth

Full article, with proofs, history, and faster modern division algorithms:
[A-long-division-story.md](A-long-division-story.md).
