#!/usr/bin/env perl

$gates = 64;
$steps = 2000;
$loops = 30000;
#$steps = 2;
#$loops = 1;
srand (123);

print qq[#include <stdio.h>
#include "rtlsim.h"
];

for ($i = 0; $i < $gates; ++$i) {
    print qq[signal_t a$i = signal_init (\"a$i\", ~0);
signal_t b$i = signal_init (\"b$i\", ~0);
signal_t c$i = signal_init (\"c$i\", ~0);
];
}

@binop = ('%s & %s', '!(%s & %s)', '(%s | %s)', '!(%s | %s)', '%s ^ %s', '!(%s ^ %s)');

for ($i = 0; $i < $gates; ++$i) {
    $x = $i + 1 + int(rand() * ($gates - 1));
    if ($x >= $gates) {
        $x = $x - $gates;
    }

    $y = $i + 1 + int(rand() * ($gates - 1));
    if ($y >= $gates) {
        $y = $y - $gates;
    }

    $op = $binop[int(rand() * 6)];
    $op = sprintf "$op", "a$x.value", "a$y.value";

    print qq[void do_b$i () {
    process_sensitive (&a$x, 0);
    process_sensitive (&a$y, 0);
    for (;;) {
        process_wait();
        signal_set (&b$i, $op);
    }
}
];
}

for ($i = 0; $i < $gates; ++$i) {
    $x = $i + 1 + int(rand() * ($gates - 1));
    if ($x >= $gates) {
        $x = $x - $gates;
    }

    $y = $i + 1 + int(rand() * ($gates - 1));
    if ($y >= $gates) {
        $y = $y - $gates;
    }

    $op = $binop[int(rand() * 6)];
    $op = sprintf "$op", "b$x.value", "b$y.value";

    print qq[void do_c$i () {
    process_sensitive (&b$x, 0);
    process_sensitive (&b$y, 0);
    for (;;) {
        process_wait();
        signal_set (&c$i, $op);
    }
}
];
}

print qq[void print_a_b_c() {
    value_t a = 0, b = 0, c = 0;
];
for ($i = 0; $i < $gates; ++$i) {
    print qq[    a |= a$i.value << $i;
    b |= b$i.value << $i;
    c |= c$i.value << $i;
];
}
print qq[    printf ("%016llx %016llx %016llx\\n", a, b, c);
}

int main() {
    int i;
];

for ($i = 0; $i < $gates; ++$i) {
    print qq[    process_init ("b$i", do_b$i, 4096);
    process_init ("c$i", do_c$i, 4096);
];
}

for ($i = 0; $i < $gates; ++$i) {
    printf "    signal_set (&a$i, 0);\n";
}

print qq[    process_delay (1);
    for (i=0; i<$loops; i++) {
];

for ($i = 0; $i < $steps; ++$i) {
    if (rand() < 0.2) {
        printf "        process_delay (1);\n";
    }
    $x = int(rand() * $gates);
    printf "        signal_set (&a$x, ! c$x.value);\n";
}

print qq[
        process_delay (1);
        /*print_a_b_c();*/
    }
    print_a_b_c();
    return 0;
}
];
