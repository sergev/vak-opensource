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

#include "node.h"

/*** Constructor ***/
Node::Node()
{
	setLineNumber(0);
	setColumnNumber(0);
}

/*** Constructor ***/
Node::Node(unsigned int pLineNum, unsigned int pColNum)
{
	setLineNumber(pLineNum);
	setColumnNumber(pColNum);
}

/*** Set the line number ***/
void Node::setLineNumber(unsigned int pLineNum)
{
	lineNum = pLineNum;
}

/*** Get the line number ***/
unsigned int Node::getLineNumber() const
{
	return lineNum;
}

/*** Set the column number ***/
void Node::setColumnNumber(unsigned int pColNum)
{
	colNum = pColNum;
}

/*** Get the column number ***/
unsigned int Node::getColumnNumber() const
{
	return colNum;
}
