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

#include "variable.h"

/*** Constructor ***/
Variable::Variable()
{
	setIdentifier("");
}

/*** Constructor ***/
Variable::Variable(Token tok)
{
	setIdentifier(tok.getLexeme());
	setLineNumber(tok.getLineNumber());
	setColumnNumber(tok.getColumnNumber());
}

/*** Constructor ***/
Variable::Variable(std::string pIden)
{
	setIdentifier(pIden);
}

/*** Get this variable's type ***/
unsigned char Variable::getType()
{
	return OP_VARIABLE;
}

/*** Set this variable's identifier ***/
void Variable::setIdentifier(std::string pIden)
{
	iden = pIden;
}

/*** Get this variable's identifier ***/
std::string Variable::getIdentifier()
{
	return iden;
}

/*** Evaluate this object ***/
Object* Variable::evaluate()
{
	VariableManager manager;
	if(!manager.hasObject(getIdentifier()))
		return new Object();
	return manager.getObject(getIdentifier())->clone();
}

/*** Destructor ***/
Variable::~Variable()
{
}
