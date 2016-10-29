/*d* === probability = n% ===        07.01.85   version    2 */

pct(n) {
    unsigned rand();
    register r;

    r = (rand()>>2) & 037777;
    return( (r%100) < n );
}
