#!/usr/bin/env python3
"""Coordinated feed/plant strategy for one game (impeachment-aware)."""

from __future__ import annotations

import os
import re
import sys

import pexpect

REPO_ROOT = os.path.dirname(os.path.abspath(__file__))


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


def plan_year(acres: int, population: int, store: int) -> tuple[int, int]:
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
            return feed, desired_plant
    return min(store, 20 * population), 0


def run_one_game() -> int:
    child = pexpect.spawn("./hamurabi", cwd=REPO_ROOT, encoding="utf-8", timeout=None)
    child.logfile = sys.stdout

    for year in range(1, 11):
        idx = child.expect([r"national fink", r"Population is now (\d+)"])
        if idx == 0:
            return 1
        population = int(child.match.group(1))

        child.expect(r"The city now owns (\d+) acres")
        acres = int(child.match.group(1))

        child.expect(r"You now have (\d+) bushels in store")
        store = int(child.match.group(1))

        child.expect(r"Land is trading at (\d+) bushels per acre")
        child.expect("buy")
        child.sendline("0")

        child.expect("sell")
        child.sendline("0")

        child.expect("feed")
        feed, max_plant = plan_year(acres, population, store)
        child.sendline(str(feed))

        child.expect("plant")
        child.sendline(str(max_plant))

        if year == 10:
            idx = child.expect([r"So long for now", r"In your 10-year term", r"national fink"])
            if idx == 2:
                return 1
            child.expect(pexpect.EOF)
            return 0

    return 1


if __name__ == "__main__":
    raise SystemExit(run_one_game())
