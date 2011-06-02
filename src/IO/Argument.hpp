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

#ifndef KKE_ARGUMENT_HPP
#define KKE_ARGUMENT_HPP

#include <string>

namespace kke
{
	enum ArgumentType
	{
		ArgumentExist,
		ArgumentString,
		ArgumentInt
	};

	class Argument
	{
	public:
		Argument();
		Argument(ArgumentType type, const std::string &lName, const std::string &sName, const std::string &info);
		
		bool IsArg(const std::string &arg, bool caseSense = true) const;
		const std::string& GetLongName() const;
		const std::string& GetShortName() const;
		const std::string& GetString() const;
		const std::string& GetInfo() const;
		int GetInt() const;
		ArgumentType GetType() const;
		void SetSvalue(const std::string &value);
		void SetIvalue(int value);
		void SetInfo(const std::string &info);
		
		static std::string StringToLow(const std::string& var);

	private:
		std::string 
			lName,
			sName,
			info,
			sValue;
		int iValue;
		
		ArgumentType type;
	};
}

#endif // KKE_ARGUMENT_HPP