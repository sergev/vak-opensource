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

#include "return.h"

/*** Constructor ***/
Return::Return()
{
	setExpression(0);
}

/*** Constructor ***/
Return::Return(Object* pExpr)
{
	setExpression(pExpr);
}

/*** Set the expression ***/
void Return::setExpression(Object* pExpr)
{
	expr = pExpr;
}

/*** Get the expression ***/
Object* Return::getExpression()
{
	return expr;
}

/*** Execute this node ***/
Outcome Return::execute()
{
	Outcome retVal(S_RETURN);
	if(expr != 0)
		retVal.setObject(expr->evaluate());
	return retVal;
}

/*** Destructor ***/
Return::~Return()
{
	delete expr;
}
