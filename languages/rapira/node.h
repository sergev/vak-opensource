#ifndef NODE_H
#define NODE_H

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

#include "outcome.h"

class Node
{

	public:

		/*** Constructor ***/
		Node();

		/*** Constructor ***/
		Node(unsigned int pLineNum, unsigned int pColNum);

		/*** Set the line number ***/
		void setLineNumber(unsigned int pLineNum);

		/*** Get the line number ***/
		unsigned int getLineNumber() const;

		/*** Set the column number ***/
		void setColumnNumber(unsigned int pColNum);

		/*** Get the column number ***/
		unsigned int getColumnNumber() const;

		/*** Execute this node ***/
		virtual Outcome execute()
		{
			return Outcome();
		}

		/*** Clone this node ***/
		virtual Node* clone() const { return new Node(*this); }

		/*** Destructor ***/
		virtual ~Node()
		{
		}

	private:

		/*** The number of the file line to which this node corresponds ***/
		unsigned int lineNum;

		/*** The number of the file column to which this node corresponds ***/
		unsigned int colNum;

};

#endif
