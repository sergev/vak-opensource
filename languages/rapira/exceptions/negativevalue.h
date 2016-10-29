#ifndef NEGATIVEVALUE_H
#define NEGATIVEVALUE_H

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

class NegativeValueException : public Excep
{

	public:

		/*** Constructor ***/
		NegativeValueException(unsigned int pLineNum, unsigned int pColNum, long pValue);

		/*** Constructor ***/
		NegativeValueException(unsigned int pLineNum, unsigned int pColNum, long pValue, unsigned char pArgNum);

		/*** Set the value ***/
		void setValue(long pValue);

		/*** Get the value ***/
		long getValue();

		/*** Set the argument number ***/
		void setArgumentNumber(unsigned int pArgNum);

		/*** Get the argument number ***/
		unsigned int getArgumentNumber();

		/*** Update the message ***/
		void updateMessage();

		/*** Destructor ***/
		~NegativeValueException() throw();

	private:

		/*** The value ***/
		long value;

		/*** The argument number to which this exception refers ***/
		unsigned int argNum;

		/*** Show the argument number ***/
		bool showArgNum;

};

#endif
