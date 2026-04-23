# Hamurabi strategy guide

## Goals

Win all 10 years without triggering a national fink (mass starvation or running out of land), and keep the average starvation rate below 3% per year — what the game calls a "fantastic" performance.

## How the game works each year

After you decide how much land to buy or sell, how much grain to feed people, and how many acres to plant, the game resolves in this order:

1. **Harvest** — each planted acre produces 1–5 bushels at random.
2. **Rats** — 40% chance they eat a quarter to half of your stored grain.
3. **Immigration** — new people arrive based on how prosperous the city looks (more land and grain = more arrivals).
4. **Plague** — about 1 year in 7, half the population dies.
5. **Starvation** — anyone not fed with 20 bushels dies. If more than 45% die in one year, you are immediately thrown out of office.
6. **End of game** — if you averaged more than 33% starvation per year, or finish with fewer than 7 acres per person, you also lose.

## What the bot does each year

**Keep a grain reserve.** After feeding everyone, the bot always tries to hold back some grain as a cushion against bad harvests and rat attacks. Without a reserve, one unlucky year can wipe out the city.

**Plant as much as possible.** Each acre planted is an investment: even at the worst harvest (1 bushel per acre), more planted means more grain next year. The reserve is kept small enough to allow planting.

**Buy land only when flush.** The bot buys land only when it has a large grain surplus — enough to cover the purchase and still feed everyone through a bad year. Buying too eagerly leaves nothing to eat when the harvest fails.

**Sell land in emergencies, but carefully.** When grain runs short, the bot sells land to raise food. It first sells only the surplus above 8 acres per person (a comfortable safety margin). If that is not enough to prevent the 45% starvation threshold, it sells more — but stops as soon as impeachment is off the table.

**Stop selling once impeachment is safe.** This is the most important and counterintuitive rule. Selling even more land to save everyone in a bad year sounds compassionate, but it backfires: the extra survivors need to be fed in future years too, and if another bad harvest follows, there are now *more* mouths to feed with *less* land to farm. The net result is that starvation is shifted to a later year, not reduced. Stopping at the impeachment floor keeps the population at a level the land can support.

## Why 3% starvation is hard to guarantee

The game's luck is harsh. With harvests drawn at random from 1 to 5 bushels per acre, roughly 60% of 10-year games include two or more consecutive bad harvests. When that happens, the grain store empties faster than land sales can refill it, and some starvation is unavoidable no matter what strategy you use.

On seeds with zero or one bad-harvest year (about 40% of all games), the bot typically achieves 0% starvation or one mild event — well under the 3% target. On the rest, it keeps deaths just below the 45% impeachment line while preserving enough land to finish the game.

## What to expect

- **Win rate**: 100% — the city always survives all 10 years.
- **Starvation average**: about 4–5% across random seeds; roughly 35–40% of games come in under 3%.
- **Final land**: usually 10–12 acres per person, well above the 7 a/p national-fink floor.
