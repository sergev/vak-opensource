#ifndef CALL_H
#define CALL_H

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
#include "../varmanager.h"
#include "../primitives/procedure.h"
#include "../exceptions/invalidtype.h"
#include "../primitives/specialfunction.h"
#include "../exceptions/missingargument.h"

class Call : public Object
{

	public:

		/*** Constructor ***/
		Call();

		/*** Constructor ***/
		Call(Object* pIden);

		/*** Get this operation's type ***/
		unsigned char getType();

		/*** Set the identifier ***/
		void setIdentifier(Object* pIden);

		/*** Push an argument ***/
		void pushArgument(Object* pArg, bool isInOut);

		/*** Get the arguments ***/
		std::vector< std::pair<Object*, bool> > getArguments();

		/*** Get an argument ***/
		Object* getArgument(unsigned int index);

		/*** Execute this node ***/
		Outcome execute();

		/*** Evaluate this object ***/
		Object* evaluate();

		/*** Clone this object ***/
		Call* clone() const
		{
			Call* clcall = new Call();
			clcall->setLineNumber(getLineNumber());
			clcall->setColumnNumber(getColumnNumber());
			if(iden != 0)
				clcall->setIdentifier(iden->clone());
			for(unsigned int i = 0; i < args.size(); i++)
				clcall->pushArgument(args.at(i).first->clone(), args.at(i).second);
			return clcall;
		}

		/*** Destructor ***/
		~Call();

	private:

		/*** The identifier ***/
		Object* iden;

		/*** The arguments ***/
		std::vector< std::pair<Object*, bool> > args;

};

#endif
