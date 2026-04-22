#!/usr/bin/env python3
"""Hamurabi game runner with baseline and improved strategy modes."""

from __future__ import annotations

import argparse
import math
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
    starved: int
    strategy_note: str


@dataclass
class GameTrace:
    outcome: str
    failure_year: int | None
    failure_reason: str
    years: list[YearSnapshot]
    final_acres_per_person: float | None = None


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


def starvation_deaths(population: int, feed: int) -> int:
    fed = feed // 20
    return max(0, population - fed)


def min_feed_avoid(population: int) -> int:
    for feed in range(20 * population + 300):
        if not would_impeach(population, feed):
            return feed
    return 20 * population


def project_immigration(multiplier: int, acres: int, store: int, population: int) -> int:
    return int(multiplier * (20 * acres + store) / max(1, population) / 100 + 1)


def sample_plant_levels(max_plant: int, store_after_trade: int, full_feed: int) -> list[int]:
    if max_plant <= 0:
        return [0]
    levels: set[int] = {0, max_plant}
    seed_headroom_for_full_feed = max(0, store_after_trade - full_feed)
    plant_for_full_feed = min(max_plant, seed_headroom_for_full_feed * 2)
    for delta in (-20, -10, -5, 0, 5, 10, 20):
        candidate = plant_for_full_feed + delta
        if 0 <= candidate <= max_plant:
            levels.add(candidate)
    coarse_step = max(1, max_plant // 12)
    for plant in range(0, max_plant + 1, coarse_step):
        levels.add(plant)
    return sorted(levels)


def build_trade_options(
    acres: int,
    population: int,
    store: int,
    price: int,
    year: int,
    acres_target: int,
    need: int,
    full_feed: int,
    reserve_target: int,
) -> list[tuple[int, int, str]]:
    options: list[tuple[int, int, str]] = [(0, 0, "hold")]

    def add_option(buy: int, sell: int, note: str) -> None:
        if buy < 0 or sell < 0:
            return
        if buy > 0 and sell > 0:
            return
        if buy > 0 and buy * price > store:
            return
        if sell > 0 and sell >= acres:
            return
        candidate = (buy, sell, note)
        if candidate not in options:
            options.append(candidate)

    # Buy set: aggressively close land deficit while keeping food safety.
    max_buy = store // max(1, price)
    land_gap = max(0, acres_target - acres)
    if max_buy > 0:
        buy_points = {
            1,
            min(max_buy, max(1, population // 10)),
            min(max_buy, max(1, population // 5)),
            min(max_buy, max(1, population // 3)),
            min(max_buy, land_gap),
            min(max_buy, land_gap // 2 if land_gap > 1 else land_gap),
            min(max_buy, max(0, land_gap + population // 10)),
            min(max_buy, max(0, land_gap + population // 4)),
            min(max_buy, max(1, population // 2)),
        }
        for buy in sorted(p for p in buy_points if p > 0):
            buffer_after = store - buy * price
            if buffer_after >= full_feed + reserve_target // 2:
                note = "buy cheap deficit cover" if price <= 20 else "buy strategic"
                add_option(buy, 0, note)

    # Sell set: fund food safety while preserving enough land for endgame quality.
    hard_floor = max(1, population // 2)
    soft_floor = max(1, 2 * population)
    strategic_floor = max(1, 10 * population)
    max_sell_hard = max(0, acres - hard_floor)
    max_sell_soft = max(0, acres - soft_floor)
    max_sell_strategic = max(0, acres - strategic_floor)
    if max_sell_hard > 0:
        emergency = store < full_feed
        cap_sell = max_sell_strategic
        if emergency:
            # If we are already under the 10 acres/person line, protect land first:
            # only sell enough for impeachment safety, not full feeding.
            required_sell = max(0, (full_feed - store + price - 1) // price)
            if acres <= strategic_floor:
                required_sell = max(0, (need - store + price - 1) // price)
            if required_sell > cap_sell:
                cap_sell = min(max_sell_hard, required_sell)
        if cap_sell > 0:
            sell_points = {
                min(max_sell_soft, max(0, (full_feed - store + price - 1) // price)),
                min(cap_sell, max(0, (full_feed + population - store + price - 1) // price)),
                min(cap_sell, max(0, (full_feed + reserve_target - store + price - 1) // price)),
                min(cap_sell, max(1, population // 8)),
                min(cap_sell, max(1, population // 4)),
                min(cap_sell, cap_sell // 2),
            }
            for sell in sorted(p for p in sell_points if p > 0):
                add_option(0, sell, "sell for food security")

    return options


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


def plan_year(acres: int, population: int, store: int, price: int, year: int) -> Decision:
    need = min_feed_avoid(population)
    full_feed = 20 * population
    reserve_target = max(220, 4 * population)
    exp_imm_mid = project_immigration(3, acres, store, population)
    next_pop_est = population + exp_imm_mid
    acres_target = 10 * next_pop_est
    if year >= 8:
        # Late game puts extra weight on not missing the final 10 acres/person threshold.
        acres_target = max(acres_target, 10 * population)
    trade_options = build_trade_options(
        acres,
        population,
        store,
        price,
        year,
        acres_target,
        need,
        full_feed,
        reserve_target,
    )

    best: Decision | None = None
    best_rank: tuple[int, int, int, int, int, int, int, int, int, int] | None = None

    for buy, sell, trade_note in trade_options:
        acres_after_trade = acres + buy - sell
        store_after_trade = store - buy * price + sell * price
        if acres_after_trade <= 0 or store_after_trade < 0:
            continue

        max_plant = min(acres_after_trade, 10 * population - 1, 2 * store_after_trade)
        for plant in sample_plant_levels(max_plant, store_after_trade, full_feed):
            seed = plant // 2
            if seed > store_after_trade:
                continue
            remaining_after_seed = store_after_trade - seed
            feed = min(full_feed, remaining_after_seed)
            if feed < need or would_impeach(population, feed):
                continue

            reserve = remaining_after_seed - feed
            deaths = starvation_deaths(population, feed)
            survivors = max(1, population - deaths)

            scenario_death_gaps: list[int] = []
            scenario_acres_deficits: list[int] = []
            scenario_store_deficits: list[int] = []
            scenario_acres_per_person_gaps: list[float] = []

            for harvest in range(1, 6):
                harvest_gain = plant * harvest
                for rat_divisor in (0, 1, 3, 5):
                    if rat_divisor == 0:
                        rats_eaten = 0
                    else:
                        rats_eaten = reserve // rat_divisor
                    store_next = max(0, reserve - rats_eaten + harvest_gain)

                    for imm_mult in range(1, 6):
                        immigration = project_immigration(imm_mult, acres_after_trade, store_next, survivors)
                        next_population_no_plague = survivors + immigration
                        for plague in (0, 1):
                            next_population = next_population_no_plague
                            if plague == 1:
                                next_population = max(1, next_population // 2)

                            feedable_next = store_next // 20
                            scenario_death_gaps.append(max(0, next_population - feedable_next))
                            scenario_store_deficits.append(max(0, 20 * next_population - store_next))
                            scenario_acres_deficits.append(max(0, 10 * next_population - acres_after_trade))
                            acres_per_person = acres_after_trade / max(1, next_population)
                            scenario_acres_per_person_gaps.append(max(0.0, 10.0 - acres_per_person))

            worst_death_gap = max(scenario_death_gaps) if scenario_death_gaps else 0
            avg_death_gap = math.ceil(statistics.mean(scenario_death_gaps)) if scenario_death_gaps else 0
            worst_store_deficit = max(scenario_store_deficits) if scenario_store_deficits else 0
            avg_store_deficit = math.ceil(statistics.mean(scenario_store_deficits)) if scenario_store_deficits else 0
            worst_acres_gap = max(scenario_acres_deficits) if scenario_acres_deficits else 0
            avg_acres_per_person_gap = (
                int(round(100 * statistics.mean(scenario_acres_per_person_gaps)))
                if scenario_acres_per_person_gaps
                else 0
            )

            short_full = max(0, full_feed - feed)
            reserve_gap = max(0, reserve_target - reserve)
            current_land_gap = max(0, 10 * population - acres_after_trade)
            projected_land_gap = max(0, acres_target - acres_after_trade)
            miss_land_goal = 1 if current_land_gap > 0 else 0
            severe_death = 1 if deaths > max(0, population // 20) else 0
            rank = (
                miss_land_goal,
                projected_land_gap,
                severe_death,
                deaths,
                current_land_gap,
                short_full,
                reserve_gap,
                avg_acres_per_person_gap,
                worst_acres_gap,
                avg_death_gap,
                worst_death_gap,
                avg_store_deficit,
                worst_store_deficit,
                -acres_after_trade,
                -plant,
            )
            if best_rank is None or rank < best_rank:
                best_rank = rank
                best = Decision(
                    buy=buy,
                    sell=sell,
                    feed=feed,
                    plant=plant,
                    reserve=reserve,
                    note=(
                        f"{trade_note}; year={year}; next_pop_est={next_pop_est}; "
                        f"acres_target={acres_target}; rank={rank}"
                    ),
                )

    if best is not None:
        return best

    # Final fallback: keep land floor strict; if already below target land base, only
    # sell enough for impeachment safety.
    land_floor = 10
    sell = 0
    if store < full_feed + reserve_target:
        max_sell_for_floor = max(0, acres - land_floor * population)
        sell_needed = math.ceil((full_feed + reserve_target - store) / price)
        if acres <= land_floor * population:
            sell_needed = math.ceil((need - store) / price) if store < need else 0
        sell = min(max_sell_for_floor, max(0, sell_needed))
    store_after_trade = store + sell * price
    feed = min(full_feed, store_after_trade)
    if feed < need and store < need:
        max_sell_for_floor = max(0, acres - land_floor * population)
        sell_needed = math.ceil((need - store) / price)
        sell = min(max_sell_for_floor, max(sell, max(0, sell_needed)))
        store_after_trade = store + sell * price
        feed = min(full_feed, store_after_trade)
    return Decision(0, sell, feed, 0, store_after_trade - feed, "fallback sell for full feed and reserve")


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
                return 1, GameTrace("fail", year, "national fink", snapshots, None)
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
                decision = plan_year(acres, population, store, price, year)

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
                    starved=starvation_deaths(population, decision.feed),
                    strategy_note=decision.note,
                )
            )

            if year == 10:
                idx = child.expect([r"So long for now", r"In your 10-year term", r"national fink"])
                if idx == 2:
                    return 1, GameTrace("fail", year, "national fink endgame", snapshots, None)
                child.expect(r"([0-9]*\.?[0-9]+) acres per person")
                final_acres_per_person = float(child.match.group(1))
                child.expect(pexpect.EOF)
                return 0, GameTrace("win", None, "", snapshots, final_acres_per_person)
    except pexpect.TIMEOUT:
        return 1, GameTrace("fail", current_year, "interaction timeout", snapshots, None)
    except pexpect.EOF:
        return 1, GameTrace("fail", current_year, "unexpected EOF", snapshots, None)

    return 1, GameTrace("fail", current_year, "loop ended unexpectedly", snapshots, None)


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
                f"plant={y.plant} reserve={y.reserve} starved={y.starved}"
            )
            print(f"    note: {y.strategy_note}")


def run_trials(trials: int, strategy: str, bad_case_limit: int) -> int:
    failures: list[GameTrace] = []
    fail_years: list[int] = []
    starvation_totals: list[int] = []
    final_acres_per_person_values: list[float] = []
    wins = 0
    for _ in range(trials):
        code, trace = run_one_game(strategy=strategy, verbose=False)
        starvation_totals.append(sum(y.starved for y in trace.years))
        if trace.final_acres_per_person is not None:
            final_acres_per_person_values.append(trace.final_acres_per_person)
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
    if starvation_totals:
        print(
            "starvation_stats: "
            f"avg={statistics.mean(starvation_totals):.2f} "
            f"median={statistics.median(starvation_totals):.2f} "
            f"max={max(starvation_totals)}"
        )
    if final_acres_per_person_values:
        print(
            "final_acres_per_person_stats: "
            f"avg={statistics.mean(final_acres_per_person_values):.2f} "
            f"median={statistics.median(final_acres_per_person_values):.2f} "
            f"min={min(final_acres_per_person_values):.2f}"
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
