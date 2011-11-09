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

#include <vector>
#include <string>

namespace kke
{
	enum Args
	{
		ArgHelp,
		ArgVersion,
		ArgDebug,
		ArgColor,
		
		ArgCdeviceName,
		ArgCdeviceUdid,
		ArgCdeviceIndex,
		ArgCperfLevel,
		ArgCpollAdaptIndex,
		ArgCpollThermIndex,
		ArgCperfValue,
		ArgCperfDefValue,
		ArgCallPerfLevels,
		ArgBypass,
		ArgAllCards,
		ArgPollMin,
		ArgPollMax,
		
		ArgHGdevices,
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
		ArgOGfan,
		ArgOSfan,
		
		ArgOSclockSmooth,
		ArgOSclocksGpu,
		ArgOSclocksMem,
		ArgOSclocksVddc,
		
		ArgOSfanReset,
		ArgOSclocksGpuReset,
		ArgOSclocksMemReset,
		ArgOSclocksVddcReset,
		ArgOSPerfReset,
		ArgOSPerfAllReset,
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
		void argCheck();
		
		void alignArg(const std::string &first, const std::string &second, const std::string& third, unsigned int col1, unsigned int col2, unsigned int col3) const;
	};
}
