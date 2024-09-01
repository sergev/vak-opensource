#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

unsigned get_digit(unsigned num, unsigned ps)
{
    while (ps-- > 0) {
        num /= 10;
    }
    return num % 10;
}

bool is_malformed(unsigned num)
{
    if (num > 9876) {
        return true;
    }
    for (unsigned i = 0; i <= 2; i++) {
        for (unsigned j = i+1; j <= 3; j++) {
            if (get_digit(num, j) == get_digit(num, i)) {
                return true;
            }
        }
    }
    return false;
}

unsigned make_number()
{
    unsigned num = 0;
    while (is_malformed(num)) {
        num = random() % 9877ul;
    }
    return num;
}

unsigned input()
{
    static char *line = NULL;
    size_t linecap = 0;
    if (getline(&line, &linecap, stdin) <= 0) {
        exit(0);
    }
    return strtoul(line, NULL, 10);
}

int main()
{
    srandom(time(NULL));

    unsigned secret = make_number();
    unsigned count  = 0;
    unsigned guess  = 0;

    printf("Welcome to bulls and cows!\n"
           "\n"
           "I choose a number made of 4 digits (from 0 to 9) without repetitions.\n"
           "You enter a number of 4 digits, and I say you how many of them\n"
           "are in my secret number but in wrong position (cows),\n"
           "and how many are in the right position (bulls).\n"
           "\n");
    while (guess != secret) {
        count++;
        do {
            printf("Guess a number.\n");
            guess = input();
        } while (is_malformed(guess));

        unsigned cows = 0;
        unsigned bulls = 0;
        for (unsigned i = 0; i <= 3; i++) {
            for (unsigned j = 0; j <= 3; j++) {
                if (get_digit(secret, i) == get_digit(guess, j)) {
                    if (i == j) {
                        bulls++;
                    } else {
                        cows++;
                    }
                }
            }
        }
        printf("You scored %u bulls and %u cows.\n\n", bulls, cows);
    }
    printf("Correct. That took you %u guesses.\n", count);
}
