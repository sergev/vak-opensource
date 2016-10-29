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

#include "invalidtype.h"

/*** Constructor ***/
InvalidTypeException::InvalidTypeException(unsigned int pLineNum, unsigned int pColNum, unsigned char pExpected)
{
	setLineNumber(pLineNum);
	setColumnNumber(pColNum);
	setExpectedType(pExpected);
	showReceived = false;
	showArgNum = false;
	updateMessage();
}

/*** Constructor ***/
InvalidTypeException::InvalidTypeException(unsigned int pLineNum, unsigned int pColNum, unsigned char pExpected, unsigned char pReceived)
{
	setLineNumber(pLineNum);
	setColumnNumber(pColNum);
	setExpectedType(pExpected);
	setReceivedType(pReceived);
	showReceived = true;
	showArgNum = false;
	updateMessage();
}

/*** Constructor ***/
InvalidTypeException::InvalidTypeException(unsigned int pLineNum, unsigned int pColNum, unsigned char pExpected, unsigned char pReceived, unsigned char pArgNum)
{
	setLineNumber(pLineNum);
	setColumnNumber(pColNum);
	setExpectedType(pExpected);
	setReceivedType(pReceived);
	setArgumentNumber(pArgNum);
	showReceived = true;
	showArgNum = true;
	updateMessage();
}

/*** Set the expected object type ***/
void InvalidTypeException::setExpectedType(unsigned char pExpected)
{
	expected = pExpected;
}

/*** Get the expected object type ***/
unsigned char InvalidTypeException::getExpectedType()
{
	return expected;
}

/*** Set the received object type ***/
void InvalidTypeException::setReceivedType(unsigned char pReceived)
{
	received = pReceived;
}

/*** Get the received object type ***/
unsigned char InvalidTypeException::getReceivedType()
{
	return received;
}

/*** Set the argument number ***/
void InvalidTypeException::setArgumentNumber(unsigned int pArgNum)
{
	argNum = pArgNum;
}

/*** Get the argument number ***/
unsigned int InvalidTypeException::getArgumentNumber()
{
	return argNum;
}

/*** Update the message ***/
void InvalidTypeException::updateMessage()
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
	mess.append("Expected object of the following type(s):" + objectToString(expected) + ".");
	if(showReceived)
		mess.append("\n\tReceived argument of type" + objectToString(received) + ". ");
	setMessage(mess);
}

/*** Destructor ***/
InvalidTypeException::~InvalidTypeException() throw()
{
}
