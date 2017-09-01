#   This is a gratuitously parallel implementation of the sieve of
    Eratosthenes after a program contributed by Lawrence
    D’Oliveiro; the number of primes it can output is limited only
    by the number of threads a68g can create.
#

MODE SIEVER = STRUCT (SEMA full, empty, REF INT n);

PROC make siever = SIEVER: (LEVEL 0, LEVEL 1, NEW INT);

PROC start siever = (SIEVER s, INT n) VOID:
    (n MOD 2 /= 0 | DOWN empty OF s; n OF s := n; UP full OF s);

PROC next unmarked = (SIEVER s) INT:
    (DOWN full OF s; INT n = n OF s; UP empty OF s; n);

PROC sieve = (SIEVER current) VOID:
    (INT n = next unmarked (current);

    print ((whole (n, -int width), new line));
    SIEVER new = make siever;
    PAR (
        DO IF INT i = next unmarked (current);
            i MOD n /= 0
            THEN start siever (new, i)
            FI
        OD,
        sieve (new)
    )
);

SIEVER first = make siever;
INT n := 1;

PAR (DO n PLUSAB 1; start siever (first, n) OD, sieve (first))
