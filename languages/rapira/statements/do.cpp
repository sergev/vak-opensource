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

#include "do.h"

/*** Constructor ***/
Do::Do()
{
	setStatements(0);
	setUntilCondition(0);
}

/*** Constructor ***/
Do::Do(NodeList* pList)
{
	setStatements(pList);
	setUntilCondition(0);
}

/*** Constructor ***/
Do::Do(NodeList* pList, Object* pUntil)
{
	setStatements(pList);
	setUntilCondition(pUntil);
}

/*** Set the statements ***/
void Do::setStatements(NodeList* pList)
{
	list = pList;
}

/*** Get the statements ***/
NodeList* Do::getStatements()
{
	return list;
}

/*** Set the until condition ***/
void Do::setUntilCondition(Object* pUntil)
{
	until = pUntil;
}

/*** Execute this node ***/
Outcome Do::execute()
{
	Outcome result(S_SUCCESS);

	do
	{
		result = list->execute();

		// Check if an until condition needs to be checked
		if(until != 0)
		{
			std::auto_ptr<Object> untilEval(until->evaluate());

			if(untilEval->getType() != OBJ_LOGICAL)
				throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_LOGICAL, untilEval->getType());

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
Do::~Do()
{
	delete list;
	delete until;
}
