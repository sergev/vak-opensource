#ifndef VARMANAGER_H
#define VARMANAGER_H

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

#include <map>
#include <stack>
#include "primitives/real.h"
#include "primitives/text.h"
#include "primitives/variable.h"
#include "primitives/specialfunction.h"
#include "exceptions/invalidassignment.h"

class VariableManager
{

	public:

		/*** Constructor ***/
		VariableManager();

		/*** If an object exists in the current entry ***/
		bool hasObject(std::string id);

		/*** Set the object to which a variable refers ***/
		void setObject(std::string id, Object* obj);

		/*** Get the object to which a variable refers ***/
		Object* getObject(std::string id);
		
		/*** Push the current entry onto the manager ***/
		void pushEntry();

		/*** Pop an entry off the manager ***/
		void popEntry();

		/*** Get an object on the top entry ***/
		Object* getTopLevelObject(std::string id);

		/*** Set an object on the top entry ***/
		void setTopLevelObject(std::string id, Object* obj);

		/*** Empty the manager ***/
		void empty();

		/*** Return the size of the stack ***/
		unsigned int getStackSize();

		/*** Destructor ***/
		~VariableManager();

	private:

		/*** The entry of special values ***/
		static std::map<std::string, Object*> special;

		/*** The stack of entries ***/
		static std::stack< std::map<std::string, Object*> > stack;

		/*** The current entry ***/
		static std::map<std::string, Object*> entry;

};

#endif
