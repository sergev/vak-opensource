#ifndef SEQUENCE_H
#define SEQUENCE_H

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

#include <vector>
#include "../object.h"

class Sequence : public Object
{

	public:

		/*** Constructor ***/
		Sequence();

		/*** Constructor ***/
		Sequence(std::vector<Object*> pSeq);

		/*** Clear the sequence ***/
		void clear();

		/*** Get this object's type ***/
		unsigned char getType();

		/*** Get this sequence's length ***/
		unsigned int getLength();

		/*** Push an object onto this sequence ***/
		void pushObject(Object* obj);

		/*** Pop an object off of this sequence ***/
		void popObject();

		/*** Set an object at an index ***/
		void setObject(unsigned int index, Object* obj);

		/*** Get an object at an index ***/
		Object* getObject(unsigned int index);

		/*** Get the list of objects ***/
		std::vector<Object*> getList();

		/*** Evaluate this object ***/
		Object* evaluate();

		// Clone this object
		Sequence* clone() const
		{
			return new Sequence(list);
		}

		/*** Destructor ***/
		~Sequence();

	private:

		/*** This sequence's list ***/
		std::vector<Object*> list;

};

#endif
