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

#include "sliceassign.h"

/*** Constructor ***/
SliceAssign::SliceAssign()
{
	setTarget(0);
	setIndex1(0);
	setIndex2(0);
	setExpression(0);
}

/*** Constructor ***/
SliceAssign::SliceAssign(Variable* pTarget, Object* pIndex1, Object* pIndex2, Object* pExpr)
{
	setTarget(pTarget);
	setIndex1(pIndex1);
	setIndex2(pIndex2);
	setExpression(pExpr);
}

/*** Set the target ***/
void SliceAssign::setTarget(Variable* pTarget)
{
	target = pTarget;
}

/*** Set the index ***/
void SliceAssign::setIndex1(Object* pIndex)
{
	index1 = pIndex;
}

/*** Set the index ***/
void SliceAssign::setIndex2(Object* pIndex)
{
	index2 = pIndex;
}

/*** Set the expression ***/
void SliceAssign::setExpression(Object* pExpr)
{
	expr = pExpr;
}

/*** Execute this node ***/
Outcome SliceAssign::execute()
{
	std::auto_ptr<Object> evalIndex1(index1->evaluate());
	std::auto_ptr<Object> evalIndex2(index2->evaluate());

	// Confirm that the indices given are integers
	if(evalIndex1->getType() != OBJ_INTEGER)
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER, evalIndex1->getType());
	if(evalIndex2->getType() != OBJ_INTEGER)
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER, evalIndex2->getType());

	long numIndex1 = static_cast<Integer*>(evalIndex1.get())->getValue();
	long numIndex2 = static_cast<Integer*>(evalIndex2.get())->getValue();

	// Confirm that numIndex is greater than zero
	if(numIndex1 <= 0)
		throw InvalidIndexException(getLineNumber(), getColumnNumber(), numIndex1);
	if(numIndex2 <= 0)
		throw InvalidIndexException(getLineNumber(), getColumnNumber(), numIndex2);

	std::auto_ptr<Object> evalTarget(target->evaluate());
	std::auto_ptr<Object> evalExpr(expr->evaluate());

	if(evalTarget->getType() == OBJ_TEXT)
	{
		if(evalExpr->getType() != OBJ_TEXT)
			throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_TEXT, evalExpr->getType());

		Text* cast1 = static_cast<Text*>(evalTarget.get());
		Text* cast2 = static_cast<Text*>(evalExpr.get());

		if(numIndex1 > (int) cast1->getLength() || numIndex1 > numIndex2)
			throw InvalidIndexException(getLineNumber(), getColumnNumber(), numIndex1);
		if(numIndex2 > (int) cast1->getLength())
			throw InvalidIndexException(getLineNumber(), getColumnNumber(), numIndex2);

		std::string modified = "";
		modified.append(cast1->getValue().substr(0, numIndex1 - 1));
		modified.append(cast2->getValue());
		modified.append(cast1->getValue().substr(numIndex2, cast1->getLength() - numIndex2));

		return Assign(target->clone(), new Text(modified)).execute();
	}

	if(evalTarget->getType() == OBJ_SEQUENCE)
	{
		if(evalExpr->getType() != OBJ_SEQUENCE)
			throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_SEQUENCE, evalExpr->getType());

		Sequence* cast1 = static_cast<Sequence*>(evalTarget.get());
		Sequence* cast2 = static_cast<Sequence*>(evalExpr.get());

		if(numIndex1 > (int) cast1->getLength() || numIndex1 > numIndex2)
			throw InvalidIndexException(getLineNumber(), getColumnNumber(), numIndex1);
		if(numIndex2 > (int) cast1->getLength())
			throw InvalidIndexException(getLineNumber(), getColumnNumber(), numIndex2);

		Sequence* seqResult = new Sequence();
		for(unsigned int i = 0; i < (unsigned) numIndex1 - 1; i++)
			seqResult->pushObject(cast1->getObject(i)->clone());
		for(unsigned int j = 0; j < cast2->getLength(); j++)
			seqResult->pushObject(cast2->getObject(j)->clone());
		for(unsigned int k = numIndex2; k < cast1->getLength(); k++)
			seqResult->pushObject(cast1->getObject(k)->clone());

		return Assign(target->clone(), seqResult).execute();
	}

	throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_SEQUENCE | OBJ_TEXT, evalTarget->getType());
	return 0;
}

/*** Destructor ***/
SliceAssign::~SliceAssign()
{
	delete target;
	delete index1;
	delete index2;
	delete expr;
}
