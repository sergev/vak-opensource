#ifndef SLICEASSIGN_H
#define SLICEASSIGN_H

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

#include "../operations/slice.h"
#include "assign.h"

class SliceAssign : public Node
{

	public:

		/*** Constructor ***/
		SliceAssign();

		/*** Constructor ***/
		SliceAssign(Variable* pTarget, Object* pIndex1, Object* pIndex2, Object* pExpr);

		/*** Set the target ***/
		void setTarget(Variable* pTarget);

		/*** Set the index ***/
		void setIndex1(Object* pIndex);

		/*** Set the index ***/
		void setIndex2(Object* pIndex);

		/*** Set the expression ***/
		void setExpression(Object* pExpr);

		/*** Execute this node ***/
		Outcome execute();

		/*** Clone this object ***/
		SliceAssign* clone() const
		{
			SliceAssign* nsn = new SliceAssign();
			nsn->setLineNumber(getLineNumber());
			nsn->setColumnNumber(getColumnNumber());
			if(target != 0)
				nsn->setTarget(target->clone());
			if(index1 != 0)
				nsn->setIndex1(index1->clone());
			if(index2 != 0)
				nsn->setIndex2(index2->clone());
			if(expr != 0)
				nsn->setExpression(expr->clone());
			return nsn;
		}

		/*** Destructor ***/
		~SliceAssign();

	private:

		/*** The target ***/
		Variable* target;

		/*** The index ***/
		Object* index1;

		/*** The index ***/
		Object* index2;

		/*** The expression ***/
		Object* expr;

};

#endif
