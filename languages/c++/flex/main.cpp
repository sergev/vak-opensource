#include "scanner.h"

int main(int argc, char *argv[])
{
    Scanner scan;

    for (;;) {
        switch (scan.get_next_token()) {
        case Scanner::Token::END:
            break;
        case Scanner::Token::NUMBER:
            std::cout << scan.lineno() << ": number " << scan.YYText() << " (" << scan.YYLeng() << " bytes)\n";
            continue;
        case Scanner::Token::NAME:
            std::cout << scan.lineno() << ": name " << scan.YYText() << " (" << scan.YYLeng() << " bytes)\n";
            continue;
        case Scanner::Token::STRING:
            std::cout << scan.lineno() << ": string " << scan.YYText() << " (" << scan.YYLeng() << " bytes)\n";
            continue;
        }
        break;
    }
    return 0;
}
