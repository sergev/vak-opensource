# The Algorithm D bug

Markdown copy of Novak Kaluđerović's **[A long division story](A-long-division-story.md)**
([original](https://kolja.rs/algorithm-d)) — a bug in Knuth's *TAOCP* Algorithm 4.3.1D,
long division, live from 1995 to 2026.

Algorithm D divides huge integers digit by digit. It *guesses* each quotient digit, then
repairs the guess at most twice. The book proves the guess overshoots by at most 2 — but
proves it for a guess the algorithm stopped computing in 1995. The one it computes now can
overshoot by 3, two repairs are then not enough, and the answer comes out silently wrong.

## The guess

Digits are machine words, base `b = 2^64`. Guessing the next quotient digit is the hard
part, so Algorithm D spends one hardware divide on the top digits. After *normalising*
(scaling both operands so the divisor's top digit is ≥ `b/2`), two flavours of that divide
are in play:

```
q̂  = ⌊(top two digits of u) / (top digit of v)⌋   full two-digit quotient (ARM)
q̂ₛ = min(b − 1, q̂)                                clamped to one digit (x86, MIX)
```

The book's two theorems, both true:

- **Theorem A** — never too small: `q ≤ q̂`
- **Theorem B** — never too large by more than 2: `q̂ₛ ≤ q + 2`

Theorem B is about `q̂ₛ`. Editions 1–2 used `q̂ₛ`; the 1995 MMIX-era rewrite switched the
algorithm to `q̂` and left the theorems alone. For `q̂` the bound is one larger:
`q̂ ≤ q + 3` — **Theorem B (N. Kaluđerović, 2026)**, proved in the article and now in
Knuth's errata.

## The gap

Step **D3** decrements the guess **at most twice**: right for `q̂ₛ ≤ q + 2`, one short for
`q̂ ≤ q + 3`. Worst case `q = b − 1` gives `q̂ = b + 2`, two digits wide. Two decrements
leave `b` — `(1,0)` in base `b`, still two digits. Step D4 multiplies back by the *low*
digit, which is `0`: it subtracts nothing, and everything downstream is garbage.

Smallest example, base 3:

```
b = 3
u = (1,2,0,0)₃ = 45     true quotient  q = ⌊45/16⌋ = 2
v =   (1,2,1)₃ = 16     guess          q̂ = ⌊5/1⌋ = 5 = q + 3 = (1,2)₃
```

D3 fires twice: `5 → 4 → 3 = (1,0)₃`. D4 multiplies by `0`. Wrong. A third decrement would
have given the correct `2`.

## Demo

[`divbug.c`](divbug.c) runs Algorithm D over an arbitrary base three ways — as printed
(two corrections, D4 uses `q̂ mod b`), with D3 as a `while` loop, and with the saturating
`q̂ₛ` of editions 1–2 — against a schoolbook reference:

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

    D3: unsaturated q^ = 5; the saturating div clamps it to b-1 = 2
    D3: q^ = 2, r^ = 3
    -> q = 2, r = 13    correct
```

It then enumerates **every** legal 4-by-3-limb division for bases 3–9:

```
  b = 3   (odd)         945 divisions   as printed:     9 wrong   while loop: 0 wrong   saturating: 0 wrong
  b = 4   (even)       6080 divisions   as printed:     0 wrong   while loop: 0 wrong   saturating: 0 wrong
  b = 5   (odd)       32625 divisions   as printed:    50 wrong   while loop: 0 wrong   saturating: 0 wrong
  b = 7   (odd)      335454 divisions   as printed:   147 wrong   while loop: 0 wrong   saturating: 0 wrong
  b = 8   (even)     785408 divisions   as printed:     0 wrong   while loop: 0 wrong   saturating: 0 wrong
  b = 9   (odd)     1917270 divisions   as printed:   324 wrong   while loop: 0 wrong   saturating: 0 wrong
```

Only the printed version fails, only in odd bases, on exactly the predicted inputs. The
last column is Theorem B in empirical form: with the guess it is proved for, two
corrections are enough in every base.

## Why it hid for 30 years

- **It used to be right.** Editions 1–2 clamped the guess, and two repairs suffice for
  `q̂ₛ ≤ q + 2`. The 1995 rewrite switched D3 to the unclamped guess but not the theorems,
  which went on proving a bound for a quantity the algorithm no longer computed. Knuth's
  published MIX program stayed correct — only the printed algorithm broke.
- **No hardware can hit it.** It needs **odd** `b`, with `v_{n-1} = u_n = (b−1)/2` and
  `u_{n-1} = b−1`. Real machines use `b = 2^32` or `2^64`.
- **Implementers wrote a loop.** Knuth's *"repeat this test if r̂ < b"* reads as *once
  more*; almost everyone coded *while*, which does the third repair and hides the bug.

## LLVM

`APInt.cpp` follows the text literally (two `if`s) and still tests `q̂ == b` — a TAOCP typo
fixed in 2005 to `q̂ ≥ b`. With even `b` the guess only reaches `b + 1`, and that case
happens to be caught by D3's other test, so LLVM is correct — for a reason proved neither
in the book nor in the code. Kaluđerović filed an NFC PR to match the corrected text.

## Outcome

Filed 2026-05-14, in Knuth's errata 2026-06-09, in print with the 53rd printing of
Volume II, where `q̂ₛ ≤ q + 2` gives way to `q̂ ≤ q + 3`. Reward: one hexadecimal dollar,
`0x$1.00` = $2.56, on the Bank of San Serriffe.

> *"I'm especially glad to have this correction, because I think the readers of TAOCP
> Vol 2 look at Algorithm 4.3.1 D more than any other algorithm!"* — Knuth

Full article, with proofs, history, and faster modern division algorithms:
[A-long-division-story.md](A-long-division-story.md).
