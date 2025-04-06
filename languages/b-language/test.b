/* Global vector declaration */
v[2] 1, 2;
foo(x) {
    auto y 5;   /* Local scalar */
    auto w[2];  /* Local vector */
    w[0] = y;   /* Vector assignment */
    x += 1;     /* Compound assignment */
    y = x > 0 ? x : -x; /* Ternary */
    return v[1] & y; /* Bitwise AND */
}
