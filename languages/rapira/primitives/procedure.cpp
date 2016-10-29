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

#include "procedure.h"

/*** Constructor ***/
Procedure::Procedure()
{
	setStatements(0);
	isFunc = false;
	ex = 0;
	in = 0;
}

/*** Constructor ***/
Procedure::Procedure(bool pIsFunc)
{
	setStatements(0);
	isFunc = pIsFunc;
	ex = 0;
	in = 0;
}

/*** Constructor ***/
Procedure::Procedure(NodeList* pStmts, bool pIsFunc)
{
	setStatements(pStmts);
	isFunc = pIsFunc;
	ex = 0;
	in = 0;
}

/*** Get this operation's type ***/
unsigned char Procedure::getType()
{
	if(isFunc)
		return OBJ_FUNCTION;
	return OBJ_PROCEDURE;
}

/*** Set the statements ***/
void Procedure::setStatements(NodeList* pStmts)
{
	stmts = pStmts;
}

/*** Push a parameters ***/
void Procedure::pushParameter(Variable* pParam, bool isInOut)
{
	params.push_back(std::pair<Variable*, bool>(pParam, isInOut));
}

/*** Get the statements ***/
NodeList* Procedure::getStatements()
{
	return stmts;
}

/*** Get the parameters ***/
std::vector< std::pair<Variable*, bool> > Procedure::getParameters()
{
	return params;
}

/*** Get a parameter ***/
Variable* Procedure::getParameterVariable(unsigned int index)
{
	return params.at(index).first;
}

/*** Execute this node ***/
Outcome Procedure::execute()
{
	return stmts->execute();
}

/*** Evaluate this object ***/
Object* Procedure::evaluate()
{
	return clone();
}

/*** Set the extern command ***/
void Procedure::setExtern(Extern* pEx)
{
	ex = pEx;
}

/*** Get the extern command ***/
Extern* Procedure::getExtern()
{
	return ex;
}

/*** Set the intern command ***/
void Procedure::setIntern(Intern* pIn)
{
	in = pIn;
}

/*** Get the intern command ***/
Intern* Procedure::getIntern()
{
	return in;
}

/*** Destructor ***/
Procedure::~Procedure()
{
	delete ex;
	delete in;
	delete stmts;
	for(unsigned int i = 0; i < params.size(); i++)
		delete params.at(i).first;
}
