#ifndef REAL_H
#define REAL_H

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

#include <sstream>
#include <cmath>
#include "../object.h"
#include "../token.h"
#include "../exceptions/invalidinit.h"

class Real : public Object
{

	public:

		/*** Constructor ***/
		Real();

		/*** Constructor ***/
		Real(Token tok);

		/*** Constructor ***/
		Real(double pValue);

		/*** Constructor ***/
		Real(std::string pValue);

		/*** Get this object's type ***/
		unsigned char getType();

		/*** Set this real's value ***/
		void setValue(double pValue);

		/*** Set this real's value ***/
		void setValue(std::string pValue);

		/*** Get this real's value ***/
		double getValue();

		/*** Evaluate this object ***/
		Object* evaluate();

		/*** Clone this object ***/
		Real* clone() const { return new Real(value); }

		/*** Destructor ***/
		~Real();

	private:

		/*** The value of this real ***/
		double value;

};

#endif
