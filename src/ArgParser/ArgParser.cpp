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

#include "ArgParser/ArgParser.hpp"

#include <sstream>
#include <iostream>

using namespace kke;

using std::string;
using std::istringstream;
using std::cout;
using std::endl;

ArgParser::ArgParser() : tolower(false)
{

}

ArgParser::~ArgParser()
{

}

bool ArgParser::Registered (int arg)
{
	return argMap.find(arg) != argMap.end();
}

bool ArgParser::Exist(int arg)
{
	return argExistMap[arg];
}

int ArgParser::ValidArg(const std::string& arg) const
{
	for (ArgMap::const_iterator it = argMap.begin(); it != argMap.end(); it++)
	{
		if (it->second.IsArg(arg, tolower))
		{
			return it->first;
		}
	}
	
	return -1;
}

int ArgParser::GetInt(int arg, int def)
{
	if (argExistMap[arg])
		return argMap.find(arg)->second.GetInt();
	else
		return def;
}

string ArgParser::GetString(int arg, const std::string& def)
{
	if (argExistMap[arg])
		return argMap.find(arg)->second.GetString();
	else
		return def;
}

string ArgParser::GetlName (int arg)
{
	if (argMap.find(arg) != argMap.end())
		return argMap.find(arg)->second.GetLongName();
	else
		return "Doesn't exist.";
}

string ArgParser::GetsName (int arg)
{
	if (argMap.find(arg) != argMap.end())
		return argMap.find(arg)->second.GetShortName();
	else
		return "Doesn't exist";
}

string ArgParser::GetInfo(int arg)
{
	if (argMap.find(arg) != argMap.end())
		return argMap.find(arg)->second.GetInfo();
	else
		return "Doesn't exist.";
}

bool ArgParser::Process(int argc, char **argv)
{
	// Skip file name (0).
	for (int i = 1; i < argc; i++)
	{
		string current = string(argv[i]);
		bool validArg(false);
		int argId;
		
		// Find argument with same name
		for (ArgMap::iterator it = argMap.begin(); it != argMap.end(); it++)
		{
			if (it->second.IsArg(current, tolower))
			{
				argExistMap[it->first] = true;
				validArg = true;
				argId = it->first;
			}
		}
		
		if (!validArg)
		{
			cout << "Invalid argument: " + current << endl;
			return false;
		}
		
		Argument &arg = argMap.find(argId)->second;
		
		switch(arg.GetType())
		{
			case kke::ArgumentExist:
				// No value to set
				break;
				
			case kke::ArgumentInt:
			{
				bool error = false;
				
				if (i == argc - 1)
				{
					error = true;
				}
				else
				{
					istringstream stream(argv[++i]);
					if (stream.good())
					{
						int val;
						stream >> val;
						
						arg.SetIvalue(val);
					}
					else
						error = true;
				}
				
				if (error)
					cout << "-" + arg.GetShortName() + ", --" + arg.GetLongName() + " argument requires integer." << endl;
			}
				break;
				
			case kke::ArgumentString:
				bool error = false;
				
				if (i == argc - 1)
					error = true;
				else
					arg.SetSvalue(argv[++i]);
				
				if (error)
					cout << "-" + arg.GetShortName() + ", --" + arg.GetLongName() + " argument requires string." << endl;
				
				break;
		}
	}
	
	return true;
}

bool ArgParser::Register(int id, kke::ArgumentType type, const std::string& lName, const std::string& sName, const std::string &info)
{
	bool isGood = true;
	
#if DEBUG
	
	for (ArgMap::const_iterator it = argMap.begin(); it != argMap.end(); it++)
	{
		bool duplicate = false;
		
		if (it->first == id)
		{
			cout << "Duplicate argument IDs exist: " << id << endl;
			duplicate = true;
		}
		
		if (it->second.GetLongName() == lName)
		{
			cout << "Duplicate long argument names exist: " + lName << endl;
			duplicate = true;
		}
		
		if (it->second.GetShortName() == sName)
		{
			cout << "Duplicate short argument names exist: " + sName << endl;
			duplicate = true;
		}
		
		if (duplicate)
		{
			cout << "Duplicates:" << endl;
			cout << "Current:" << endl;
			cout << lName << endl << sName << endl << id << endl << info << endl << endl;
			
			cout << "Existing:" << endl;
			cout << it->second.GetLongName() << endl << it->second.GetShortName() << endl << it->first << endl << it->second.GetInfo() << endl << endl;
			
			isGood = false;
		}
	}
	
#endif
	
	argMap[id] = Argument(type, lName, sName, info);
	return isGood;
}

void ArgParser::SetToLower(bool value)
{
	tolower = value;
}