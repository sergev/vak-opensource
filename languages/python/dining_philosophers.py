#!/usr/bin/env python3
import asyncio
import random
from collections import deque
from tqdm import tqdm

class DiningPhilosophersSimulator:
    def __init__(self, max_time=20):
        self.current_time = 0
        self.max_time = max_time
        self.event_queue = deque()  # (time, philosopher_id, action, args)
        self.results = []  # Store completed actions
        self.forks = [True] * 5  # True if fork is available
        self.philosophers = [f"Philosopher {i}" for i in range(5)]  # Names for clarity

    async def schedule_event(self, time, philosopher_id, action, args=None):
        """Schedule an event for a philosopher (e.g., think, try_eat, eat, release_forks)."""
        #tqdm.write(f"Time {self.current_time}: Scheduling {action} for {self.philosophers[philosopher_id]} at time {time}")
        self.event_queue.append((time, philosopher_id, action, args))

    async def think(self, philosopher_id, duration):
        """Simulate philosopher thinking."""
        tqdm.write(f"Time {self.current_time}: {self.philosophers[philosopher_id]} is thinking for {duration} units")
        await asyncio.sleep(duration * 0.2)
        self.results.append((philosopher_id, "think", self.current_time))

    async def try_eat(self, philosopher_id):
        """Attempt to pick up both forks to eat."""
        left_fork = philosopher_id
        right_fork = (philosopher_id + 1) % 5
        # Pick up lower-numbered fork first to prevent deadlock
        first_fork, second_fork = min(left_fork, right_fork), max(left_fork, right_fork)

        if self.forks[first_fork] and self.forks[second_fork]:
            self.forks[first_fork] = False
            self.forks[second_fork] = False
            tqdm.write(f"Time {self.current_time}: {self.philosophers[philosopher_id]} picked up forks {first_fork} and {second_fork}")
            eat_duration = random.randint(1, 3)
            await self.schedule_event(self.current_time, philosopher_id, "eat", eat_duration)
        else:
            # If forks aren't available, retry after a short delay
            tqdm.write(f"Time {self.current_time}: {self.philosophers[philosopher_id]} cannot eat, retrying")
            await self.schedule_event(self.current_time + 1, philosopher_id, "try_eat")

    async def eat(self, philosopher_id, duration):
        """Simulate philosopher eating."""
        tqdm.write(f"Time {self.current_time}: {self.philosophers[philosopher_id]} is eating for {duration} units")
        await asyncio.sleep(duration * 0.2)
        self.results.append((philosopher_id, "eat", self.current_time))
        await self.schedule_event(self.current_time, philosopher_id, "release_forks")

    async def release_forks(self, philosopher_id):
        """Release both forks and schedule next think."""
        left_fork = philosopher_id
        right_fork = (philosopher_id + 1) % 5
        self.forks[left_fork] = True
        self.forks[right_fork] = True
        tqdm.write(f"Time {self.current_time}: {self.philosophers[philosopher_id]} released forks {left_fork} and {right_fork}")
        think_duration = random.randint(1, 3)
        await self.schedule_event(self.current_time, philosopher_id, "think", think_duration)

    async def run(self):
        """Run the simulation until max_time or no more events."""
        with tqdm(total=self.max_time, desc="Simulation Progress", unit="time") as pbar:
            while self.current_time < self.max_time and self.event_queue:
                # Get the next event
                next_event = self.event_queue[0]
                event_time, philosopher_id, action, args = next_event

                if event_time > self.current_time:
                    # Advance time to the next event
                    time_diff = event_time - self.current_time
                    tqdm.write(f"Time {self.current_time}: Advancing to time {event_time}")
                    await asyncio.sleep(time_diff * 0.2)
                    self.current_time = event_time
                    pbar.update(time_diff)

                # Process the event
                self.event_queue.popleft()
                if action == "think":
                    await self.think(philosopher_id, args)
                    await self.schedule_event(self.current_time, philosopher_id, "try_eat")
                elif action == "try_eat":
                    await self.try_eat(philosopher_id)
                elif action == "eat":
                    await self.eat(philosopher_id, args)
                elif action == "release_forks":
                    await self.release_forks(philosopher_id)
                self.current_time += 1
                pbar.update(1)

async def main():
    sim = DiningPhilosophersSimulator(max_time=30)

    # Schedule initial thinking for all philosophers
    for i in range(5):
        think_duration = random.randint(1, 3)
        await sim.schedule_event(0, i, "think", think_duration)

    # Sort event queue by time
    sim.event_queue = deque(sorted(sim.event_queue, key=lambda x: x[0]))

    # Run the simulation
    await sim.run()

    # Print results
    tqdm.write("\nSimulation Results:")
    for philosopher_id, action, time in sim.results:
        tqdm.write(f"Time {time}: {sim.philosophers[philosopher_id]} completed {action}")

if __name__ == "__main__":
    asyncio.run(main())
