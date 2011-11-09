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
