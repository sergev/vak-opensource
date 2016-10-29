#ifndef EXTERN_H
#define EXTERN_H

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
#include <string>
#include "../node.h"
#include "../varmanager.h"

class Extern : public Node
{

	public:

		/*** Constructor ***/
		Extern();

		/*** Constructor ***/
		Extern(std::vector<std::string> pList);

		/*** Add an identifier to the list ***/
		void addIdentifier(std::string iden);

		/*** Get the identifiers ***/
		std::vector<std::string> getIdentifiers();

		/*** Execute this node ***/
		Outcome execute();

		/*** Clone this object ***/
		Extern* clone() const
		{
			Extern* nen = new Extern(list);
			nen->setLineNumber(getLineNumber());
			nen->setColumnNumber(getColumnNumber());
			return nen;
		}

		/*** Destructor ***/
		~Extern();

	private:

		/*** The vector of identifiers ***/
		std::vector<std::string> list;
};

#endif
