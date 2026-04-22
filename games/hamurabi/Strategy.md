# Hamurabi strategy guide (current bot)

This guide documents the strategy currently implemented in `one_game.py`.
It focuses on practical decision rules used by the bot, not source-level
internals of the original game implementation.

The game is stochastic, so there is no guaranteed perfect sequence. The goal
is to maximize survival reliability (avoid impeachment), while still planting
enough to keep long-term grain and land-per-person healthy.

## Strategy goals

Each year the bot optimizes three priorities, in order:

1. Avoid immediate impeachment risk from starvation.
2. Maintain strong food coverage (close to full rations when possible).
3. Plant as much as possible without creating dangerous food shortfalls.

## Core constraints the bot always respects

- Feeding effectiveness is `feed // 20` people.
- Impeachment happens if more than `45%` of population starves in one year.
- Planting must satisfy:
  - `plant <= acres`
  - `plant < 10 * population`
  - `plant // 2 <= grain_after_trade`

## Decision pipeline per year

The improved policy computes one coordinated decision:

`Decision(buy, sell, feed, plant, reserve, note)`

based on the observed state:

`(acres, population, grain_store, land_price)`.

### 1) Compute safety targets

- `need = min_feed_avoid(population)`:
  minimum feed that avoids impeachment if feasible.
- `max_feed = 20 * population`:
  full rationing.
- `reserve_floor = max(100, 2 * population)`:
  conservative grain cushion target.
- `feed_target = min(max_feed, 19 * population)`:
  high-feeding objective used during optimization.

Interpretation:
- `need` is the hard safety floor.
- `feed_target` is the soft quality target.
- `reserve_floor` prevents all-in behavior that causes next-year collapse.

### 2) Conservative land trading

The bot trades only when strongly justified:

- **Sell for food security**  
  If `store < need + reserve_floor`, sell land to close the grain deficit:
  `sell = min(acres // 2, ceil(deficit / price))`.
- **Buy only with strong surplus at cheap prices**  
  If not selling, and `price <= 17`, and grain is well above
  `max_feed + reserve_floor + 400`, buy a bounded amount:
  `buy = min(surplus // price, population // 2)`.

This keeps trading rare and utility-driven, rather than speculative.

### 3) Joint feed/plant optimization

After trade effects are applied, the bot evaluates all feasible planting values
from high to low and computes corresponding feed/reserve values.

For each candidate `plant`:

- `seed = plant // 2`
- `remaining = store_after_trade - seed`
- `feed = min(feed_target, remaining)` but must satisfy `feed >= need`
- `reserve = remaining - feed`

Candidates are scored by:

- favoring larger `plant` (future production),
- favoring larger `reserve` (stability),
- penalizing shortfall from `feed_target` and from `max_feed`.

The highest-scoring feasible candidate is chosen.

### 4) Safety fallback

If no planting candidate can satisfy minimum feeding:

- set `plant = 0`
- feed as much as possible (`min(max_feed, store_after_trade)`)

This fallback reduces catastrophic starvation risk in bad years.

## Why this policy performs better

Compared to the previous greedy approach, the new strategy avoids two common
failure modes:

- **Over-planting with zero buffer**: old behavior often consumed nearly all
  grain for seed/feeding, then collapsed after a bad harvest or rats.
- **Myopic no-trade behavior**: old behavior could not convert land to food in
  emergency years.

The current policy explicitly protects a minimum food safety level, still
pushes planting when safe, and allows targeted land sales when survival is at
risk.

## Prompt-flow robustness note

The runtime interaction now correctly handles the game’s two valid prompt paths:

- if `buy == 0`: game asks `sell` then `feed`
- if `buy > 0`: game may skip `sell` and ask `feed` directly

The bot detects either prompt sequence and stays synchronized.

## Practical checklist (manual play)

If playing by hand with this strategy style:

1. First secure anti-impeachment feeding (`need`).
2. Keep a grain cushion (`reserve_floor`) before aggressive planting.
3. Plant heavily only when food safety remains strong.
4. Sell land when grain is insufficient for safe feeding; buy only when price is
   low and grain surplus is clearly large.
5. Never use all grain on one dimension (all food or all seed) unless forced.
