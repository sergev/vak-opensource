#!/usr/bin/env python3
"""Hamurabi game runner with naive and improved strategy modes."""

from __future__ import annotations

import argparse
import os
import statistics
import sys
from dataclasses import dataclass

import pexpect

REPO_ROOT = os.path.dirname(os.path.abspath(__file__))


@dataclass
class YearSnapshot:
    year: int
    population: int
    acres: int
    store: int
    price: int
    buy: int
    sell: int
    feed: int
    plant: int
    reserve: int
    strategy_note: str


@dataclass
class GameTrace:
    outcome: str
    failure_year: int | None
    failure_reason: str
    years: list[YearSnapshot]
    final_acres_per_person: float | None = None
    avg_starved_pct: float | None = None   # game's p1: % of population starved per year, averaged


@dataclass
class Decision:
    buy: int
    sell: int
    feed: int
    plant: int
    reserve: int
    note: str


def would_impeach(population: int, feed: int) -> bool:
    fed = feed // 20
    deaths = max(0, population - fed)
    return deaths > 0.45 * population


def min_feed_avoid(population: int) -> int:
    """Minimum grain to feed enough people that we avoid the >45% starvation impeachment."""
    for feed in range(20 * population + 300):
        if not would_impeach(population, feed):
            return feed
    return 20 * population


def project_immigration(multiplier: int, acres: int, store: int, population: int) -> int:
    """Game immigration formula: i = INT(c*(20*a+s)/p/100+1)."""
    return int(multiplier * (20 * acres + store) / max(1, population) / 100 + 1)


# ---------------------------------------------------------------------------
# Naive strategy: never trade land, feed everyone, plant as much as possible.
# ---------------------------------------------------------------------------

def naive_plan_year(acres: int, population: int, store: int) -> Decision:
    feed = min(20 * population, store)
    leftover = store - feed
    plant = min(acres, 10 * population - 1, 2 * leftover)
    plant = max(0, plant)
    reserve = leftover - plant // 2
    return Decision(0, 0, feed, plant, reserve, "naive")


# ---------------------------------------------------------------------------
# Improved strategy: Feed-Plant-Land
#
# Each year we allocate grain in priority order:
#   1. Feed everyone (avoid starvation and impeachment).
#   2. Keep a reserve against rats and bad harvests.
#   3. Plant as much as possible with what remains (highest long-term ROI).
#   4. Buy land with any surplus beyond reserve (price-sensitive).
#
# Sell land only as a last resort to avoid impeachment.
# ---------------------------------------------------------------------------

# Grain per person to keep as post-feed/post-seed reserve against bad harvests and rats.
_RESERVE_PER_PERSON = 4
_RESERVE_MIN = 400

# Land target ratio (acres per projected next-year person).
# Early game: 11 a/p builds a buffer for immigration growth.
# Late game (year >= 9): target exactly 10 a/p.
_LAND_RATIO_EARLY = 11
_LAND_RATIO_LATE = 10
_LATE_YEAR = 9

# Acres/person floor below which we treat land as too precious to sell voluntarily.
# 7 is the game's national-fink threshold, so we add a small margin.
_SOFT_LAND_FLOOR = 8   # sell freely above this; stop voluntary sells below
_HARD_LAND_FLOOR = 1   # absolute minimum (1 acre per person) for impeachment-avoidance sells


def plan_year(acres: int, population: int, store: int, price: int, year: int) -> Decision:
    full_feed = 20 * population
    safe_feed = min_feed_avoid(population)
    reserve_target = max(_RESERVE_MIN, population * _RESERVE_PER_PERSON)

    # Estimate next-year population at median immigration multiplier (c = 3).
    imm_est = project_immigration(3, acres, store, population)
    next_pop = population + imm_est

    ratio = _LAND_RATIO_LATE if year >= _LATE_YEAR else _LAND_RATIO_EARLY
    land_target = ratio * next_pop
    land_gap = land_target - acres

    # ------------------------------------------------------------------
    # Step 1: compute surplus grain in a no-trade plan.
    #
    # No-trade allocation: feed → reserve_target → plant (seed) → surplus.
    # ------------------------------------------------------------------
    feed_0 = min(full_feed, store)
    leftover_0 = store - feed_0
    seed_budget_0 = max(0, leftover_0 - reserve_target)
    plant_0 = max(0, min(acres, 10 * population - 1, 2 * seed_budget_0))
    seed_cost_0 = plant_0 // 2
    surplus_0 = max(0, leftover_0 - seed_cost_0 - reserve_target)

    # ------------------------------------------------------------------
    # Step 2: trade decision.
    #
    # Buy land conservatively: require 4× price in surplus, spend only 1/3 of it.
    # This keeps a large extra cushion to absorb bad harvests without starvation.
    # Voluntary sell: only when we have surplus land and grain is genuinely tight.
    # ------------------------------------------------------------------
    buy, sell = 0, 0

    if land_gap > 0 and surplus_0 >= price * 4:
        buy = min(land_gap, (surplus_0 // 3) // price)

    elif buy == 0 and land_gap < 0 and year < 8:
        surplus_acres = max(0, acres - land_target)
        if store < full_feed + reserve_target:
            shortfall = full_feed + reserve_target - store
            sell = min(surplus_acres, (shortfall + price - 1) // price)
            sell = min(sell, max(0, acres - _SOFT_LAND_FLOOR * population))

    # Apply trade.
    acres_now = acres + buy - sell
    store_now = store - buy * price + sell * price

    # ------------------------------------------------------------------
    # Step 3: emergency sell to avoid starvation.
    #
    # Priority: cancel any buy first, then sell land.
    # Soft sell: toward full_feed staying above 8 a/p.
    # Hard sell: toward safe_feed (impeachment floor) down to 1 a/p.
    # We stop at safe_feed rather than full_feed: selling more to save
    # everyone now keeps a larger population alive to face the next crisis,
    # which empirically shifts rather than reduces total starvation.
    # ------------------------------------------------------------------
    if store_now < full_feed:
        if buy > 0:
            store_now += buy * price
            acres_now -= buy
            buy = 0

        # Soft sell: toward full_feed, staying above 8 a/p.
        if store_now < full_feed:
            soft_cap = max(0, acres_now - _SOFT_LAND_FLOOR * population)
            shortfall = full_feed - store_now
            soft_sell = min(soft_cap, (shortfall + price - 1) // price)
            sell += soft_sell
            acres_now -= soft_sell
            store_now += soft_sell * price

        # Hard sell: toward safe_feed if still below it, down to 1 a/p.
        # safe_feed is the minimum to avoid the >45% impeachment threshold.
        # Going further (toward full_feed) keeps more people alive but means
        # a larger population to feed next year — empirically this shifts
        # starvation rather than reducing it, so we stop at safe_feed.
        if store_now < safe_feed:
            hard_cap = max(0, acres_now - _HARD_LAND_FLOOR * population)
            shortfall = safe_feed - store_now
            hard_sell = min(hard_cap, (shortfall + price - 1) // price)
            sell += hard_sell
            acres_now -= hard_sell
            store_now += hard_sell * price

    # ------------------------------------------------------------------
    # Step 4: allocate remaining grain to feed → reserve → plant.
    # ------------------------------------------------------------------
    feed = min(full_feed, store_now)
    leftover = store_now - feed
    seed_budget = max(0, leftover - reserve_target)
    plant = max(0, min(acres_now, 10 * population - 1, 2 * seed_budget))
    reserve = leftover - plant // 2

    return Decision(
        buy, sell, feed, plant, reserve,
        f"y{year} gap={land_gap} buy={buy} sell={sell} price={price}",
    )


# ---------------------------------------------------------------------------
# Game runner
# ---------------------------------------------------------------------------

def run_one_game(
    strategy: str = "improved",
    verbose: bool = True,
    seed: int | None = None,
) -> tuple[int, GameTrace]:
    argv = ["./hamurabi"]
    if seed is not None:
        argv.extend(["-s", str(seed)])
    child = pexpect.spawn(argv[0], argv[1:], cwd=REPO_ROOT, encoding="utf-8", timeout=10)
    child.logfile_read = sys.stdout if verbose else None
    snapshots: list[YearSnapshot] = []
    current_year = 1

    try:
        for year in range(1, 11):
            current_year = year
            idx = child.expect([r"national fink", r"Population is now (\d+)"])
            if idx == 0:
                return 1, GameTrace("fail", year, "national fink", snapshots)

            population = int(child.match.group(1))
            child.expect(r"The city now owns (\d+) acres")
            acres = int(child.match.group(1))
            child.expect(r"You now have (\d+) bushels in store")
            store = int(child.match.group(1))
            child.expect(r"Land is trading at (\d+) bushels per acre")
            price = int(child.match.group(1))

            if strategy == "naive":
                decision = naive_plan_year(acres, population, store)
            else:
                decision = plan_year(acres, population, store, price, year)

            child.expect("buy")
            child.sendline(str(decision.buy))

            next_prompt = child.expect(["sell", "feed"])
            if next_prompt == 0:
                child.sendline(str(decision.sell))
                child.expect("feed")
            child.sendline(str(decision.feed))

            child.expect("plant")
            child.sendline(str(decision.plant))

            snapshots.append(YearSnapshot(
                year=year,
                population=population,
                acres=acres,
                store=store,
                price=price,
                buy=decision.buy,
                sell=decision.sell,
                feed=decision.feed,
                plant=decision.plant,
                reserve=decision.reserve,
                strategy_note=decision.note,
            ))

            if year == 10:
                # Two end-game paths after the plant input:
                # A) year-10 starvation >45% → "national fink" (no summary printed)
                # B) normal finish → year-11 report, then "In your 10-year term" summary
                idx = child.expect([r"national fink", r"In your 10-year term"])
                if idx == 0:
                    return 1, GameTrace("fail", year, "national fink", snapshots)

                # Capture p1 (game's rolling average starvation %) and final acres/person.
                child.expect(r"([0-9]+\.?[0-9]*) percent of the")
                avg_starved_pct = float(child.match.group(1))
                child.expect(r"ended with\s+([0-9]+\.?[0-9]*) acres per person")
                final_acres_per_person = float(child.match.group(1))

                # Check for endgame national fink (p1>33 or l<7): it appears before EOF.
                idx2 = child.expect([r"national fink", pexpect.EOF])
                if idx2 == 0:
                    return 1, GameTrace("fail", year, "national fink endgame", snapshots,
                                        final_acres_per_person, avg_starved_pct)
                return 0, GameTrace("win", None, "", snapshots,
                                    final_acres_per_person, avg_starved_pct)

    except pexpect.TIMEOUT:
        return 1, GameTrace("fail", current_year, "timeout", snapshots)
    except pexpect.EOF:
        return 1, GameTrace("fail", current_year, "unexpected EOF", snapshots)

    return 1, GameTrace("fail", current_year, "loop ended unexpectedly", snapshots)


# ---------------------------------------------------------------------------
# Trial runner and reporting
# ---------------------------------------------------------------------------

def print_failure_cases(cases: list[GameTrace], limit: int) -> None:
    print(f"\nBad cases (showing up to {limit}):")
    if not cases:
        print("  none")
        return
    for i, case in enumerate(cases[:limit], start=1):
        print(f"  case #{i}: failure_year={case.failure_year}, reason={case.failure_reason}")
        tail = case.years[-2:] if len(case.years) >= 2 else case.years
        for y in tail:
            print(
                f"    y{y.year}: pop={y.population} acres={y.acres} store={y.store} "
                f"price={y.price} buy={y.buy} sell={y.sell} feed={y.feed} "
                f"plant={y.plant} reserve={y.reserve}"
            )
            print(f"      {y.strategy_note}")


def run_trials(
    trials: int,
    strategy: str,
    bad_case_limit: int,
    seed: int | None = None,
) -> int:
    failures: list[GameTrace] = []
    fail_years: list[int] = []
    starved_pct_values: list[float] = []
    acres_per_person_values: list[float] = []
    wins = 0

    for i in range(trials):
        trial_seed = seed + i if seed is not None else None
        code, trace = run_one_game(strategy=strategy, verbose=False, seed=trial_seed)
        if trace.avg_starved_pct is not None:
            starved_pct_values.append(trace.avg_starved_pct)
        if trace.final_acres_per_person is not None:
            acres_per_person_values.append(trace.final_acres_per_person)
        if code == 0:
            wins += 1
        else:
            failures.append(trace)
            if trace.failure_year is not None:
                fail_years.append(trace.failure_year)

    fails = trials - wins
    print(f"strategy={strategy}  trials={trials}  wins={wins}  fails={fails}  win_rate={wins/trials:.3f}")
    if starved_pct_values:
        print(
            f"starved_pct: avg={statistics.mean(starved_pct_values):.2f}%"
            f"  median={statistics.median(starved_pct_values):.2f}%"
            f"  max={max(starved_pct_values):.1f}%"
        )
    if acres_per_person_values:
        print(
            f"acres/person: avg={statistics.mean(acres_per_person_values):.1f}"
            f"  median={statistics.median(acres_per_person_values):.1f}"
            f"  min={min(acres_per_person_values):.1f}"
        )
    if fail_years:
        print(
            f"fail years:  min={min(fail_years)}  max={max(fail_years)}"
            f"  median={int(statistics.median(fail_years))}"
        )
    print_failure_cases(failures, bad_case_limit)
    return 0 if fails == 0 else 1


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run Hamurabi strategy bot.")
    parser.add_argument("--trials", type=int, default=1,
                        help="Number of games to run (default: 1).")
    parser.add_argument("--strategy", choices=["naive", "improved"], default="improved",
                        help="Strategy to use (default: improved).")
    parser.add_argument("--bad-case-limit", type=int, default=5,
                        help="Failing traces to print in trial mode (default: 5).")
    parser.add_argument("--seed", type=int, default=None, metavar="NUM",
                        help="PRNG seed; with --trials N uses NUM, NUM+1, …, NUM+N-1.")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    if args.trials <= 1:
        code, _trace = run_one_game(strategy=args.strategy, verbose=True, seed=args.seed)
        return code
    return run_trials(args.trials, args.strategy, args.bad_case_limit, seed=args.seed)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
