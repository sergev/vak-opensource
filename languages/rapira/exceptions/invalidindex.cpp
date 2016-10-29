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

#include "invalidindex.h"

/*** Constructor ***/
InvalidIndexException::InvalidIndexException(unsigned int pLineNum, unsigned int pColNum, long pValue)
{
	setLineNumber(pLineNum);
	setColumnNumber(pColNum);
	setValue(pValue);
	showArgNum = false;
	updateMessage();
}

/*** Constructor ***/
InvalidIndexException::InvalidIndexException(unsigned int pLineNum, unsigned int pColNum, long pValue, unsigned char pArgNum)
{
	setLineNumber(pLineNum);
	setColumnNumber(pColNum);
	setValue(pValue);
	setArgumentNumber(pArgNum);
	showArgNum = true;
	updateMessage();
}

/*** Set the value ***/
void InvalidIndexException::setValue(long pValue)
{
	value = pValue;
}

/*** Get the value ***/
long InvalidIndexException::getValue()
{
	return value;
}

/*** Set the argument number ***/
void InvalidIndexException::setArgumentNumber(unsigned int pArgNum)
{
	argNum = pArgNum;
}

/*** Get the argument number ***/
unsigned int InvalidIndexException::getArgumentNumber()
{
	return argNum;
}

/*** Update the message ***/
void InvalidIndexException::updateMessage()
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
	mess.append("Invalid integer index with value of " + oss.str() + ".");
	setMessage(mess);
}

/*** Destructor ***/
InvalidIndexException::~InvalidIndexException() throw()
{
}
