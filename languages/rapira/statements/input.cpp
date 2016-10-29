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

#include "input.h"

/*** Constructor ***/
Input::Input()
{
	setText(false);
}

/*** Constructor ***/
Input::Input(Object* pExpr, bool pGetText)
{
	addExpression(pExpr);
	setText(pGetText);
}

/*** Constructor ***/
Input::Input(bool pGetText)
{
	setText(pGetText);
}

/*** Add an expression ***/
void Input::addExpression(Object* pExpr)
{
	exprs.push_back(pExpr);
}

/*** Set text ***/
void Input::setText(bool pGetText)
{
	getText = pGetText;
}

/*** Execute this node ***/
Outcome Input::execute()
{
	for(int i = 0; i < (int) exprs.size(); i++)
	{
		if(exprs.at(i)->getType() != OP_VARIABLE)
			throw Excep(getLineNumber(), getColumnNumber(), "Expected variable name to input statement!");

		// Input text
		if(getText)
		{
			char str[256];
			std::cin.getline(str, 256);
			Assign(static_cast<Variable*>(exprs.at(i))->clone(), new Text(std::string(str))).execute();
			continue;
		}

		// Input a number or logical
		std::string text;
		do
		{
			char str[256];
			std::cin.getline(str, 256);
			text = std::string(str);
		}
		while(text == "" || (!isInteger(text) && !isReal(text) && text != "yes" && text != "no"));

		if(isInteger(text))
			Assign(static_cast<Variable*>(exprs.at(i))->clone(), new Integer(text)).execute();
		else if(isReal(text))
			Assign(static_cast<Variable*>(exprs.at(i))->clone(), new Real(text)).execute();
		else if(text == "yes")
			Assign(static_cast<Variable*>(exprs.at(i))->clone(), new Logical(true)).execute();
		else
			Assign(static_cast<Variable*>(exprs.at(i))->clone(), new Logical(false)).execute();
	}

	return Outcome(S_SUCCESS);
}

/*** Destructor ***/
Input::~Input()
{
	for(unsigned int i = 0; i < exprs.size(); i++)
		delete exprs.at(i);
}

/*** If a string is an integer ***/
bool Input::isInteger(std::string text)
{
	std::string chars = text;
	if(chars.length() > 0 && (chars.at(0) == '+' || chars.at(0) == '-'))
		chars.erase(0, 1);

	for(unsigned int i = 0; i < chars.length(); i++)
	{
		if(!isDigit(chars.at(i)))
			return false;
	}

	return true;
}

/*** If a string is a real ***/
bool Input::isReal(std::string text)
{
	std::string chars = text;
	if(chars.length() > 0 && (chars.at(0) == '+' || chars.at(0) == '-'))
		chars.erase(0, 1);

	// Store the index of the e
	int eIndex = chars.find('e', 0);

	if(eIndex == (int) std::string::npos)
		return isDecimal(chars);

	return (isDecimal(chars.substr(0, eIndex)) || isInteger(chars.substr(0, eIndex))) && isInteger(chars.substr(eIndex + 1, chars.length()));
}

/*** If a character is a digit ***/
bool Input::isDigit(char letter)
{
	return (letter >= '0' && letter <= '9');
}

/*** If a string is a number with a decimal ***/
bool Input::isDecimal(std::string text)
{
	bool hasDecimal = false;
	for(unsigned int i = 0; i < text.length(); i++)
	{
		if(text.at(i) == '.')
		{
			if(hasDecimal)
				return false;
			hasDecimal = true;
		}
		else
		if(!isDigit(text.at(i)))
			return false;
	}

	return hasDecimal;
}
