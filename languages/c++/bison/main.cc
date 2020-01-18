#include <iostream>
#include "parser.hh"

int main()
{
    Demo::Parser parser;

    parser.parse();
    parser.print();
    return 0;
}
