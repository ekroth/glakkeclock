/*
 This file is part of GlakkeGame.

 GlakkeGame, a 2D game.
 Copyright (C) 2010-2011 Andrée Ekroth

 GlakkeGame is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 GlakkeGame is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GlakkeGame.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KKE_LOGGER_HPP
#define KKE_LOGGER_HPP

#include <sstream>

#ifndef LOGGER_MAX_LEVEL
#define LOGGER_MAX_LEVEL kke::Log_Debug
#endif
#define LOGGER(level) \
  if (level > LOGGER_MAX_LEVEL) ;\
  else if (level > kke::Logger::GetLogLevel()) ; \
   else kke::Logger().Stream(level)
	   
#define LOGGROUP(level, name) LOGGER(level) << '<' << name << "> "

namespace kke
{

	enum LogLevel
	{
		Log_Error = 0,
		Log_Warning = 1,
		Log_Message = 2,
		Log_Debug = 3,
		Log_Debug2 = 4,
		Log_Debug3 = 5
	};

	class Logger
	{
	public:
		Logger();
		~Logger();
		
		std::ostringstream& Stream(LogLevel level);
		
	public:	
		static int GetLogLevel();
		static void SetLogLevel(LogLevel level);
		static void SetColorOutput(bool value);
		
	private:
		static int logLevel;
		static FILE* output;
		
	#ifdef LINUX
		static bool color;
	#endif
		
	private:
		LogLevel messageLevel;
		std::ostringstream stream;
	};
}

#endif // KKE_LOGGER_HPP