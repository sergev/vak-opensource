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
// Revert list `p' and append `next' to it.
//
list_t *revert(list_t *p, list_t *next)
{
#if 0
    // Recursive
    list_t *tail = p->next;

    p->next = next;
    if (!tail)
        return p;
    return revert(tail, p);
#else
    // Converted to a loop
    for (;;) {
        list_t *tail = p->next;

        p->next = next;
        if (!tail)
            return p;
        next = p;
        p = tail;
    }
#endif
}

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

    // Revert the list.
    list_t *r = revert(&a, 0);
    print_list(r);
    return 0;
}
