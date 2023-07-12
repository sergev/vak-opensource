// Translation of
// <http://www.cs.brandeis.edu/~storer/LunarLander/LunarLander/LunarLanderListing.jpg>
// by Jim Storer from FOCAL to C.

#include "rocket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

static int echo_input = 0;

// Input routines (substitutes for FOCAL ACCEPT command)
static int accept_double(double *value);
static void accept_line(char **buffer, size_t *buffer_length);

double Rocket::ask_fuel_rate() const
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

void Rocket::play_interactive()
{
    // When not on a terminal, then write all input back to standard output.
    // (This is useful for testing with files as redirected input.)
    echo_input = !isatty(0);

    while (!is_landed()) { // 02.10 in original FOCAL code
        K = ask_fuel_rate();
        move();
    }
}

//
// Play game with given control.
// Return the resulting impact velocity (in mph).
//
double Rocket::play_vector(const std::vector<unsigned> &control)
{
    unsigned index = 0;

    while (!is_landed()) {

        if (index < control.size()) {
            K = control[index++];
        } else {
            K = 0;
        }
#if 0
        printf("%7.0f%16.0f%7.0f%15.2f%12.1f      K=:%.0f\n",
               L,
               trunc(A),
               5280 * (A - trunc(A)),
               3600 * V,
               M - N,
               K);
#endif
        move();
    }

    return 3600 * V;
}

void Rocket::move()
{
    T = 10;

turn_loop:
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

        if (I <= 0) {
loop_until_on_the_moon: // 07.10 in original FOCAL code
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
                    goto loop_until_on_the_moon;
                }

                update_lander_state();
                if (-J < 0) {
                    goto turn_loop;
                }
                if (V <= 0) {
                    goto turn_loop;
                }
            }
        }

        update_lander_state();
    }
}

// Subroutine at line 06.10 in original FOCAL code
void Rocket::update_lander_state()
{
    L += S;
    T -= S;
    M -= S * K;
    A = I;
    V = J;
}

// Subroutine at line 09.10 in original FOCAL code
void Rocket::apply_thrust()
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
int Rocket::accept_yes_or_no()
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
