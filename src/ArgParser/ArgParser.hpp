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


#ifndef KKE_ARGPARSER_HPP
#define KKE_ARGPARSER_HPP

#include "ArgParser/Argument.hpp"

#include <string>
#include <map>

namespace kke
{
	typedef std::map<int, Argument> ArgMap;
	typedef std::map<int, bool> ArgExistMap;
	class ArgParser
	{
	public:
		ArgParser();
		~ArgParser();
		
		bool Process(int argc, char **argv);
		
		int GetInt(int arg, int def = 0);
		std::string GetString(int arg, const std::string &def = std::string());
		
		std::string GetlName(int arg);
		std::string GetsName(int arg);
		std::string GetInfo(int arg);
		
		bool Registered(int arg);
		bool Exist(int arg);
		int ValidArg(const std::string &arg) const;
		
		bool Register(int id, ArgumentType type, const std::string &lName, const std::string &sName = std::string(), const std::string &info = "No description.");
		
		void SetToLower(bool value);
	private:
		ArgMap argMap;
		ArgExistMap argExistMap;
		bool tolower;
	};
}

#endif // KKE_ARGPARSER_HPP
