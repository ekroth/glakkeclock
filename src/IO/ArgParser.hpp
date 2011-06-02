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

#ifndef KKE_ARGPARSER_HPP
#define KKE_ARGPARSER_HPP

#include "IO/Argument.hpp"
#include "Interface/Noncopyable.hpp"

#include <string>
#include <map>

namespace kke
{
	/**
	 * @brief Registers arguments to an integer, and parses them.
	 **/
	class ArgParser : public Noncopyable
	{
	public:
		static ArgParser& Instance();
		static void Terminate();
		
	public:
		
		/**
		 * @brief Process the arguments.
		 * @return bool
		 **/
		bool Process(int argc, char **argv);
		
		/**
		 * @brief Read integer from argument.
		 *
		 * @param arg argument index
		 * @param def Default value if argument not available. Defaults to 0.
		 * @return int
		 **/
		int GetInt(int arg, int def = 0);
		
		/**
		 * @brief Read string from argument.
		 *
		 * @param arg argument index
		 * @param def Default value if argument not available. Defaults to 0.
		 * @return int
		 **/
		std::string GetString(int arg, const std::string &def = std::string());
		
		/**
		 * @brief Get long name of argument. (--NAME)
		 * @return :string
		 **/
		std::string GetlName(int arg);
		
		/**
		 * @brief Get short name of argument. (-NAME)
		 * @return :string
		 **/
		std::string GetsName(int arg);
		
		/**
		 * @brief Get argument information. (--NAME)
		 * @return :string
		 **/
		std::string GetInfo(int arg);
		
		/**
		 * @brief If argument has been registered.
		 * @return bool
		 **/
		bool Registered(int arg);
		
		/**
		 * @brief If registered argument has been parsed.
		 * @return bool
		 **/
		bool Exist(int arg);
		
		
		/**
		 * @brief Return argument index.
		 * @return int
		 **/
		int ValidArg(const std::string &arg) const;
		
		/**
		 * @brief Register a argument.
		 *
		 * @param id Unique argument ID.
		 * @param type Type of argument.
		 * @param lName Long name of argument. (--<lName>)
		 * @param sName Short name of argument. (-<sName>) Defaults to std::string().
		 * @param info Description of argument. Defaults to "No description.".
		 * @return bool
		 **/
		bool Register(int id, ArgumentType type, const std::string &lName, const std::string &sName = std::string(), const std::string &info = "No description.");
		
		void CaseSense(bool value);
	private:
		static ArgParser *instance;
		
	private:
		
		ArgParser();
		~ArgParser();
		
		typedef std::map<int, Argument> ArgMap;
		typedef std::map<int, bool> ArgExistMap;
	
		ArgMap argMap;
		ArgExistMap argExistMap;
		bool caseSense;
	};
}

#endif // KKE_ARGPARSER_HPP
