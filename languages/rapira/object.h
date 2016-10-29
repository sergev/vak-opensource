#ifndef OBJECT_H
#define OBJECT_H

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

#include <memory>
#include "node.h"

/*** Rapira primitive types ***/
#define OBJ_EMPTY			0
#define OBJ_INTEGER			1
#define OBJ_REAL			2
#define OBJ_TEXT			4
#define OBJ_SEQUENCE		8
#define OBJ_LOGICAL			16
#define OBJ_PROCEDURE		32
#define OBJ_FUNCTION		64
#define OBJ_SPFUNCTION		128

/*** Rapira operations ***/
#define OP_VARIABLE			108
#define OP_ADD				109
#define OP_SUBTRACT			110
#define OP_MULTIPLY			111
#define OP_DIVIDE			112
#define OP_NEGATE			113
#define OP_INTDIVIDE		114
#define OP_REMAINDER		115
#define OP_EXPONENT			116
#define OP_EQUAL			117
#define OP_UNEQUAL			118
#define OP_GREATER			119
#define OP_LESS				120
#define OP_GREATEQ			121
#define OP_LESSEQ			122
#define OP_NOT				123
#define OP_AND				124
#define OP_OR				125
#define OP_SYMBOL			126
#define OP_SLICE			127
#define OP_LENGTH			150
#define OP_CALL				129
#define OP_IN				130
#define OP_INOUT			131
#define OP_PARAMETER		132

class Object : public Node
{

	public:

		/*** Constructor ***/
		Object();

		/*** Execute this node ***/
		Outcome execute();

		/*** Get this object's type ***/
		virtual unsigned char getType()
		{
			return OBJ_EMPTY;
		}

		/*** Evaluate this object ***/
		virtual Object* evaluate()
		{
			return clone();
		}

		/*** Clone this object ***/
		Object* clone() const { return new Object(*this); }

		/*** Destructor ***/
		virtual ~Object()
		{
		}

};

#endif
