#ifndef EXCEP_H
#define EXCEP_H

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

#include <exception>
#include <string>
#include "../object.h"

class Excep : public std::exception
{

	public:

		/*** Constructor ***/
		Excep();

		/*** Constructor ***/
		Excep(unsigned int pLineNum, unsigned int pColNum, std::string pMessage);

		/*** Set the line number ***/
		void setLineNumber(unsigned int pLineNum);

		/*** Set the column number ***/
		void setColumnNumber(unsigned int pColNum);

		/*** Set the message ***/
		void setMessage(std::string pMessage);

		/*** Get the line number ***/
		unsigned int getLineNumber();

		/*** Get the column number ***/
		unsigned int getColumnNumber();

		/*** Get the message ***/
		std::string getMessage();

		const char* what()
		{
			return message.c_str();
		}

		/*** Destructor ***/
		~Excep() throw();

	private:

		/*** The line number ***/
		unsigned int lineNum;

		/*** The column number ***/
		unsigned int colNum;

		/*** The message ***/
		std::string message;

	protected:

		/*** Convert an object type to an object name ***/
		std::string objectToString(unsigned char type);

};

#endif
