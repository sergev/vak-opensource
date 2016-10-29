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

#include "output.h"

/*** Constructor ***/
Output::Output()
{
	newline = true;
}

/*** Constructor ***/
Output::Output(Object* pExpr, bool pNewline)
{
	newline = pNewline;
	addExpression(pExpr);
}

/*** Constructor ***/
Output::Output(bool pNewline)
{
	newline = pNewline;
}

/*** Add an expression ***/
void Output::addExpression(Object* pExpr)
{
	exprs.push_back(pExpr);
}

/*** Execute this node ***/
Outcome Output::execute()
{
	for(unsigned int i = 0; i < exprs.size(); i++)
	{
		// Evaluate the current expression
		std::auto_ptr<Object> val(exprs.at(i)->evaluate());

		if(val->getType() == OBJ_EMPTY)
			std::cout << "empty";
		else if(val->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(val.get());
			std::cout << cast->getValue();
		}
		else if(val->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(val.get());
			std::cout << cast->getValue();
		}
		else if(val->getType() == OBJ_TEXT)
		{
			Text* cast = static_cast<Text*>(val.get());
			std::cout << cast->getValue();
		}
		else if(val->getType() == OBJ_LOGICAL)
		{
			Logical* cast = static_cast<Logical*>(val.get());
			if(cast->getValue() == true)
				std::cout << "yes";
			else
				std::cout << "no";
		}
		else if(val->getType() == OBJ_SEQUENCE)
		{
			Sequence* seq = static_cast<Sequence*>(val.get());
			std::cout << "<* ";
			for(unsigned int i = 0; i < seq->getLength(); i++)
			{
				Output outElement(seq->getObject(i)->clone(), false);
				outElement.execute();
				if(i != seq->getLength() - 1)
					std::cout << ", ";
			}
			std::cout << " *>";
		}
		else
			throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL | OBJ_TEXT | OBJ_LOGICAL | OBJ_SEQUENCE, val->getType(), i + 1);
	}

	if(newline)
		std::cout << std::endl;

	return Outcome(S_SUCCESS);
}

/*** Destructor ***/
Output::~Output()
{
	for(unsigned int i = 0; i < exprs.size(); i++)
		delete exprs.at(i);
}
