#ifndef INVALIDASSIGNMENT_H
#define INVALIDASSIGNMENT_H

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

#include "excep.h"

class InvalidAssignmentException : public Excep
{

	public:

		/*** Constructor ***/
		InvalidAssignmentException(std::string pIden, std::string pInfo);

		/*** Constructor ***/
		InvalidAssignmentException(unsigned int pLineNum, unsigned int pColNum, std::string pIden, std::string pInfo);

		/*** Set the object id ***/
		void setIdentifier(std::string pIden);

		/*** Get the object id ***/
		std::string getIdentifier();

		/*** Set the information ***/
		void setInformation(std::string pInfo);

		/*** Get the information ***/
		std::string getInformation();

		/*** Update the message ***/
		void updateMessage();

		/*** Destructor ***/
		~InvalidAssignmentException() throw();

	private:

		/*** The object id ***/
		std::string iden;

		/*** The information ***/
		std::string info;

};

#endif
