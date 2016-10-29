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

#include "if.h"

/*** Constructor ***/
If::If()
{
	setIfCondition(0);
	setIfStatements(0);
	setElseStatements(0);
}

/*** Set the if condition ***/
void If::setIfCondition(Object* pIfExpr)
{
	ifExpr = pIfExpr;
}

/*** Set the if statements ***/
void If::setIfStatements(NodeList* pIfStmts)
{
	ifStmts = pIfStmts;
}

/*** Set the else statements ***/
void If::setElseStatements(NodeList* pElseStmts)
{
	elseStmts = pElseStmts;
}

/*** Execute this node ***/
Outcome If::execute()
{
	std::auto_ptr<Object> ifEval(ifExpr->evaluate());

	if(ifEval->getType() != OBJ_LOGICAL)
		throw InvalidTypeException(ifExpr->getLineNumber(), ifExpr->getColumnNumber(), OBJ_LOGICAL, ifEval->getType());

	// Cast the if evaluation to a logical object
	Logical* ifResult = static_cast<Logical*>(ifEval.get());

	Outcome result(S_SUCCESS);

	// Execute the if block
	if(ifResult->getValue() == true)
		result = ifStmts->execute();

	// Execute an else block, if necessary
	else if(elseStmts != 0 && elseStmts->getLength() != 0)
		result = elseStmts->execute();

	return result;
}

/*** Destructor ***/
If::~If()
{
	delete ifExpr;
	delete ifStmts;
	delete elseStmts;
}
