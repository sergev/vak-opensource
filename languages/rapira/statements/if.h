#ifndef IF_H
#define IF_H

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

class If : public Node
{

	public:

		/*** Constructor ***/
		If();

		/*** Set the if condition ***/
		void setIfCondition(Object* pIfExpr);

		/*** Set the if statements ***/
		void setIfStatements(NodeList* pIfStmts);

		/*** Set the else statements ***/
		void setElseStatements(NodeList* pElseStmts);

		/*** Execute this node ***/
		Outcome execute();

		/*** Clone this object ***/
		If* clone() const
		{
			If* nif = new If();
			nif->setLineNumber(getLineNumber());
			nif->setColumnNumber(getColumnNumber());
			if(ifExpr != 0)
				nif->setIfCondition(ifExpr->clone());
			if(ifStmts != 0)
				nif->setIfStatements(ifStmts->clone());
			if(elseStmts != 0)
				nif->setElseStatements(elseStmts->clone());
			return nif;
		}

		/*** Destructor ***/
		~If();

	private:

		/*** The if condition ***/
		Object* ifExpr;

		/*** The if statements ***/
		NodeList* ifStmts;

		/*** The else statements ***/
		NodeList* elseStmts;
		
};

#endif
