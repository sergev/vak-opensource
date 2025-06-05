#include <deque>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>

// Types for Turing machine
enum class Symbol { One, Blank };
enum class Direction { Left, Right };
using State           = std::string;
using TransitionKey   = std::tuple<State, Symbol>;
using TransitionValue = std::tuple<State, Symbol, Direction>;
using Transitions     = std::unordered_map<TransitionKey, TransitionValue, std::hash<TransitionKey>>;

struct Tape {
    std::deque<Symbol> data;
    size_t head_pos;
};

// Hash function for TransitionKey
namespace std {
template <>
struct hash<TransitionKey> {
    size_t operator()(const TransitionKey &key) const
    {
        auto [state, sym] = key;
        return hash<string>()(state) ^ (hash<size_t>()(static_cast<size_t>(sym)) << 1);
    }
};
} // namespace std

// Convert symbol to string for printing
std::string symbol_to_string(Symbol sym)
{
    return sym == Symbol::One ? "1" : "_";
}

// Print the current tape and head position
void print_tape(const Tape &tape, const State &state)
{
    std::string left_str, right_str;
    size_t i = 0;
    for (const auto &sym : tape.data) {
        if (i < tape.head_pos) {
            left_str += symbol_to_string(sym);
        } else if (i > tape.head_pos) {
            right_str += symbol_to_string(sym);
        }
        ++i;
    }
    std::cout << "[" << left_str << "] " << symbol_to_string(tape.data[tape.head_pos]) << " ("
              << state << ") " << right_str << "\n";
}

// Move the tape head
void move_head(Tape &tape, Direction dir)
{
    if (dir == Direction::Right) {
        if (tape.head_pos + 1 >= tape.data.size()) {
            tape.data.push_back(Symbol::Blank);
        }
        ++tape.head_pos;
    } else { // Left
        if (tape.head_pos == 0) {
            tape.data.push_front(Symbol::Blank);
        } else {
            --tape.head_pos;
        }
    }
}

// Step the Turing machine
std::optional<State> step(Tape &tape, const State &state, const Transitions &transitions)
{
    TransitionKey current = { state, tape.data[tape.head_pos] };
    auto it               = transitions.find(current);
    if (it == transitions.end()) {
        return std::nullopt; // No transition: halt
    }
    const auto &[new_state, new_symbol, dir] = it->second;
    tape.data[tape.head_pos]                 = new_symbol;
    move_head(tape, dir);
    return new_state;
}

// Run the Turing machine
void run_turing_machine(Tape tape, const State &start_state, const Transitions &transitions,
                        const State &accept_state, const State &reject_state)
{
    State current_state = start_state;
    while (true) {
        print_tape(tape, current_state);
        if (current_state == accept_state) {
            std::cout << "Accepted\n";
            break;
        }
        if (current_state == reject_state) {
            std::cout << "Rejected\n";
            break;
        }
        auto next_state = step(tape, current_state, transitions);
        if (!next_state) {
            std::cout << "Halted (no transition)\n";
            break;
        }
        current_state = *next_state;
    }
}

// Turing machine to add two unary numbers
void example_add()
{
    Transitions transitions = {
        // q0: Move right to find blank
        { { "q0", Symbol::One }, { "q0", Symbol::One, Direction::Right } },
        { { "q0", Symbol::Blank }, { "q1", Symbol::One, Direction::Left } },
        // q1: Move left to start
        { { "q1", Symbol::One }, { "q1", Symbol::One, Direction::Left } },
        { { "q1", Symbol::Blank }, { "qaccept", Symbol::Blank, Direction::Right } }
    };
    // Tape represents 2 + 3: 11_111
    Tape tape = {
        { Symbol::One, Symbol::One, Symbol::Blank, Symbol::One, Symbol::One, Symbol::One }, 0
    };
    run_turing_machine(tape, "q0", transitions, "qaccept", "qreject");
}

int main()
{
    example_add();
    return 0;
}
