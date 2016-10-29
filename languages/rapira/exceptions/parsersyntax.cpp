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

#include "parsersyntax.h"

/*** Constructor ***/
ParserSyntaxException::ParserSyntaxException(Token pToken, std::string pNotFound)
{
	setToken(pToken);
	setNotFound(pNotFound);
	updateMessage();
}

/*** Set the token ***/
void ParserSyntaxException::setToken(Token pToken)
{
	token = pToken;
}

/*** Get the token ***/
Token ParserSyntaxException::getToken()
{
	return token;
}

/*** Set the not found string ***/
void ParserSyntaxException::setNotFound(std::string pNotFound)
{
	notFound = pNotFound;
}

/*** Get the not found string ***/
std::string ParserSyntaxException::getNotFound()
{
	return notFound;
}

/*** Update the message ***/
void ParserSyntaxException::updateMessage()
{
	setLineNumber(token.getLineNumber());
	setColumnNumber(token.getColumnNumber());
	setMessage("Syntax error during parsing.\n\t" + notFound + "\n\tReceived \"" + getToken().getLexeme() + "\".");
}

/*** Destructor ***/
ParserSyntaxException::~ParserSyntaxException() throw()
{
}
