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

#include "intern.h"

/*** Constructor ***/
Intern::Intern()
{
}

/*** Constructor ***/
Intern::Intern(std::vector<std::string> pList)
{
	list = pList;
}

/*** Add an identifier to the list ***/
void Intern::addIdentifier(std::string iden)
{
	list.push_back(iden);
}

/*** Execute this node ***/
Outcome Intern::execute()
{
	VariableManager manager;

	for(unsigned int i = 0; i < list.size(); i++)
	{
		// Make sure there is no variable with the
		// current identifier already in existence
		if(manager.hasObject(list.at(i)))
			throw Excep(getLineNumber(), getColumnNumber(), "A variable with the identifier \"" + list.at(i) + "\" already exists!");
		manager.setObject(list.at(i), new Object());
	}

	return Outcome(S_SUCCESS);
}

/*** Destructor ***/
Intern::~Intern()
{
}
