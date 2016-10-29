#ifndef DO_H
#define DO_H

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

#include "../nodelist.h"
#include "../primitives/logical.h"
#include "../exceptions/invalidtype.h"

class Do : public Node
{

	public:

		/*** Constructor ***/
		Do();

		/*** Constructor ***/
		Do(NodeList* pList);

		/*** Constructor ***/
		Do(NodeList* pList, Object* pUntil);

		/*** Set the statements ***/
		void setStatements(NodeList* pList);

		/*** Get the statements ***/
		NodeList* getStatements();

		/*** Set the until condition ***/
		void setUntilCondition(Object* pUntil);

		/*** Execute this node ***/
		Outcome execute();

		/*** Clone this object ***/
		Do* clone() const
		{
			Do* ndn = new Do();
			ndn->setLineNumber(getLineNumber());
			ndn->setColumnNumber(getColumnNumber());
			if(list != 0)
				ndn->setStatements(list->clone());
			if(until != 0)
				ndn->setUntilCondition(until->clone());
			return ndn;
		}

		/*** Destructor ***/
		~Do();

	private:

		/*** This loop's list of nodes ***/
		NodeList* list;

		/*** The until condition ***/
		Object* until;

};

#endif
