#ifndef RETURN_H
#define RETURN_H

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

#include "../object.h"

class Return : public Node
{

	public:

		/*** Constructor ***/
		Return();

		/*** Constructor ***/
		Return(Object* pExpr);

		/*** Set the expression ***/
		void setExpression(Object* pExpr);

		/*** Get the expression ***/
		Object* getExpression();

		/*** Execute this node ***/
		Outcome execute();

		/*** Clone this node ***/
		Return* clone() const
		{
			Return* nrn = new Return();
			nrn->setLineNumber(getLineNumber());
			nrn->setColumnNumber(getColumnNumber());
			if(expr != 0)
				nrn->setExpression(expr->clone());
			return nrn;
		}

		/*** Destructor ***/
		~Return();

	private:

		/*** The return expression ***/
		Object* expr;

};

#endif
