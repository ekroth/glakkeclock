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

#include "IO/ArgParser.hpp"

#include <sstream>
#include <iostream>

using namespace kke;

using std::string;
using std::istringstream;
using std::cout;
using std::endl;

ArgParser *ArgParser::instance = 0;

ArgParser& ArgParser::Instance()
{
	if (instance == 0)
	{
		instance = new ArgParser();
	}
	
	return *instance;
}

void ArgParser::Terminate()
{
	if (instance != 0)
	{
		delete instance;
		instance = 0;
	}
}

ArgParser::ArgParser() : caseSense(true)
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
		if (it->second.IsArg(arg, caseSense))
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
		string current(argv[i]);
		bool validArg = false;
		int argId = 0;
		
		// Find argument with same name
		for (ArgMap::iterator it = argMap.begin(); it != argMap.end(); it++)
		{
			if (it->second.IsArg(current, caseSense))
			{
				argExistMap[it->first] = true;
				validArg = true;
				argId = it->first;
			}
		}
		
		if (!validArg)
		{
			// LOG
			cout << "Invalid argument: " << current << endl;
			return false;
// 			continue;
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
							arg.SetSvalue(argv[i]); // Also set string
						}
						else
							error = true;
					}
					
					if (error)
					{
						cout << '-' + arg.GetShortName() + ", --" + arg.GetLongName() + " argument requires integer." << endl;
						return false;
					}
				}
				break;
				
			case kke::ArgumentString:
				{
					bool error = false;
					
					if (i == argc - 1)
						error = true;
					else
						arg.SetSvalue(argv[++i]);
					
					if (error)
					{
						cout << '-' + arg.GetShortName() + ", --" + arg.GetLongName() + " argument requires string." << endl;
						return false;
					}
				}
				break;
		}
	}
	
	return true;
}

bool ArgParser::Register(int id, kke::ArgumentType type, const std::string& lName, const std::string& sName, const std::string &info)
{
	bool isGood = true;
	
// #if DEBUG
	
	for (ArgMap::const_iterator it = argMap.begin(); it != argMap.end(); it++)
	{
		bool duplicate = false;
		
		if (it->first == id)
		{
			cout << "Duplicate argument IDs exist: " << id << endl;
			duplicate = true;
		}
		
		if ((!caseSense && Argument::StringToLow(it->second.GetLongName()) == Argument::StringToLow(lName)) || 
			it->second.GetLongName() == lName)
		{
			cout << "Duplicate long argument names exist: " + lName << endl;
			duplicate = true;
		}
		
		if ((!caseSense && Argument::StringToLow(it->second.GetShortName()) == Argument::StringToLow(sName)) || 
			it->second.GetShortName() == sName)
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
	
// #endif
	
	argMap[id] = Argument(type, lName, sName, info);
	return isGood;
}

void ArgParser::CaseSense(bool value)
{
	caseSense = value;
}