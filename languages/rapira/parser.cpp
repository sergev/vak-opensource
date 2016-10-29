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

#include "parser.h"

/*** Constructor ***/
Parser::Parser(std::string pFilename, std::vector<Token> pTokens)
{
	filename = pFilename;
	tokens = pTokens;
	index = 0;
	srand((unsigned)time(0));
	list = new NodeList();
}

/*** Parse a separation ***/
void Parser::parseSeparation()
{
	if(!hasTokens())
		return;

	if(peekToken().getType() != T_SEMICOLON && peekToken().getType() != T_EOL)
		throw ParserSyntaxException(getToken(), "Expected ';' or EOL to separate statements!");

	while(hasTokens() && (peekToken().getType() == T_EOL || peekToken().getType() == T_SEMICOLON))
		getToken();
}

/*** Parse from zero-level ***/
void Parser::parse()
{
	if(hasTokens() && peekToken().getType() == T_EOL)
		parseSeparation();

	while(hasTokens())
	{
		

	//		if(peekToken().getType() == T_PROC)
	//			parseProcedureDefinition();
	//	else	if(peekToken().getType() == T_FUN)
	//			parseFunctionDefinition();
	//	else
			if(peekToken().getType() == T_PROC)
			{
				parseProcedure();
				parseSeparation();
			}
			else if(peekToken().getType() == T_FUN)
			{
				parseFunction();
				parseSeparation();
			}
			else
			{
			list->pushNode(parseStatement());
			parseSeparation();
			}
	}
//	VariableManager manager;
}

/*** Descend the asbtract syntax tree ***/
void Parser::executeProgram()
{
	Outcome result;
	result = list->execute();
	//if(result != S_SUCCESS)
	//	throw ParserSyntaxException(list->getBreakToken(), "Invalid command!");
}

/*** Destructor ***/
Parser::~Parser()
{
	VariableManager manager;
	manager.empty();
	delete list;
}

/*** If the token list contains more tokens ***/
bool Parser::hasTokens()
{
	return (index < tokens.size());
}

/*** If the token list contains more tokens ***/
bool Parser::hasTokens(unsigned int offset)
{
	return ((index + offset - 1) < tokens.size());
}

/*** Get the next token ***/
Token Parser::getToken()
{
	if(index >= tokens.size())
	{
		Token endToken;
		unsigned long eline = 0;
		unsigned long ecol = 0;
		if(tokens.size() >= 1)
		{
			eline = tokens.at(index - 1).getLineNumber();
			ecol = tokens.at(index - 1).getColumnNumber();
		}
		endToken.setLineNumber(eline);
		endToken.setColumnNumber(ecol);
		endToken.setType(T_EOF);
		endToken.setLexeme("EOF");
		return endToken;
	}

	Token result = tokens.at(index);
	index++;
	return result;
}

/*** Put back a token ***/
void Parser::putToken()
{
	index--;
}

/*** Peek at the next token ***/
Token Parser::peekToken()
{
	return tokens.at(index);
}

/*** Peek at the next token ***/
Token Parser::peekToken(unsigned int offset)
{
	return tokens.at(index + offset - 1);
}

/*** Parse an expression ***/
Object* Parser::parseExpression()
{
	return parseSubExpression10();
}

/*** Parse a statement ***/
Node* Parser::parseStatement()
{
	if(hasTokens(2) && peekToken().getType() == T_IDENTIFIER && peekToken(2).getType() == T_ASSIGN)
		return parseAssign();
	if(hasTokens(2) && peekToken().getType() == T_IDENTIFIER && peekToken(2).getType() == T_LPAREN)
		return parseCall();
	if(hasTokens() && peekToken().getType() == T_CALL)
		return parseCall();
	if(hasTokens() && peekToken().getType() == T_CASE)
		return parseCase();
	if(hasTokens() && peekToken().getType() == T_DO)
		return parseDo();
	if(hasTokens() && peekToken().getType() == T_END)
		return parseEnd();
	if(hasTokens() && peekToken().getType() == T_EXIT)
		return parseExit();
	if(hasTokens() && peekToken().getType() == T_FOR)
		return parseFor();
	if(hasTokens() && peekToken().getType() == T_IF)
		return parseIf();
	if(hasTokens() && peekToken().getType() == T_INPUT)
		return parseInput();
	if(hasTokens() && peekToken().getType() == T_OUTPUT)
		return parseOutput();
	if(hasTokens() && peekToken().getType() == T_REPEAT)
		return parseRepeat();
	if(hasTokens() && peekToken().getType() == T_RETURN)
		return parseReturn();
	if(hasTokens(2) && peekToken(1).getType() == T_IDENTIFIER && peekToken(2).getType() == T_LBRACKET)
		return parseSliceSelectAssign();
	if(hasTokens() && peekToken().getType() == T_WHILE)
		return parseWhile();
	throw ParserSyntaxException(getToken(), "Invalid statement!");
	return 0;
}

/*** Parse an assign statement ***/
Assign* Parser::parseAssign()
{
	if(!hasTokens() || peekToken().getType() != T_IDENTIFIER)
		throw ParserSyntaxException(getToken(), "Expected variable identifier!");

	std::auto_ptr<Variable> target(new Variable(getToken().getLexeme()));

	if(!hasTokens() || peekToken().getType() != T_ASSIGN)
		throw ParserSyntaxException(getToken(), "Expected ':='!");
	Token tok = getToken();

	std::auto_ptr<Object> expr(parseExpression());

	Assign* as = new Assign(target.release(), expr.release());
	as->setLineNumber(tok.getLineNumber());
	as->setColumnNumber(tok.getColumnNumber());
	return as;
}

/*** Parse a call statement ***/
Call* Parser::parseCall()
{
	// Remove the optional call command
	if(hasTokens() && peekToken().getType() == T_CALL)
		getToken();

	/*** Parse a value ***/
	std::auto_ptr<Object> iden(parseValue());

	/*** Parse the call ***/
	std::auto_ptr<Object> call(parseSubExpression1a(iden.release()));

	/*** Check for invalid types ***/
	if(call->getType() != OP_CALL)
		throw ParserSyntaxException(Token(), "Malformed procedure/function call!");

	return static_cast<Call*>(call.release());
}

/*** Parse a case statement ***/
Case* Parser::parseCase()
{
	if(!hasTokens() || peekToken().getType() != T_CASE)
		throw ParserSyntaxException(getToken(), "Expected \"case\" command!");
	Token tok = getToken();

	// Parse any newlines
	while(hasTokens() && peekToken().getType() == T_EOL)
		getToken();

	// Create a case object
	std::auto_ptr<Case> result(new Case());
	result->setLineNumber(tok.getLineNumber());
	result->setColumnNumber(tok.getColumnNumber());

	// Look for the optional condition
	if(hasTokens() && peekToken().getType() != T_WHEN)
		result->setCondition(parseExpression());

	// Parse any newlines
	while(hasTokens() && peekToken().getType() == T_EOL)
		getToken();

	// Parse while statements
	while(hasTokens() && peekToken().getType() == T_WHEN)
	{
		// Create a nodelist to store when conditions
		std::auto_ptr<NodeList> whenExprs(new NodeList());

		// Store the when conditions
		do
		{
			getToken();
			whenExprs->pushNode(parseExpression());
		}
		while(hasTokens() && peekToken().getType() == T_COMMA);

		// Look for the colon
		if(!hasTokens() || peekToken().getType() != T_COLON)
			throw ParserSyntaxException(getToken(), "Expected ':'!");
		getToken();

		// Parse any newlines
		while(hasTokens() && peekToken().getType() == T_EOL)
			getToken();

		// Store statements in a NodeList
		std::auto_ptr<NodeList> whenStmts(new NodeList());
		while(hasTokens() && peekToken().getType() != T_WHEN && peekToken().getType() != T_ELSE && peekToken().getType() != T_ESAC)
		{
			whenStmts->pushNode(parseStatement());
			parseSeparation();
		}

		// Add this when blocks to the case object
		for(unsigned int i = 0; i < whenExprs->getLength(); i++)
			result->addWhen(static_cast<Object*>(whenExprs->getNode(i))->clone(), whenStmts.release());
	}

	// Look for an else block
	if(hasTokens() && peekToken().getType() == T_ELSE)
	{
		getToken();

		// Parse any newlines
		while(hasTokens() && peekToken().getType() == T_EOL)
			getToken();

		// Store statements in a NodeList
		std::auto_ptr<NodeList> elseStmts(new NodeList());
		while(hasTokens() && peekToken().getType() != T_ESAC)
		{
			elseStmts->pushNode(parseStatement());
			parseSeparation();
		}
		result->setElse(elseStmts.release());
	}

	if(!hasTokens() || peekToken().getType() != T_ESAC)
		throw ParserSyntaxException(getToken(), "Expected \"esac\" command!");
	getToken();

	return result.release();
}

/*** Parse a do statement ***/
Do* Parser::parseDo()
{
	if(!hasTokens() || peekToken().getType() != T_DO)
		ParserSyntaxException(getToken(), "Expected do command!");
	Token tok = getToken();

	// Parse any newlines
	while(hasTokens() && peekToken().getType() == T_EOL)
		getToken();

	std::auto_ptr<Do> result(new Do());
	result->setLineNumber(tok.getLineNumber());
	result->setColumnNumber(tok.getColumnNumber());

	// Add commands until "od" or "until" is found
	std::auto_ptr<NodeList> doStmts(new NodeList());
	while(hasTokens() && !(peekToken().getType() == T_OD || peekToken().getType() == T_UNTIL))
	{
		doStmts->pushNode(parseStatement());
		parseSeparation();
	}

	if(hasTokens() && peekToken().getType() == T_OD)
	{
		getToken();
		result->setStatements(doStmts.release());
		return result.release();
	}
	else if(hasTokens() && peekToken().getType() == T_UNTIL)
	{
		getToken();
		std::auto_ptr<Object> until(parseExpression());
		result->setStatements(doStmts.release());
		result->setUntilCondition(until.release());
		return result.release();
	}
	else
		throw ParserSyntaxException(getToken(), "Expected \"od\" or \"until\" command!");

	return 0;
}

/*** Parse an end statement ***/
End* Parser::parseEnd()
{
	if(!hasTokens() || peekToken().getType() != T_END)
		throw ParserSyntaxException(getToken(), "Expected \"end\" command!");
	Token tok = getToken();

	End* en = new End();
	en->setLineNumber(tok.getLineNumber());
	en->setColumnNumber(tok.getColumnNumber());
	return en;
}

/*** Parse an exit statement ***/
Exit* Parser::parseExit()
{
	if(!hasTokens() || peekToken().getType() != T_EXIT)
		ParserSyntaxException(getToken(), "Expected \"exit\" command!");
	Token tok = getToken();

	Exit* ex = new Exit();
	ex->setLineNumber(tok.getLineNumber());
	ex->setColumnNumber(tok.getColumnNumber());
	return ex;
}

/*** Parse an extern statement ***/
Extern* Parser::parseExtern()
{
	if(!hasTokens() || peekToken().getType() != T_EXTERN)
		throw new ParserSyntaxException(getToken(), "Expected \"extern\" command!");
	Token tok = getToken();

	if(!hasTokens() || peekToken().getType() != T_COLON)
		throw new ParserSyntaxException(getToken(), "Expected \":\"!");
	getToken();

	// Loop through, storing variable names
	std::auto_ptr<Extern> result(new Extern());
	result->setLineNumber(tok.getLineNumber());
	result->setColumnNumber(tok.getColumnNumber());
	while(hasTokens() && peekToken().getType() == T_IDENTIFIER)
	{
		result->addIdentifier(getToken().getLexeme());

		if(!hasTokens() || peekToken().getType() != T_COMMA)
			break;
		getToken();
	}

	return result.release();
}

/*** Parse a for statement ***/
For* Parser::parseFor()
{
	if(!hasTokens() || peekToken().getType() != T_FOR)
		throw ParserSyntaxException(getToken(), "Expected \"for\" command!");
	Token tok = getToken();

	// Create a For object
	std::auto_ptr<For> result(new For());
	result->setLineNumber(tok.getLineNumber());
	result->setColumnNumber(tok.getColumnNumber());

	// Set the for value
	result->setFor(parseExpression());

	// Look for the optional from
	if(hasTokens() && peekToken().getType() == T_FROM)
	{
		getToken();
		result->setFrom(parseExpression());
	}

	// Look for the optional to
	if(hasTokens() && peekToken().getType() == T_TO)
	{
		getToken();
		result->setTo(parseExpression());
	}

	// Look for the optional step
	if(hasTokens() && peekToken().getType() == T_STEP)
	{
		getToken();
		result->setStep(parseExpression());
	}

	// Parse any newlines
	while(hasTokens() && peekToken().getType() == T_EOL)
		getToken();

	// Look for the optional while
	if(hasTokens() && peekToken().getType() == T_WHILE)
	{
		getToken();
		result->setWhileCondition(parseExpression());
	}

	if(!hasTokens() || peekToken().getType() != T_DO)
		throw ParserSyntaxException(getToken(), "Expected \"do\" command!");
	getToken();

	// Parse any newlines
	while(hasTokens() && peekToken().getType() == T_EOL)
		getToken();

	// Add commands until "od" or "until" is found
	std::auto_ptr<NodeList> stmts(new NodeList());
	while(hasTokens() && peekToken().getType() != T_OD && peekToken().getType() != T_UNTIL)
	{
		stmts->pushNode(parseStatement());
		parseSeparation();
	}
	result->setStatements(stmts.release());

	if(hasTokens() && peekToken().getType() == T_UNTIL)
	{
		getToken();
		result->setUntilCondition(parseExpression());
	}
	else if(hasTokens() && peekToken().getType() == T_OD)
		getToken();
	else
		throw ParserSyntaxException(getToken(), "Expected \"od\" or \"until\" command!");

	return result.release();
}

/*** Parse an input statement ***/
Input* Parser::parseInput()
{
	if(!hasTokens() || peekToken().getType() != T_INPUT)
		throw ParserSyntaxException(getToken(), "Expected \"input\" statement.");
	Token tok = getToken();

	// Check if a getText symbol is present
	bool getText = false;
	if(hasTokens() && peekToken().getType() == T_TEXT)
	{
		getToken();
		getText = true;
	}

	// Remove the colon
	if(!hasTokens() || peekToken().getType() != T_COLON)
		throw ParserSyntaxException(getToken(), "Expected ':'.");

	std::auto_ptr<Input> result(new Input(getText));
	result->setLineNumber(tok.getLineNumber());
	result->setColumnNumber(tok.getColumnNumber());

	do
	{
		getToken();

		// Parse an expression
		result->addExpression(parseExpression());
	}
	while(hasTokens() && peekToken().getType() == T_COMMA);

	return result.release();
}

/*** Parse an intern statement ***/
Intern* Parser::parseIntern()
{
	if(!hasTokens() || peekToken().getType() != T_INTERN)
		throw new ParserSyntaxException(getToken(), "Expected \"intern\" command!");
	Token tok = getToken();

	if(!hasTokens() || peekToken().getType() != T_COLON)
		throw new ParserSyntaxException(getToken(), "Expected \":\"!");
	getToken();

	// Loop through, storing variable names
	std::auto_ptr<Intern> result(new Intern());
	result->setLineNumber(tok.getLineNumber());
	result->setColumnNumber(tok.getColumnNumber());

	while(hasTokens() && peekToken().getType() == T_IDENTIFIER)
	{
		result->addIdentifier(getToken().getLexeme());

		if(!hasTokens() || peekToken().getType() != T_COMMA)
			break;
		getToken();
	}

	return result.release();
}

/*** Parse an if statement ***/
If* Parser::parseIf()
{
	if(!hasTokens() || peekToken().getType() != T_IF)
		throw ParserSyntaxException(getToken(), "Expected \"if\".");
	Token tok = getToken();

	// Parse the condition
	std::auto_ptr<Object> expr(parseExpression());

	if(!hasTokens() || peekToken().getType() != T_THEN)
		throw ParserSyntaxException(getToken(), "Expected \"then\"!");
	getToken();

	// Parse any newlines
	while(hasTokens() && peekToken().getType() == T_EOL)
		getToken();

	// Parse if statement body
	std::auto_ptr<NodeList> ifStmts(new NodeList());
	while(hasTokens() && !(peekToken().getType() == T_ELSE || peekToken().getType() == T_FI))
	{
		ifStmts->pushNode(parseStatement());
		parseSeparation();
	}

	// Create the new if object
	std::auto_ptr<If> result(new If());
	result->setLineNumber(tok.getLineNumber());
	result->setColumnNumber(tok.getColumnNumber());

	// Parse an else statement body if present
	if(hasTokens() && peekToken().getType() == T_ELSE)
	{
		getToken();
		std::auto_ptr<NodeList> elseStmts(new NodeList());

		// Parse any newlines
		while(hasTokens() && peekToken().getType() == T_EOL)
			getToken();

		while(hasTokens() && peekToken().getType() != T_FI)
		{
			elseStmts->pushNode(parseStatement());
			parseSeparation();
		}

		result->setElseStatements(elseStmts.release());
	}

	if(!hasTokens() || peekToken().getType() != T_FI)
		throw ParserSyntaxException(getToken(), "Expected \"fi\"!");
	getToken();

	result->setIfCondition(expr.release());
	result->setIfStatements(ifStmts.release());
	return result.release();
}

/*** Parse an output statement***/
Output* Parser::parseOutput()
{
	if(!hasTokens() || peekToken().getType() != T_OUTPUT)
		throw ParserSyntaxException(getToken(), "Expected \"output\" statement.");
	Token tok = getToken();

	// Check if a no-line-feed symbol is present
	bool lineFeed = true;
	if(hasTokens() && peekToken().getType() == T_NLF)
	{
		getToken();
		lineFeed = false;
	}

	// Remove the colon
	if(!hasTokens() || peekToken().getType() != T_COLON)
		throw ParserSyntaxException(getToken(), "Expected ':'.");

	std::auto_ptr<Output> result(new Output(lineFeed));
	result->setLineNumber(tok.getLineNumber());
	result->setColumnNumber(tok.getColumnNumber());

	do
	{
		getToken();

		// Parse an expression
		result->addExpression(parseExpression());
	}
	while(hasTokens() && peekToken().getType() == T_COMMA);

	return result.release();
}

/*** Parse a procedure definition ***/
void Parser::parseProcedure()
{
	if(!hasTokens() || peekToken().getType() != T_PROC)
		throw ParserSyntaxException(getToken(), "Expected \"proc\" keyword!");
	Token tokProc = getToken();

	if(!hasTokens() || peekToken().getType() != T_IDENTIFIER)
		throw ParserSyntaxException(getToken(), "Expected procedure name!");

	// Store the name of this procedure
	Token tok = getToken();
	std::string procName = tok.getLexeme();

	// Make sure an object with this name does not already exist
	VariableManager manager;
	if(manager.hasObject(procName))
		throw ParserSyntaxException(tok, "An object with the identifier \"" + procName + "\" already exists!");

	// Create a new procedure object
	std::auto_ptr<Procedure> proc(new Procedure());
	proc->setLineNumber(tokProc.getLineNumber());
	proc->setColumnNumber(tokProc.getColumnNumber());

	// Look for the left parentheses
	if(!hasTokens() || peekToken().getType() != T_LPAREN)
		throw ParserSyntaxException(getToken(), "Expected '('!");
	getToken();

	// Loop through the list of parameters and add them to a NodeList
	while(hasTokens() && peekToken().getType() != T_RPAREN)
	{
		// Look for an in-out symbol
		bool isInOut = false;
		if(hasTokens() && peekToken().getType() == T_LESSEQ)
		{
			getToken();
			isInOut = true;
		}
		else if(hasTokens() && peekToken().getType() == T_EQGREAT)
			getToken();

		// Look for an identifier
		if(!hasTokens() || peekToken().getType() != T_IDENTIFIER)
			throw ParserSyntaxException(getToken(), "Expected parameter identifier!");

		// Store the parameter name
		Token paramName = getToken();

		// Store the parameter
		proc->pushParameter(new Variable(paramName), isInOut);

		if(!hasTokens() || peekToken().getType() != T_COMMA)
			break;
		getToken();
	}

	// Look for the right parentheses
	if(!hasTokens() || peekToken().getType() != T_RPAREN)
		throw ParserSyntaxException(getToken(), "Expected ')'!");
	getToken();

	// Parse a separation
	parseSeparation();

	// Create a NodeList to store statements
	std::auto_ptr<NodeList> stmts(new NodeList());

	// Parse intern/extern commands
	if(hasTokens() && peekToken().getType() == T_INTERN)
	{
		proc->setIntern(parseIntern());
		parseSeparation();
		if(hasTokens() && peekToken().getType() == T_EXTERN)
		{
			proc->setExtern(parseExtern());
			parseSeparation();
		}
	}
	else if(hasTokens() && peekToken().getType() == T_EXTERN)
	{
		proc->setExtern(parseExtern());
		parseSeparation();
		if(hasTokens() && peekToken().getType() == T_INTERN)
		{
			proc->setIntern(parseIntern());
			parseSeparation();
		}
	}

	// Store statements until an end command is found
	while(hasTokens() && peekToken().getType() != T_END)
	{
		stmts->pushNode(parseStatement());
		parseSeparation();
	}

	// Add the end command
	stmts->pushNode(parseStatement());

	// Set the procedure's statements
	proc->setStatements(stmts.release());

	// Add the object to the VariableManager
	manager.setObject(procName, proc.release());
}

/*** Parse a function definition ***/
void Parser::parseFunction()
{
	if(!hasTokens() || peekToken().getType() != T_FUN)
		throw ParserSyntaxException(getToken(), "Expected \"fun\" keyword!");
	Token tokFun = getToken();

	if(!hasTokens() || peekToken().getType() != T_IDENTIFIER)
		throw ParserSyntaxException(getToken(), "Expected function name!");

	// Store the name of this procedure
	Token tok = getToken();
	std::string procName = tok.getLexeme();

	// Make sure an object with this name does not already exist
	VariableManager manager;
	if(manager.hasObject(procName))
		throw ParserSyntaxException(tok, "An object with the identifier \"" + procName + "\" already exists!");

	// Create a new procedure object
	std::auto_ptr<Procedure> proc(new Procedure(true));
	proc->setLineNumber(tokFun.getLineNumber());
	proc->setColumnNumber(tokFun.getColumnNumber());

	// Look for the left parentheses
	if(!hasTokens() || peekToken().getType() != T_LPAREN)
		throw ParserSyntaxException(getToken(), "Expected '('!");
	getToken();

	// Loop through the list of parameters and add them to a NodeList
	while(hasTokens() && peekToken().getType() != T_RPAREN)
	{
		// Look for an in-out symbol
		if(hasTokens() && peekToken().getType() == T_LESSEQ)
			throw ParserSyntaxException(getToken(), "Functions cannot have in-out parameters!");

		if(hasTokens() && peekToken().getType() == T_EQGREAT)
			getToken();

		// Look for an identifier
		if(!hasTokens() || peekToken().getType() != T_IDENTIFIER)
			throw ParserSyntaxException(getToken(), "Expected parameter identifier!");

		// Store the parameter name
		Token paramName = getToken();

		// Store the parameter
		proc->pushParameter(new Variable(paramName), false);

		if(!hasTokens() || peekToken().getType() != T_COMMA)
			break;
		getToken();
	}

	// Look for the right parentheses
	if(!hasTokens() || peekToken().getType() != T_RPAREN)
		throw ParserSyntaxException(getToken(), "Expected ')'!");
	getToken();

	// Parse a separation
	parseSeparation();

	// Create a NodeList to store statements
	std::auto_ptr<NodeList> stmts(new NodeList());

	// Parse intern/extern commands
	if(hasTokens() && peekToken().getType() == T_INTERN)
	{
		proc->setIntern(parseIntern());
		parseSeparation();
		if(hasTokens() && peekToken().getType() == T_EXTERN)
		{
			proc->setExtern(parseExtern());
			parseSeparation();
		}
	}
	else if(hasTokens() && peekToken().getType() == T_EXTERN)
	{
		proc->setExtern(parseExtern());
		parseSeparation();
		if(hasTokens() && peekToken().getType() == T_INTERN)
		{
			proc->setIntern(parseIntern());
			parseSeparation();
		}
	}

	// Store statements until an end command is found
	while(hasTokens() && peekToken().getType() != T_END)
	{
		stmts->pushNode(parseStatement());
		parseSeparation();
	}

	// Add the end command
	stmts->pushNode(parseStatement());

	// Set the procedure's statements
	proc->setStatements(stmts.release());

	// Add the object to the VariableManager
	manager.setObject(procName, proc.release());
}

/*** Parse a repeat statement ***/
Repeat* Parser::parseRepeat()
{
	if(!hasTokens() || peekToken().getType() != T_REPEAT)
		ParserSyntaxException(getToken(), "Expected \"repeat\" command!");
	Token tok = getToken();

	// Parse any newlines
	while(hasTokens() && peekToken().getType() == T_EOL)
		getToken();

	// Create a new repeat object
	std::auto_ptr<Repeat> result(new Repeat());
	result->setLineNumber(tok.getLineNumber());
	result->setColumnNumber(tok.getColumnNumber());

	// Set the counter
	result->setCounter(parseExpression());

	// Parse any newlines
	while(hasTokens() && peekToken().getType() == T_EOL)
		getToken();

	// Look for the optional while condition
	if(hasTokens() && peekToken().getType() == T_WHILE)
	{
		getToken();
		result->setWhileCondition(parseExpression());
	}

	// Parse any newlines
	while(hasTokens() && peekToken().getType() == T_EOL)
		getToken();

	if(!hasTokens() || peekToken().getType() != T_DO)
		ParserSyntaxException(getToken(), "Expected \"do\" command!");
	getToken();

	// Parse any newlines
	while(hasTokens() && peekToken().getType() == T_EOL)
		getToken();

	// Add commands until "od" or "until" is found
	std::auto_ptr<NodeList> stmts(new NodeList());
	while(hasTokens() && !(peekToken().getType() == T_OD || peekToken().getType() == T_UNTIL))
	{
		stmts->pushNode(parseStatement());
		parseSeparation();
	}
	result->setStatements(stmts.release());

	if(hasTokens() && peekToken().getType() == T_OD)
		getToken();
	else if(hasTokens() && peekToken().getType() == T_UNTIL)
	{
		getToken();
		result->setUntilCondition(parseExpression());
	}
	else
		throw ParserSyntaxException(getToken(), "Expected \"od\" or \"until\" command!");

	return result.release();
}

/*** Parse a return statement ***/
Return* Parser::parseReturn()
{
	if(!hasTokens() || peekToken().getType() != T_RETURN)
		throw ParserSyntaxException(getToken(), "Expected \"return\" command!");
	Token tok = getToken();

	std::auto_ptr<Return> ret(new Return());
	ret->setLineNumber(tok.getLineNumber());
	ret->setColumnNumber(tok.getColumnNumber());

	if(!hasTokens() || peekToken().getType() == T_EOL)
		return ret.release();
	std::auto_ptr<Object> expr(parseExpression());
	ret->setExpression(expr.release());
	return ret.release();
}

/*** Parse a slice/select assign statement ***/
Node* Parser::parseSliceSelectAssign()
{
	if(!hasTokens() || peekToken().getType() != T_IDENTIFIER)
		throw ParserSyntaxException(getToken(), "Expected variable identifier!");

	// Store the variable
	std::auto_ptr<Variable> target(new Variable(getToken()));

	if(!hasTokens() || peekToken().getType() != T_LBRACKET)
		throw ParserSyntaxException(getToken(), "Expected '['!");
	Token tok = getToken();

	std::auto_ptr<Object> index1(parseExpression());

	if(hasTokens() && peekToken().getType() == T_RBRACKET)
	{
		getToken();

		if(!hasTokens() || peekToken().getType() != T_ASSIGN)
			throw ParserSyntaxException(getToken(), "Expected ':='!");
		getToken();

		std::auto_ptr<Object> expr(parseExpression());

		SelectAssign* result = new SelectAssign(target.release(), index1.release(), expr.release());
		result->setLineNumber(tok.getLineNumber());
		result->setColumnNumber(tok.getColumnNumber());
		return result;
	}

	if(!hasTokens() || peekToken().getType() != T_COLON)
		throw ParserSyntaxException(getToken(), "Expected ':' or ']'!");
	getToken();

	std::auto_ptr<Object> index2(parseExpression());

	if(!hasTokens() || peekToken().getType() != T_RBRACKET)
		throw ParserSyntaxException(getToken(), "Expected ']'!");
	getToken();

	if(!hasTokens() || peekToken().getType() != T_ASSIGN)
		throw ParserSyntaxException(getToken(), "Expected ':='!");
	getToken();

	std::auto_ptr<Object> expr(parseExpression());

	SliceAssign* result = new SliceAssign(target.release(), index1.release(), index2.release(), expr.release());
	result->setLineNumber(tok.getLineNumber());
	result->setColumnNumber(tok.getColumnNumber());
	return result;
}

/*** Parse a while statement ***/
While* Parser::parseWhile()
{
	if(!hasTokens() || peekToken().getType() != T_WHILE)
		ParserSyntaxException(getToken(), "Expected \"while\" command!");
	Token tok = getToken();

	// Store the condition
	std::auto_ptr<Object> cond(parseExpression());

	if(!hasTokens() || peekToken().getType() != T_DO)
		ParserSyntaxException(getToken(), "Expected \"do\" command!");
	getToken();

	// Parse any newlines
	while(hasTokens() && peekToken().getType() == T_EOL)
		getToken();

	// Add commands until "od" or "until" is found
	std::auto_ptr<NodeList> whileStmts(new NodeList());
	while(hasTokens() && !(peekToken().getType() == T_OD || peekToken().getType() == T_UNTIL))
	{
		whileStmts->pushNode(parseStatement());
		parseSeparation();
	}

	std::auto_ptr<While> result(new While());
	result->setLineNumber(tok.getLineNumber());
	result->setColumnNumber(tok.getColumnNumber());

	if(hasTokens() && peekToken().getType() == T_OD)
	{
		getToken();
		result->setCondition(cond.release());
		result->setStatements(whileStmts.release());
		return result.release();
	}

	if(hasTokens() && peekToken().getType() == T_UNTIL)
	{
		getToken();
		std::auto_ptr<Object> until(parseExpression());
		result->setCondition(cond.release());
		result->setStatements(whileStmts.release());
		result->setUntilCondition(until.release());
		return result.release();
	}

	throw ParserSyntaxException(getToken(), "Expected \"od\" or \"until\" command!");
	return 0;
}

Object* Parser::parseSubExpression10()
{
	std::auto_ptr<Object> expr1(parseSubExpression9());

	while(hasTokens() && peekToken().getType() == T_OR)
	{
		Token tok = getToken();
		std::auto_ptr<Object> expr2(parseSubExpression9());
		expr1.reset(new Or(expr1.release(), expr2.release()));
		expr1->setLineNumber(tok.getLineNumber());
		expr1->setColumnNumber(tok.getColumnNumber());
	}

	return expr1.release();
}

Object* Parser::parseSubExpression9()
{
	std::auto_ptr<Object> expr1(parseSubExpression8());

	while(hasTokens() && peekToken().getType() == T_AND)
	{
		Token tok = getToken();
		std::auto_ptr<Object> expr2(parseSubExpression8());
		expr1.reset(new And(expr1.release(), expr2.release()));
		expr1->setLineNumber(tok.getLineNumber());
		expr1->setColumnNumber(tok.getColumnNumber());
	}

	return expr1.release();
}

Object* Parser::parseSubExpression8()
{
	bool notResult = false;
	Token tok;
	if(hasTokens() && peekToken().getType() == T_NOT)
	{
		tok = getToken();
		notResult = true;
	}

	std::auto_ptr<Object> expr(parseSubExpression7());

	if(notResult)
	{
		expr.reset(new Not(expr.release()));
		expr->setLineNumber(tok.getLineNumber());
		expr->setColumnNumber(tok.getColumnNumber());
	}

	return expr.release();
}

Object* Parser::parseSubExpression7()
{
	std::auto_ptr<Object> expr1(parseSubExpression6());

	while(hasTokens() && (peekToken().getType() == T_SLASHEQ || peekToken().getType() == T_EQUAL))
	{
		Token tok = getToken();
		std::auto_ptr<Object> expr2(parseSubExpression6());
		if(tok.getType() == T_SLASHEQ)
			expr1.reset(new Unequal(expr1.release(), expr2.release()));
		else
			expr1.reset(new Equal(expr1.release(), expr2.release()));
		expr1->setLineNumber(tok.getLineNumber());
		expr1->setColumnNumber(tok.getColumnNumber());
	}

	return expr1.release();
}

Object* Parser::parseSubExpression6()
{
	std::auto_ptr<Object> expr1(parseSubExpression5());

	while(hasTokens() && (peekToken().getType() == T_LESS || peekToken().getType() == T_GREAT || peekToken().getType() == T_LESSEQ || peekToken().getType() == T_GREATEQ))
	{
		Token tok = getToken();
		unsigned char operation = tok.getType();
		std::auto_ptr<Object> expr2(parseSubExpression5());
		if(operation == T_LESS)
			expr1.reset(new Less(expr1.release(), expr2.release()));
		else if(operation == T_GREAT)
			expr1.reset(new Greater(expr1.release(), expr2.release()));
		else if(operation == T_LESSEQ)
			expr1.reset(new LessEq(expr1.release(), expr2.release()));
		else
			expr1.reset(new GreatEq(expr1.release(), expr2.release()));
		expr1->setLineNumber(tok.getLineNumber());
		expr1->setColumnNumber(tok.getColumnNumber());
	}

	return expr1.release();
}

Object* Parser::parseSubExpression5()
{
	std::auto_ptr<Object> expr1(parseSubExpression4());

	while(hasTokens() && (peekToken().getType() == T_PLUS || peekToken().getType() == T_MINUS))
	{
		Token tok = getToken();
		unsigned char operation = tok.getType();
		std::auto_ptr<Object> expr2(parseSubExpression4());
		if(operation == T_PLUS)
			expr1.reset(new Add(expr1.release(), expr2.release()));
		else
			expr1.reset(new Subtract(expr1.release(), expr2.release()));
		expr1->setLineNumber(tok.getLineNumber());
		expr1->setColumnNumber(tok.getColumnNumber());
	}

	return expr1.release();
}

Object* Parser::parseSubExpression4()
{
	std::auto_ptr<Object> expr1(parseSubExpression3());

	while(hasTokens() && (peekToken().getType() == T_MULTIPLY || peekToken().getType() == T_DIVIDE || peekToken().getType() == T_INTDIVIDE || peekToken().getType() == T_REMAINDER))
	{
		Token tok = getToken();
		unsigned char operation = tok.getType();
		std::auto_ptr<Object> expr2(parseSubExpression3());
		if(operation == T_MULTIPLY)
			expr1.reset(new Multiply(expr1.release(), expr2.release()));
		else if(operation == T_DIVIDE)
			expr1.reset(new Divide(expr1.release(), expr2.release()));
		else if(operation == T_INTDIVIDE)
			expr1.reset(new IntDivide(expr1.release(), expr2.release()));
		else
			expr1.reset(new Remainder(expr1.release(), expr2.release()));
		expr1->setLineNumber(tok.getLineNumber());
		expr1->setColumnNumber(tok.getColumnNumber());
	}

	return expr1.release();
}

Object* Parser::parseSubExpression3()
{
	std::auto_ptr<Object> expr1(parseSubExpression2());

	while(hasTokens() && peekToken().getType() == T_EXPONENT)
	{
		Token tok = getToken();
		std::auto_ptr<Object> expr2(parseSubExpression2());
		expr1.reset(new Exponent(expr1.release(), expr2.release()));
		expr1->setLineNumber(tok.getLineNumber());
		expr1->setColumnNumber(tok.getColumnNumber());
	}

	return expr1.release();
}

Object* Parser::parseSubExpression2()
{
	bool negateResult = false;
	Token negToken;
	if(hasTokens() && (peekToken().getType() == T_MINUS || peekToken().getType() == T_PLUS))
	{
		if(peekToken().getType() == T_MINUS)
			negateResult = true;
		negToken = getToken();
	}

	std::auto_ptr<Object> result;

	if(hasTokens() && peekToken().getType() == T_HASH)
	{
		Token lengthToken = getToken();
		std::auto_ptr<Object> sub(parseSubExpression1());
		result.reset(new Length(sub.release()));
		result->setLineNumber(lengthToken.getLineNumber());
		result->setColumnNumber(lengthToken.getColumnNumber());
	}

	else
		result.reset(parseSubExpression1());

	if(negateResult)
	{
		result.reset(new Negate(result.release()));
		result->setLineNumber(negToken.getLineNumber());
		result->setColumnNumber(negToken.getColumnNumber());
	}

	return result.release();
}

Object* Parser::parseSubExpression1()
{
	std::auto_ptr<Object> result;

	if(hasTokens() && peekToken().getType() == T_LPAREN)
	{
		getToken();
		result.reset(parseExpression());
		if(!hasTokens() || peekToken().getType() != T_RPAREN)
			throw ParserSyntaxException(getToken(), "Expected closing parentheses!");
		getToken();
	}

	else if(hasTokens() && peekToken().getType() == T_LARROW)
	{
		std::auto_ptr<Sequence> seq(new Sequence());
		Token tok = getToken();
		seq->setLineNumber(tok.getLineNumber());
		seq->setColumnNumber(tok.getColumnNumber());

		while(hasTokens() && peekToken().getType() != T_RARROW)
		{
			std::auto_ptr<Object> subExpr(parseExpression());
			seq->pushObject(subExpr.release());
			if(hasTokens() && peekToken().getType() != T_COMMA)
				break;
			getToken();
		}

		if(!hasTokens() || peekToken().getType() != T_RARROW)
			throw ParserSyntaxException(getToken(), "Expected right arrow to end sequence!");
		getToken();

		result.reset(seq.release());
	}

	else
		result.reset(parseValue());

	return parseSubExpression1a(result.release());
}

Object* Parser::parseSubExpression1a(Object* pExpr)
{
	std::auto_ptr<Object> expr(pExpr);

	if(hasTokens() && peekToken().getType() == T_LBRACKET)
	{
		Token tok = getToken();

		std::auto_ptr<Object> num1;
		if(hasTokens() && peekToken().getType() != T_COLON)
			num1.reset(parseExpression());

		if(hasTokens() && peekToken().getType() == T_RBRACKET)
		{
			getToken();
			std::auto_ptr<Select> result(new Select(expr.release(), num1.release()));
			result->setLineNumber(tok.getLineNumber());
			result->setColumnNumber(tok.getColumnNumber());
			return parseSubExpression1a(result.release());
		}

		if(!hasTokens() || peekToken().getType() != T_COLON)
			throw ParserSyntaxException(getToken(), "Expected ':' or ']'!");
		getToken();

		std::auto_ptr<Object> num2;
		if(hasTokens() && peekToken().getType() != T_RBRACKET)
			num2.reset(parseExpression());

		if(!hasTokens() || peekToken().getType() != T_RBRACKET)
			throw ParserSyntaxException(getToken(), "Expected ']'!");
		getToken();

		std::auto_ptr<Slice> result(new Slice(expr.release(), num1.release(), num2.release()));
		result->setLineNumber(tok.getLineNumber());
		result->setColumnNumber(tok.getColumnNumber());
		return parseSubExpression1a(result.release());
	}
	
	else if(hasTokens() && peekToken().getType() == T_LPAREN)
	{
		// Remove the left parentheses
		Token tok = getToken();

		// Create a new call
		std::auto_ptr<Call> procCall(new Call());
		procCall->setLineNumber(expr->getLineNumber());
		procCall->setColumnNumber(expr->getColumnNumber());

		// Set the identifier of the call
		procCall->setIdentifier(expr.release());

		// Loop through the list of arguments and add them to a NodeList
		while(hasTokens() && peekToken().getType() != T_RPAREN)
		{
			// Look for an in-out symbol
			bool isInOut = false;
			if(hasTokens() && peekToken().getType() == T_LESSEQ)
			{
				getToken();
				isInOut = true;
			}
			else if(hasTokens() && peekToken().getType() == T_EQGREAT)
				getToken();

			// Create a new object for the argument
			std::auto_ptr<Object> currentArg(parseExpression());

			// Store the parameter depending upon if it is in-out or not
			procCall->pushArgument(currentArg.release(), isInOut);

			if(!hasTokens() || peekToken().getType() != T_COMMA)
				break;
			getToken();
		}

		// Look for the right parentheses
		if(!hasTokens() || peekToken().getType() != T_RPAREN)
			throw ParserSyntaxException(getToken(), "Expected ')'!");
		getToken();

		return parseSubExpression1a(procCall.release());
	}

	return expr.release();
}

Object* Parser::parseValue()
{
	if(!hasTokens())
		return 0;

	if(peekToken().getType() == T_EMPTY)
	{
		Object* obj = new Object();
		Token tok = getToken();
		obj->setLineNumber(tok.getLineNumber());
		obj->setColumnNumber(tok.getColumnNumber());
		return obj;
	}

	if(peekToken().getType() == T_UNSIGNEDINT)
		return new Integer(getToken());

	if(peekToken().getType() == T_UNSIGNEDREAL)
		return new Real(getToken());

	if(peekToken().getType() == T_YES || peekToken().getType() == T_NO)
		return new Logical(getToken());

	if(peekToken().getType() == T_STRING)
		return new Text(getToken());

	if(peekToken().getType() == T_IDENTIFIER)
		return new Variable(getToken());

	return 0;
}
