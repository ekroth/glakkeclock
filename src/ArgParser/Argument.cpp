/*
    This file is part of GlakkeClock.

    GlakkeClock, ATI GPU Overclock.
    Copyright (C) 2010-2011  Andrée Ekroth

    GlakkeClock is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    GlakkeClock is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GlakkeClock.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ArgParser/Argument.hpp"

// Use Logger
#include <iostream>

using kke::Argument;

using std::string;

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
		std::cout << "Argument is not int type." << std::endl;
	
	return iValue;
}

const std::string& Argument::GetString() const
{
	if (type != kke::ArgumentString)
		std::cout << "Argument is not string type." << std::endl;
	
	return sValue;
}

const std::string& Argument::GetInfo() const
{
	return info;
}

kke::ArgumentType Argument::GetType() const
{
	return type;
}

bool Argument::IsArg(const std::string& arg) const
{
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
