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

#include "Debug/Logger.hpp"

#include <stdio.h>

using kke::Logger;

using std::endl;

int Logger::logLevel = 0;
FILE *Logger::output = stdout;

#ifdef LINUX
bool Logger::color = false;

#define TERMINAL_BLACK "\033[22;30m"
#define TERMINAL_RED "\033[22;31m"
#define TERMINAL_GREEN "\033[22;31m"
#define TERMINAL_BLUE "\033[22;34m"
#define TERMINAL_GREY "\033[22;37m"
#define TERMINAL_YELLOW "\033[01;33m"
#define TERMINAL_WHITE "\033[01;37m"
#define TERMINAL_LIGHT_RED "\033[01;31m"

#endif

Logger::Logger() : messageLevel (kke::Log_Message)
{

}

Logger::~Logger()
{
	stream << endl;
#ifdef LINUX
	if (color)
		stream << TERMINAL_GREY;
#endif
	fprintf (output, "%s", stream.str().c_str());
}

std::ostringstream& Logger::Stream (kke::LogLevel level)
{
#ifdef LINUX
	// Write some information about the message
	if (color)
	{
		switch (level)
		{
			case kke::Log_Error:
				stream << TERMINAL_LIGHT_RED;
				break;

			case kke::Log_Warning:
				stream << TERMINAL_YELLOW;
				break;
				
			case kke::Log_Message:
				stream << TERMINAL_WHITE;
				break;

			default:
				stream << TERMINAL_GREY;
				break;
		}
	}

#endif

	switch (level)
	{
		case kke::Log_Error:
			stream << "[ERROR] ";
			break;

		case kke::Log_Warning:
			stream << "[WARNING] ";
			break;
			
		case kke::Log_Message:
			stream << "[MESSAGE] ";
			break;

		default:
			stream << "[DEBUG] ";
			break;	
	}

// 	stream << '[' << level << "] ";
	messageLevel = level;
	return stream;
}

int Logger::GetLogLevel()
{
	return logLevel;
}

void Logger::SetLogLevel (kke::LogLevel level)
{
	logLevel = level;
}

void Logger::SetColorOutput (bool value)
{
#ifdef LINUX
	color = value;
#endif
}
