#ifndef OUTPUT_H
#define OUTPUT_H

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

#include <iostream>
#include <vector>

#include "../primitives/integer.h"
#include "../primitives/real.h"
#include "../primitives/text.h"
#include "../primitives/logical.h"
#include "../primitives/sequence.h"
#include "../exceptions/invalidtype.h"

class Output : public Node
{

	public:

		/*** Constructor ***/
		Output();

		/*** Constructor ***/
		Output(Object* pExpr, bool pNewline);

		/*** Constructor ***/
		Output(bool pNewline);

		/*** Add an expression ***/
		void addExpression(Object* pExpr);

		/*** Execute this node ***/
		Outcome execute();

		/*** Clone this object ***/
		Output* clone() const
		{
			Output* nout = new Output(newline);
			nout->setLineNumber(getLineNumber());
			nout->setColumnNumber(getColumnNumber());
			for(unsigned int i = 0; i < exprs.size(); i++)
					nout->addExpression(exprs.at(i)->clone());
			return nout;
		}

		/*** Destructor ***/
		~Output();

	private:

		/*** The list of expressions ***/
		std::vector<Object*> exprs;

		/*** If there should be a newline after output ***/
		bool newline;

};

#endif
