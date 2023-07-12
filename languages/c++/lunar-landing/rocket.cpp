// Translation of
// <http://www.cs.brandeis.edu/~storer/LunarLander/LunarLander/LunarLanderListing.jpg>
// by Jim Storer from FOCAL to C.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

class Lunar_Module {
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
    void play();

    // Getters.
    double get_seconds() const { return L; }
    double get_velocity() const { return V; }
    double get_fuel() const { return M - N; }
    bool is_landed() const { return on_the_moon; }
};

static int echo_input = 0;

// Input routines (substitutes for FOCAL ACCEPT command)
static int accept_double(double *value);
static int accept_yes_or_no();
static void accept_line(char **buffer, size_t *buffer_length);

int main(int argc, const char **argv)
{
    if (argc > 1)
    {
        // If --echo is present, then write all input back to standard output.
        // (This is useful for testing with files as redirected input.)
        if (strcmp(argv[1], "--echo") == 0)
            echo_input = 1;
    }

    puts("CONTROL CALLING LUNAR MODULE. MANUAL CONTROL IS NECESSARY");
    puts("YOU MAY RESET FUEL RATE K EACH 10 SECS TO 0 OR ANY VALUE");
    puts("BETWEEN 8 & 200 LBS/SEC. YOU'VE 16000 LBS FUEL. ESTIMATED");
    puts("FREE FALL IMPACT TIME-120 SECS. CAPSULE WEIGHT-32500 LBS\n\n");

    do // 01.20 in original FOCAL code
    {
        puts("FIRST RADAR CHECK COMING UP\n\n");
        puts("COMMENCE LANDING PROCEDURE");
        puts("TIME,SECS   ALTITUDE,MILES+FEET   VELOCITY,MPH   FUEL,LBS   FUEL RATE");

        Lunar_Module game;
        game.play();

        // On the Moon: 05.10 in original FOCAL code
        printf("ON THE MOON AT %8.2f SECS\n", game.get_seconds());
        double impact_velocity = 3600 * game.get_velocity();
        printf("IMPACT VELOCITY OF %8.2f M.P.H.\n", impact_velocity);
        printf("FUEL LEFT: %8.2f LBS\n", game.get_fuel());
        if (impact_velocity <= 1) {
            puts("PERFECT LANDING !-(LUCKY)");
        } else if (impact_velocity <= 10) {
            puts("GOOD LANDING-(COULD BE BETTER)");
        } else if (impact_velocity <= 22) {
            puts("CONGRATULATIONS ON A POOR LANDING");
        } else if (impact_velocity <= 40) {
            puts("CRAFT DAMAGE. GOOD LUCK");
        } else if (impact_velocity <= 60) {
            puts("CRASH LANDING-YOU'VE 5 HRS OXYGEN");
        } else {
            puts("SORRY,BUT THERE WERE NO SURVIVORS-YOU BLEW IT!");
            printf("IN FACT YOU BLASTED A NEW LUNAR CRATER %8.2f FT. DEEP\n", impact_velocity * .277777);
        }

        puts("\n\n\nTRY AGAIN?");
    } while (accept_yes_or_no());

    puts("CONTROL OUT\n\n");
    return 0;
}

double Lunar_Module::ask_fuel_rate() const
{
    printf("%7.0f%16.0f%7.0f%15.2f%12.1f      ",
           L,
           trunc(A),
           5280 * (A - trunc(A)),
           3600 * V,
           M - N);

    for (;;) {
        // Prompt for K.
        fputs("K=:", stdout);
        double fuel_rate = 0;
        int is_valid_input = accept_double(&fuel_rate);
        if (is_valid_input && (fuel_rate == 0 || fuel_rate >= 8) && fuel_rate <= 200) {
            return fuel_rate;
        }

        fputs("NOT POSSIBLE", stdout);
        for (int x = 1; x <= 51; ++x)
            putchar('.');
    }
}

void Lunar_Module::play()
{
    while (!is_landed()) { // 02.10 in original FOCAL code
        K = ask_fuel_rate();
        move();
    }
}

void Lunar_Module::move()
{
    T = 10;
    for (;;) { // 03.10 in original FOCAL code

        if (M - N < .001) { // 04.10 in original FOCAL code
            printf("FUEL OUT AT %8.2f SECS\n", L);
            S = (sqrt(V * V + 2 * A * G) - V) / G;
            V += G * S;
            L += S;
            on_the_moon = true;
            return;
        }

        if (T < .001) {
            // This time interval is finished.
            return;
        }
        S = T;

        if (N + S * K - M > 0) {
            S = (M - N) / K;
        }
        apply_thrust();

        if (I <= 0) { // 07.10 in original FOCAL code
            while (S >= .005) {
                S = 2 * A / (V + sqrt(V * V + 2 * A * (G - Z * K / M)));
                apply_thrust();
                update_lander_state();
            }
            on_the_moon = true;
            return;
        }

        if ((V > 0) && (J < 0)) {
            for (;;) { // 08.10 in original FOCAL code
                // FOCAL-to-C gotcha: In FOCAL, multiplication has a higher
                // precedence than division.  In C, they have the same
                // precedence and are evaluated left-to-right.  So the
                // original FOCAL subexpression `M * G / Z * K` can't be
                // copied as-is into C: `Z * K` has to be parenthesized to
                // get the same result.

                double w = (1 - M * G / (Z * K)) / 2;
                S = M * V / (Z * K * (w + sqrt(w * w + V / Z))) + 0.5;
                apply_thrust();
                if (I <= 0) {
                    break;
                }
                update_lander_state();
                if (-J < 0) {
                    continue;
                }
                if (V <= 0) {
                    continue;
                }
            }
        }

        update_lander_state();
    }
}

// Subroutine at line 06.10 in original FOCAL code
void Lunar_Module::update_lander_state()
{
    L += S;
    T -= S;
    M -= S * K;
    A = I;
    V = J;
}

// Subroutine at line 09.10 in original FOCAL code
void Lunar_Module::apply_thrust()
{
    double Q = S * K / M;
    double Q_2 = pow(Q, 2);
    double Q_3 = pow(Q, 3);
    double Q_4 = pow(Q, 4);
    double Q_5 = pow(Q, 5);

    J = V + G * S + Z * (-Q - Q_2 / 2 - Q_3 / 3 - Q_4 / 4 - Q_5 / 5);
    I = A - G * S * S / 2 - V * S + Z * S * (Q / 2 + Q_2 / 6 + Q_3 / 12 + Q_4 / 20 + Q_5 / 30);
}

// Read a floating-point value from stdin.
//
// Returns 1 on success, or 0 if input did not contain a number.
//
// Calls exit(-1) on EOF or other failure to read input.
int accept_double(double *value)
{
    char *buffer = NULL;
    size_t buffer_length = 0;
    accept_line(&buffer, &buffer_length);
    int is_valid_input = sscanf(buffer, "%lf", value);
    free(buffer);
    if (is_valid_input != 1)
        is_valid_input = 0;
    return is_valid_input;
}

// Reads input and returns 1 if it starts with 'Y' or 'y', or returns 0 if it
// starts with 'N' or 'n'.
//
// If input starts with none of those characters, prompt again.
//
// If unable to read input, calls exit(-1);
int accept_yes_or_no()
{
    int result = -1;
    do {
        fputs("(ANS. YES OR NO):", stdout);
        char *buffer = NULL;
        size_t buffer_length = 0;
        accept_line(&buffer, &buffer_length);

        if (buffer_length > 0) {
            switch (buffer[0]) {
            case 'y':
            case 'Y':
                result = 1;
                break;
            case 'n':
            case 'N':
                result = 0;
                break;
            default:
                break;
            }
        }

        free(buffer);
    } while (result < 0);

    return result;
}

// Reads a line of input.  Caller is responsible for calling free() on the
// returned buffer.
//
// If unable to read input, calls exit(-1).
void accept_line(char **buffer, size_t *buffer_length)
{
    if (getline(buffer, buffer_length, stdin) == -1) {
        fputs("\nEND OF INPUT\n", stderr);
        exit(-1);
    }

    if (echo_input) {
        fputs(*buffer, stdout);
    }
}
