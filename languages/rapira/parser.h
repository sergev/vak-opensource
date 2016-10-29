#ifndef PARSER_H
#define PARSER_H

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

#include <cstdlib>
#include <ctime>
#include <vector>

#include "exceptions/parsersyntax.h"

#include "nodelist.h"
#include "token.h"

/*** Operations ***/
#include "operations/add.h"
#include "operations/and.h"
#include "operations/call.h"
#include "operations/divide.h"
#include "operations/equal.h"
#include "operations/exponent.h"
#include "operations/greater.h"
#include "operations/greateq.h"
#include "operations/intdivide.h"
#include "operations/length.h"
#include "operations/less.h"
#include "operations/lesseq.h"
#include "operations/multiply.h"
#include "operations/negate.h"
#include "operations/not.h"
#include "operations/remainder.h"
#include "operations/select.h"
#include "operations/slice.h"
#include "operations/subtract.h"
#include "operations/unequal.h"
#include "operations/or.h"

/*** Statements ***/
#include "statements/assign.h"
#include "statements/case.h"
#include "statements/do.h"
#include "statements/end.h"
#include "statements/exit.h"
#include "statements/extern.h"
#include "statements/for.h"
#include "statements/if.h"
#include "statements/input.h"
#include "statements/intern.h"
#include "statements/output.h"
#include "statements/repeat.h"
#include "statements/return.h"
#include "statements/selectassign.h"
#include "statements/sliceassign.h"
#include "statements/while.h"

class Parser
{
	public:

		/*** Constructor ***/
		Parser(std::string pFilename, std::vector<Token> pTokens);

		/*** Parse from zero-level ***/
		void parse();

		/*** Descend the asbtract syntax tree ***/
		void executeProgram();

		/*** Destructor ***/
		~Parser();

	private:

		/*** The name of the file being parsed ***/
		std::string filename;

		/*** The list of nodes for the generated abstract syntax tree ***/
		NodeList* list;

		/*** The list of tokens ***/
		std::vector<Token> tokens;

		/*** The current index of the token being processed ***/
		unsigned int index;

		/*** If the token list contains more tokens ***/
		bool hasTokens();

		/*** If the token list contains more tokens ***/
		bool hasTokens(unsigned int offset);

		/*** Get the next token ***/
		Token getToken();

		/*** Put back a token ***/
		void putToken();

		/*** Peek at the next token ***/
		Token peekToken();

		/*** Peek at the next token ***/
		Token peekToken(unsigned int offset);

		/*** Parse an expression ***/
		Object* parseExpression();

		/*** Parse a statement ***/
		Node* parseStatement();

		/*** Parse an assign statement ***/
		Assign* parseAssign();

		/*** Parse a call statement ***/
		Call* parseCall();

		/*** Parse a case statement ***/
		Case* parseCase();

		/*** Parse a do statement ***/
		Do* parseDo();

		/*** Parse an end statement ***/
		End* parseEnd();

		/*** Parse an exit statement ***/
		Exit* parseExit();

		/*** Parse an extern statement ***/
		Extern* parseExtern();

		/*** Parse a for statement ***/
		For* parseFor();

		/*** Parse an if statement ***/
		If* parseIf();

		/*** Parse an input statement***/
		Input* parseInput();

		/*** Parse an intern statement ***/
		Intern* parseIntern();

		/*** Parse an output statement***/
		Output* parseOutput();

		/*** Parse a procedure definition ***/
		void parseProcedure();

		/*** Parse a function definition ***/
		void parseFunction();

		/*** Parse a repeat statement ***/
		Repeat* parseRepeat();

		/*** Parse a return statement ***/
		Return* parseReturn();

		/*** Parse a slice/select assign statement ***/
		Node* parseSliceSelectAssign();

		/*** Parse a while statement ***/
		While* parseWhile();

		/*** Parse a separation ***/
		void parseSeparation();

		/*** Begin sub-expression parser definitions ***/
		Object* parseSubExpression10();
		Object* parseSubExpression9();
		Object* parseSubExpression8();
		Object* parseSubExpression7();
		Object* parseSubExpression6();
		Object* parseSubExpression5();
		Object* parseSubExpression4();
		Object* parseSubExpression3();
		Object* parseSubExpression2();
		Object* parseSubExpression1();
		Object* parseSubExpression1a(Object* pExpr);
		Object* parseValue();
		/*** End sub-expression parser definitions ***/

};

#endif
