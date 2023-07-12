#include <vector>

class Rocket {
private:
    // Variables
    double A{ 120 };            // Altitude (miles)
    double I{};                 // Intermediate altitude (miles)
    double J{};                 // Intermediate velocity (miles/sec)
    double K{};                 // Fuel rate (lbs/sec)
    double L{ 0 };              // Elapsed time (sec)
    double M{ 32500 };          // Total weight (lbs)
    double S{};                 // Time elapsed in current 10-second turn (sec)
    double T{};                 // Time remaining in current 10-second turn (sec)
    double V{ 1 };              // Downward speed (miles/sec)

    // Constants
    const double G{ 0.001 };    // G - Gravity
    const double N{ 16500 };    // N - Empty weight (lbs, Note: M - N is remaining fuel weight)
    const double Z{ 1.8 };      // Z - Thrust per pound of fuel burned

    // Status
    bool on_the_moon{};

    // Methods.
    double ask_fuel_rate() const;
    void move();
    void update_lander_state();
    void apply_thrust();

public:
    // Methods.
    void play_interactive();
    double play_vector(const std::vector<unsigned> &control);

    // Getters.
    double get_seconds() const { return L; }
    double get_velocity() const { return V; }
    double get_fuel() const { return M - N; }
    bool is_landed() const { return on_the_moon; }

    static int accept_yes_or_no();
};
