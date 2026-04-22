#!/usr/bin/env python3
"""Hamurabi game runner with baseline and improved strategy modes."""

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
    if population < fed:
        return False
    deaths = population - fed
    return deaths > 0.45 * population


def min_feed_avoid(population: int) -> int:
    for feed in range(20 * population + 300):
        if not would_impeach(population, feed):
            return feed
    return 20 * population


def legacy_plan_year(acres: int, population: int, store: int) -> Decision:
    need = min_feed_avoid(population)
    max_plant = min(acres, 10 * population - 1, 2 * store)
    for desired_plant in range(max_plant, -1, -1):
        seed = desired_plant // 2
        cap = store - seed
        feed = min(20 * population, cap)
        if feed < need:
            feed = cap
        if would_impeach(population, feed):
            continue
        if seed + feed <= store:
            reserve = store - seed - feed
            return Decision(0, 0, feed, desired_plant, reserve, "legacy greedy plant")
    feed = min(store, 20 * population)
    return Decision(0, 0, feed, 0, store - feed, "legacy fallback")


def plan_year(acres: int, population: int, store: int, price: int) -> Decision:
    need = min_feed_avoid(population)
    max_feed = 20 * population

    reserve_floor = max(100, 2 * population)
    trade_note = "hold"
    buy = 0
    sell = 0

    # Sell proactively if grain is not enough to avoid impeachment with buffer.
    if store < need + reserve_floor:
        deficit = need + reserve_floor - store
        sell = min(acres // 2, (deficit + price - 1) // price)
        trade_note = f"sell {sell} for food security" if sell > 0 else "hold (cannot sell enough)"
    # Buy only with clear surplus at low prices.
    elif price <= 17 and store > max_feed + reserve_floor + 400:
        surplus = store - (max_feed + reserve_floor + 400)
        buy = min(surplus // price, max(0, population // 2))
        trade_note = f"buy {buy} at low price" if buy > 0 else "hold (low-price but no budget)"

    acres_after_trade = acres + buy - sell
    store_after_trade = store - buy * price + sell * price
    max_plant = min(acres_after_trade, 10 * population - 1, 2 * store_after_trade)

    feed_target = min(max_feed, 19 * population)
    best: Decision | None = None
    best_score = -10**18
    for plant in range(max_plant, -1, -1):
        seed = plant // 2
        if seed > store_after_trade:
            continue
        remaining_after_seed = store_after_trade - seed
        feed = min(feed_target, remaining_after_seed)
        if feed < need:
            continue
        reserve = remaining_after_seed - feed
        short_target = max(0, feed_target - feed)
        short_full = max(0, max_feed - feed)
        # Balance expected grain growth from planting and food stability.
        score = plant * 3 + reserve - short_target * 6 - short_full * 2
        if score > best_score:
            best_score = score
            best = Decision(
                buy=buy,
                sell=sell,
                feed=feed,
                plant=plant,
                reserve=reserve,
                note=f"{trade_note}; target_feed={feed_target}; reserve_floor={reserve_floor}",
            )

    if best is not None:
        return best

    feed = min(max_feed, store_after_trade)
    return Decision(
        buy=buy,
        sell=sell,
        feed=feed,
        plant=0,
        reserve=store_after_trade - feed,
        note=f"{trade_note}; emergency feed-only",
    )


def run_one_game(strategy: str = "improved", verbose: bool = True) -> tuple[int, GameTrace]:
    child = pexpect.spawn("./hamurabi", cwd=REPO_ROOT, encoding="utf-8", timeout=10)
    child.logfile = sys.stdout if verbose else None
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

            if strategy == "legacy":
                decision = legacy_plan_year(acres, population, store)
            else:
                decision = plan_year(acres, population, store, price)

            child.expect("buy")
            child.sendline(str(decision.buy))

            # Game asks either sell (if buy was 0) or directly feed (if buy > 0).
            next_prompt = child.expect(["sell", "feed"])
            if next_prompt == 0:
                child.sendline(str(decision.sell))
                child.expect("feed")
            child.sendline(str(decision.feed))

            child.expect("plant")
            child.sendline(str(decision.plant))

            snapshots.append(
                YearSnapshot(
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
                )
            )

            if year == 10:
                idx = child.expect([r"So long for now", r"In your 10-year term", r"national fink"])
                if idx == 2:
                    return 1, GameTrace("fail", year, "national fink endgame", snapshots)
                child.expect(pexpect.EOF)
                return 0, GameTrace("win", None, "", snapshots)
    except pexpect.TIMEOUT:
        return 1, GameTrace("fail", current_year, "interaction timeout", snapshots)
    except pexpect.EOF:
        return 1, GameTrace("fail", current_year, "unexpected EOF", snapshots)

    return 1, GameTrace("fail", current_year, "loop ended unexpectedly", snapshots)


def print_failure_cases(cases: list[GameTrace], limit: int) -> None:
    print(f"\nBad cases (showing up to {limit}):")
    if not cases:
        print("- none")
        return
    for i, case in enumerate(cases[:limit], start=1):
        print(f"- case #{i}: failure_year={case.failure_year}, reason={case.failure_reason}")
        tail = case.years[-2:] if len(case.years) >= 2 else case.years
        for y in tail:
            print(
                f"  y{y.year}: pop={y.population} acres={y.acres} store={y.store} "
                f"price={y.price} buy={y.buy} sell={y.sell} feed={y.feed} "
                f"plant={y.plant} reserve={y.reserve}"
            )
            print(f"    note: {y.strategy_note}")


def run_trials(trials: int, strategy: str, bad_case_limit: int) -> int:
    failures: list[GameTrace] = []
    fail_years: list[int] = []
    wins = 0
    for _ in range(trials):
        code, trace = run_one_game(strategy=strategy, verbose=False)
        if code == 0:
            wins += 1
            continue
        failures.append(trace)
        if trace.failure_year is not None:
            fail_years.append(trace.failure_year)

    fails = trials - wins
    print(
        f"strategy={strategy} trials={trials} wins={wins} fails={fails} "
        f"win_rate={wins / trials:.3f}"
    )
    if fail_years:
        print(
            "failure_year_stats: "
            f"min={min(fail_years)} max={max(fail_years)} "
            f"median={int(statistics.median(fail_years))}"
        )
    print_failure_cases(failures, bad_case_limit)
    return 0 if fails == 0 else 1


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run Hamurabi strategy bot.")
    parser.add_argument(
        "--trials",
        type=int,
        default=1,
        help="Number of independent games to run (default: 1).",
    )
    parser.add_argument(
        "--strategy",
        choices=["legacy", "improved"],
        default="improved",
        help="Strategy mode to use.",
    )
    parser.add_argument(
        "--bad-case-limit",
        type=int,
        default=5,
        help="How many failing traces to print in trial mode.",
    )
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    if args.trials <= 1:
        code, _trace = run_one_game(strategy=args.strategy, verbose=True)
        return code
    return run_trials(args.trials, args.strategy, args.bad_case_limit)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
