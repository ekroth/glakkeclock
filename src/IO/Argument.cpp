/*
    This file is part of GlakkeClock.

    GlakkeClock, ATI/AMD GPU utility.
    Copyright (C) 2010-2011  Andrée Ekroth

    GlakkeClock is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GlakkeClock is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GlakkeClock.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "IO/Argument.hpp"

#include <iostream>

using namespace kke;

using std::string;
using std::cout;
using std::endl;

Argument::Argument() : 
	lName(string()),
	sName(string()),
	info(string()),
	sValue(string()),
	iValue(0),
	type(kke::ArgumentExist)
{

}

Argument::Argument(kke::ArgumentType type, const string& lName, const std::string& sName, const std::string &info) : 
	lName(lName),
	sName(sName),
	info(info),
	sValue(string()),
	iValue(0),
	type(type)
{

}

const std::string& Argument::GetLongName() const
{
	return lName;
}

const std::string& Argument::GetShortName() const
{
	return sName;
}

int Argument::GetInt() const
{
	if (type != kke::ArgumentInt)
		cout << "Argument is not int type." << endl;
	
	return iValue;
}

const std::string& Argument::GetString() const
{
	if (type != kke::ArgumentString)
		cout << "Argument is not string type." << endl;
	
	return sValue;
}

const std::string& Argument::GetInfo() const
{
	return info;
}

ArgumentType Argument::GetType() const
{
	return type;
}

bool Argument::IsArg(const std::string& arg, bool caseSense) const
{
	if (!caseSense)
	{
		std::string low = StringToLow(arg);
		return low == "--" + StringToLow(lName) || low == "-" + StringToLow(sName);
	}
	else
		return arg == "--" + lName || arg == "-" + sName;
}

void Argument::SetSvalue(const std::string& value)
{
	sValue = value;
}

void Argument::SetIvalue(int value)
{
	iValue = value;
}

void Argument::SetInfo(const std::string& info)
{
	this->info = info;
}

// Static
string Argument::StringToLow(const string& var)
{
	std::string low;
	low.reserve(var.length());
	
	for (unsigned int i = 0; i < var.length(); i++)
		low.push_back(tolower(var[i]));
	
	return low;
}
