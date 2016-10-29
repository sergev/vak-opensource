#ifndef REPEAT_H
#define REPEAT_H

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

#include "../nodelist.h"
#include "../primitives/logical.h"
#include "../primitives/integer.h"
#include "../exceptions/invalidtype.h"
#include "../exceptions/negativevalue.h"

class Repeat : public Node
{

	public:

		/*** Constructor ***/
		Repeat();

		/*** Set the counter ***/
		void setCounter(Object* pCounter);

		/*** Set the while condition ***/
		void setWhileCondition(Object* pWhileCond);

		/*** Set the until condition ***/
		void setUntilCondition(Object* pUntilCond);

		/*** Set the statements ***/
		void setStatements(NodeList* pList);

		/*** Execute this node ***/
		Outcome execute();

		/*** Clone this object ***/
		Repeat* clone() const
		{
			Repeat* nrn = new Repeat();
			nrn->setLineNumber(getLineNumber());
			nrn->setColumnNumber(getColumnNumber());
			if(counter != 0)
				nrn->setCounter(counter->clone());
			if(whileCond != 0)
				nrn->setWhileCondition(whileCond->clone());
			if(untilCond != 0)
				nrn->setUntilCondition(untilCond->clone());
			if(list != 0)
				nrn->setStatements(list->clone());
			return nrn;
		}

		/*** Destructor ***/
		~Repeat();

	private:

		/*** The counter ***/
		Object* counter;

		/*** This loop's list of nodes ***/
		NodeList* list;

		/*** The optional while condition ***/
		Object* whileCond;

		/*** The optional until condition ***/
		Object* untilCond;

};

#endif
