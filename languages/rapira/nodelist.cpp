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

#include "nodelist.h"

/*** Constructor ***/
NodeList::NodeList()
{
	bIndex = 0;
}

/*** Get the list's length ***/
unsigned int NodeList::getLength()
{
	return list.size();
}

/*** Clear the sequence ***/
void NodeList::clear()
{
	// Delete all objects
	for(unsigned int i = 0; i < list.size(); i++)
		delete list.at(i);

	list.clear();
}

/*** Push a node ***/
void NodeList::pushNode(Node* pNode)
{
	list.push_back(pNode);
}

/*** Pop a node ***/
void NodeList::popNode()
{
	delete getNode(getLength() - 1);
	list.pop_back();
}

/*** Get a node ***/
Node* NodeList::getNode(unsigned int index)
{
	return list.at(index);
}

/*** Set a node ***/
void NodeList::setNode(unsigned int index, Node* pNode)
{
	list.at(index) = pNode;
}

/*** Get the break node ***/
Node* NodeList::getBreak()
{
	return getNode(bIndex);
}

/*** Execute this list ***/
Outcome NodeList::execute()
{
	for(unsigned int i = 0; i < getLength(); i++)
	{
		Outcome result = getNode(i)->execute();
		if(result.getStatus() != S_SUCCESS)
		{
			bIndex = i;
			return result;
		}
	}
	return Outcome(S_SUCCESS);
}

/*** Destructor ***/
NodeList::~NodeList()
{
	for(unsigned int i = 0; i < list.size(); i++)
		delete list.at(i);
}
