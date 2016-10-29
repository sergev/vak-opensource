#ifndef TEXT_H
#define TEXT_H

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
#include "../object.h"
#include "../token.h"
#include "../exceptions/invalidinit.h"

class Text : public Object
{

	public:

		/*** Constructor ***/
		Text();

		/*** Constructor ***/
		Text(Token tok);

		/*** Constructor ***/
		Text(std::string pValue);

		/*** Constructor ***/
		Text(char pValue);

		/*** Get this object's type ***/
		unsigned char getType();

		/*** Get this text's length ***/
		unsigned int getLength();

		/*** Set this text's value ***/
		void setValue(std::string pValue);

		/*** Set this text's value ***/
		void setValue(char pValue);

		/*** Get this text's value ***/
		std::string getValue();

		/*** Evaluate this object ***/
		Object* evaluate();

		/*** Get a character ***/
		Text* getChar(unsigned int index);

		/*** Clone this object ***/
		Text* clone() const { return new Text(*this); }

		/*** Destructor ***/
		~Text();

	private:

		/*** The value of this text ***/
		std::string value;

};

#endif
