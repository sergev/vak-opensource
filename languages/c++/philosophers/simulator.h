#include <iostream>
#include <set>
#include <unistd.h>

class Simulator;

class Event {
public:
    virtual void behave(Simulator &sim) = 0;

private:
    uint64_t start_time{};

    // Compare two events by start time and index.
    struct Cmp {
        bool operator()(const Event *a, const Event *b) const
        {
            if (a->start_time < b->start_time)
                return true;
            if (a->start_time > b->start_time)
                return false;
            return (intptr_t)a < (intptr_t)b;
        }
    };

    friend class Simulator;
};

class Simulator {
public:
    bool is_active() const { return !queue.empty(); }

    void wait(Event &event, unsigned num_cycles)
    {
        // Insert event into queue.
        event.start_time = master_clock + num_cycles;
        queue.insert(&event);
    }

    void run()
    {
        // Get next event from queue.
        Event *event = *queue.begin();
        queue.erase(event);

        // Update time.
        if (event->start_time > master_clock) {
            unsigned msec = event->start_time - master_clock;
            usleep(msec * 1000);
        }
        master_clock = event->start_time;

        // Run event.
        event->behave(*this);
    }

    uint64_t get_clock() const { return master_clock; }

private:
    uint64_t master_clock{};

    // Event queue, ordered by start time.
    std::set<Event *, Event::Cmp> queue;
};
