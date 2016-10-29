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

#include "negativevalue.h"

/*** Constructor ***/
NegativeValueException::NegativeValueException(unsigned int pLineNum, unsigned int pColNum, long pValue)
{
	setLineNumber(pLineNum);
	setColumnNumber(pColNum);
	setValue(pValue);
	showArgNum = false;
	updateMessage();
}

/*** Constructor ***/
NegativeValueException::NegativeValueException(unsigned int pLineNum, unsigned int pColNum, long pValue, unsigned char pArgNum)
{
	setLineNumber(pLineNum);
	setColumnNumber(pColNum);
	setValue(pValue);
	showArgNum = true;
	setArgumentNumber(pArgNum);
	updateMessage();
}

/*** Set the value ***/
void NegativeValueException::setValue(long pValue)
{
	value = pValue;
}

/*** Get the value ***/
long NegativeValueException::getValue()
{
	return value;
}

/*** Set the argument number ***/
void NegativeValueException::setArgumentNumber(unsigned int pArgNum)
{
	argNum = pArgNum;
}

/*** Get the argument number ***/
unsigned int NegativeValueException::getArgumentNumber()
{
	return argNum;
}

/*** Update the message ***/
void NegativeValueException::updateMessage()
{
	std::string mess = "";
	if(showArgNum)
	{
		std::ostringstream oss;
		oss << argNum;
		mess.assign("At argument ");
		mess.append(oss.str());
		mess.append(". . .\n\t");
	}
	std::ostringstream oss;
	oss << getValue();
	mess.append("Received integer value of " + oss.str() + ".\n\tExpected non-negative value.");
	setMessage(mess);
}

/*** Destructor ***/
NegativeValueException::~NegativeValueException() throw()
{
}
