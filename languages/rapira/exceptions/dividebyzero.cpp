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

#include "dividebyzero.h"

/*** Constructor ***/
DivideByZeroException::DivideByZeroException(unsigned int pLineNum, unsigned int pColNum)
{
	setLineNumber(pLineNum);
	setColumnNumber(pColNum);
	showArgNum = false;
	updateMessage();
}

/*** Constructor ***/
DivideByZeroException::DivideByZeroException(unsigned int pLineNum, unsigned int pColNum, unsigned char pArgNum)
{
	setLineNumber(pLineNum);
	setColumnNumber(pColNum);
	showArgNum = true;
	setArgumentNumber(pArgNum);
	updateMessage();
}

/*** Set the argument number ***/
void DivideByZeroException::setArgumentNumber(unsigned int pArgNum)
{
	argNum = pArgNum;
}

/*** Get the argument number ***/
unsigned int DivideByZeroException::getArgumentNumber()
{
	return argNum;
}

/*** Update the message ***/
void DivideByZeroException::updateMessage()
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
	mess.append("Attempt at division by zero occurred.");
	setMessage(mess);
}

/*** Destructor ***/
DivideByZeroException::~DivideByZeroException() throw()
{
}
