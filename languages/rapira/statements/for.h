#ifndef FOR_H
#define FOR_H

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
#include "assign.h"
#include "../operations/greater.h"
#include "../operations/less.h"
#include "../operations/add.h"
#include "../exceptions/parsersyntax.h"
#include "../exceptions/invalidtype.h"

class For : public Node
{

	public:

		/*** Constructor ***/
		For();

		/*** Set the from value ***/
		void setFrom(Object* pFrom);

		/*** Set the to value ***/
		void setTo(Object* pTo);

		/*** Set the step value ***/
		void setStep(Object* pStep);

		/*** Set the for value ***/
		void setFor(Object* pForValue);

		/*** Set the while condition ***/
		void setWhileCondition(Object* pWhileCond);

		/*** Set the until condition ***/
		void setUntilCondition(Object* pUntilCond);

		/*** Set the statements ***/
		void setStatements(NodeList* pList);

		/*** Execute this node ***/
		Outcome execute();

		/*** Clone this object ***/
		For* clone() const
		{
			For* nfn = new For();
			nfn->setLineNumber(getLineNumber());
			nfn->setColumnNumber(getColumnNumber());
			if(from != 0)
				nfn->setFrom(from->clone());
			if(forValue != 0)
				nfn->setFor(forValue->clone());
			if(to != 0)
				nfn->setTo(to->clone());
			if(step != 0)
				nfn->setStep(step->clone());
			if(whileCond != 0)
				nfn->setWhileCondition(whileCond->clone());
			if(untilCond != 0)
				nfn->setUntilCondition(untilCond->clone());
			if(list != 0)
				nfn->setStatements(list->clone());
			return nfn;
		}

		/*** Destructor ***/
		~For();

	private:

		/*** This loop's list of nodes ***/
		NodeList* list;

		/*** The for value ***/
		Object* forValue;

		/*** The optional while condition ***/
		Object* whileCond;

		/*** The optional until condition ***/
		Object* untilCond;

		/*** The from value ***/
		Object* from;

		/*** The to value ***/
		Object* to;

		/*** The step value ***/
		Object* step;

};

#endif
