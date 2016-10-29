#ifndef WHILE_H
#define WHILE_H

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

class While : public Node
{

	public:

		/*** Constructor ***/
		While();

		/*** Constructor ***/
		While(Object* pCondition, NodeList* pList);

		/*** Constructor ***/
		While(Object* pCondition, NodeList* pList, Object* pUntil);

		/*** Set the condition ***/
		void setCondition(Object* pCond);

		/*** Set the until condition ***/
		void setUntilCondition(Object* pUntil);

		/*** Set the statements ***/
		void setStatements(NodeList* pList);

		/*** Execute this node ***/
		Outcome execute();

		/*** Clone this object ***/
		While* clone() const
		{
			While* nwn = new While();
			nwn->setLineNumber(getLineNumber());
			nwn->setColumnNumber(getColumnNumber());
			if(cond != 0)
				nwn->setCondition(cond->clone());
			if(list != 0)
				nwn->setStatements(list->clone());
			if(until != 0)
				nwn->setUntilCondition(until->clone());
			return nwn;
		}

		/*** Destructor ***/
		~While();

	private:

		/*** This loop's list of nodes ***/
		NodeList* list;

		/*** The condition ***/
		Object* cond;

		/*** The optional until condition ***/
		Object* until;

};

#endif
