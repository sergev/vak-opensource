# Hamurabi strategy guide

This document summarizes the rules implemented in [`hamurabi.c`]([hamurabi.c](https://gitlab.com/esr/hamurabi/-/blob/master/hamurabi.c)) (Eric S. Raymond’s interlinear port of the 1973 BASIC game), explains how randomness affects play, and recommends a practical policy supported by Monte Carlo simulation.

The game is heavily random: there is **no** sequence of inputs that guarantees the best ending every time. The goal is to **maximize the chance** of surviving ten years without impeachment and to land in the top scoring tier when luck cooperates.

---

## Year loop (what happens each year)

Each turn the adviser prints a report, then you choose **land trade**, **feeding**, and **planting**, then the game resolves **harvest**, **rats**, **immigration**, and sets the **plague flag** for the *next* report.

Initial values are set at lines 157–167 in [`hamurabi.c`]([hamurabi.c](https://gitlab.com/esr/hamurabi/-/blob/master/hamurabi.c)): population `p = 95`, grain `s = 2800`, last harvest display uses `y = 3` bushels per acre, acres `a = h / y` with `h = 3000`, immigrants `i = 5`, plague gate `q = 1` (so the first year does not start with a plague halving).

### 1. Land price and trading

- Each year (after the report, before questions), the price per acre is `Y = INT(10 * RND()) + 17`, so **17–26** bushels per acre (lines 203–207).
- You are asked how many acres to **buy**. If that number is positive and affordable (`Y * buy <= s`), those acres are purchased and the game skips the sell prompt (lines 225–234).
- If you buy **zero** acres, you are asked how many acres to **sell**. You must sell **strictly fewer** than you own (`sell < a`) (lines 235–255).

### 2. Feeding

- You feed `q` bushels. The number of people who receive full rations is `fed = INT(q / 20)` (lines 360–361, starvation block uses the fed count).
- One bushel does not partially feed a person; only whole groups of **20 bushels** count toward full rations.

### 3. Planting

- Plant `d` acres only if `d <= a`, seed `INT(d / 2) <= s` (after paying for feeding), and **`d < 10 * p`** (strict inequality: `10 * p` acres is rejected) (lines 296–326).
- Seed is removed before harvest: `s -= INT(d / 2)` (lines 328–329).

### 4. Harvest

- Yield per acre is the first `L800()` roll: **1–5** bushels per acre (lines 331–337). Revenue `h = d * yield`.

### 5. Rats

- A second `L800()` roll `c` is drawn (lines 339–340).
- In C, `if (INT(c / 2.0) != c / 2.0) goto L530` **skips** assigning rat loss (lines 341–344).
- For integer `c`, that inequality holds when **`c` is odd**, so **no rats** then. When **`c` is even**, execution does **not** branch away: `E = INT(s / c)` (lines 347–348), where `s` is grain **after paying for seed**, before harvest income is added (lines 345–351).
- Then `s = s - E + h` (line 351).

### 6. Immigration

- Third `L800()` roll `c` (lines 352–357):
  `i = INT(c * (20 * a + s) / p / 100 + 1)`.
  More land and more grain in store increase immigration.

### 7. Starvation, population update, plague gate

- `fed = INT(q_feed / 20)` uses the **same** `q` as feeding input for this year (lines 360–361).
- If `p < fed`, everyone who exists can be counted as fed in the famine logic; the code jumps to `L210` (lines 366–368) without the usual `p = fed` assignment—so excess grain does not shrink population.
- Otherwise starvers `d_starve = p - fed`. If `d_starve > 0.45 * p`, you are **impeached** immediately (lines 372–375).
- Rolling average starvation rate `p1` is updated (lines 377–378), then `p = fed` (survivors).
- Plague draws `q = INT(10 * (2 * RND() - 0.3))` (lines 364–365). **Next year’s** report applies halving when `q <= 0` (lines 180–187).

---

## Endgame scoring (after the year-11 report)

When `z == 11`, the game prints totals and evaluates `p1` (average percent starved per year), `d1` (total deaths), and `l = a / p` (acres per person) (lines 399–410), then branches (lines 411–434):

| Message tier | Conditions (from source) |
|--------------|---------------------------|
| Disaster / “national fink” text path | `p1 > 33` **or** `l < 7` (then falls into harsh wording at 565) |
| “Heavy-handed” (Nero / Ivan) | else `p1 > 10` **or** `l < 9` |
| “Somewhat better” | else `p1 > 3` **or** `l < 10` |
| **“Fantastic performance”** | else (`p1 <= 3` **and** `l >= 10`) |

So the **best** ending needs **low average starvation** and **at least ~10 acres per person** at the end. Starting conditions imply about **10 acres per person** initially; maintaining or improving that ratio while keeping `p1` tiny is the scoring core.

---

## Hard constraints (instant loss)

- **Impeachment**: more than **45%** of the population starves in a single year (`d_starve > 0.45 * p`).
- **Invalid commands**: negative purchases or impossible land sales are rejected with game over (labels 850 / 990).

---

## Recommended policy

### A. Feed and plant together (most important)

The game always asks for **feeding before planting**. A naive approach is “feed `min(s, 20 * p)` then plant with what is left.” That **starves the treasury of seed** and can force `0` planted acres after a thin year; the next year’s harvest collapses and you spiral.

**Better:** decide planted acres `d` and feeding `q` jointly so that:

1. **Grain balance:** `INT(d / 2) + q <= s` after trading (seed plus feed cannot exceed stores).
2. **Labor:** `d < 10 * p` and `d <= a`.
3. **Survival:** whenever possible, choose `q` high enough that `fed = INT(q / 20)` avoids impeachment (`d_starve <= 0.45 * p`). When grain is insufficient to feed everyone, prioritize **minimum safe feeding** before spending on seed.
4. **Objective:** among feasible pairs `(q, d)`, prefer **larger `d`** when it still allows safe feeding—planted acres drive expected harvest (`d` times mean yield 3).

### B. Land trading

Monte Carlo over **80 000** distinct `srand(seed)` runs (same `rand()` model as the C binary on this platform) compared:

| Policy (planting + trading) | Impeachment rate | “Fantastic” tier (share of all trials) |
|------------------------------|------------------|----------------------------------------|
| Naive feed-then-plant, no trade | ~67.5% | ~21.4% |
| Coordinated feed/plant, **no trade** | ~31.5% | ~30.0% |
| Coordinated + buy/sell bands (cheap buy / dear sell) | ~31–46% depending on bands | **Lower** than coordinated no-trade in tested bands |

**Conclusion:** simple **band trading** (buy when price is low, sell when high) **hurt** the top-tier rate in this simulation because purchases spent grain that was needed for safe feeding and seed, and sales trimmed acres needed for `l >= 10`. The best *tested* default is **coordinated feeding and planting with no land trading**.

If you trade manually, keep a **large cushion**: never buy so much that the next year cannot both **hit minimum safe feeding** and **fund seed** for enough acres to recover.

### C. What you cannot control

- **Plague** halves population when the prior year’s plague draw `q <= 0`. You cannot prevent it; you can only keep **grain and acreage** from collapsing so badly that the next year becomes an impeachment roll.
- **Rats** remove up to `INT(s / c)` when the second harvest-phase die `c` is **even**, with `s` measured after seeding.
- **Yield 1–5** is uniform in expectation (mean 3). Long-run growth favors **keeping people alive and acres planted**, not betting on one lucky harvest.

---

## Tools in this repository

| Script | Purpose |
|--------|---------|
| [`one_game.expect`](one_game.expect) | Drives the `hamurabi` binary using **pexpect** and the coordinated plan. |

---

## Brief “checklist” per year

1. Read `p`, `a`, `s`, and land price `Y`.
2. **Trading:** skip or be very conservative; default **no trade** scored best in simulation.
3. **Feed + plant:** use a coordinated plan—**do not** spend all grain on food if that forces **zero** seed unless you have no alternative.
4. Verify mentally: `INT(feed / 20)` leaves you short of impeachment; `INT(plant / 2) + feed <= s`; `plant < 10 * p`.

Following that pattern maximizes your **chance** of finishing with both **low `p1`** and **healthy `l`**, which is what the code rewards as a “fantastic performance.”
