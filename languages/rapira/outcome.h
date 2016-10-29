#ifndef OUTCOME_H
#define OUTCOME_H

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

class Object;

#define S_SUCCESS	0	// Execution was successful
#define	S_EXIT		1	// Exit command encountered during execution
#define	S_RETURN	2	// Return command encountered during execution
#define S_END		3	// Exit from a procedure or function

// Note:
// After reading the return value, it is
// the responsibility of the procedure/function call
// to delete the object referenced by the pointer.

class Outcome
{
	public:

		/*** Constructor ***/
		Outcome();

		/*** Constructor ***/
		Outcome(unsigned char pStatus);

		/*** Constructor ***
		Outcome(const Outcome& cop)
		{
			setStatus(cop.status);
			setObject(cop.obj);
		}*/

		/*** Set the status of this outcome ***/
		void setStatus(unsigned char pStatus);

		/*** Get the status of this outcome ***/
		unsigned char getStatus();

		/*** Set the object ***/
		void setObject(Object* pObj);

		/*** Get the object ***/
		Object* getObject();

		/*** Destructor ***/
		~Outcome();

	private:

		/*** The status of this outcome ***/
		unsigned char status;

		/*** A pointer optionally associated with this outcome ***/
		Object* obj;

};

#endif
