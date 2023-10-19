#include "simulator.h"

enum class Fork {
    FREE,
    TAKEN,
};

class Philosopher : public Event {
private:
    const unsigned index;
    Fork &left_fork;
    Fork &right_fork;

    enum class State {
        THINKING,
        EATING,
        WAITING,
    };
    State state{};

    unsigned random(unsigned lo, unsigned hi)
    {
        return lo + arc4random_uniform(hi - lo + 1);
    }

public:
    Philosopher(unsigned i, Fork &l, Fork &r)
        : index(i), left_fork(l), right_fork(r) {}

    void behave(Simulator &sim) override
    {
        if (state != State::EATING) {
            // Try to start eating.
            if (left_fork == Fork::FREE && right_fork == Fork::FREE) {
                left_fork  = Fork::TAKEN;
                right_fork = Fork::TAKEN;
                state      = State::EATING;
            } else {
                // Waiting for forks.
                state = State::WAITING;
            }
        } else {
            // Stop eating.
            left_fork  = Fork::FREE;
            right_fork = Fork::FREE;
            state      = State::THINKING;
        }
        sim.wait(*this, random(10, 2000));
        display(sim.get_clock());
    }

    void display(uint64_t current_time) const
    {
#if 1
        static const unsigned xpos[] = { 35, 54, 47, 23, 16 };
        static const unsigned ypos[] = { 3, 10, 20, 20, 10 };
        static bool initialized{};

        if (!initialized) {
            std::cout << "\033[2J"; // Clear screen.
            initialized = true;
        }
        std::cout << "\033[" << ypos[index] << ';' << xpos[index] << 'H';
        switch (state) {
        case State::THINKING:
            std::cout << "\033[1;34m" << (index + 1) << ": thinking"; // Light blue
            break;
        case State::EATING:
            std::cout << "\033[1;32m" << (index + 1) << ": eating  "; // Light green
            break;
        case State::WAITING:
            std::cout << "\033[1;31m" << (index + 1) << ": waiting "; // Light red
            break;
        }
        std::cout << std::flush;
#else
        std::cout << '(' << current_time << ") " << "abcde"[index] << ": ";
        switch (state) {
        case State::THINKING:
            std::cout << "thinking";
            break;
        case State::EATING:
            std::cout << "eating";
            break;
        case State::WAITING:
            std::cout << "waiting";
            break;
        }
        std::cout << " until " << get_time() << std::endl;
#endif
    }
};

int main(int argc, char **argv)
{
    Fork forks[5]{};
    Philosopher a(0, forks[0], forks[1]);
    Philosopher b(1, forks[1], forks[2]);
    Philosopher c(2, forks[2], forks[3]);
    Philosopher d(3, forks[3], forks[4]);
    Philosopher e(4, forks[4], forks[0]);
    Simulator sim;

    a.behave(sim);
    b.behave(sim);
    c.behave(sim);
    d.behave(sim);
    e.behave(sim);

    while (sim.is_active()) {
        sim.run();
    }
}
