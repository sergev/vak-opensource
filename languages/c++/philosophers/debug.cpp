void Philosopher::display(uint64_t current_time) const
{
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
}
