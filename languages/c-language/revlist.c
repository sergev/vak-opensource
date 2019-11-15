//
// A typical task asked on interview:
// revert a single-linked list.
//
#include <stdio.h>

typedef struct list_t list_t;

struct list_t {
    list_t *next;
    char data;
};

//
// Revert list `p' and append `append' to it.
// Recursive algorithm.
//
list_t *revert_recursive(list_t *p, list_t *append)
{
    list_t *tail = p->next;

    p->next = append;
    if (!tail)
        return p;

    return revert_recursive(tail, p);
}

//
// Revert list `p'.
// Iterative algorithm.
//
list_t *revert(list_t *p)
{
    list_t *append = 0;

    for (;;) {
        list_t *tail = p->next;

        p->next = append;
        if (!tail)
            return p;

        append = p;
        p = tail;
    }
}

//
// Print list `p'.
//
void print_list(list_t *p)
{
    while (p) {
        printf("%c ", p->data);
        p = p->next;
    }
    printf("nul\n");
}

int main()
{
    // Build a list of three items.
    list_t c = { 0,  'C' };
    list_t b = { &c, 'B' };
    list_t a = { &b, 'A' };
    print_list(&a);

    // Revert with recursive algorithm.
    list_t *r = revert_recursive(&a, 0);
    print_list(r);

    // Revert back with iterative algorithm.
    r = revert(r);
    print_list(r);
    return 0;
}
