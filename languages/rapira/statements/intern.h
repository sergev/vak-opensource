#ifndef INTERN_H
#define INTERN_H

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
#include "../node.h"
#include "../varmanager.h"

class Intern : public Node
{

	public:

		/*** Constructor ***/
		Intern();

		/*** Constructor ***/
		Intern(std::vector<std::string> pList);

		/*** Add an identifier to the list ***/
		void addIdentifier(std::string iden);

		/*** Execute this node ***/
		Outcome execute();

		/*** Clone this object ***/
		Intern* clone() const
		{
			Intern* nin = new Intern(list);
			nin->setLineNumber(getLineNumber());
			nin->setColumnNumber(getColumnNumber());
			return nin;
		}

		/*** Destructor ***/
		~Intern();

	private:

		/*** The vector of identifiers ***/
		std::vector<std::string> list;
};

#endif
