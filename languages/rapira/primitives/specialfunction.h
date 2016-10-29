#ifndef SPECIALFUNCTION_H
#define SPECIALFUNCTION_H

// ReRap Version 0.9
// Copyright 2011 Matthew Mikolay.
//
// This file is part of ReRap.
//
// ReRap is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ReRap is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ReRap.  If not, see <http://www.gnu.org/licenses/>.

#include <vector>
#include <cmath>
#include <errno.h>
#include <stdlib.h>
#include "../object.h"
#include "variable.h"
#include "sequence.h"
#include "../operations/equal.h"
#include "../exceptions/parsersyntax.h"
#include "../exceptions/invalidtype.h"
#include "../exceptions/negativevalue.h"
#include "../primitives/integer.h"
#include "../primitives/real.h"
#include "../primitives/logical.h"

#define SPF_ABS		0
#define SPF_SIGN	1
#define SPF_SQRT	2
#define SPF_ENTIER	3
#define SPF_ROUND	4
#define SPF_RAND	5
#define SPF_INTRAND	6
#define SPF_INDEX	7
#define SPF_ISEMPTY	8
#define SPF_ISLOG	9
#define SPF_ISINT	10
#define SPF_ISREAL	11
#define SPF_ISTEXT	12
#define SPF_ISSEQ	13
#define SPF_ISPROC	14
#define SPF_ISFUN	15
#define SPF_SIN		16
#define SPF_COS		17
#define SPF_TG		18
#define SPF_ARCSIN	19
#define SPF_ARCTG	20
#define SPF_EXP		21
#define SPF_LN		22
#define SPF_LG		23

class SpecialFunction : public Object
{

	public:

		/*** Constructor ***/
		SpecialFunction(unsigned char pId);

		/*** Get this operation's type ***/
		unsigned char getType();

		/*** Add an argument ***/
		void addArgument(Object *pArg);

		/*** Evaluate this object ***/
		Object* evaluate();

		/*** Clone this object ***/
		SpecialFunction* clone() const
		{
			return new SpecialFunction(id);
		}

		/*** Destructor ***/
		~SpecialFunction();

	private:

		/*** The ID number of this special function ***/
		unsigned char id;

		/*** The arguments to this special function ***/
		std::vector<Object*> args;
};

#endif
