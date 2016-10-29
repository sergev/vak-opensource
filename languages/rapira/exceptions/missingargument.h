#ifndef MISSINGARGUMENT_H
#define MISSINGARGUMENT_H

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

#include <sstream>
#include "excep.h"

class MissingArgumentException : public Excep
{

	public:

		/*** Constructor ***/
		MissingArgumentException(unsigned int pLineNum, unsigned int pColNum, unsigned char pExpected);

		/*** Constructor ***/
		MissingArgumentException(unsigned int pLineNum, unsigned int pColNum, unsigned char pExpected, unsigned char pArgNum);

		/*** Set the expected object type ***/
		void setExpectedType(unsigned char pType);

		/*** Get the expected object type ***/
		unsigned char getExpectedType();

		/*** Set the argument number ***/
		void setArgumentNumber(unsigned int pReceived);

		/*** Get the argument number ***/
		unsigned int getArgumentNumber();

		/*** Update the message ***/
		void updateMessage();

		/*** Destructor ***/
		~MissingArgumentException() throw();

	private:

		/*** The expected type of object to which this exception refers ***/
		unsigned char expected;

		/*** The argument number to which this exception refers ***/
		unsigned int argNum;

		/*** Show the argument number ***/
		bool showArgNum;

};

#endif
