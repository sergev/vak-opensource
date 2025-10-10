module bulls_and_cows;
import std::io;
import std::os;
import std::math::random;
import libc;

fn uint get_digit(uint num, uint ps)
{
    while (ps-- > 0) {
        num /= 10;
    }
    return num % 10;
}

fn bool is_malformed(uint num)
{
    if (num > 9876u) {
        return true;
    }
    for (uint i = 0; i <= 2; i++) {
        for (uint j = i + 1; j <= 3; j++) {
            if (get_digit(num, j) == get_digit(num, i)) {
                return true;
            }
        }
    }
    return false;
}

fn uint make_number()
{
    uint num = 0;
    while (is_malformed(num)) {
        num = (uint) (random::rand(9877));
    }
    return num;
}

fn uint? input()
{
    String line = io::treadline()!;
    return (uint) libc::strtoul(line, null, 10);
}

fn void main()
{
    uint secret = make_number();
    uint count  = 0;
    uint guess  = 0;

    io::printn("Welcome to bulls and cows!\n"
           "\n"
           "I choose a number made of 4 digits (from 0 to 9) without repetitions.\n"
           "You enter a number of 4 digits, and I say you how many of them\n"
           "are in my secret number but in wrong position (cows),\n"
           "and how many are in the right position (bulls).\n");
    while (guess != secret) {
        count++;
        do {
            io::printn("Guess a number.");
            if (try reply = input()) {
                guess = reply;
            } else {
                return;
            }
        } while (is_malformed(guess));

        uint cows = 0;
        uint bulls = 0;
        for (uint i = 0; i <= 3; i++) {
            for (uint j = 0; j <= 3; j++) {
                if (get_digit(secret, i) == get_digit(guess, j)) {
                    if (i == j) {
                        bulls++;
                    } else {
                        cows++;
                    }
                }
            }
        }
        io::printfn("You scored %d bulls and %d cows.\n", bulls, cows);
    }
    io::printfn("Correct. That took you %d guesses.", count);
}
