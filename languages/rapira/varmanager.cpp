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

#include "varmanager.h"

/*** The entry of special values ***/
std::map<std::string, Object*> VariableManager::special;

/*** The stack of entries ***/
std::stack< std::map<std::string, Object*> > VariableManager::stack;

/*** The current entry ***/
std::map<std::string, Object*> VariableManager::entry;

/*** Constructor ***/
VariableManager::VariableManager()
{
	// Set up special values
	if(special.empty())
	{
		special["pi"]		= new Real(3.14159265358979323846);
		special["lf"]		= new Text("\n");
		special["abs"]		= new SpecialFunction(SPF_ABS);
		special["sign"]		= new SpecialFunction(SPF_SIGN);
		special["sqrt"]		= new SpecialFunction(SPF_SQRT);
		special["entier"]	= new SpecialFunction(SPF_ENTIER);
		special["round"]	= new SpecialFunction(SPF_ROUND);
		special["rand"]		= new SpecialFunction(SPF_RAND);
		special["int_rand"]	= new SpecialFunction(SPF_INTRAND);
		special["index"]	= new SpecialFunction(SPF_INDEX);
		special["is_empty"]	= new SpecialFunction(SPF_ISEMPTY);
		special["is_log"]	= new SpecialFunction(SPF_ISLOG);
		special["is_int"]	= new SpecialFunction(SPF_ISINT);
		special["is_real"]	= new SpecialFunction(SPF_ISREAL);
		special["is_text"]	= new SpecialFunction(SPF_ISTEXT);
		special["is_seq"]	= new SpecialFunction(SPF_ISSEQ);
		special["is_proc"]	= new SpecialFunction(SPF_ISPROC);
		special["is_fun"]	= new SpecialFunction(SPF_ISFUN);
		special["sin"]		= new SpecialFunction(SPF_SIN);
		special["cos"]		= new SpecialFunction(SPF_COS);
		special["tg"]		= new SpecialFunction(SPF_TG);
		special["arcsin"]	= new SpecialFunction(SPF_ARCSIN);
		special["arctg"]	= new SpecialFunction(SPF_ARCTG);
		special["exp"]		= new SpecialFunction(SPF_EXP);
		special["ln"]		= new SpecialFunction(SPF_LN);
		special["lg"]		= new SpecialFunction(SPF_LG);
	}
}

/*** If an object exists in the current entry ***/
bool VariableManager::hasObject(std::string id)
{
	return (special.count(id) != 0 || entry.count(id) != 0);
}

/*** Set the object to which a variable refers ***/
void VariableManager::setObject(std::string id, Object* obj)
{
	if(special.count(id) != 0)
	{
		delete obj;
		throw InvalidAssignmentException(id, "Cannot assign value to a special variable.");
	}
	if(entry.count(id) != 0)
		delete entry[id];
	entry[id] = obj;
}

/*** Get the object to which a variable refers ***/
Object* VariableManager::getObject(std::string id)
{
	if(special.count(id) != 0)
		return special[id];
	return entry[id];
}
		
/*** Push the current entry onto the manager ***/
void VariableManager::pushEntry()
{
	stack.push(entry);
	entry.clear();
}

/*** Pop an entry off the manager ***/
void VariableManager::popEntry()
{
	// Clear all items from the current entry
	for(std::map<std::string, Object*>::iterator it = entry.begin(); it != entry.end(); it++)
		delete it->second;
	entry.clear();

	if(stack.empty())
		return;

	entry = stack.top();
	stack.pop();
}

/*** Get an object on the top entry ***/
Object* VariableManager::getTopLevelObject(std::string id)
{
	if(stack.top().count(id) == 0)
		return 0;
	return stack.top()[id];
}

/*** Set an object on the top entry ***/
void VariableManager::setTopLevelObject(std::string id, Object* obj)
{
	if(special.count(id) != 0)
		throw InvalidAssignmentException(id, "Cannot assign value to a special variable.");
	if(stack.top().count(id) != 0)
		delete stack.top()[id];
	stack.top()[id] = obj;
}

/*** Empty the manager ***/
void VariableManager::empty()
{
	// Clear all items from the special entry
	for(std::map<std::string, Object*>::iterator it = special.begin(); it != special.end(); it++)
		delete it->second;
	special.clear();
	// Clear all entries from the stack
	while(!stack.empty())
		popEntry();
	// Clear the last entry, not stored on the stack
	popEntry();
}

/*** Return the size of the stack ***/
unsigned int VariableManager::getStackSize()
{
	return stack.size();
}

/*** Destructor ***/
VariableManager::~VariableManager()
{
}
