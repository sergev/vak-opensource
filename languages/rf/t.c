struct _symbol {
	struct _chain *top, *tail;
};
struct _chain {
	struct _chain *next;
	char type;
	union {
		char ch;
		int num;
		struct _chain *rb;
		struct _chain (*fun) ();
		struct _symbol *val;
	} v;
};
extern struct _chain *_rfalloc ();

static struct _chain *entry (_a)
struct _chain *_a;
{
	struct _chain *_b, *_c;
	_c = _a;
	if (!_c) goto _0;
	if (_c->type != 2) goto _0;
	if (_c->v.ch != 42) goto _0;
	_c = _c->next;
	if (_c) goto _0;
	while (_a) { _a->type = 1; _a = _a->next; }
	_b = _c = 0;
	return (_b);
_0:
	_c = _a;
	if (!_c) goto _1;
	if (_c->type != 2) goto _1;
	if (_c->v.ch != 97) goto _1;
	_c = _c->next;
	if (!_c) goto _1;
	if (_c->type != 2) goto _1;
	if (_c->v.ch != 98) goto _1;
	_c = _c->next;
	if (!_c) goto _1;
	if (_c->type != 2) goto _1;
	if (_c->v.ch != 99) goto _1;
	_c = _c->next;
	if (_c) goto _1;
	while (_a) { _a->type = 1; _a = _a->next; }
	_b = _c = _rfalloc ();
	_c->type = 2;
	_c->v.ch = 113;
	_c->next = _rfalloc ();
	_c = _c->next;
	_c->type = 2;
	_c->v.ch = 119;
	_c->next = _rfalloc ();
	_c = _c->next;
	_c->type = 2;
	_c->v.ch = 101;
	_c->next = 0;
	return (_b);
_1:
	_rfexit ("entry");
}
/*
 *	entry	local	fuction
 */
