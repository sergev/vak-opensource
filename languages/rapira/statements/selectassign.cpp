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

#include "selectassign.h"

/*** Constructor ***/
SelectAssign::SelectAssign()
{
	setTarget(0);
	setIndex(0);
	setExpression(0);
}

/*** Constructor ***/
SelectAssign::SelectAssign(Variable* pTarget, Object* pIndex, Object* pExpr)
{
	setTarget(pTarget);
	setIndex(pIndex);
	setExpression(pExpr);
}

/*** Set the target ***/
void SelectAssign::setTarget(Variable* pTarget)
{
	target = pTarget;
}

/*** Set the index ***/
void SelectAssign::setIndex(Object* pIndex)
{
	index = pIndex;
}

/*** Set the expression ***/
void SelectAssign::setExpression(Object* pExpr)
{
	expr = pExpr;
}

/*** Execute this node ***/
Outcome SelectAssign::execute()
{
	std::auto_ptr<Object> indexObject(index->evaluate());

	// Confirm that the index given is an integer
	if(indexObject->getType() != OBJ_INTEGER)
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER, indexObject->getType());

	long numIndex = static_cast<Integer*>(indexObject.get())->getValue();

	// Confirm that numIndex is greater than zero
	if(numIndex <= 0)
		throw InvalidIndexException(getLineNumber(), getColumnNumber(), numIndex);

	// Store the evaluated target and expr
	std::auto_ptr<Object> obj1(target->evaluate());
	std::auto_ptr<Object> obj2(expr->evaluate());

	if(obj1->getType() == OBJ_TEXT)
	{
		if(obj2->getType() != OBJ_TEXT)
			throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_TEXT, obj2->getType());

		Text* cast1 = static_cast<Text*>(obj1.get());
		Text* cast2 = static_cast<Text*>(obj2.get());

		if(numIndex > (int) cast1->getLength())
			throw InvalidIndexException(getLineNumber(), getColumnNumber(), numIndex);

		if(cast2->getLength() != 1)
			throw InvalidAssignmentException(getLineNumber(), getColumnNumber(), target->getIdentifier(), "Cannot perform assignment with text value of length greater than one!");

		std::string modified = cast1->getValue();
		modified.replace(numIndex - 1, 1, cast2->getValue());

		Text* modText = new Text(modified);
		return Assign(target->clone(), modText).execute();
	}

	if(obj1->getType() == OBJ_SEQUENCE)
	{
		Sequence* cast = static_cast<Sequence*>(obj1.get());

		if(numIndex > (int) cast->getLength())
			throw InvalidIndexException(getLineNumber(), getColumnNumber(), numIndex);

		std::auto_ptr<Sequence> seqClone(cast->clone());
		seqClone->setObject(numIndex - 1, obj2->clone());
		return Assign(target->clone(), seqClone.release()).execute();
	}

	throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_SEQUENCE | OBJ_TEXT, obj1->getType());
	return 0;
}

/*** Destructor ***/
SelectAssign::~SelectAssign()
{
	delete target;
	delete index;
	delete expr;
}
