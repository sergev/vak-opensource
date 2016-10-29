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

#include "case.h"

/*** Constructor ***/
Case::Case()
{
	setCondition(0);
	setElse(0);
}

/*** Set the case condition ***/
void Case::setCondition(Object* expr)
{
	condition = expr;
}

/*** Add a when ***/
void Case::addWhen(Object* expr, NodeList* list)
{
	whenStmts.push_back(std::pair<Object*, NodeList*>(expr, list));
}

/*** Add an else ***/
void Case::setElse(NodeList* list)
{
	elseStmts = list;
}

/*** Execute this node ***/
Outcome Case::execute()
{
	// If no condition is specified
	if(condition == 0)
	{
		for(unsigned int i = 0; i < whenStmts.size(); i++)
		{
			std::auto_ptr<Object> whenVal(whenStmts.at(i).first->evaluate());

			if(whenVal->getType() != OBJ_LOGICAL)
				throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_LOGICAL, whenVal->getType());

			bool execWhen = static_cast<Logical*>(whenVal.get())->getValue();

			if(execWhen)
				return whenStmts.at(i).second->execute();
		}

		// Execute the else statements
		if(elseStmts != 0 && elseStmts->getLength() != 0)
			return elseStmts->execute();

		return Outcome(S_SUCCESS);
	}

	// If a condition is specified
	std::auto_ptr<Object> condVal(condition->evaluate());

	// Begin looking through the list of when conditions/statements
	for(unsigned int i = 0; i < whenStmts.size(); i++)
	{
		// Look for an equal condition
		std::auto_ptr<Object> whenVal(whenStmts.at(i).first->evaluate());

		std::auto_ptr<Object> equOperation(Equal(whenVal.release(), condVal->clone()).evaluate());
		bool execWhen = static_cast<Logical*>(equOperation.get())->getValue();

		if(execWhen)
			return whenStmts.at(i).second->execute();
	}

	// Execute the else statements
	if(elseStmts != 0 && elseStmts->getLength() != 0)
		return elseStmts->execute();

	return Outcome(S_SUCCESS);
}

/*** Destructor ***/
Case::~Case()
{
	delete condition;
	delete elseStmts;
	for(unsigned int i = 0; i < whenStmts.size(); i++)
	{
		delete whenStmts.at(i).first;
		delete whenStmts.at(i).second;
	}
	whenStmts.clear();
}
