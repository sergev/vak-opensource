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

#include "assign.h"

/*** Constructor ***/
Assign::Assign()
{
	setTarget(0);
	setExpression(0);
}

/*** Constructor ***/
Assign::Assign(Variable* pTarget, Object* pExpr)
{
	setTarget(pTarget);
	setExpression(pExpr);
}

/*** Set the target ***/
void Assign::setTarget(Variable* pTarget)
{
	target = pTarget;
}

/*** Set the expression ***/
void Assign::setExpression(Object* pExpr)
{
	expr = pExpr;
}

/*** Execute this node ***/
Outcome Assign::execute()
{
	std::auto_ptr<Object> var(target->evaluate());
	if(var->getType() == OBJ_PROCEDURE || var->getType() == OBJ_FUNCTION)
		throw InvalidAssignmentException(getLineNumber(), getColumnNumber(), target->getIdentifier(), "Cannot overwrite a procedure or function!");

	VariableManager manager;
	try
	{
		manager.setObject(target->getIdentifier(), expr->evaluate());
	}
	catch(InvalidAssignmentException& e)
	{
		throw InvalidAssignmentException(getLineNumber(), getColumnNumber(), e.getIdentifier(), e.getInformation());
	}
	return Outcome(S_SUCCESS);
}

/*** Destructor ***/
Assign::~Assign()
{
	delete target;
	delete expr;
}
