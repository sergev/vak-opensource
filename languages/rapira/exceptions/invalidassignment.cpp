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

#include "invalidassignment.h"

/*** Constructor ***/
InvalidAssignmentException::InvalidAssignmentException(std::string pIden, std::string pInfo)
{
	setIdentifier(pIden);
	setInformation(pInfo);
	updateMessage();
}

/*** Constructor ***/
InvalidAssignmentException::InvalidAssignmentException(unsigned int pLineNum, unsigned int pColNum, std::string pIden, std::string pInfo)
{
	setLineNumber(pLineNum);
	setColumnNumber(pColNum);
	setIdentifier(pIden);
	setInformation(pInfo);
	updateMessage();
}

/*** Set the object id ***/
void InvalidAssignmentException::setIdentifier(std::string pIden)
{
	iden = pIden;
}

/*** Get the object id ***/
std::string InvalidAssignmentException::getIdentifier()
{
	return iden;
}

/*** Set the information ***/
void InvalidAssignmentException::setInformation(std::string pInfo)
{
	info = pInfo;
}

/*** Get the information ***/
std::string InvalidAssignmentException::getInformation()
{
	return info;
}

/*** Update the message ***/
void InvalidAssignmentException::updateMessage()
{
	setMessage("Invalid assignment to variable with identifier \"" + getIdentifier() + "\".\n\t" + getInformation());
}

/*** Destructor ***/
InvalidAssignmentException::~InvalidAssignmentException() throw()
{
}
