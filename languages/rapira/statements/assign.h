#ifndef ASSIGN_H
#define ASSIGN_H

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

#include "../primitives/variable.h"
#include "../varmanager.h"

class Assign : public Node
{

	public:

		/*** Constructor ***/
		Assign();

		/*** Constructor ***/
		Assign(Variable* pTarget, Object* pExpr);

		/*** Set the target ***/
		void setTarget(Variable* pTarget);

		/*** Set the expression ***/
		void setExpression(Object* pExpr);

		/*** Execute this node ***/
		Outcome execute();

		/*** Clone this object ***/
		Assign* clone() const
		{
			Assign* nan = new Assign();
			nan->setLineNumber(getLineNumber());
			nan->setColumnNumber(getColumnNumber());
			if(target != 0)
				nan->setTarget(target->clone());
			if(expr != 0)
				nan->setExpression(expr->clone());
			return nan;
		}

		/*** Destructor ***/
		~Assign();

	private:

		/*** The first argument ***/
		Variable* target;

		/*** The second argument ***/
		Object* expr;

};

#endif
