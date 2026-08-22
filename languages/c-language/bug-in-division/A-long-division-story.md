# A long division story

*Novak Kaluđerović — [kolja.rs/algorithm-d](https://kolja.rs/algorithm-d), 13 Aug 2026*

## How I received a theorem in Knuth's *"The Art of Computer Programming"* after finding a decades-old bug in Algorithm D (+ a *"bug"* in llvm)

I was implementing Algorithm D, the well-known long division algorithm from Knuth's *"The Art of Computer Programming"*, and I stumbled upon an issue that I couldn't let go. The correctness of the algorithm relied on Theorem B, and its proof bugged me. It felt unnatural, it took a very convoluted path to proving a simple statement, and it isolated a special case which was not a corner case and that seemed unrelated to the problem at hand. There was something odd about it, so I tried to prove the theorem myself, and I failed. However, the failure handed me a counterexample to Algorithm D that had passed as correct for decades, and with it a theorem on the correctness of the algorithm carrying my name.

![Theorem B (N. Kaluđerović, 2026), added to the TAOCP errata](https://bear-images.sfo2.cdn.digitaloceanspaces.com/kolja/teo.webp)

In this post I'll give some background, then cover long division from scratch for those who want it, share my thoughts on how the bug came to be and how it stayed hidden so long, and finish with a preview of more modern ways to implement long division. In the process of writing this blog I also found a *"bug"* in this algorithm's implementation in llvm, and I will expand on that too. If you're only interested in the bug you can jump directly to [The bug](#the-bug).

**Contents**

- [How I got here](#how-i-got-here)
- [Long division from scratch](#long-division-from-scratch)
    - [Multiprecision integers in hardware](#multiprecision-integers-in-hardware)
    - [Reducing long division to medium division](#reducing-long-division-to-medium-division)
    - [Reducing medium division to small division](#reducing-medium-division-to-small-division)
        - [Normalisation](#normalisation)
- [The bug](#the-bug)
    - [How did it stay hidden for decades](#how-did-it-stay-hidden-for-decades)
    - [Can it be exploited](#can-it-be-exploited)
    - [The llvm "bug"](#the-llvm-bug)
    - [AI didn't find it](#ai-didnt-find-it)
- [The check](#the-check)
- [A little trit more](#a-little-trit-more)
    - [Stronger bounds](#stronger-bounds)
    - [Doubling the quotient limbs](#doubling-the-quotient-limbs)
    - [Division by a constant](#division-by-a-constant)

## How I got here

Preparing for an interview, I decided to do a small project: build a little library for arithmetic over prime fields. This meant fixed-size multiprecision integers, arithmetic operations, some field operations, constant-time, constant memory access, all in all a starting point for modern cryptographic protocols.

As I worked on implementing it, the process turned into a game with one rule: avoid division at all costs. You can get almost all the way there, and to the best of my knowledge cryptographic libraries never execute the division instruction at runtime. Whenever a divide is needed, it is generally substituted by a multiplication followed by a bit of shuffling.[^1]

Why do we go so far out of our way to avoid division? Well, multiplication is very simple, in fact it can be thought of as an axiom of the natural numbers. Division is far more complicated. Firstly, it's not everywhere defined: we can't divide by zero. But we can't divide 5 by 2 either! What we actually have is *"division with remainder"*, a more complicated operation which returns two answers: the quotient *and the remainder*, the smallest non-negative difference between the dividend and a multiple of the divisor. The issue hides in what *"smallest"* exactly means, why we choose this particular definition, and why the notion of *size* enters the picture at all. One may choose differently, say zero-centred remainders. But we could go further and choose a [different size function](https://en.wikipedia.org/wiki/P-adic_valuation), which gives rise to a different division algorithm altogether.[^2]

With all that in mind it's no wonder that the theoretical complication transfers into practice. A multiply instruction costs a cycle or two on modern machines and fully pipelines, while a divide can cost up to twenty cycles and usually doesn't pipeline.

Eventually the only gap left in the multiprecision implementation was the multiprecision division algorithm. So I did the obvious thing and sat down to implement long division, and for reference I used Donald Knuth's *"The Art of Computer Programming"* Vol. II, Third Edition, Algorithm 4.3.1D.

Let's look at how long division actually works.

## Long division from scratch

### Multiprecision integers in hardware

Cryptographic integers run to hundreds or thousands of bits, well past a single register, so we store them in base $b$, one *limb* per machine word:

$$
x=(x_{n-1},\dots ,x_0)_b=\sum_{i=0}^{n-1}x_ib^i,\quad 0\le x_i<b.
$$

The main building blocks of multiprecision arithmetic algorithms are the four primitive instructions that operate over single/double limbs:

```
addc:  x, y          ->  s, carry     # s = (x+y) mod b,   carry  = 1 iff overflow
subc:  x, y          ->  d, borrow    # d = (x−y) mod b,   borrow = 1 iff underflow
mul:   x, y          ->  (hi, lo)     # x·y = hi·b + lo
div:   (hi, lo), y   ->  (q1, q0), r  # hi·b + lo = q·y + r,  0 ≤ r < y,  q = q1·b + q0
```

The first three are unremarkable, but the division stands as the odd one out. While multiplying two single-limb multiplicands always returns a two-limb product, the quotient of a two-limb dividend over a one-limb divisor does not always fit in a single limb, so we use a two-limb quotient. In addition to that, the remainder shows up as a necessary byproduct. And in division by zero we assume *undefined behaviour*, i.e. that $q$ and $r$ may take any value, although some architectures treat this case differently as we will see later.

### Reducing long division to medium division

Our task is to divide a multiprecision integer $u$ by $v$, that is, find integers $q$ and $r$ such that $u=q\cdot v+r$ and $0\le r<v$. Assume without loss of generality that the divisor $v$ is an $n$-limb integer with non-zero top limb $v_{n-1}$, and pad the dividend $u$ with leading zeros until it has strictly more limbs than $v$; write $n+m+1$ for the number of limbs of $u$.

We have another requirement which will prove natural in what follows: the highest $n$ limbs of $u$, read as an $n$-limb integer, must be strictly smaller than $v$,

$$
(u_{n+m},\dots ,u_{m+1})_b<(v_{n-1},\dots ,v_0)_b,\quad \text{equivalently}\quad \left\lfloor \frac{u}{b^{m+1}}\right\rfloor <v.
$$

If that is not already the case, appending a further zero to $u$ will guarantee it. This assumption pins down the size of the quotient to $m+1$ limbs:

$$
\left\lfloor \frac{u}{b^{m+1}}\right\rfloor <v\quad \iff \quad u<v\;b^{m+1}\quad \iff \quad q=\left\lfloor \frac{u}{v}\right\rfloor <b^{m+1}.
$$

With the padding in place every operand has a fixed shape. Writing $q$ and $r$ for the quotient and remainder of $u$ by $v$, we have:

```
u = (u_{n+m}, u_{n+m-1}, ..., u_0)_b
v = (v_{n-1}, v_{n-2},   ..., v_0)_b    0 < v_{n-1}
q = (q_{m},   q_{m-1},   ..., q_0)_b    u = q·v + r
r = (r_{n-1}, r_{n-2},   ..., r_0)_b    0 ≤ r < v
```

In this exposition we will use three different division algorithms, at three levels. The one we want is the $n+m+1$ by $n$ limb, or the *"long"* **n+m+1/n** division. The one we have is the *"short"* **2/1** division instruction. To bridge them we use the *"medium"* **n+1/n** division.

A natural way to compute the limbs of $q$ is by going from the top down.[^3] The top limb is[^4]

$$
q_m=\left\lfloor \frac{\lfloor u/v\rfloor}{b^m}\right\rfloor =\left\lfloor \frac{\lfloor u/b^m\rfloor}{v}\right\rfloor =\left\lfloor \frac{(u_{n+m},\dots ,u_m)_b}{(v_{n-1},\dots ,v_0)_b}\right\rfloor .
$$

The numerator $\lfloor u/b^m\rfloor$ is simply the top $n+1$ limbs of $u$, so the top limb of the quotient $q_m$ is itself a quotient of an **n+1/n** division. And it fits in a single limb due to the hypothesis on the top $n$ limbs of $u$ being less than $v$

$$
(u_{n+m},\dots ,u_{m+1})_b<v\quad \iff \quad (u_{n+m},\dots ,u_m)_b<bv,
$$

hence $q_m=\lfloor \frac{(u_{n+m},\dots ,u_m)_b}{v}\rfloor <b$. Upon computing $q_m$, we proceed by subtracting the multiple $b^mq_mv$ of $v$ from $u$, and continuing the algorithm with the updated $u$. This operation of updating $u$ is equivalent to replacing the top $n+1$ limbs of $u$ by $R_m$, the remainder corresponding to $q_m$, which we know to fit in $n$ limbs. Therefore the updated $u$ will have one limb less, and we continue the algorithm to compute the remaining $m$ limbs of $q$. While it might not be straightforward that this folding technique computes the correct answer, it can easily be deduced from observing the following algorithm and noting that the two invariants are satisfied at each entrance and exit of the loop:

```
Algorithm 1: Long Division
Input:
        u = (u_{n+m}, ..., u_0)_b,  padded so top n limbs < v
        v = (v_{n-1}, ..., v_0)_b,  v_{n-1} > 0
Start:
        r = u,  q = 0                         // r starts as u and shrinks to final remainder
Loop:
        for k = m down to 0:
            u' = (r_{k+n}, ..., r_k)_b        // top n+1 limbs of r at position k
            (q_k, R_k) = ⌊u'/v⌋, u' mod v     // an n+1/n division
            q += q_k·b^k                      // k'th limb of the quotient
            r -= q_k·v·b^k                    // replaces (r_{k+n},...,r_k) by (0, R_k)
Return: (q, r)
Invariants:
        1. u = q·v + r
        2. top n limbs of r < v               // from limb k+1 to n+k
```

Running invariant 2 until $k=0$ leaves $r<v$, and so the pair $(q,r)$ is exactly the quotient and the remainder.

> **Interactive animation — Long division of 1758293 by 314 (b = 10).**
> r is initialised to u. Each iteration: divide the highlighted top four words of r by v = 314 (quotient digit goes to q); take them modulo 314 into r_k; clear those words of r; then move r_k up into r. After four steps q = 5599 and r = 207. 15 steps.
> *(Step-through animation; see the [original article](https://kolja.rs/algorithm-d).)*

So an **n+m+1/n** *long* division costs $m+1$ *medium* **n+1/n** divisions, one per quotient limb.

### Reducing medium division to small division

The previous section left us with the medium division:

```
u = (u_n, u_{n-1}, ..., u_0)_b,
v = (v_{n-1},      ..., v_0)_b,
(u_n, ..., u_1) < v, i.e., u/b < v, i.e., u < vb
```

The division of $u$ by $v$ returns a quotient $0\le q=\lfloor \frac{u}{v}\rfloor <b$ and remainder $0\le r<v$.

A natural step is to approximate the **n+1/n** division by means of a **2/1** division of the top limbs, which can be computed with the `div` instruction. Re-write the parameters as

```
u = u''·b^{n-1} + u'      u'' = (u_n, u_{n-1})_b     0 ≤ u'' < b²,    0 ≤ u' < b^{n-1},
v = v''·b^{n-1} + v'      v'' = v_{n-1}              0 < v'' < b,     0 ≤ v' < b^{n-1},
```

and set $\hat{q}=\lfloor \frac{u''}{v''}\rfloor$ computed by a single `div((u_{n},u_{n-1}), v_{n-1})`. Note that $\hat{q}$ is the full **two-limb** quotient $(q_1,q_0)_b$ of that instruction. Our constraint bounds $u$ against $v\cdot b$, but not $u''$ against $v''\cdot b$, so nothing stops $\hat{q}$ from exceeding a single limb.

How good of a guess is $\hat{q}$? Can it overshoot badly? Can it undershoot? Both theorems below lean on the following simple fact about floors: for $d=\lfloor m/n\rfloor$ we have $dn\le m\le (d+1)n-1$.

**Theorem A** $(q\le \hat{q})$: No undershoot

$$
q\le \frac{u}{v}\le \frac{u''b^{n-1}+u'}{v''b^{n-1}}\le \frac{(u''+1)\;b^{n-1}-1}{v''b^{n-1}}<\frac{u''+1}{v''}\le \hat{q}+1.
$$

The second step uses $v\ge v''b^{n-1}$, the third uses $u'\le b^{n-1}-1$, and the last is the floor fact, in the form $u''+1\le (\hat{q}+1)\;v''$. Since $q<\hat{q}+1$ and both are integers, $q\le \hat{q}$.

**Theorem B'** $(\hat{q}<q+1+b/v'')$: Bounded overshoot

$$
\hat{q}\le \frac{u''}{v''}=\frac{u''b^{n-1}}{v''b^{n-1}}\le \frac{u}{v''b^{n-1}}\le \frac{(q+1)\;v-1}{v''b^{n-1}}<q+1+\frac{b}{v''}.
$$

The third step is the floor fact for $q=\lfloor u/v\rfloor$. The last step expands $v=v''b^{n-1}+v'$ and uses $v'\le b^{n-1}-1$ together with $q+1\le b$.

Taken together we have the following bound: $q\le \hat{q}<q+1+b/v''$. For small $v''$ this bound is not so good, with $v''=1$ the guess can overshoot by up to $b$. However, we can control $v''$ by taking a short detour.

#### Normalisation

The quotient is unchanged when both operands are scaled by the same factor: $q=\lfloor u/v\rfloor =\lfloor uf/vf\rfloor$. So we look for a factor $f$ that makes $v''$ large. The choice $f=\lfloor b/(v''+1)\rfloor$ does the job. The denominator $vf$ still fits in $n$ limbs, numerator $uf$ fits in $n+1$ limbs, and after renaming $vf$ to $v$ the new top limb satisfies $v''\ge \lfloor b/2\rfloor$.[^5] We call such a $v$ *normalised*. In practice $f$ is taken to be a power of two, so both scalings are plain shifts.

Notice that the remainder scales too. If $r_f=uf\bmod vf$, then $r_f=r\cdot f$, so once the whole long division finishes with quotient $q$ and remainder $r_f$, the true remainder is $r=r_f/f$. This may be computed via an *exact* division by a single limb. This adds another level of recursion: another, albeit small, long division. But with $n=1$ the approximate quotients $\hat{q}$ are in fact exact (and with $f$ a power of two division is just a shift, so this step is trivial). Note that normalisation happens once, up front, for the whole long division (not once per medium step).

With $v''\ge \lfloor b/2\rfloor$, Theorem B' becomes

**Theorem B** ( $\hat{q}\le q+3$ ):

$$
\hat{q}<q+1+\frac{b}{v''}\le q+1+\frac{b}{\lfloor b/2\rfloor}\le q+4,
$$

and since $\hat{q},q$ are integers, $\hat{q}\le q+3$.

So how does this help the **n+1/n** division? The procedure: compute $\hat{q}$, then test the guess by forming $u-\hat{q}v$, which by the two theorems lies in $[-3v,v)$. Concretely, we compute one `2 x n` multiprecision multiplication and one multiprecision subtraction. If the subtraction underflows, we add $v$ back and decrement $\hat{q}$ by one. We repeat the correction at most three times, until the result is non-negative. At that point $\hat{q}=q$ and what remains of $u$ is the remainder.

```
Algorithm 2: Medium Division
Input:
        u = (u_n, u_{n-1}, ..., u_0)_b,  ⌊u/b⌋ < v          // hence q < b
        v = (v_{n-1},      ..., v_0)_b,  v_{n-1} ≥ ⌊b/2⌋    // normalised
Trial:
        (q̂, r̂) = div((u_n, u_{n-1}), v_{n-1})      // one 2/1 division; q̂ two limbs
Fix:
        r = u - q̂·v                                // one 2xn mul, one mp subtraction
Correction:
        if r < 0:  q̂ -= 1, r += v                  // underflow -> 1st mp addition correction
        if r < 0:  q̂ -= 1, r += v                  // underflow -> 2nd mp addition correction
        if r < 0:  q̂ -= 1, r += v                  // underflow -> 3rd mp addition correction
Return: (q̂, r)                                     // (q̂, r) = (q, r), correct by Theorem B
Invariant:
        u = q̂·v + r                                // holds after every line
```

> **Interactive animation — (n+1)/n division, 3129 / 314 (b = 10).**
> Trial quotient q̂ = ⌊(u_n b + u_{n-1}) / v_{n-1}⌋, then multiply-and-subtract q̂·v, and add back v while the borrow is non-zero, decrementing q̂ each time. 17 steps.
> *(Step-through animation; see the [original article](https://kolja.rs/algorithm-d).)*

## The bug

In Algorithm D, step D3, Knuth proposes a method for improving the trial quotient before the fix stage.
While we cannot expect to obtain the exact value $\hat{q}=q$ without reading the whole $u$ and $v$, we can tighten the gap by reading an additional limb of both the dividend and the divisor. The benefits of Knuth's steps are twofold:

- Firstly, $\hat{q}$ is tightened to a bound of $\hat{q}\le q+1$. This means that the number of correction steps in `Correction` in `Algorithm 2` will be at most 1, down from at most 3.
- Secondly, Knuth argues that the trial quotient $\hat{q}$ fits in a single limb at the end of step D3, i.e. that the subsequent multiplication $\hat{q}\cdot v$ is a multiplication by a single-limb number. This creates an additional performance improvement since we perform a $1\times n$ multiplication instead of a $2\times n$ one.

These improvements are expressed in steps D3 (`Trial`) and D4 (`Fix`) of Knuth's Algorithm D.

*TAOCP Vol. 2, §4.3.1, Algorithm D, steps D3-D4 (pp. 272–273).* Indices renamed to correspond to **n+1/n** division.

> **D3.** [Calculate q̂.] Set q̂ ← ⌊(uₙb + uₙ₋₁)/vₙ₋₁⌋ and let r̂ be the remainder, (uₙb + uₙ₋₁) mod vₙ₋₁. Now test if q̂ ≥ b or q̂·vₙ₋₂ > b·r̂ + uₙ₋₂; if so, decrease q̂ by 1, increase r̂ by vₙ₋₁, and repeat this test if r̂ < b. (The test on vₙ₋₂ determines at high speed most of the cases in which the trial value q̂ is one too large, and it eliminates all cases where q̂ is two too large; see exercises 19, 20, 21.)

> **D4.** [Multiply and subtract.] Replace (uₙ, uₙ₋₁, ..., u₀)_b by (uₙ, uₙ₋₁, ..., u₀)_b − q̂·(0, vₙ₋₁, ..., v₁, v₀)_b. This computation (analogous to steps M3, M4, and M5 of Algorithm M) consists of a simple multiplication by a one-place number, combined with a subtraction. The digits (uₙ, uₙ₋₁, ..., u₀) should be kept positive; if the result of this step is actually negative, (uₙ, uₙ₋₁, ..., u₀)_b should be left as the true value plus bⁿ⁺¹, namely as the b's complement of the true value, and a "borrow" to the left should be remembered.

```
Algorithm 3: Medium Division, Knuth
Input:
        u = (u_n, u_{n-1}, ..., u_0)_b,  ⌊u/b⌋ < v          // hence q < b
        v = (v_{n-1},      ..., v_0)_b,  v_{n-1} ≥ ⌊b/2⌋    // normalised
D3:
        (q̂, r̂) = div((u_n, u_{n-1}), v_{n-1})          // one 2/1 division; q̂ two limbs
        if q̂ ≥ b or q̂·v_{n-2} > b·r̂ + u_{n-2}:
            q̂ -= 1;  r̂ += v_{n-1};
        if (r̂ < b) and (q̂ ≥ b or q̂·v_{n-2} > b·r̂ + u_{n-2}):
            q̂ -= 1;  r̂ += v_{n-1};
D4:                                                    // q̂ down to 1 limb
        r = u - q̂·v                                    // one 1xn mul, one mp subtraction
D5:
        if r < 0:  q̂ -= 1, r += v                      // underflow -> 1st mp addition correction
Return: (q̂, r)                                         // (q̂, r) = (q, r), correct by Theorem B (?)
Invariant:
        u = q̂·v + r                                    // holds after every line
```

An example of Knuth's trial quotient approximation can be seen below.

> **Interactive animation — Algorithm D (Knuth), 3129 / 314 (b = 10).**
> Same division with Knuth's D3 correction step: while q̂ ≥ b or q̂·v_{n-2} > b·r̂ + u_{n-2}, decrement q̂ and add v_{n-1} to r̂, stopping once r̂ ≥ b. 16 steps.
> *(Step-through animation; see the [original article](https://kolja.rs/algorithm-d).)*

An attentive reader will notice an issue in Algorithm 3. In Theorem B we had the bound $\hat{q}\le q+3$, but step D3 does at most 2 corrections after which it expects $\hat{q}$ to fit in one limb. The worst case, $q=b-1$ and $\hat{q}=q+3=b+2$, would violate the second property since the two corrections in D3 will not be enough to cram $\hat{q}$ in a single limb. Does this case actually happen, or is our bound in Theorem B not tight?

Indeed a case in which Algorithm D returns a wrong answer exists, and the exact setting is rather *odd*. The following is the smallest example of Algorithm D failing:

```
b = 3
u = (1,2,0,0)₃ = 45        q = ⌊45/16⌋ = 2
v =   (1,2,1)₃ = 16        q̂ = ⌊(1,2)₃/(1)₃⌋ = ⌊5/1⌋ = 5 = q + 3 = (1,2)₃
```

After two corrections of $\hat{q}$ in step D3, $\hat{q}$ will still be a two-limb value $\hat{q}=(1,0)_3$. From that point onwards the $1\times n$ multiplication sees only the low limb of $\hat{q}$ so it multiplies with zero, subtracts nothing, and the errors propagate.

The animation below shows the erroneous division:

> **Interactive animation — Algorithm D in base 3, (1200)₃ ÷ (121)₃.**
> The counterexample: the D3 loop decrements q̂ twice and still leaves q̂ = b + 1, out of range. 9 steps.
> *(Step-through animation; see the [original article](https://kolja.rs/algorithm-d).)*

### How did it stay hidden for decades

If you continue reading chapter 4.3.1 of TAOCP you will find, surprisingly, that Program D, Knuth's implementation of Algorithm D written in MIX[^6] assembly, does not exhibit this error. The program is indeed correct, the algorithm is wrong, and behind this difference lies the exact reason for the bug.

A meticulous reader would have noticed that the definition of the division instruction that I introduced is not the only one that exists. There are in fact (≥)two ways to implement a division instruction, divided along the question of what happens when the quotient does not fit in a single limb:

```
div_arm: (hi, lo), (y) -> (q1, q0), r
div_x86: (hi, lo), (y) ->     (q0), r, F
```

The former returns the full two-limb quotient and leaves division by zero undefined.[^7] It is a simplification of division on ARM, and of what `__udivti3` computes for 128-bit integers.[^8]

The latter returns a single-limb quotient and raises a flag in case of quotient overflow or division by zero. It is a simplification of division on x86.[^9] This is the division instruction used in Knuth's MIX machine.

The two approaches produce two different trial quotients:

$$
\hat{q}_\mathrm{arm}=\left\lfloor \frac{u_nb+u_{n-1}}{v_{n-1}}\right\rfloor ,\quad \hat{q}_{x86}=\min \{\hat{q}_\mathrm{arm},\;b-1\}.
$$

In the *first* and *second editions* of TAOCP Vol. II, Algorithm D used the saturated quotient $\hat{q}_{x86}$. This matched MIX's own `div` instruction. For *that* quantity the book's theorems are true:

*TAOCP Vol. 2, **Second Edition**, §4.3.1, Theorems A and B (pp. 256–257).* Theorems tidied for exposition:

> **Theorems A and B**: If $v_{n-1}\ge \lfloor b/2\rfloor$ then $q\le \hat{q}_{x86}\le q+2$.

Step D3 was worded as follows:[^10]

> **D3.** [Calculate q̂.] If uₙ = vₙ₋₁, set q̂ ← b-1; otherwise set q̂ ← ⌊(uₙb + uₙ₋₁)/vₙ₋₁⌋. Now test if q̂·vₙ₋₂ > (uₙb + uₙ₋₁ − q̂·vₙ₋₁)·b + uₙ₋₂; if so, decrease q̂ by 1 and repeat this test. (The latter test determines at high speed most of the cases in which the trial value q̂ is one too large, and it eliminates all cases where q̂ is two too large; see exercises 19, 20, 21.)

As written, both theorems, as well as Algorithm D, were correct.

In the '90s Knuth introduced a more modern abstract machine, the MMIX,[^11] to replace the outdated MIX. With this introduction came an overhaul of the original TAOCP books. Among other things, there was a change in notation, indices were re-written to little-endian 0-indexed, and algorithms were adjusted for MMIX where needed (though printed programs in the first three volumes are still written in MIX).

The main change to Algorithm D was introduced on 1995-Sep-28, as can be seen in [Knuth's errata](https://www-cs-faculty.stanford.edu/~knuth/taocp.html) of the second edition.
![step_d3_errata](https://bear-images.sfo2.cdn.digitaloceanspaces.com/kolja/step_d3_errata.webp)

The 1995 change swapped the $\hat{q}_{x86}$ trial quotient for $\hat{q}_\mathrm{arm}$, but it did not adjust Theorems A and B for it; they still only proved the bound for $\hat{q}_{x86}$. For most inputs this was not an issue as the trial quotients agree below $b$, but for the degenerate case $\hat{q}_\mathrm{arm}\ge b$ the gap $\hat{q}_\mathrm{arm}-q$ was technically unbounded, even for $q<b-1$. The updated Theorem B shows that the damage is not that severe. The bound fails only at $\hat{q}_\mathrm{arm}=q+3=b+2$, which in turn only happens when $b$ is odd, $v_{n-1}=(b-1)/2$, $u_n=(b-1)/2$, $u_{n-1}=b-1$.

### Can it be exploited

Not really, unless you are using Setun.[^12] Since the error requires an odd base, it cannot occur on modern machines, which use $b=2^{64}$, or in any case a very even $b$. Even MIX's generic $b$ gave way to MMIX's fixed $b=2^{64}$. So while the correction was needed for a full proof of the algorithm, it still performed correctly in practice.

Where might odd limbs arise at all? I am not aware of any systems that store integers in limbs of odd size.

One use case that crossed my mind is the [$p$-adic numbers](https://en.wikipedia.org/wiki/P-adic_number) for odd $p$.
A non-zero $p$-adic $x$ is usually represented as a pair $x=up^e$ where $e\in \mathbb{N}$ is the $p$-adic valuation of $x$ and $u\in \mathbb{Z}_p^{\times}$ a unit. The valuation can then be represented as a regular integer, and the unit as an infinite-digit number, truncated to whatever precision we are interested in.
Another way to see $\mathbb{Z}_p$ is as *"infinite-precision"* numbers $(x_0,x_1,\dots )$ with $0\le x_i<p$, which correspond to the $p$-adic numbers $x=\sum_{i=0}^{\infty}x_ip^i$.[^13] In this representation $e$ is simply the index of the lowest non-zero $x_i$, and $u$ the number "shifted" by $p^e$.
Basic arithmetic operations work the same as over (infinite) integers, with the corresponding instructions `add`,`sub`,`mul` using $p$ in place of $b$.[^14] On a finite machine these infinite integers would be truncated to a finite precision, bringing us to finite sequences and exactly odd-base multiprecision arithmetic. Division, however, would not generalise the same way, and the reason is that division with remainder rounds with respect to the Archimedean norm. The natural norm on the $p$-adics, however, is the $p$-adic valuation. Division with remainder with respect to that one is computed by [Hensel lifting](https://en.wikipedia.org/wiki/Hensel%27s_lemma), and I don't know a use-case where division with remainder, and thus the corresponding `div` and long division algorithm, would be used.

Another candidate is radix conversion.
Suppose we want to convert a number $x=(x_n,x_{n-1},\dots ,x_0)_3$ from base 3 into hexadecimal $(y_k,y_{k-1},\dots ,y_0)_{16}$.
The standard method is to divide $x$ repeatedly by $16$ with the arithmetic performed in base 3, and read the hex digits off the remainders. In fact the division when converting $45=0x2d=(1,2,0,0)_3$ from ternary to hexadecimal is $(1200)_3/(121)_3$, precisely the smallest counterexample in which the bug in Algorithm D occurs. However a much simpler way to do the same thing would be to convert the number directly into binary/hex/base $2^{64}$ e.g. via Horner's method, and then just read off the nibbles.

If you are aware of an actually practical use-case for long division for odd-limb integers please do let me know!

### The llvm "bug"

While searching for implementations of Knuth's Algorithm D, it was difficult to find one where the bug could realistically occur even under the assumption that the machine words were odd. The reason is that most of the implementations used a while loop for Step D3, and the while loop would correct the trial quotient a third time thus mitigating the bug. You can find a long list of implementations of Algorithm D, meticulously analysed, on [Stefan Kanthak's blog](https://skanthak.hier-im-netz.de/division.html), with various bugs and issues highlighted and commented. Every single one of them uses a while loop. They all implement this part of step D3

> *"Now test if q̂ ≥ b or q̂·vₙ₋₂ > b·r̂ + uₙ₋₂; if so, decrease q̂ by 1, increase r̂ by vₙ₋₁, and repeat this test if r̂ < b."*

as follows:

```
loop: if (q̂ ≥ b or q̂·v_{n-2} > b·r̂ + u_{n-2}):
    q̂ = q̂ - 1;
    r̂ = r̂ + v_{n-1};
    if (r̂ < b)
      goto loop;
```

Personally, that reading never occurred to me. I read the sentence, without a single doubt in my mind, as *"do this test **one more time** if $\hat{r}<b$"*:

```
if (q̂ ≥ b or q̂·v_{n-2} > b·r̂ + u_{n-2}):
    q̂ = q̂ - 1;
    r̂ = r̂ + v_{n-1};
    if ((r̂ < b) and (q̂ ≥ b or q̂·v_{n-2} > b·r̂ + u_{n-2}))
        q̂ = q̂ - 1;
        r̂ = r̂ + v_{n-1};
```

Two things point this way. First, the text does not say *"repeat this test **while** $\hat{r}<b$"*. Second, the sentence is immediately followed by

> *"(The test on vₙ₋₂ determines at high speed most of the cases in which the trial value q̂ is one too large, and it eliminates all cases where q̂ is two too large; see exercises 19, 20, 21.)"*

which implies that the test and the correction run at most twice (which is also expected from the original Theorem B). From my perspective the loop implementation is wrong, or at the very least not aligned with the text. The blog above even rephrases Knuth's sentence as *"repeat this test **while** r̂ is less than b"*, deviating from the original text.

In the end I did find an implementation with two if statements, and to my surprise I found another *"bug"*. The implementation is in llvm's arbitrary-precision integer library, [APInt.cpp](https://github.com/llvm/llvm-project/blob/5bf59e2c4b54a85e4e7f0e188b99061beb2708f6/llvm/lib/Support/APInt.cpp#L1312-L1474). It is the best documented and most readable implementation of Algorithm D that I encountered, fully commented with all steps from the book. It is written in C++ for 64-bit machines, uses 32-bit words and 64-bit double-words, and the division instruction is a plain `uint64_t / uint32_t` division (which promotes to a 64/64 division).

```c
    // D3. [Calculate q'.].
    //     Set qp = (u[j+n]*b + u[j+n-1]) / v[n-1]. (qp=qprime=q')
    //     Set rp = (u[j+n]*b + u[j+n-1]) % v[n-1]. (rp=rprime=r')
    // Now test if qp == b or qp*v[n-2] > b*rp + u[j+n-2]; if so, decrease
    // qp by 1, increase rp by v[n-1], and repeat this test if rp < b. The test
    // on v[n-2] determines at high speed most of the cases in which the trial
    // value qp is one too large, and it eliminates all cases where qp is two
    // too large.
    uint64_t dividend = Make_64(u[j+n], u[j+n-1]);
    DEBUG_KNUTH(dbgs() << "KnuthDiv: dividend == " << dividend << '\n');
    uint64_t qp = dividend / v[n-1];
    uint64_t rp = dividend % v[n-1];
    if (qp == b || qp*v[n-2] > b*rp + u[j+n-2]) {
      qp--;
      rp += v[n-1];
      if (rp < b && (qp == b || qp*v[n-2] > b*rp + u[j+n-2]))
        qp--;
    }
    DEBUG_KNUTH(dbgs() << "KnuthDiv: qp == " << qp << ", rp == " << rp << '\n');
```

On closer inspection we see that step D3 checks `qp == b`, that is, $\hat{q}=b$, while the quoted text has $\hat{q}\ge b$. What happened? The `==` was a typo in TAOCP Vol. II, corrected to `>=` in 2005. The D3 we quoted in [The bug](#the-bug) is the corrected text, and llvm's comment and code still preserve the old version.

Since $b=2^{32}$ is even we have $\hat{q}\le q+2\le b+1$, so the `==` check can only miss the $\hat{q}=b+1$ case. The $\hat{q}\ge b$ clause is genuinely needed at $\hat{q}=b$[^15] and for odd $b$ additional care is needed at $\hat{q}=b+2$.[^16] But in a weird stroke of luck, the $\hat{q}=b+1$ case is fully covered by the $\hat{q}\cdot v_{n-2}>b\cdot \hat{r}+u_{n-2}$ check.

However this property is not proven in TAOCP. It does not follow from Theorems A and B, nor from exercises 19, 20, 21. In fact exercise 20 which covers this property explicitly excludes the case $\hat{q}\ge b$ from the analysis of the $v_{n-2}$ check. So in the end llvm computes the right answer, but the proof for that can be found neither in the book nor in the code, only here[^17]. Still, even though it is not technically a bug, I opened a [non-functional-change PR](https://github.com/llvm/llvm-project/pull/215695) to align the check with the corrected text.

### AI didn't find it

![meticulous](https://bear-images.sfo2.cdn.digitaloceanspaces.com/kolja/meticulously.webp)
I gave Claude Fable 5 a simple prompt: a pdf of pp 270-275 of TAOCP Vol 2 including the whole long division algorithm, and a request to find a mistake, an error or a bug, to try hard, think long and return the findings.
It would go through the theorems and Algorithm D relatively quickly, and then it would spend so much time verifying the correctness of the complexity analysis of Program D that it burned through my weekly tokens. It did find an error, which it quickly retracted:

> *I have to correct myself before anything else: my previous answer was wrong. The "bug" I reported in Program D's running time was my own arithmetic mistake, not Knuth's.*

I then directed it to look only at the theorems and Algorithm D, but it didn't find anything. Even after telling it to concentrate on step D3 it was unsuccessful in finding the issue. Finally I told it to analyse the size of $\hat{q}$ in the D3 to D4 transition, and not come back until it found a bug. In the end it finally found it, but only after fetching the latest errata from Knuth's website.

I would certainly be interested in seeing how Mythos would have fared.

## The check

Knuth famously rewards every error found in TAOCP with one hexadecimal dollar (`0x$1.00`) deposited at the [Bank of San Serriffe](https://en.wikipedia.org/wiki/Knuth_reward_check). I received the check and I also received my letter back, with Knuth's handwritten notes and comments. In particular, he wrote:

> *"I'm especially glad to have this correction, because I think the readers of TAOCP Vol 2 look at Algorithm 4.3.1 D more than any other algorithm!"*

The correction was filed on 2026-05-14 and the errata was published on 2026-06-09. It will appear in print with the 53rd printing of Volume II. You can find the full errata on the [TAOCP website](https://www-cs-faculty.stanford.edu/~knuth/taocp.html), and the part pertinent to this bug [here](https://bear-images.sfo2.cdn.digitaloceanspaces.com/kolja/full_errata.webp).

By the way, if you noticed a typo in the errata ($v_1$ where $v_{n-1}$ is meant), it has already been acknowledged.

## A little trit more

I am planning to cover alternative division algorithms, and tricks for speeding up division, in a separate blog post. For completeness, I just want to give a high-level preview of the three main methods used to speed up long division.

### Stronger bounds

While Theorem A proves $q\le \hat{q}$ unconditionally, in Theorem B there are some conditions, such as the lower bound on $v''$. These theorems can be improved if we change the primitive we build them on from a **2/1** division to a **(k+1)/k** one for some $k\ge 2$.

Split the operands at position $n-k$ instead of $n-1$, so that $v''=(v_{n-1},\dots ,v_{n-k})_b$ is the top $k$ limbs and $v'$ the remaining $n-k$, similarly for $u$.
The same argument as in Theorem B' gives:

$$
\hat{q}<q+1+\frac{b}{v''}\le q+1+b^{2-k},
$$

so for $k\ge 2$ we get $q\le \hat{q}\le q+1$ *with no normalisation needed* since $v''\ge b^{k-1}$.

### Doubling the quotient limbs

We can compute **4/2** instead of **2/1** trial divisions, so that we obtain two limbs of the quotient per iteration and halve the number of passes over $u$. This approach would require a specialised **4/2** routine, but it would stay limited to a $\hat{q}\le q+2$ bound, as opposed to the tighter bound for **3/2** divisions. We could go further by trying **8/4** and **16/8** divisions, and eventually arrive at [Burnikel-Ziegler's](https://pure.mpg.de/rest/items/item_1819444_4/component/file_2599480/content) recursive division algorithm.

### Division by a constant

Notice that all the divisions performed so far were divisions by $v$, or divisions by its top limb(s) $v''$. Even though the dividend changes over time during the division computation, the divisor always stays the same. This invariance can be used to further optimise the division operation. Division by a known fixed divisor [can be replaced](https://en.wikipedia.org/wiki/Division_algorithm#Division_by_a_constant) by a multiplication with a precomputed reciprocal, followed by a shift/index reordering. So no matter how long the division is, we need just one division instruction to precompute the reciprocal, and every trial quotient can be computed by a multiplication instead of a division.

This idea is originally due to [Granlund and Montgomery](https://gmplib.org/~tege/divcnst-pldi94.pdf). It can be further improved by combining it with a **3/2** division as above, due to [Möller and Granlund](https://gmplib.org/~tege/division-paper.pdf), which is used in GMP.

Another way to compute divisionless divisions, due to [Svoboda](https://en.wikipedia.org/wiki/Anton%C3%ADn_Svoboda_%28computer_scientist%29)[^18], is to normalise $v$ all the way to the form $(1,0,\dots )_b$ by multiplying $u$ and $v$ by a suitable factor, at the cost of at most one extra limb. The trial quotient is now simply read off the top limbs of the remainder, and this being a **3/2** division, the trial quotient is a tight approximation. However, the normalisation requires at least three division instructions with different dividends, which makes this algorithm overall costlier than Möller and Granlund.

Further improvements and some optimal results on replacing divisions with cheaper operations can also be found in a very approachable paper by [Lemire, Bartlett, Kaser](https://arxiv.org/pdf/2012.12369) and some great [Jeon's Dragonbox paper](https://fmt.dev/papers/Dragonbox.pdf) together with his [blog post](https://jk-jeon.github.io/posts/2023/08/optimal-bounds-integer-division/).

---

## Notes

[^1]: Division in modular reduction is taken care of by means of special prime forms or Montgomery/Barrett/similar "divisionless" reduction techniques; modular division is computed through modular inverses, and inverses come from Fermat's little theorem or a version of the extended Euclidean algorithm; division by a constant can be performed via a precomputation step and runtime multiplication, while the precomputation step can be done by means of Hensel lifting or similar methods. All of these "divisions" can be computed without *long* division.

[^2]: If we generalise further to a larger ring, for example polynomials or number rings, multiplication carries over simply, while division weakens with the introduction of new structure. For non-[Euclidean rings](https://en.wikipedia.org/wiki/Euclidean_domain) we might only get pseudo-division, for some not even that. The various ways of generalising division are too many to cover in this post.

[^3]: By "natural" I mean the first thing you'd think of. Schoolbook multiplication *"naturally"* computes output words from the lowest toward the highest, while Karatsuba and more advanced methods work recursively over the whole output at once. In a similar fashion there is a recursive division algorithm [Burnikel-Ziegler](https://pure.mpg.de/rest/items/item_1819444_4/component/file_2599480/content) which is out of scope for this post.

[^4]: $\lfloor \lfloor x/a\rfloor /c\rfloor =\lfloor x/(ac)\rfloor$ for $ x\in \mathbb{R};; a, c\in\mathbb{N}$.

[^5]: The Art of Computer Programming, Volume II, Chapter 4.3 Multiple-precision arithmetic, Exercise 23.

    > [M23] Given that $v$ and $b$ are integers, and that $1\le v<b$, prove that we always have $\lfloor b/2\rfloor \le v\lfloor b/(v+1)\rfloor$ $<(v+1)\lfloor b/(v+1)\rfloor \le b$.

[^6]: [MIX](https://en.wikipedia.org/wiki/MIX_%28abstract_machine%29) is an abstract computer introduced by Knuth in The Art of Computer Programming. It has a CISC instruction set, 9 registers, and a total of 4000 words of memory, each with 5 bytes and a sign. A byte can hold $b$ distinct values where $b\ge 64$ (so that any memory address can fit in two bytes). Technically the counterexample for Algorithm D with $b=3$ is not applicable to MIX, but one with $b=65$ is.

[^7]: On ARM, division by zero returns 0.

[^8]: `__udivti3` is the software builtin (libgcc / compiler-rt) that GCC, Clang, and Rust call for 128-bit unsigned division on targets with no native 128-bit divide. A **2/1** `u128 / u64` division compiles down to a call to it.

[^9]: Technically x86 does not raise a flag but instead raises the `#DE` division error exception.

[^10]: In previous editions vectors were indexed starting from 1 to n.
    ![step_D3_old](https://bear-images.sfo2.cdn.digitaloceanspaces.com/kolja/step_d3_old.webp)

[^11]: [MMIX](https://mmix.cs.hm.edu/index.html) is an abstract computer introduced by Knuth in the third edition of The Art of Computer Programming. It has a 64-bit RISC instruction set, 256 64-bit general purpose registers, 32 64-bit special-purpose registers, fixed-length 32-bit instructions and a 64-bit virtual address space. Notably it uses IEEE 754 floating-point numbers, which the MIX lacks.

[^12]: [Setun/Сетунь](https://en.wikipedia.org/wiki/Setun) is a Soviet ternary computer. Strictly speaking, balanced ternary. I am not sure how unsigned multiprecision arithmetic would look on it and if it would exhibit the same issue, though my hunch is that a balanced ternary system would inherently avoid it.

[^13]: Yet another way to see $\mathbb{Z}_p$ is as the inverse limit $\underset{\leftarrow}{\lim}\mathbb{Z}/p^n$, that is, as compatible infinite sequences $(x_1,x_2,\dots )\in \prod_{n=1}^{\infty}\mathbb{Z}/p^n$ where $x_{n+1}\equiv x_n\quad (\mathrm{mod}\;p^n)$.

[^14]: This would of course require implementing a new instruction set for the odd size $p$ since the $b=2^{64}$ instructions would not work. We need `add_p: x, y -> (x + y) % p, carry` and not `add: x, y -> (x + y) % 2^64, carry` etc.

[^15]: A counterexample which does not pass the $v_{n-2}$ check is $u=(t,0,0,0)_b,v=(t,0,1)_b$, where $t=\lfloor b/2\rfloor$. This is the smallest such example. For $b=2^{32}$ you can take `u=0x80000000_00000000_00000000_00000000` and `v=0x80000000_00000000_00000001`.

[^16]: The $v_{n-2}$ check can fail if overflow is not properly accounted for. Let $t=(b-1)/2=\lfloor b/2\rfloor$, and let $u=(t,2t,2t-1,0)_b,v=(t,2t,2t)_b$. Then $\hat{q}\cdot v_{n-2}=(b+2)(b-1)=b^2+b-2$. If a third register is not provided for storing $b^2$, this value will overflow, and the $v_{n-2}$ check will fail since $b-2\not>b-2=u_{n-2}$. This is the smallest such example.

[^17]: Proof: If $\hat{q}=b+1$, then $u=(t,u_{n-1},u_{n-2},\dots )$, $v=(t,v_{n-2},\dots )$ for $\lfloor b/2\rfloor \le t\le u_{n-1}\le v_{n-2}$. We have $\hat{r}=u_{n-1}-t$ and therefore $0\le \hat{r}\le v_{n-2}-t<v_{n-2}$. The clause $\hat{q}v_{n-2}>b\hat{r}+u_{n-2}$ follows from $b\hat{r}+u_{n-2}<b(\hat{r}+1)\le bv_{n-2}$ together with $bv_{n-2}<(b+1)v_{n-2}=\hat{q}v_{n-2}$. Furthermore the product $\hat{q}v_{n-2}\le (b+1)(b-1)\le b^2-1$ fits in two limbs without overflow.

[^18]: A. Svoboda, *Stroje na Zpracování Informací* **9** (1963), 25–32.

---

Tags: #TAOCP #bug #division #math

[View original](https://kolja.rs/algorithm-d)
