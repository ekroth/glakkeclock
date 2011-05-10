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

#include "IO/ArgParser.hpp"

#include <ADL/adl_sdk.h>

#include <vector>
#include <string>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

namespace kke
{
	enum Args
	{
		ArgHelp,
		ArgVersion,
		ArgDebug,
		
		ArgCdeviceName,
		ArgCdeviceUdid,
		ArgCdeviceIndex,
// 		ArgCadapter,
		ArgCperfLevel,
		
		ArgHGinfo,
		ArgHGinfoLevels,
		ArgHGname,
		ArgHGudid,
		ArgHGindex,
		ArgHGpart,
		ArgHGversion,
		ArgHGdate,
		ArgHGadapters,
		ArgHGbusLanes,
		ArgHGbusLanesMax,
		ArgHGbusSpeed,
		
		ArgOGperfLevel,
		ArgOGclocksGpu,
		ArgOGclocksMem,
		ArgOGclocksVddc,
		ArgOGactivity,
		ArgOGtemperature,
		
		ArgOCfanType,
		ArgOGfanType,
		ArgOGfan,
		ArgOGfanMin,
		ArgOGfanMax,
		ArgOSfan,
		
		ArgOSclocksGpu,
		ArgOSclocksMem,
		ArgOSclocksVddc,
		
		ArgOSfanReset,
		ArgOSclocksGpuReset,
		ArgOSclocksMemReset,
		ArgOSclocksVddcReset,
		ArgOSclocksReset,
		ArgOSallReset,
		
// 		ArgCdisplay,
// 		ArgDGPixelFormat,
// 		ArgDSPixelFormat,
		ArgDGConDisplays,
		ArgDGMapDisplays,
		ArgDGAllDisplays
	};

	class GlakkeClock
	{
	public:
		// Main function
		int Execute(int argc, char** argv);
		
	private:
		// set devices and get information
		bool registerArgs();
		
		void output();
		
		void alignArg(const std::string &first, const std::string &second, const std::string& third, uint col1, uint col2, uint col3) const;
	};
}
