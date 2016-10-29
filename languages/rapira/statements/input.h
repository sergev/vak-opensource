#ifndef INPUT_H
#define INPUT_H

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

#include <iostream>
#include <vector>
#include "../primitives/integer.h"
#include "../primitives/real.h"
#include "../primitives/text.h"
#include "../primitives/logical.h"
#include "assign.h"
#include "../exceptions/invalidtype.h"

class Input : public Node
{

	public:

		/*** Constructor ***/
		Input();

		/*** Constructor ***/
		Input(Object* pExpr, bool pGetText);

		/*** Constructor ***/
		Input(bool pGetText);

		/*** Add an expression ***/
		void addExpression(Object* pExpr);

		/*** Set text ***/
		void setText(bool pGetText);

		/*** Execute this node ***/
		Outcome execute();

		/*** Clone this object ***/
		Input* clone() const
		{
			Input* nin = new Input(getText);
			nin->setLineNumber(getLineNumber());
			nin->setColumnNumber(getColumnNumber());
			for(unsigned int i = 0; i < exprs.size(); i++)
					nin->addExpression(exprs.at(i)->clone());
			return nin;
		}

		/*** Destructor ***/
		~Input();

	private:

		/*** The list of expressions ***/
		std::vector<Object*> exprs;

		/*** If this is a text input statement ***/
		bool getText;

		/*** If a string is an integer ***/
		bool isInteger(std::string text);

		/*** If a string is a real ***/
		bool isReal(std::string text);

		/*** If a character is a digit ***/
		bool isDigit(char letter);

		/*** If a string is a number with a decimal ***/
		bool isDecimal(std::string text);

};

#endif
