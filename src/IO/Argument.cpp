/*

Copyright (C) 2010-2011 by Andrée Ekroth

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

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
