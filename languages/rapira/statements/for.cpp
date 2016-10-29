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

#include "for.h"

/*** Constructor ***/
For::For()
{
	setFrom(0);
	setTo(0);
	setStep(0);
	setFor(0);
	setWhileCondition(0);
	setUntilCondition(0);
	setStatements(0);
}

/*** Set the from value ***/
void For::setFrom(Object* pFrom)
{
	from = pFrom;
}

/*** Set the to value ***/
void For::setTo(Object* pTo)
{
	to = pTo;
}

/*** Set the step value ***/
void For::setStep(Object* pStep)
{
	step = pStep;
}

/*** Set the for value ***/
void For::setFor(Object* pForValue)
{
	forValue = pForValue;
}

/*** Set the while condition ***/
void For::setWhileCondition(Object* pWhileCond)
{
	whileCond = pWhileCond;
}

/*** Set the until condition ***/
void For::setUntilCondition(Object* pUntilCond)
{
	untilCond = pUntilCond;
}

/*** Set the statements ***/
void For::setStatements(NodeList* pList)
{
	list = pList;
}

/*** Execute this node ***/
Outcome For::execute()
{
	// Check for the for value
	if(forValue == 0)
		throw Excep(getLineNumber(), getColumnNumber(), "No variable specified in for loop.");

	if(forValue->getType() != OP_VARIABLE)
		throw Excep(getLineNumber(), getColumnNumber(), "For loop must be given a valid variable.");

	// Store the for value variable
	Variable* forVar = static_cast<Variable*>(forValue);

	// If a from value was given, set the counter to this initial value
	if(from != 0)
	{
		// Evaluate the 
		std::auto_ptr<Object> fromEval(from->evaluate());
		if(fromEval->getType() != OBJ_INTEGER && fromEval->getType() != OBJ_REAL)
			throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, fromEval->getType());
		Assign(forVar->clone(), fromEval.release()).execute();
	}
	else
		Assign(forVar->clone(), new Integer(1)).execute();

	// Create an Outcome object to store the result
	Outcome result(S_SUCCESS);

	do
	{
		// Check the while condition, if present
		if(whileCond != 0)
		{
			std::auto_ptr<Object> whileCondEval(whileCond->evaluate());
			if(whileCondEval->getType() != OBJ_LOGICAL)
				throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_LOGICAL, whileCondEval->getType());
			bool whileCondResult = static_cast<Logical*>(whileCondEval.get())->getValue();

			if(!whileCondResult)
				break;
		}

		// Check the to value
		if(to != 0)
		{
			std::auto_ptr<Object> toEval(to->evaluate());

			// Store the step value evaluated
			std::auto_ptr<Object> stepEval;
			if(step == 0)
				stepEval.reset(new Integer(1));
			else
				stepEval.reset(step->evaluate());

			// Confirm that the step is an integer
			if(stepEval->getType() != OBJ_INTEGER && stepEval->getType() != OBJ_REAL)
				throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, stepEval->getType());

			std::auto_ptr<Real> castStep;
			// Cast the step value to a real
			if(stepEval->getType() == OBJ_INTEGER)
				castStep.reset(new Real(static_cast<Integer*>(stepEval.get())->getValue()));
			else
				castStep.reset(static_cast<Real*>(stepEval.release()));

			bool toCondition = false;

			if(castStep->getValue() > 0)
			{
				std::auto_ptr<Object> operation(Greater(forVar->clone(), toEval.release()).evaluate());
				toCondition = static_cast<Logical*>(operation.get())->getValue();
			}
			if(castStep->getValue() < 0)
			{
				std::auto_ptr<Object> operation(Less(forVar->clone(), toEval.release()).evaluate());
				toCondition = static_cast<Logical*>(operation.get())->getValue();
			}

			if(toCondition)
				break;
		}

		// Execute the NodeList of statements
		result = list->execute();

		// Modify forVar based on step
		if(step == 0)
		{
			std::auto_ptr<Object> addOperation(Add(forVar->clone(), new Integer(1)).evaluate());
			Assign(forVar->clone(), addOperation.release()).execute();
		}
		else
		{
			std::auto_ptr<Object> stepEval(step->evaluate());
			if(stepEval->getType() != OBJ_INTEGER && stepEval->getType() != OBJ_REAL)
				throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, stepEval->getType());
			std::auto_ptr<Object> addOperation(Add(forVar->clone(), stepEval.release()).evaluate());
			Assign(forVar->clone(), addOperation.release()).execute();
		}

		// Check the until condition, if present
		if(untilCond != 0)
		{
			std::auto_ptr<Object> untilEval(untilCond->evaluate());
			if(untilEval->getType() != OBJ_LOGICAL)
				throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_LOGICAL, untilEval->getType());
			bool untilResult = static_cast<Logical*>(untilEval.get())->getValue();

			if(untilResult)
				break;
		}
	}
	while(result.getStatus() == S_SUCCESS);

	return result;
}

/*** Destructor ***/
For::~For()
{
	delete list;
	delete forValue;
	delete whileCond;
	delete untilCond;
	delete from;
	delete to;
	delete step;
}
