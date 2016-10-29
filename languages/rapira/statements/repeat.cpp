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

#include "repeat.h"

/*** Constructor ***/
Repeat::Repeat()
{
	setCounter(0);
	setWhileCondition(0);
	setUntilCondition(0);
	setStatements(0);
}

/*** Set the counter ***/
void Repeat::setCounter(Object* pCounter)
{
	counter = pCounter;
}

/*** Set the while condition ***/
void Repeat::setWhileCondition(Object* pWhileCond)
{
	whileCond = pWhileCond;
}

/*** Set the until condition ***/
void Repeat::setUntilCondition(Object* pUntilCond)
{
	untilCond = pUntilCond;
}

/*** Set the statements ***/
void Repeat::setStatements(NodeList* pList)
{
	list = pList;
}

/*** Execute this node ***/
Outcome Repeat::execute()
{
	// Make sure that the counter evaluates to a number
	std::auto_ptr<Object> countEval(counter->evaluate());
	if(countEval->getType() != OBJ_INTEGER)
		throw InvalidTypeException(counter->getLineNumber(), counter->getColumnNumber(), OBJ_INTEGER, countEval->getType());
	long loopNum = static_cast<Integer*>(countEval.get())->getValue();

	// Make sure loopNum is not less than zero
	if(loopNum < 0)
		throw NegativeValueException(getLineNumber(), getColumnNumber(), loopNum);

	// Create an Outcome object to store the result
	Outcome result(S_SUCCESS);

	while(loopNum > 0 && result.getStatus() == S_SUCCESS)
	{
		// Check the while condition, if present
		if(whileCond != 0)
		{
			std::auto_ptr<Object> whileCondEval(whileCond->evaluate());
			if(whileCondEval->getType() != OBJ_LOGICAL)
				throw InvalidTypeException(whileCond->getLineNumber(), whileCond->getColumnNumber(), OBJ_LOGICAL, whileCondEval->getType());
			bool whileCondResult = static_cast<Logical*>(whileCondEval.get())->getValue();

			if(!whileCondResult)
				break;
		}

		// Execute the statements
		result = list->execute();

		// Evaluate the until condition, if present
		if(untilCond != 0)
		{
			std::auto_ptr<Object> untilEval(untilCond->evaluate());

			if(untilEval->getType() != OBJ_LOGICAL)
				throw InvalidTypeException(untilCond->getLineNumber(), untilCond->getColumnNumber(), OBJ_LOGICAL, untilEval->getType());

			bool untilResult = static_cast<Logical*>(untilEval.get())->getValue();

			if(untilResult)
				break;
		}

		loopNum--;
	}

	if(result.getStatus() == S_EXIT)
		result = Outcome(S_SUCCESS);

	return result;
}

/*** Destructor ***/
Repeat::~Repeat()
{
	delete list;
	delete counter;
	delete whileCond;
	delete untilCond;
}
