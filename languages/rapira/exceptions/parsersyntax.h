#ifndef PARSERSYNTAX_H
#define PARSERSYNTAX_H

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

#include "../token.h"
#include "excep.h"

class ParserSyntaxException : public Excep
{

	public:

		/*** Constructor ***/
		ParserSyntaxException(Token pToken, std::string pNotFound);

		/*** Set the token ***/
		void setToken(Token pToken);

		/*** Get the token ***/
		Token getToken();

		/*** Set the not found string ***/
		void setNotFound(std::string pNotFound);

		/*** Get the not found string ***/
		std::string getNotFound();

		/*** Update the message ***/
		void updateMessage();

		/*** Destructor ***/
		~ParserSyntaxException() throw();

	private:

		/*** The token ***/
		Token token;

		/*** A string explaining what was not found ***/
		std::string notFound;

};

#endif
