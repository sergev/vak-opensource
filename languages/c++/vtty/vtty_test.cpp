#include "vtty.h"

int main()
{
    Vtty cons("console");
    Vtty net("network", 32323);

    Vtty::start();
    for (;;) {
        if (cons.is_char_avail()) {
            auto ch = cons.get_char();
            net.put_char(ch);
            continue;
        }

        if (net.is_char_avail()) {
            auto ch = net.get_char();
            cons.put_char(ch);
            continue;
        }

        fd_set bitmask;
        Vtty::wait(&bitmask);
    }

    return 0;
}
