//
// A simple Prolog interpreter written in C++,
// including an example test run as main().
//
// Copyright (c) Alan Mycroft, University of Cambridge, 2000.
//
// Source: https://www.cl.cam.ac.uk/~am21/research/funnel/prolog.c
//
#include <iostream>
#include <string.h>

class Atom {
    std::string atomname;

public:
    Atom(std::string s) : atomname(s) {}

    void print() { std::cout << atomname; }
    bool eqatom(Atom *t) { return (atomname == t->atomname); }
};

class TermCons;

class Term {
public:
    virtual void print() = 0;
    virtual bool unify(Term *) = 0;
    virtual bool unify2(TermCons *) = 0;
    virtual Term *copy() = 0;
};

class TermCons : public Term {
private:
    Atom *fsym;
    int arity;
    Term **args;

public:
    TermCons(Atom *f) : fsym(f), arity(0), args(NULL) {}
    TermCons(Atom *f, Term *a1) : fsym(f), arity(1), args(new Term *[1]) { args[0] = a1; };

    TermCons(Atom *f, Term *a1, Term *a2) : fsym(f), arity(2), args(new Term *[2])
    {
        args[0] = a1, args[1] = a2;
    }

    TermCons(Atom *f, Term *a1, Term *a2, Term *a3) : fsym(f), arity(3), args(new Term *[3])
    {
        args[0] = a1, args[1] = a2, args[2] = a3;
    }

    void print()
    {
        fsym->print();
        if (arity > 0) {
            std::cout << "(";
            for (int i = 0; i < arity;) {
                args[i]->print();
                if (++i < arity)
                    std::cout << ",";
            }
            std::cout << ")";
        }
    }

    bool unify(Term *t) { return t->unify2(this); }
    Term *copy() { return copy2(); }
    TermCons *copy2() { return new TermCons(this); }

private:
    TermCons(TermCons *p)
        : fsym(p->fsym), arity(p->arity), args(p->arity == 0 ? NULL : new Term *[p->arity])
    {
        for (int i = 0; i < arity; i++)
            args[i] = p->args[i]->copy();
    }

    bool unify2(TermCons *t)
    {
        if (!(fsym->eqatom(t->fsym) && arity == t->arity))
            return false;
        for (int i = 0; i < arity; i++)
            if (!args[i]->unify(t->args[i]))
                return false;
        return true;
    }
};

class TermVar : public Term {
private:
    Term *instance;
    int varno;
    static int timestamp;

public:
    TermVar() : instance(this), varno(++timestamp) {}

    bool unify(Term *t);
    Term *copy();
    void reset() { instance = this; }

    void print()
    {
        if (instance != this)
            instance->print();
        else
            std::cout << "_" << varno;
    };

private:
    bool unify2(TermCons *t) { return this->unify(t); }
};

int TermVar::timestamp = 0;

class Program;
class TermVarMapping;

class Goal {
private:
    TermCons *car;
    Goal *cdr;

public:
    Goal(TermCons *h, Goal *t) : car(h), cdr(t) {}
    Goal *copy() { return new Goal(car->copy2(), cdr == NULL ? NULL : cdr->copy()); }
    Goal *append(Goal *l) { return new Goal(car, cdr == NULL ? NULL : cdr->append(l)); }
    void solve(Program *p, int level, TermVarMapping *map);

    void print()
    {
        car->print();
        if (cdr != NULL) {
            std::cout << "; ", cdr->print();
        }
    }

    void indent(int n)
    {
        for (int i = 0; i < n; i++)
            std::cout << "    ";
    }

};

class Clause {
public:
    TermCons *head;
    Goal *body;
    Clause(TermCons *h, Goal *t) : head(h), body(t) {}
    Clause *copy() { return new Clause(head->copy2(), body == NULL ? NULL : body->copy()); }

    void print()
    {
        head->print();
        std::cout << " :- ";
        if (body == NULL)
            std::cout << "true";
        else
            body->print();
    }
};

class Program {
public:
    Clause *pcar;
    Program *pcdr;
    Program(Clause *h, Program *t) : pcar(h), pcdr(t) {}
};

class Trail {
private:
    TermVar *tcar;
    Trail *tcdr;
    static Trail *sofar;
    Trail(TermVar *h, Trail *t) : tcar(h), tcdr(t) {}

public:
    static Trail *Note() { return sofar; }
    static void Push(TermVar *x) { sofar = new Trail(x, sofar); }

    static void Undo(Trail *whereto)
    {
        for (; sofar != whereto; sofar = sofar->tcdr)
            sofar->tcar->reset();
    }
};

Trail *Trail::sofar = NULL;

bool TermVar::unify(Term *t)
{
    if (instance != this)
        return instance->unify(t);

    Trail::Push(this);
    instance = t;
    return true;
}

Term *TermVar::copy()
{
    if (instance == this) {
        Trail::Push(this);
        instance = new TermVar();
    }
    return instance;
}

class TermVarMapping {
private:
    TermVar **varvar;
    std::string *vartext;
    int size;

public:
    TermVarMapping(TermVar *vv[], std::string vt[], int vs) : varvar(vv), vartext(vt), size(vs) {}

    void showanswer()
    {
        if (size == 0)
            std::cout << "yes\n";
        else {
            for (int i = 0; i < size; i++) {
                std::cout << vartext[i] << " = ";
                varvar[i]->print();
                std::cout << "\n";
            }
        }
    }
};

void Goal::solve(Program *p, int level, TermVarMapping *map)
{
    indent(level);
    std::cout << "solve@" << level << ": ";
    this->print();
    std::cout << "\n";
    for (Program *q = p; q != NULL; q = q->pcdr) {
        Trail *t = Trail::Note();
        Clause *c = q->pcar->copy();
        Trail::Undo(t);
        indent(level);
        std::cout << "  try:";
        c->print();
        std::cout << "\n";
        if (car->unify(c->head)) {
            Goal *gdash = c->body == NULL ? cdr : c->body->append(cdr);
            if (gdash == NULL)
                map->showanswer();
            else
                gdash->solve(p, level + 1, map);
        } else {
            indent(level);
            std::cout << "  nomatch.\n";
        }
        Trail::Undo(t);
    }
}

//
// A sample test program: append
//
int main()
{
    Atom *at_app = new Atom("app");
    Atom *at_cons = new Atom("cons");
    TermCons *f_nil = new TermCons(new Atom("nil"));
    TermCons *f_1 = new TermCons(new Atom("1"));
    TermCons *f_2 = new TermCons(new Atom("2"));
    TermCons *f_3 = new TermCons(new Atom("3"));

    Term *v_x = new TermVar();
    TermCons *lhs1 = new TermCons(at_app, f_nil, v_x, v_x);
    Clause *c1 = new Clause(lhs1, NULL);

    Term *v_l = new TermVar();
    Term *v_m = new TermVar();
    Term *v_n = new TermVar();
    TermCons *rhs2 = new TermCons(at_app, v_l, v_m, v_n);
    TermCons *lhs2 = new TermCons(at_app, new TermCons(at_cons, v_x, v_l),
                                  v_m, new TermCons(at_cons, v_x, v_n));
    Clause *c2 = new Clause(lhs2, new Goal(rhs2, NULL));

    TermVar *v_i = new TermVar();
    TermVar *v_j = new TermVar();
    TermCons *rhs3 = new TermCons(at_app, v_i, v_j,
                                  new TermCons(at_cons, f_1,
                                               new TermCons(at_cons, f_2,
                                                            new TermCons(at_cons, f_3,
                                                                         f_nil))));
    Goal *g1 = new Goal(rhs3, NULL);

    Program *test_p = new Program(c1, new Program(c2, NULL));
    Program *test_p2 = new Program(c2, new Program(c1, NULL));

    TermVar *varvar[] = { v_i, v_j };
    std::string varname[] = { "I", "J" };
    TermVarMapping *var_name_map = new TermVarMapping(varvar, varname, 2);


    std::cout << "=======Append with normal clause order:\n";
    g1->solve(test_p, 0, var_name_map);

    std::cout << "\n=======Append with reversed normal clause order:\n";
    g1->solve(test_p2, 0, var_name_map);
    return 0;
}
