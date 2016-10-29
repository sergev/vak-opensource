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

#include "sequence.h"

/*** Constructor ***/
Sequence::Sequence()
{
}

/*** Constructor ***/
Sequence::Sequence(std::vector<Object*> pSeq)
{
	// Copy the objects from the
	// given list to the current list
	for(unsigned int i = 0; i < pSeq.size(); i++)
		pushObject(pSeq.at(i)->clone());
}

/*** Clear the sequence ***/
void Sequence::clear()
{
	// Delete all objects
	for(unsigned int i = 0; i < getLength(); i++)
		delete list.at(i);
}

/*** Get this object's type ***/
unsigned char Sequence::getType()
{
	return OBJ_SEQUENCE;
}

/*** Get this sequence's length ***/
unsigned int Sequence::getLength()
{
	return list.size();
}

/*** Push an object onto this sequence ***/
void Sequence::pushObject(Object* obj)
{
	list.push_back(obj);
}

/*** Pop an object off of this sequence ***/
void Sequence::popObject()
{
	delete getObject(getLength() - 1);
	list.pop_back();
}

/*** Set an object at an index ***/
void Sequence::setObject(unsigned int index, Object* obj)
{
	if(list.at(index) != 0)
		delete list.at(index);
	list.at(index) = obj;
}

/*** Get an object at an index ***/
Object* Sequence::getObject(unsigned int index)
{
	return list.at(index);
}

/*** Get the list of objects ***/
std::vector<Object*> Sequence::getList()
{
	return list;
}

/*** Evaluate this object ***/
Object* Sequence::evaluate()
{
	// Note to self: When a sequence is evaluated,
	// a sequence is returned that contains only Objects
	// that are not operations.
	Sequence* result = new Sequence();

	for(unsigned int i = 0; i < getLength(); i++)
	{
		std::auto_ptr<Object> curObject(getObject(i)->clone());
		result->pushObject(curObject->evaluate());
	}

	return result;
}

/*** Destructor ***/
Sequence::~Sequence()
{
	clear();
}
