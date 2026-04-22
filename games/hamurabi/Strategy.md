# Hamurabi strategy guide (scenario-search policy)

This document describes the strategy implemented in `one_game.py`. The game is
stochastic, so no policy can guarantee a fixed outcome on every seed. The bot is
tuned for a **balanced** objective:

1. **Land quality:** keep a credible path to finishing at **`>= 10 acres/person`**
   (and avoid selling below that baseline except where the rules force trade-offs).
2. **Human cost:** keep starvation low and heavily penalize **large** death counts
   relative to population.

Stated goals in the intro are ordered that way; the rank tuple below encodes the
same priorities (land signals first, then death severity, then finer risk metrics).

## Rule timing that drives strategy

The order in `hamurabi.c` matters for planning:

- You choose **buy / sell**, then **feed**, then **plant** from the state shown at
  the start of the year.
- **Harvest** and **rats** then change grain in store.
- **Immigration** depends on the new `(acres, store, population)` after that.
- **Plague** may halve population before the next year’s report.

Because randomness happens **after** your inputs, the planner scores each candidate
across many synthetic next-year outcomes instead of trusting a single point estimate.

## Core constants (per year)

These mirror the bot’s internal targets (see `plan_year` in `one_game.py`):

- `need` — minimum bushels to feed so impeachment is avoided (`>45%` starvation rule).
- `full_feed = 20 * population` — feed everyone (no starvation that year).
- `reserve_target = max(220, 4 * population)` — desired grain left after seed and
  feed as a buffer against rats and bad harvests.
- `acres_target` — land level the policy steers toward, usually `10 * next_pop_est`
  where `next_pop_est` uses mid-range immigration; from **year 8 onward** the
  target is at least `10 * population` so late-game does not undershoot the final
  acres/person bar.

## Trade candidate generation

The bot never buys and sells land in the same year (same as the game).

### Buys

- Several bounded buy sizes (including fractions of `land_gap` toward
  `acres_target`, plus population-scaled steps).
- A buy is only added if post-purchase store remains at least
  `full_feed + reserve_target // 2`
  so purchases do not immediately risk impeachment.

### Sells

Land floors (all in **acres** after the sell):

- **Hard floor:** `acres > population // 2` (never sell down to half an acre per
  person in the worst case).
- **Soft floor:** `acres > 2 * population` for some “gentler” sell candidates.
- **Strategic floor:** `acres > 10 * population` — the policy treats **10 acres per
  person** as the normal minimum land bank.

**Emergency** means `store < full_feed`. The default sell cap is
`acres - 10 * population` (only surplus above the strategic line). In an emergency,
if land is **already** at or below `10 * population`, the bot **does not** try to
sell enough for full feeding; it caps required selling toward **impeachment-safe**
grain (`need`) instead, to avoid digging a deeper hole in acres/person.

## Plant sampling

For each `(buy, sell)` option, planting levels are a **bounded sample**: zero,
maximum legal plant, points near the boundary where seed still allows `full_feed`,
and a coarse grid. That keeps runtime reasonable while still exploring meaningful
feed vs plant trade-offs.

## Scenario grid (next-year stress test)

For each feasible `(buy, sell, plant, feed)` the bot aggregates over a small grid:

- Harvest yield `1..5`
- Rats: none, or loss `reserve // d` for `d` in `{1, 3, 5}` (odd divisors)
- Immigration multiplier `1..5` (same formula as the game, `project_immigration`)
- Plague on/off (halve next population or not)

From each scenario it tracks gaps for next-year feeding, grain vs full feed, and
land vs `10 * next_population`, plus acres/person shortfall from 10.

## Lexicographic ranking (exact tie-break order)

Candidates that fail `need` or would impeach are discarded. Among survivors the
**lower tuple wins** in this order:

1. `miss_land_goal` — 1 if current post-trade acres are below `10 * population`, else 0.
2. `projected_land_gap` — `max(0, acres_target - acres_after_trade)`.
3. `severe_death` — 1 if deaths exceed `population // 20`, else 0.
4. `deaths` — current-year starvation deaths.
5. `current_land_gap` — `max(0, 10 * population - acres_after_trade)`.
6. `short_full` — bushels short of `full_feed` if not everyone is fed.
7. `reserve_gap` — shortfall vs `reserve_target` after seed and feed.
8. `avg_acres_per_person_gap` — mean next-scenario gap from 10 acres/person (scaled).
9. `worst_acres_gap` — worst-case next-scenario land shortfall vs `10 * next_pop`.
10. `avg_death_gap` / `worst_death_gap` — next-year feedability stress.
11. `avg_store_deficit` / `worst_store_deficit` — grain stress vs full feed.
12. `-acres_after_trade` — prefer **more** land when earlier terms tie.
13. `-plant` — prefer **more** planting when still tied.

So: **fix land vs the 10 line first**, then **avoid mass starvation**, then smooth
next-year risk, then push productivity.

## Fallback path

If no candidate from the search survives the filters, the bot uses a last-resort
branch: sell only up to keeping at least **`10 * population` acres** when possible;
if already at or below that line, it only sells toward **`need`** when store is
below `need`, then feeds as much as grain allows.

## Endgame scoring note

The C port prints term summary numbers with **one decimal** (`hamurabi.c`), which
keeps regex parsing of “acres per person” stable for `one_game.py`.

## Practical expectations

- You should see **fewer** finishes below `10 acres/person` than under a naive or
  purely greedy policy, because land deficit and projected gap are at the top of
  the rank tuple and selling below `10 * population` is constrained.
- **Some** starvation may still occur in exchange for land safety; the `severe_death`
  and `deaths` terms limit how far the bot goes in that direction.
- Rare total failures remain possible under hostile random sequences; the doc above
  describes intent and mechanics, not a proof of optimality.
