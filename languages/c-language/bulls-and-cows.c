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

int main()
{
    srandom(time(NULL));

    unsigned count = 0;
    unsigned num   = make_number();
    unsigned guess = 0;

    while (guess != num) {
        count++;
        do {
            printf("Guess a number.\n");

            static char *line = NULL;
            size_t linecap = 0;
            if (getline(&line, &linecap, stdin) <= 0) {
                exit(0);
            }
            guess = strtoul(line, NULL, 10);
        } while (is_malformed(guess));

        unsigned cows = 0;
        unsigned bulls = 0;

        for (unsigned i = 0; i <= 3; i++) {
            for (unsigned j = 0; j <= 3; j++) {
                if (get_digit(num, i) == get_digit(guess, j)) {
                    if (i == j) {
                        bulls++;
                    } else {
                        cows++;
                    }
                }
            }
        }
        printf("You scored %u bulls and %u cows.\n", bulls, cows);
    }
    printf("Correct. That took you %u guesses.\n", count);
}
