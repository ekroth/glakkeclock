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

#include "GlakkeClock.hpp"
#include "kkeADL.hpp"
#include "Device.hpp"
#include "Debug/Logger.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace kke;

using std::cout;
using std::endl;
using std::string;

int GlakkeClock::Execute (int argc, char** argv)
{
#ifdef DEBUG
	cout << "GlakkeClock - ATI/AMD GPU Utility. Version: " << VERSION << endl;
	cout << "By Andrée 'Glaucous' Ekroth, 2011" << endl << endl;
#endif

	// After setting devices, we will have to clean afterwards.
	ArgParser::Instance().CaseSense(false); // Ignore capital letters
	if (!registerArgs())
	{
		LOGGROUP(Log_Error, "Main") << "Failed when registering arguments.";
	}
	else if (!ArgParser::Instance().Process(argc, argv))
	{
		LOGGROUP(Log_Error, "Main") << "Failed when processing arguments.";
	}
	else if (!ADLManager::Init())
	{
		LOGGROUP(Log_Error, "Main") << "Failed when initializing ADLManager.";
	}
	else
	{
		LOGGROUP(Log_Debug, "Main") << "Main initializating successful.";
		output();
		
		// Terminate ADL
		ADLManager::Terminate();
	}

	return 0;
}

void GlakkeClock::output()
{
	// Output debug
	if (ArgParser::Instance().Exist(kke::ArgDebug))
	{
		ADLManager::SetOutputErrors(true);
		Logger::SetLogLevel(Log_Debug);
	}
	else
	{
		ADLManager::SetOutputErrors(false);
		Logger::SetLogLevel(Log_Message);
	}
	
	if (ArgParser::Instance().Exist(kke::ArgColor))
		Logger::SetColorOutput(true);
	else
		Logger::SetColorOutput(false);
	
	kke::DeviceVector devices;
	Device::CreateDevices(devices);
	
	for (kke::DeviceVector::iterator it = devices.begin(); it != devices.end(); it++)
	{
		(*it)->DetectDisplays();
	}
	
	uint startDevice = 0, endDevice = devices.size();
	
	// --- Set device
	if (ArgParser::Instance().Exist(kke::ArgCdeviceUdid))
	{
		bool exists = false;
		for (uint i = 0; i < devices.size(); i++)
			if (devices[i]->GetUDID().find(ArgParser::Instance().GetString(kke::ArgCdeviceUdid)) != string::npos)
			{
				if (exists)
				{
					LOGGROUP(Log_Error, "Main") << "Multiple devices with same UDID! (This is odd.)";
					startDevice = 0; 
					endDevice = 0;
					break;
				}
				else
				{			
					exists = true;
					startDevice = i;
					endDevice = i + 1;
				}
			}
			
		if (!exists)
		{
			LOGGROUP(Log_Error, "Main") << "UDID doesn't exist.";
			startDevice = 0;
			endDevice = 0;
		}
	}
	else if (ArgParser::Instance().Exist(kke::ArgCdeviceIndex))
	{
		uint specIndex = ArgParser::Instance().GetInt(kke::ArgCdeviceIndex);
		if (specIndex >= devices.size() || specIndex < 0)
		{
			LOGGROUP(Log_Error, "Main") << "Device index out of range. Valid: " << '0' << '-' << (devices.size() - 1);
			startDevice = 0;
			endDevice = 0;
		}
		else
		{
			startDevice = specIndex;
			endDevice = specIndex + 1;
		}
	}
	else if (ArgParser::Instance().Exist(kke::ArgCdeviceName))
	{
		bool exists = false;
		for (uint i = 0; i < devices.size(); i++)
			if (devices[i]->GetName().find(ArgParser::Instance().GetString(kke::ArgCdeviceName)) != string::npos)
			{	// Use find, since AMD somehow felt like adding a space at the end of the name.
				if (exists)
				{
					LOGGROUP(Log_Error, "Main") << "Multiple devices with same name, use UDID.";
					startDevice = 0; 
					endDevice = 0;
					break;
				}
				else
				{			
					exists = true;
					startDevice = i;
					endDevice = i + 1;
				}
			}
			
		if (!exists)
		{
			LOGGROUP(Log_Error, "Main") << "Name doesn't exist.";
			startDevice = 0;
			endDevice = 0;
		}
	}
	
	// Run through devices
	for (uint i = startDevice; i < endDevice; i++)
	{
		Device &device = *devices[i];
		
		if (ArgParser::Instance().Exist(kke::ArgCpollAdaptIndex))
		{
			if (ArgParser::Instance().GetInt(ArgCpollAdaptIndex) < 0 || ArgParser::Instance().GetInt(ArgCpollAdaptIndex) >= (int)device.GetAdapters().size())
			{
				LOGGROUP(Log_Error, "Main") << "Polling adapter out of range. Valid: " << 0 << '-' << (device.GetAdapters().size() - 1);
				continue;
			}
			else
			{
				device.SetPollAdapter(ArgParser::Instance().GetInt(ArgCpollAdaptIndex));
				LOGGROUP(Log_Debug, "Main") << "Adapter index handle: " << device.GetAdapters()[device.GetPollAdapter()]->GetInfo().iAdapterIndex;
			}
		}
		
		// --- Big info
		if (ArgParser::Instance().Exist(kke::ArgHGinfo) || ArgParser::Instance().Exist(kke::ArgHGinfoLevels))
		{
			// Little fix to add line between multiple cards
			if (i != startDevice && startDevice != endDevice)
				cout << endl << endl;
			
			cout << "Name: " << device.GetName() << endl;
			cout << "UDID: " << device.GetUDID() << endl;
			cout << "Index: " << i << endl;
			cout << "Adapters: " << device.GetAdapters().size() << endl;
			
			if (device.PollBios().Valid)
			{
				cout << "Part: " << device.PollBios().Data.strPartNumber << endl;
				cout << "Version: " << device.PollBios().Data.strVersion << endl;
				cout << "Date: " << device.PollBios().Data.strDate << endl;
			}
			
			cout << endl;

			if (device.PollODParams().Valid)
			{
				if (device.PollActivity().Valid)
				{
					cout << "Current Performance Level: " << device.PollActivity().Data.iCurrentPerformanceLevel << " [0-" << device.PollODParams().Data.iNumberOfPerformanceLevels - 1 << ']' << endl;
					cout << "GPU (MHz): " << device.PollActivity().Data.iEngineClock / 100 << " [" << device.PollODParams().Data.sEngineClock.iMin / 100 << '-' << device.PollODParams().Data.sEngineClock.iMax / 100 << ']' << endl;
					cout << "Memory (MHz): " << device.PollActivity().Data.iMemoryClock / 100 << " [" << device.PollODParams().Data.sMemoryClock.iMin / 100  << '-' << device.PollODParams().Data.sMemoryClock.iMax / 100 << ']' << endl;
					cout << "VDDC Volt (mV): " << device.PollActivity().Data.iVddc << " [" << device.PollODParams().Data.sVddc.iMin <<  '-' << device.PollODParams().Data.sVddc.iMax << ']' << endl;
				}
			}

			if (device.PollTemperature().Valid)
				cout << "Temperature (C): " << device.PollTemperature().Data / 1000 << endl;

			if (device.PollFanSpeed().Valid && device.PollFanInfo().Valid)
			{
				if (device.PollFanSpeed().Data.iSpeedType == ADL_DL_FANCTRL_SPEED_TYPE_RPM)
				{
					cout << "Fan type: RPM" << endl;
					cout << "Fan speed: " << device.PollFanSpeed().Data.iFanSpeed << " [" << device.PollFanInfo().Data.iMinRPM << '-' << device.PollFanInfo().Data.iMaxRPM << ']' << endl;
				}
				else
				{
					cout << "Fan type: Percent" << endl;
					cout << "Fan speed: " << device.PollFanSpeed().Data.iFanSpeed << " [" << device.PollFanInfo().Data.iMinPercent << '-' << device.PollFanInfo().Data.iMaxPercent << ']' << endl;
				}
			}
			
			if (device.PollActivity().Valid)
			{
				cout << "GPU Activity (%): " << device.PollActivity().Data.iActivityPercent << endl;
				cout << "Current Bus Lanes: " <<  device.PollActivity().Data.iCurrentBusLanes << "/" << device.PollActivity().Data.iMaximumBusLanes << endl;
				cout << "Current Bus Speed: " <<  device.PollActivity().Data.iCurrentBusSpeed << endl;
			}
			
			if (ArgParser::Instance().Exist(kke::ArgHGinfoLevels))
			{
				if (device.PollODParams().Valid && device.PollPerfLvls(false).Valid)
				{
					cout << endl;
					
					for (int v = 0; v < device.PollODParams().Data.iNumberOfPerformanceLevels; v++)
					{
						cout << "Performance Level: " << v << endl;
						cout << "GPU (MHz): " << 	device.PollPerfLvls(false).Data[v].iEngineClock / 100 << " [" << device.PollODParams().Data.sEngineClock.iMin / 100 << '-' << device.PollODParams().Data.sEngineClock.iMax / 100 << ']' << endl;
						cout << "Memory (MHz): " << device.PollPerfLvls(false).Data[v].iMemoryClock / 100 << " [" << device.PollODParams().Data.sMemoryClock.iMin / 100  << '-' << device.PollODParams().Data.sMemoryClock.iMax / 100 << ']' << endl;
						cout << "Vddc (mV): " << 	device.PollPerfLvls(false).Data[v].iVddc << " [" << device.PollODParams().Data.sVddc.iMin << '-' << device.PollODParams().Data.sVddc.iMax << ']' << endl;

						if (v != device.PollODParams().Data.iNumberOfPerformanceLevels - 1)
							cout << endl;
					}
				}
			}
		}
		
		// Device info
		if (ArgParser::Instance().Exist(kke::ArgHGname))
			cout << device.GetName() << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgHGudid))
			cout << device.GetUDID() << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgHGindex))
			cout << i << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgHGpart))
			cout << device.PollBios().Data.strPartNumber << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgHGversion))
			cout << device.PollBios().Data.strVersion << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgHGdate))
			cout << device.PollBios().Data.strDate << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgHGadapters))
			cout << device.GetAdapters().size() << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgHGbusLanes))
			if (device.PollActivity().Valid)
				cout << device.PollActivity().Data.iCurrentBusLanes << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgHGbusLanesMax))
			if (device.PollActivity().Valid)
				cout << device.PollActivity().Data.iMaximumBusLanes << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgHGbusSpeed))
			if (device.PollActivity().Valid)
				cout << device.PollActivity().Data.iCurrentBusSpeed << endl;
		
		// Overdrive
		if (ArgParser::Instance().Exist(kke::ArgOGperfLevel))
			if (device.PollActivity().Valid)
				cout << device.PollActivity().Data.iCurrentPerformanceLevel << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgOGtemperature) && device.PollTemperature().Valid)
			cout << device.PollTemperature().Data / 1000 << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgOGactivity) && device.PollActivity().Valid)
			cout << device.PollActivity().Data.iActivityPercent << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgOGclocksGpu) && device.PollActivity().Valid)
			cout << device.PollActivity().Data.iEngineClock / 100 << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgOGclocksMem) && device.PollActivity().Valid)
			cout << device.PollActivity().Data.iMemoryClock / 100 << endl;
		
		if (ArgParser::Instance().Exist(kke::ArgOGclocksVddc) && device.PollActivity().Valid)
			cout << device.PollActivity().Data.iVddc << endl;
		
		// Fan
		if (ArgParser::Instance().Exist(kke::ArgOGfanType))
		{
			if (device.PollFanSpeed().Valid && device.PollFanInfo().Valid)
			{
				if (device.PollFanSpeed().Data.iSpeedType == ADL_DL_FANCTRL_SPEED_TYPE_RPM)
					cout << "RPM" << endl;
				else
					cout << "Percent" << endl;
			}
		}
		
		if (ArgParser::Instance().Exist(kke::ArgOGfan))
		{
			if (device.PollFanSpeed().Valid && device.PollFanInfo().Valid)
			{
				if (ArgParser::Instance().GetString(kke::ArgOCfanType, "Percent") == "RPM")
					cout << device.PollFanSpeed().Data.iFanSpeed << endl;
				else // Calculate percentage
					cout << (int)((float)device.PollFanSpeed().Data.iFanSpeed / device.PollFanInfo().Data.iMaxRPM * 100.0f) << endl;
			}
		}
		
		if (ArgParser::Instance().Exist(kke::ArgOGfanMin))
		{
			if (device.PollFanSpeed().Valid && device.PollFanInfo().Valid)
			{
				if (ArgParser::Instance().GetString(kke::ArgOCfanType, "Percent") == "RPM")
					cout << device.PollFanInfo().Data.iMinRPM << endl;
				else
					cout << device.PollFanInfo().Data.iMinPercent << endl;
			}
		}
		
		if (ArgParser::Instance().Exist(kke::ArgOGfanMax))
		{
			if (device.PollFanSpeed().Valid && device.PollFanInfo().Valid)
			{
				if (ArgParser::Instance().GetString(kke::ArgOCfanType, "Percent") == "RPM")
					cout << device.PollFanInfo().Data.iMaxRPM << endl;
				else
					cout << device.PollFanInfo().Data.iMaxPercent << endl;
			}
		}

		
		// --- Overclocking
		if (ArgParser::Instance().Exist(kke::ArgOSclocksGpu) || ArgParser::Instance().Exist(kke::ArgOSclocksMem) || ArgParser::Instance().Exist(kke::ArgOSclocksVddc) 
		|| ArgParser::Instance().Exist(kke::ArgOSfan) || ArgParser::Instance().Exist(kke::ArgOSfanReset)
		|| ArgParser::Instance().Exist(kke::ArgOSclocksGpuReset) || ArgParser::Instance().Exist(kke::ArgOSclocksMemReset) || ArgParser::Instance().Exist(kke::ArgOSclocksVddcReset)
		|| ArgParser::Instance().Exist(kke::ArgOSclocksReset))
		{
			if (startDevice != endDevice - 1)
			{
				LOGGROUP(Log_Error, "Main") << "You should probably not set clocks/fan on ALL devices, see -h, --help.";
				LOGGROUP(Log_Debug, "Main") << "TODO: Be able to bypass this?";
				continue;
			}
			
			{
				int tmpGpu = ArgParser::Instance().GetInt(kke::ArgOSclocksGpu, 0) * 100, 
					tmpMem = ArgParser::Instance().GetInt(kke::ArgOSclocksMem, 0) * 100, 
					tmpVddc = ArgParser::Instance().GetInt(kke::ArgOSclocksVddc, 0);
					
				if (device.PollODParams().Valid)
				{
					if (tmpGpu != 0 && (tmpGpu < device.PollODParams().Data.sEngineClock.iMin || tmpGpu > device.PollODParams().Data.sEngineClock.iMax))
					{
						LOGGROUP(Log_Error, "Main") << "GPU speed not within range.";
						tmpGpu = 0;
					}
					
					if (tmpMem != 0 &&(tmpMem < device.PollODParams().Data.sMemoryClock.iMin || tmpMem > device.PollODParams().Data.sMemoryClock.iMax))
					{
						LOGGROUP(Log_Error, "Main") << "Memory speed not within range.";
						tmpMem = 0;
					}
					
					if (tmpVddc != 0 && (tmpVddc < device.PollODParams().Data.sVddc.iMin || tmpVddc > device.PollODParams().Data.sVddc.iMax))
					{
						LOGGROUP(Log_Error, "Main") << "VDDC not within range.";
						tmpVddc = 0;
					}
					
					if (tmpGpu != 0 || tmpMem != 0 || tmpVddc != 0)
						if (!device.ODSetAllLevels(tmpGpu, tmpMem, tmpVddc))
						{
							LOGGROUP(Log_Error, "Main") << "Error when setting clocks/vddc.";
						}
				}

				if (ArgParser::Instance().Exist(kke::ArgOSclocksReset)  || ArgParser::Instance().Exist(kke::ArgOSallReset))
				{
					if (device.PollPerfLvls(true).Valid)
					{
						if (!device.ODSetLevels(device.PollPerfLvls(true).Data))
						{
							LOGGROUP(Log_Error, "Main") << "Error when setting performance levels.";
						}
					}
				}
			}
		}

		// --- Fan
		if (ArgParser::Instance().Exist(kke::ArgOSfan))
		{
			if (device.PollFanInfo().Valid && device.PollFanSpeed().Valid)
			{
				if (ArgParser::Instance().GetString(kke::ArgOCfanType, "Percent") == "Percent")
				{
					if (ArgParser::Instance().GetInt(kke::ArgOSfan) < device.PollFanInfo().Data.iMinPercent || ArgParser::Instance().GetInt(kke::ArgOSfan) > device.PollFanInfo().Data.iMaxPercent)
					{
						LOGGROUP(Log_Error, "Main") << "Fan value is incorrect. Valid are (PERCENT): " << device.PollFanInfo().Data.iMinPercent << "-" << device.PollFanInfo().Data.iMaxPercent;
					}
					else
					{
						if (!device.ODSetFan(ArgParser::Instance().GetInt(kke::ArgOSfan), ADL_DL_FANCTRL_SPEED_TYPE_PERCENT))
						{
							LOGGROUP(Log_Error, "Main") << "Setting fan speed failed (PERCENT)." << endl;
						}
					}
				}
				else // RPM
				{
					if (ArgParser::Instance().GetInt(kke::ArgOSfan) < device.PollFanInfo().Data.iMinRPM || ArgParser::Instance().GetInt(kke::ArgOSfan) > device.PollFanInfo().Data.iMaxRPM)
					{
						LOGGROUP(Log_Error, "Main") << "Fan value is invalid. Valid are (RPM): " << device.PollFanInfo().Data.iMinRPM << "-" << device.PollFanInfo().Data.iMaxRPM;
					}
					else
					{
						if (!device.ODSetFan(ArgParser::Instance().GetInt(kke::ArgOSfan), ADL_DL_FANCTRL_SPEED_TYPE_RPM))
						{
							LOGGROUP(Log_Error, "Main") << "Setting fan speed failed (RPM).";
						}
					}
				}
			}
		}
	
		if (ArgParser::Instance().Exist(kke::ArgOSfanReset) || ArgParser::Instance().Exist(kke::ArgOSallReset))
		{
			// Set default fan
			if (!device.ODSetFanDefault())
			{
				LOGGROUP(Log_Error, "Main") << "Error when setting default fan speed." << endl;
			}
		}
		
		// Display
	
		if (ArgParser::Instance().Exist(kke::ArgDGConDisplays) || ArgParser::Instance().Exist(kke::ArgDGMapDisplays) || ArgParser::Instance().Exist(kke::ArgDGAllDisplays))
		{
			for (kke::DisplayVector::iterator disp = device.GetDisplays().begin(); disp != device.GetDisplays().end(); disp++)
			{			
				if (ArgParser::Instance().Exist(kke::ArgDGAllDisplays) || (disp->IsConnected() && ArgParser::Instance().Exist(kke::ArgDGConDisplays)) ||
					(disp->IsMapped() && ArgParser::Instance().Exist(kke::ArgDGMapDisplays)))
				{					
					cout << "Name: " << disp->GetInfo().strDisplayName << endl;
					cout << "Manufacturer: " << disp->GetInfo().strDisplayManufacturerName << endl;
					
					cout << "Connected: " << (disp->IsConnected() ? "Yes" : "No") << endl;
					cout << "Mapped: " << (disp->IsMapped() ? "Yes" : "No") << endl;
	// 					cout << "Connector: " << disp->GetInfo().
				}
			}
		}
	}
	
	// --- Help
	
	if (ArgParser::Instance().Exist(kke::ArgHelp))
	{
		// TODO: Add synopsis/examples
		const int col1 = 1, col2 = 7, col3 = 25;
		
// 		alignArg("- SYNOPSIS", "", "", col1, col2, col3);
		
		
		for (int i = 0; ArgParser::Instance().Registered(i); i++)
		{

			alignArg("-" + ArgParser::Instance().GetsName(i), "--" + ArgParser::Instance().GetlName(i), ArgParser::Instance().GetInfo(i), col1, col2, col3);
			
			switch (i + 1)
			{
				case kke::ArgCdeviceName:
					cout << endl;
					alignArg("- Device options", "", "", col1, col2, col3);
					break;
					
				case kke::ArgHGinfo:
					cout << endl;
					alignArg("- Hardware", "", "", col1, col2, col3);
					break;
					
				case kke::ArgOGperfLevel:
					cout << endl;
					alignArg("- Speed/Clock/Temp", "", "", col1, col2, col3);
					break;
					
				case kke::ArgOCfanType:
					cout << endl;
					alignArg("- Fan", "", "", col1, col2, col3);
					break;
					
				case kke::ArgHGbusLanes:
					cout << endl;
					alignArg("- Bus", "", "", col1, col2, col3);
					break;
					
				case kke::ArgOSclocksGpu:
					cout << endl;
					alignArg("- Overclocking", "", "", col1, col2, col3);
					break;
					
				case kke::ArgOSfanReset:
					cout << endl;
					alignArg("- Reset clocks/fan to driver defaults", "", "", col1, col2, col3);
					break;
					
				case kke::ArgDGConDisplays:
					cout << endl;
					alignArg("- Display options. Work in progress.", "", "", col1, col2, col3);
					break;
			}
		}
	}
	
	if (ArgParser::Instance().Exist(kke::ArgVersion))
	{
		cout << VERSION << endl;
	}
	
	for (kke::DeviceVector::iterator it = devices.begin(); it != devices.end(); it++)
		delete *it;
}

bool GlakkeClock::registerArgs()
{
	bool good = true;

	// Program
	good = good && ArgParser::Instance().Register (kke::ArgHelp, kke::ArgumentExist, "help", "h", "Help dialog.");
	good = good && ArgParser::Instance().Register (kke::ArgVersion, kke::ArgumentExist, "version", "v", "Version.");
	good = good && ArgParser::Instance().Register (kke::ArgDebug, kke::ArgumentExist, "debug", "d", "Output debug messages.");
	good = good && ArgParser::Instance().Register (kke::ArgColor, kke::ArgumentExist, "color", "c", "Output colored messages.");

	// Device options
	good = good && ArgParser::Instance().Register (kke::ArgCdeviceName, kke::ArgumentString, "device-name", "Cdn", "Choose device by name. (Not for Crossfire)");
	good = good && ArgParser::Instance().Register (kke::ArgCdeviceUdid, kke::ArgumentString, "device-udid", "Cdu", "Choose device by UDID.");
	good = good && ArgParser::Instance().Register (kke::ArgCdeviceIndex, kke::ArgumentInt, "device-index", "Cdi", "Choose device by index.");
	good = good && ArgParser::Instance().Register (kke::ArgCperfLevel, kke::ArgumentInt, "perf-level", "Cpl", "Choose performance level. (WIP, for poll Hz.)");
	good = good && ArgParser::Instance().Register (kke::ArgCpollAdaptIndex, kke::ArgumentInt, "poll-adapter", "Cai", "Choose polling adapter index.");

	// Hardware info
	good = good && ArgParser::Instance().Register (kke::ArgHGinfo, kke::ArgumentExist, "get-info", "HGi", "Device information.");
	good = good && ArgParser::Instance().Register (kke::ArgHGinfoLevels, kke::ArgumentExist, "get-info-levels", "HGil", "Extended device information.");
	good = good && ArgParser::Instance().Register (kke::ArgHGname, kke::ArgumentExist, "get-name", "HGn", "Device name.");
	good = good && ArgParser::Instance().Register (kke::ArgHGudid, kke::ArgumentExist, "get-udid", "HGu", "Device UDID.");
	good = good && ArgParser::Instance().Register (kke::ArgHGindex, kke::ArgumentExist, "get-index", "HGin", "Device index.");
	good = good && ArgParser::Instance().Register (kke::ArgHGpart, kke::ArgumentExist, "get-part", "HGp", "Device part number.");
	good = good && ArgParser::Instance().Register (kke::ArgHGversion, kke::ArgumentExist, "get-version", "HGv", "Device version.");
	good = good && ArgParser::Instance().Register (kke::ArgHGdate, kke::ArgumentExist, "get-date", "HGd", "Device date.");
	good = good && ArgParser::Instance().Register (kke::ArgHGadapters, kke::ArgumentExist, "get-adapters", "HGa", "Amount of device adapters.");
	good = good && ArgParser::Instance().Register (kke::ArgHGbusLanes, kke::ArgumentExist, "get-bus-lanes", "HGbl", "PCI-E bus lanes.");
	good = good && ArgParser::Instance().Register (kke::ArgHGbusLanesMax, kke::ArgumentExist, "get-bus-lanes-max", "HGblm", "Max PCI-E bus lanes.");
	good = good && ArgParser::Instance().Register (kke::ArgHGbusSpeed, kke::ArgumentExist, "get-bus-speed", "HGbs", "PCI-E bus speed.");

	// Device varying info
	good = good && ArgParser::Instance().Register (kke::ArgOGperfLevel, kke::ArgumentExist, "get-perf-level", "OGpl", "Performance level.");
	good = good && ArgParser::Instance().Register (kke::ArgOGclocksGpu, kke::ArgumentExist, "get-clocks-gpu", "OGcg", "GPU speed (MHz).");
	good = good && ArgParser::Instance().Register (kke::ArgOGclocksMem, kke::ArgumentExist, "get-clocks-mem", "OGcm", "Memory speed (MHz).");
	good = good && ArgParser::Instance().Register (kke::ArgOGclocksVddc, kke::ArgumentExist, "get-clocks-vddc", "OGcv", "VDDC voltage (mV).");
	good = good && ArgParser::Instance().Register (kke::ArgOGactivity, kke::ArgumentExist, "get-gpu-activity", "OGga", "GPU activity (%).");
	good = good && ArgParser::Instance().Register (kke::ArgOGtemperature, kke::ArgumentExist, "get-gpu-temp", "OGgt", "GPU temperature (C).");

	// Device fan
	good = good && ArgParser::Instance().Register (kke::ArgOCfanType, kke::ArgumentString, "fan-type", "OCft", "Choose fan type (RPM, Percent (default)).");
	good = good && ArgParser::Instance().Register (kke::ArgOGfanType, kke::ArgumentExist, "get-fan-type", "OGft", "Fan type (RPM, percent).");
	good = good && ArgParser::Instance().Register (kke::ArgOGfan, kke::ArgumentExist, "get-fan", "OGf", "Fan speed.");
	good = good && ArgParser::Instance().Register (kke::ArgOGfanMin, kke::ArgumentExist, "get-fan-min", "OGfmi", "Minimum fan speed.");
	good = good && ArgParser::Instance().Register (kke::ArgOGfanMax, kke::ArgumentExist, "get-fan-max", "OGfma", "Maximum fan speed.");
	good = good && ArgParser::Instance().Register (kke::ArgOSfan, kke::ArgumentInt, "set-fan", "OSf", "Set fan speed, recommend Percent (default) type.");

	// Set device values. (capital S to avoid typo).
	good = good && ArgParser::Instance().Register (kke::ArgOSclocksGpu, kke::ArgumentInt, "set-clocks-gpu", "OScg", "Set GPU speed (MHz).");
	good = good && ArgParser::Instance().Register (kke::ArgOSclocksMem, kke::ArgumentInt, "set-clocks-mem", "OScm", "Set Memory speed (MHz).");
	good = good && ArgParser::Instance().Register (kke::ArgOSclocksVddc, kke::ArgumentInt, "set-clocks-vddc", "OScv", "Set VDDC voltage (mV).");
	// Set reset
	good = good && ArgParser::Instance().Register (kke::ArgOSfanReset, kke::ArgumentExist, "set-fan-reset", "OSfr", "Reset fan speed.");
	good = good && ArgParser::Instance().Register (kke::ArgOSclocksGpuReset, kke::ArgumentExist, "set-clocks-gpu-reset", "OScgr", "Reset GPU clocks.");
	good = good && ArgParser::Instance().Register (kke::ArgOSclocksMemReset, kke::ArgumentExist, "set-clocks-mem-reset", "OScmr", "Reset Memory clocks.");
	good = good && ArgParser::Instance().Register (kke::ArgOSclocksVddcReset, kke::ArgumentExist, "set-clocks-vddc-reset", "OScvr", "Reset VDDC voltage.");
	good = good && ArgParser::Instance().Register (kke::ArgOSclocksReset, kke::ArgumentExist, "set-clocks-reset", "OScr", "Reset all clocks.");
	good = good && ArgParser::Instance().Register (kke::ArgOSallReset, kke::ArgumentExist, "set-all-reset", "OSar", "Reset clocks and fan.");
	
	// Display	
// 	good = good && ArgParser::Instance().Register(kke::ArgDGPixelFormat, kke::ArgumentExist, "get-pixel-format", "DGpf", "Pixel format (HDMI).");
// 	good = good && ArgParser::Instance().Register(kke::ArgDSPixelFormat, kke::ArgumentString, "set-pixel-format", "DSpf", "Pixel format (HDMI).");
	good = good && ArgParser::Instance().Register(kke::ArgDGConDisplays, kke::ArgumentExist, "get-con-displays", "DGcd", "Show connected displays.");
	good = good && ArgParser::Instance().Register(kke::ArgDGMapDisplays, kke::ArgumentExist, "get-map-displays", "DGmd", "Show used/mapped displays.");
	good = good && ArgParser::Instance().Register(kke::ArgDGAllDisplays, kke::ArgumentExist, "get-all-displays", "DGad", "Show all displays.");

	return good;
}

void GlakkeClock::alignArg (const string &first, const string &second, const string& third, uint col1, uint col2, uint col3) const
{
	for (uint i = 0; i < col1; i++)
		cout << " ";

	cout << first;

	if (second.length() > 0)
		for (uint i = 0; i < col2 - first.length(); i++)
			cout << " ";

	cout << second;
	
	if (third.length() > 0)
		for (uint i = 0; i < col3 - second.length(); i++)
			cout << " ";
	
	cout << third << endl;
}