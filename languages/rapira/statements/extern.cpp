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

#include "extern.h"

/*** Constructor ***/
Extern::Extern()
{
}

/*** Constructor ***/
Extern::Extern(std::vector<std::string> pList)
{
	list = pList;
}

/*** Add an identifier to the list ***/
void Extern::addIdentifier(std::string iden)
{
	list.push_back(iden);
}

/*** Get the identifiers ***/
std::vector<std::string> Extern::getIdentifiers()
{
	return list;
}

/*** Execute this node ***/
Outcome Extern::execute()
{
	// Call the variable manager
	VariableManager manager;

	// Loop through the identifiers, adding references to the current entry
	for(unsigned int i = 0; i < list.size(); i++)
	{
		// Store the name of the current identifier
		std::string name = list.at(i);

		// Make sure a variable with the given
		// identifier does not already exist
		if(manager.hasObject(name))
			throw Excep(getLineNumber(), getColumnNumber(), "Variable with identifier \"" + name + "\" already found!");

		Object* obj = manager.getTopLevelObject(name);
		if(obj == 0)
			throw Excep(getLineNumber(), getColumnNumber(), "No variable with identifier \"" + name + "\" found!");

		manager.setObject(name, obj->clone());
	}

	return Outcome(S_SUCCESS);
}

/*** Destructor ***/
Extern::~Extern()
{
}
