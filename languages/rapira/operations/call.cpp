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

#include "call.h"

/*** Constructor ***/
Call::Call()
{
	setIdentifier(0);
}

/*** Constructor ***/
Call::Call(Object* pIden)
{
	setIdentifier(pIden);
}

/*** Get this operation's type ***/
unsigned char Call::getType()
{
	return OP_CALL;
}

/*** Set the identifier ***/
void Call::setIdentifier(Object* pIden)
{
	iden = pIden;
}

/*** Push an argument ***/
void Call::pushArgument(Object* pArg, bool isInOut)
{
	args.push_back(std::pair<Object*, bool>(pArg, isInOut));
}

/*** Execute this node ***/
Outcome Call::execute()
{
	std::auto_ptr<Object> evalCall(evaluate());
	return Outcome(S_SUCCESS);
}

/*** Evaluate this object ***/
Object* Call::evaluate()
{
	if(iden == 0)
		throw MissingArgumentException(getLineNumber(), getColumnNumber(), OBJ_PROCEDURE | OBJ_FUNCTION, 1);

	// Evaluate the object specified as the procedure
	std::auto_ptr<Object> idenEval(iden->evaluate());

	// Execute a special function if we need
	if(idenEval->getType() == OBJ_SPFUNCTION)
	{
		SpecialFunction* spf = static_cast<SpecialFunction*>(idenEval.get());
		// Loop through the arguments to transfer them to the special function
		for(unsigned int i = 0; i < args.size(); i++)
		{
			if(args.at(i).second == true)
				throw Excep(getLineNumber(), getColumnNumber(), "Special functions cannot receive in-out arguments!");
			spf->addArgument(args.at(i).first->clone());
		}
		std::auto_ptr<Object> retVal(spf->evaluate());
		return retVal.release();
	}

	// Confirm that the given object is a procedure or function
	if(idenEval->getType() != OBJ_PROCEDURE && idenEval->getType() != OBJ_FUNCTION)
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_PROCEDURE | OBJ_FUNCTION, idenEval->getType());

	// Cast the evaluated identifier to a procedure
	std::auto_ptr<Procedure> proc(static_cast<Procedure*>(idenEval.release()));

	// Confirm that the number of arguments in this call
	// is equal to the number of parameters accepted by the procedure
	if(proc->getParameters().size() != args.size())
		throw Excep(getLineNumber(), getColumnNumber(), "Invalid number of arguments passed to procedure or function!");

	// Create a vector of pair of strings
	// First : <<< old name >>>
	// Second :<<< new name>>>
	std::vector< std::pair<std::string, std::string> > transfer;

	// Create a new Variable Manager
	VariableManager manager;

	// Create a vector to store the evaluated arguments,
	// which will later be added to the variable manager
	std::vector< std::pair<std::string, Object*> > addArgs;

	// Loop through the arguments making sure they match the corresponding parameter type
	for(unsigned int i = 0; i < args.size(); i++)
	{
		// If we are working on with a function,
		// make sure we are not receiving in-out arguments.
		if(proc->getType() == OBJ_FUNCTION && args.at(i).second == true)
			throw Excep(getLineNumber(), getColumnNumber(), "Functions cannot receive in-out arguments!");

		// Compare procedure and argument types
		if(args.at(i).second != proc->getParameters().at(i).second)
			throw Excep(getLineNumber(), getColumnNumber(), "Argument in/in-out type did not match parameter in/in-out type!");

		// Evaluate the argument
		std::auto_ptr<Object> argEval(args.at(i).first->evaluate());

		// If this argument is a variable and
		// in-out, keep track of it!
		if(args.at(i).first->getType() == OP_VARIABLE && args.at(i).second == true)
			transfer.push_back(std::pair<std::string, std::string>(static_cast<Variable*>(args.at(i).first)->getIdentifier(), proc->getParameters().at(i).first->getIdentifier()));

		// Push a new variable onto addArgs
		// It has the identifier of the corresponding parameter
		// And the value of the corresponding argument
		addArgs.push_back(std::pair<std::string, Object*>(proc->getParameterVariable(i)->getIdentifier(), argEval.release()));
	}

	// Push a new entry onto the Variable Manager
	manager.pushEntry();

	// Add a reference to the procedure which was
	// called to the variable manager
	if(iden->getType() == OP_VARIABLE)
		manager.setObject(static_cast<Variable*>(iden)->getIdentifier(), proc->clone());

	// Loop through addArgs, adding to the Variable Manager
	for(unsigned int i = 0; i < addArgs.size(); i++)
		manager.setObject(addArgs.at(i).first, addArgs.at(i).second);

	// Execute any intern command
	if(proc->getIntern() != 0)
		proc->getIntern()->execute();

	// Execute any extern command
	if(proc->getExtern() != 0)
		proc->getExtern()->execute();

	// Execute the statements of the procedure
	std::auto_ptr<Object> retVal;
	Outcome retOut = proc->execute();
	retVal.reset(retOut.getObject());

	// Use the transfer vector to set the variables on the lower level to the ones on the current level
	for(unsigned int i = 0; i < transfer.size(); i++)
		manager.setTopLevelObject(transfer.at(i).first, manager.getObject(transfer.at(i).second)->clone());

	// Transfer any modified extern variables to the lower level
	if(proc->getExtern() != 0)
	{
		Extern* ex = proc->getExtern();
		std::vector<std::string> idens = ex->getIdentifiers();
		for(unsigned int i = 0; i < idens.size(); i++)
			manager.setTopLevelObject(idens.at(i), manager.getObject(idens.at(i))->clone());
	}

	// Pop an entry off the variable manager
	manager.popEntry();

	return retVal.release();
}

/*** Destructor ***/
Call::~Call()
{
	delete iden;
	for(unsigned int i = 0; i < args.size(); i++)
		delete args.at(i).first;
}
