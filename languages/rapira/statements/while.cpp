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

#include "while.h"

/*** Constructor ***/
While::While()
{
	setCondition(0);
	setUntilCondition(0);
	setStatements(0);
}

/*** Constructor ***/
While::While(Object* pCondition, NodeList* pList)
{
	setCondition(pCondition);
	setStatements(pList);
	setUntilCondition(0);
}

/*** Constructor ***/
While::While(Object* pCondition, NodeList* pList, Object* pUntil)
{
	setCondition(pCondition);
	setStatements(pList);
	setUntilCondition(pUntil);
}

/*** Set the condition ***/
void While::setCondition(Object* pCond)
{
	cond = pCond;
}

/*** Set the until condition ***/
void While::setUntilCondition(Object* pUntil)
{
	until = pUntil;
}

/*** Set the statements ***/
void While::setStatements(NodeList* pList)
{
	list = pList;
}

/*** Execute this node ***/
Outcome While::execute()
{
	Outcome result(S_SUCCESS);

	do
	{
		// Check the condition
		std::auto_ptr<Object> condEval(cond->evaluate());
		if(condEval->getType() != OBJ_LOGICAL)
			throw InvalidTypeException(cond->getLineNumber(), cond->getColumnNumber(), OBJ_LOGICAL, condEval->getType());
		bool condResult = dynamic_cast<Logical*>(condEval.get())->getValue();

		if(!condResult)
			break;

		result = list->execute();

		// Evaluate the until condition, if present
		if(until != 0)
		{
			std::auto_ptr<Object> untilEval(until->evaluate());

			if(untilEval->getType() != OBJ_LOGICAL)
				throw InvalidTypeException(until->getLineNumber(), until->getColumnNumber(), OBJ_LOGICAL, untilEval->getType());
			bool untilResult = static_cast<Logical*>(untilEval.get())->getValue();

			if(untilResult)
				break;
		}
	}
	while(result.getStatus() == S_SUCCESS);

	if(result.getStatus() == S_EXIT)
		result = Outcome(S_SUCCESS);

	return result;
}

/*** Destructor ***/
While::~While()
{
	delete list;
	delete cond;
	delete until;
}
