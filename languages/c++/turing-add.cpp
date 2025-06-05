#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

// Types for Turing machine
enum class Symbol { One, Blank };
enum class Direction { Left, Right };
using State           = std::string;
using TransitionKey   = std::tuple<State, Symbol>;
using TransitionValue = std::tuple<State, Symbol, Direction>;
using Transitions     = std::map<TransitionKey, TransitionValue>;

struct Tape {
    std::vector<Symbol> left;
    Symbol head;
    std::vector<Symbol> right;
};

// Convert symbol to string for printing
std::string symbol_to_string(Symbol sym)
{
    return sym == Symbol::One ? "1" : "_";
}

// Print the current tape and head position
void print_tape(const Tape &tape, const State &state)
{
    std::string left_str;
    for (const auto &sym : tape.left) {
        left_str += symbol_to_string(sym);
    }
    std::string right_str;
    for (const auto &sym : tape.right) {
        right_str += symbol_to_string(sym);
    }
    std::cout << "[" << left_str << "] " << symbol_to_string(tape.head) << " (" << state << ") "
              << right_str << "\n";
}

// Move the tape head
Tape move_head(Tape tape, Direction dir)
{
    if (dir == Direction::Right) {
        std::vector<Symbol> new_right;
        Symbol new_head = Symbol::Blank;
        if (!tape.right.empty()) {
            new_head  = tape.right.front();
            new_right = std::vector<Symbol>(tape.right.begin() + 1, tape.right.end());
        }
        tape.left.push_back(tape.head);
        return { std::move(tape.left), new_head, std::move(new_right) };
    } else { // Left
        std::vector<Symbol> new_left;
        Symbol new_head = Symbol::Blank;
        if (!tape.left.empty()) {
            new_head = tape.left.back();
            new_left = std::vector<Symbol>(tape.left.begin(), tape.left.end() - 1);
        }
        tape.right.insert(tape.right.begin(), tape.head);
        return { std::move(new_left), new_head, std::move(tape.right) };
    }
}

// Step the Turing machine
std::optional<std::pair<Tape, State>> step(const Tape &tape, const State &state,
                                           const Transitions &transitions)
{
    TransitionKey current = { state, tape.head };
    auto it               = transitions.find(current);
    if (it == transitions.end()) {
        return std::nullopt; // No transition: halt
    }
    const auto &[new_state, new_symbol, dir] = it->second;
    Tape new_tape                            = tape;
    new_tape.head                            = new_symbol;
    return std::make_pair(move_head(new_tape, dir), new_state);
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
        auto next = step(tape, current_state, transitions);
        if (!next) {
            std::cout << "Halted (no transition)\n";
            break;
        }
        std::tie(tape, current_state) = *next;
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
    Tape tape = { {},
                  Symbol::One,
                  { Symbol::One, Symbol::Blank, Symbol::One, Symbol::One, Symbol::One } };
    run_turing_machine(tape, "q0", transitions, "qaccept", "qreject");
}

int main()
{
    example_add();
    return 0;
}