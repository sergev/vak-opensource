#ifndef INVALIDINIT_H
#define INVALIDINIT_H

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

class InvalidInitException : public Excep
{

	public:

		/*** Constructor ***/
		InvalidInitException(unsigned int pLineNum, unsigned int pColNum, unsigned char pType);

		/*** Set the object type ***/
		void setType(unsigned char pType);

		/*** Get the object type ***/
		unsigned char getType();

		/*** Update the message ***/
		void updateMessage();

		/*** Destructor ***/
		~InvalidInitException() throw();

	private:

		/*** The type of object to which this exception refers ***/
		unsigned char type;

};

#endif
