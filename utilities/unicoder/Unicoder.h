/*
 * Copyright (C) 2007 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is part of Unicoder utility, which is distributed
 * under the terms of the GNU General Public License (GPL).
 * See the accompanying file "COPYING" for more details.
 */
#include <stdio.h>

class Unicode_engine {
public:
        virtual ~Unicode_engine() {}
	virtual int get_char (FILE *fd) = 0;
	virtual void put_char (unsigned short c, FILE *fd) = 0;
	virtual const char *name (void) = 0;

	friend class Unicoder;
};

class Unicoder {
public:
	Unicoder (char *fmt = 0)
		{ engine = 0; set_format (fmt); }

	int set_format (char *fmt);

	const char *format (void)
		{ return engine->name(); }

	int get_char (FILE *fd)
		{ return engine->get_char (fd); }

	void put_char (unsigned short c, FILE *fd)
		{ engine->put_char (c, fd); }

private:
	Unicode_engine *engine;
};
