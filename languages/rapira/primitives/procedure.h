#ifndef PROCEDURE_H
#define PROCEDURE_H

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
#include "../object.h"
#include "variable.h"
#include "../statements/intern.h"
#include "../statements/extern.h"

class Procedure : public Object
{

	public:

		/*** Constructor ***/
		Procedure();

		/*** Constructor ***/
		Procedure(NodeList* pStmts, bool pIsFunc);

		/*** Constructor ***/
		Procedure(bool pIsFunc);

		/*** Get this operation's type ***/
		unsigned char getType();

		/*** Set the statements ***/
		void setStatements(NodeList* pStmts);

		/*** Push a parameters ***/
		void pushParameter(Variable* pParam, bool isInOut);

		/*** Get the statements ***/
		NodeList* getStatements();

		/*** Get the parameters ***/
		std::vector< std::pair<Variable*, bool> > getParameters();

		/*** Get a parameter ***/
		Variable* getParameterVariable(unsigned int index);

		/*** Execute this node ***/
		Outcome execute();

		/*** Evaluate this object ***/
		Object* evaluate();

		/*** Clone this object ***/
		Procedure* clone() const
		{
			Procedure* result = new Procedure(stmts->clone(), isFunc);
			for(unsigned int i = 0; i < params.size(); i++)
				result->pushParameter(params.at(i).first->clone(), params.at(i).second);
			if(ex != 0)
				result->setExtern(ex->clone());
			if(in != 0)
				result->setIntern(in->clone());
			return result;
		}

		/*** Set the extern command ***/
		void setExtern(Extern* pEx);

		/*** Get the extern command ***/
		Extern* getExtern();

		/*** Set the intern command ***/
		void setIntern(Intern* pIn);

		/*** Get the intern command ***/
		Intern* getIntern();

		/*** Destructor ***/
		~Procedure();

	private:

		/*** The procedure statements ***/
		NodeList* stmts;

		/*** The procedure parameters ***/
		std::vector< std::pair<Variable*, bool> > params;

		/*** If this procedure is a function ***/
		bool isFunc;

		/*** The extern command for this procedure ***/
		Extern* ex;

		/*** The intern command for this procedure ***/
		Intern* in;

};

#endif
