#ifndef VARIABLE_H
#define VARIABLE_H

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

#include <string>
#include "../token.h"
#include "../object.h"
#include "../varmanager.h"

class Variable : public Object
{

	public:

		/*** Constructor ***/
		Variable();

		/*** Constructor ***/
		Variable(Token tok);

		/*** Constructor ***/
		Variable(std::string pIden);

		/*** Get this object's type ***/
		unsigned char getType();

		/*** Set this variable's identifier ***/
		void setIdentifier(std::string pIden);

		/*** Get this variable's identifier ***/
		std::string getIdentifier();

		/*** Evaluate this object ***/
		Object* evaluate();

		/*** Clone this object ***/
		Variable* clone() const { return new Variable(iden); }

		/*** Destructor ***/
		~Variable();

	private:

		/*** The identifier of this variable ***/
		std::string iden;

};

#endif
