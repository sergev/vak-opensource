#ifndef NODELIST_H
#define NODELIST_H

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
#include "node.h"

class NodeList : public Node
{

	public:

		/*** Constructor ***/
		NodeList();

		/*** Get the list's length ***/
		unsigned int getLength();

		/*** Clear the list ***/
		void clear();

		/*** Push a node ***/
		void pushNode(Node* pNode);

		/*** Pop a node ***/
		void popNode();

		/*** Get a node ***/
		Node* getNode(unsigned int index);

		/*** Set a node ***/
		void setNode(unsigned int index, Node* pNode);

		/*** Get the break node ***/
		Node* getBreak();

		/*** Execute this list ***/
		Outcome execute();

		/*** Clone this object ***/
		NodeList* clone() const
		{
			NodeList* nnl = new NodeList();
			for(unsigned int i = 0; i < list.size(); i++)
				nnl->pushNode(list.at(i)->clone());
			return nnl;
		}

		/*** Destructor ***/
		~NodeList();

	private:

		/*** The list of nodes ***/
		std::vector<Node*> list;

		/*** The index of the break ***/
		unsigned int bIndex;

};

#endif
