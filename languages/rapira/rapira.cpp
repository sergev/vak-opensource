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

//#include <vld.h>

#include "lexer.h"
#include "parser.h"

std::string filename;

/*** Return an error ***/
void error(std::exception& e)
{
	std::cout << filename;
	std::cout << ':' << "Interpreter error";
	std::cout << ':' << e.what() << std::endl;
}

/*** Return an error ***/
void error(Excep& e)
{
	std::cout << "***Error occured in file \"" + filename + "\"!***" << std::endl;
	std::cout << " ++Line number: " << e.getLineNumber() << std::endl;
	std::cout << " ++Column number: " << e.getColumnNumber() << std::endl;
	std::cout << '\t' << e.getMessage() << std::endl;
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		std::cerr << "Invalid number of arguments!" << std::endl;
		std::cerr << "Usage: rapira [filename]" << std::endl;
		return 1;
	}

	// Set the filename
	filename = argv[1];

	// Initialize the lexer
	Lexer lexer(argv[1]);

	// Confirm that the input file has been opened correctly
	if(!lexer.isOpen())
	{
		std::cerr << "Error opening file!" << std::endl;
		//system("PAUSE");
		return 1;
	}

	// Create a vector of tokens to store
	std::vector<Token> tokens;
	while(lexer.hasMoreTokens())
	{
		try
		{
			Token tok = lexer.getNextToken();
			tokens.push_back(tok);
		}
		catch(Excep& e)
		{
			error(e);
			//system("PAUSE");
			return 1;
		}
	}

	// Send the token list to a parser
	Parser parser(filename, tokens);
	try
	{
		parser.parse();
		parser.executeProgram();
	}
	catch(Excep& e)
	{
		error(e);
		//system("PAUSE");
		return 1;
	}

	//system("PAUSE");
	return 0;
}
