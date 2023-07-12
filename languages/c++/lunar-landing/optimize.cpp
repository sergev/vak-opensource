#include <iostream>
#include "rocket.h"

//
// Print vector.
//
std::ostream &operator<<(std::ostream &out, const std::vector<unsigned> &vec)
{
    out << '{';
    for (const auto &item : vec) {
        out << ' ' << item;
    }
    out << " }";
    return out;
}

//
// Print array.
//
std::ostream &operator<<(std::ostream &out, const std::array<unsigned, 8> &arr)
{
    out << '{';
    for (const auto &item : arr) {
        out << ' ' << item;
    }
    out << " }";
    return out;
}

//
// Play game with given control.
// Return the resulting impact velocity (in mph).
//
double play_game(const std::vector<unsigned> &control)
{
    Rocket game;
    double score = game.play_vector(control);

    //std::cout << "Impact velocity " << score << " mph, control " << control << std::endl;
    return score;
}

//
// Given a control and it's score, vary it's item with given index
// to find a better score. Return the new control and score
// (or old values, in case no enhancement has been made).
//
std::vector<unsigned> optimize_control(std::vector<unsigned> control,
                                       unsigned index, double &score)
{
    double initial_score = score;

    // Increase control at given index.
    while (control[index] < 200) {
        auto new_control = control; // make a copy
        new_control[index] += 1;
        auto new_score = play_game(new_control);
        if (new_score > score) {
            // It got worse.
            break;
        }
        control = new_control;
        score = new_score;
    }

    if (score < initial_score) {
        // Found better control.
        return control;
    }

    // Decrease control at given index.
    while (control[index] > 8) {
        auto new_control = control; // make a copy
        new_control[index] -= 1;
        auto new_score = play_game(new_control);
        if (new_score > score) {
            // It got worse.
            break;
        }
        control = new_control;
        score = new_score;
    }
    return control;
}

//
// Play a series of games starting with initial control,
// and optimize it according to the given variant.
// Print the best score (minimal) and appropriate control.
//
void optimize_variant(const std::array<unsigned, 8> &variant,
                      const std::vector<unsigned> &initial_control,
                      const double initial_score,
                      std::vector<unsigned> &best_control,
                      double &best_score)
{
    std::vector<unsigned> control = initial_control;
    double score = initial_score;
    double last_score;

    do {
        last_score = score;

        for (auto const index : variant) {
            control = optimize_control(control, 7 + index, score);
        }
    } while (score < last_score); // Stop when no enhancement anymore

    // Print the score.
    if (score < initial_score) {
        std::cout << "Score " << score
                  << ", control " << control
                  << ", variant " << variant << std::endl;
    }

    // Update the best score
    if (score < best_score) {
        best_score = score;
        best_control = control;
    }
}

//
// Try all variants one by one.
//
int main()
{
    std::array<unsigned, 8> variant = { 0, 1, 2, 3, 4, 5, 6, 7 };

    const std::vector<unsigned> initial_control = {
        0, 0, 0, 0, 0, 0, 0,
        180, 180, 180, 180, 180, 180, 180, 180,
    };
    const double initial_score = play_game(initial_control);

    std::vector<unsigned> best_control = initial_control;
    double best_score = initial_score;

    do {
        //std::cout << variant[0] << ' ' << variant[1] << ' ' << variant[2] << ' ' << variant[3] << ' '
        //          << variant[4] << ' ' << variant[5] << ' ' << variant[6] << ' ' << variant[7] << '\n';
        optimize_variant(variant, initial_control, initial_score,
                         best_control, best_score);

    } while (std::next_permutation(variant.begin(), variant.end()));

    // Print the best score and control.
    std::cout << "The best score " << best_score
              << ", control " << best_control << std::endl;
}
