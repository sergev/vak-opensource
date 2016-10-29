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

#include "excep.h"

/*** Constructor ***/
Excep::Excep()
{
	setLineNumber(0);
	setColumnNumber(0);
	setMessage("");
}

/*** Constructor ***/
Excep::Excep(unsigned int pLineNum, unsigned int pColNum, std::string pMessage)
{
	setLineNumber(pLineNum);
	setColumnNumber(pColNum);
	setMessage(pMessage);
}

/*** Set the line number ***/
void Excep::setLineNumber(unsigned int pLineNum)
{
	lineNum = pLineNum;
}

/*** Set the column number ***/
void Excep::setColumnNumber(unsigned int pColNum)
{
	colNum = pColNum;
}

/*** Set the message ***/
void Excep::setMessage(std::string pMessage)
{
	message = pMessage;
}

/*** Get the line number ***/
unsigned int Excep::getLineNumber()
{
	return lineNum;
}

/*** Get the column number ***/
unsigned int Excep::getColumnNumber()
{
	return colNum;
}

/*** Get the message ***/
std::string Excep::getMessage()
{
	return message;
}

/*** Destructor ***/
Excep::~Excep() throw()
{
}

/*** Convert an object type to an object name ***/
std::string Excep::objectToString(unsigned char type)
{
	std::string result = "";
	if(type == 0)
		return " empty";
	if(type & OBJ_INTEGER)
		result += " integer";
	if(type & OBJ_REAL)
		result += " real";
	if(type & OBJ_LOGICAL)
		result += " logical";
	if(type & OBJ_TEXT)
		result += " text";
	if(type & OBJ_SEQUENCE)
		result += " sequence";
	if(type & OBJ_PROCEDURE)
		result += " procedure";
	if(type & OBJ_FUNCTION)
		result += " function";
	return result;
}