#include <iostream>
#include "rocket.h"

std::ostream &operator<<(std::ostream &out, const std::vector<unsigned> &vec)
{
    out << '{';
    for (const auto &item : vec) {
        out << ' ' << item;
    }
    out << " }";
    return out;
}

int main()
{
    std::vector<unsigned> control = {
        0, 0, 0, 0, 0, 0, 0,
        200, 200, 200, 200, 200, 200, 200, 199,
    };

    Rocket game;
    double score = game.play_vector(control);

    std::cout << "Impact velocity " << score << " mph, control " << control << std::endl;
    return 0;
}
