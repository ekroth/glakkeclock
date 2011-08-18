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