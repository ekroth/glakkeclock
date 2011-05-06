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

#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
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

	// Initialize ADL
	if (!ADLManager::Init())
		return 0;
	
	// After setting devices, we will have to clean afterwards.
	argParser.SetToLower(true); // Ignore capital letters
	if (!registerArgs())
	{
		cout << "Failed when registering arguments." << endl;
	}
	else if (!argParser.Process(argc, argv))
	{
		cout << "Failed when parsing arguments." << endl;
	}
	else
	{
		output();
	}

	// Terminate ADL
	ADLManager::Terminate();

	return 0;
}

void GlakkeClock::output()
{
	kke::DeviceVector devices;
	Device::CreateDevices(devices);
	
	for (kke::DeviceVector::iterator it = devices.begin(); it != devices.end(); it++)
	{
		(*it)->DetectDisplays();
	}
	
	uint startDevice = 0, endDevice = devices.size();
	
	// --- Set device
	if (argParser.Exist(kke::ArgCdeviceUdid))
	{
		bool exists = false;
		for (uint i = 0; i < devices.size(); i++)
			if (devices[i]->GetUDID().find(argParser.GetString(kke::ArgCdeviceUdid)) != string::npos)
			{
				if (exists)
				{
					cout << "Multiple devices with same UDID! (This is odd.)" << endl;
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
			cout << "UDID doesn't exist." << endl;
			startDevice = 0;
			endDevice = 0;
		}
	}
	else if (argParser.Exist(kke::ArgCdeviceIndex))
	{
		uint specIndex = argParser.GetInt(kke::ArgCdeviceIndex);
		if (specIndex >= devices.size())
		{
			cout << "Index out of range. Valid: " << '0' << '-' << (devices.size() - 1) << endl;
			startDevice = 0;
			endDevice = 0;
		}
		else
		{
			startDevice = specIndex;
			endDevice = specIndex + 1;
		}
	}
	else if (argParser.Exist(kke::ArgCdeviceName))
	{
		bool exists = false;
		for (uint i = 0; i < devices.size(); i++)
			if (devices[i]->GetName().find(argParser.GetString(kke::ArgCdeviceName)) != string::npos)
			{	// Use find, since AMD somehow felt like adding a space at the end of the name.
				if (exists)
				{
					cout << "Multiple devices with same name, use UDID." << endl;
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
			cout << "Name doesn't exist." << endl;
			startDevice = 0;
			endDevice = 0;
		}
	}
	
	// Run through devices
	for (uint i = startDevice; i < endDevice; i++)
	{
		Device &device = *devices[i];
		
		// --- Big info
		if (argParser.Exist(kke::ArgHGinfo) || argParser.Exist(kke::ArgHGinfoLevels))
		{
			cout << "Name: " << device.GetName() << endl;
			cout << "UDID: " << device.GetUDID() << endl;
			cout << "Index: " << i << endl;
			
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
			
			if (argParser.Exist(kke::ArgHGinfoLevels))
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
		
		if (argParser.Exist(kke::ArgHGname))
			cout << device.GetName() << endl;
		
		if (argParser.Exist(kke::ArgHGudid))
			cout << device.GetUDID() << endl;
		
		if (argParser.Exist(kke::ArgHGindex))
			cout << i << endl;
		
		if (argParser.Exist(kke::ArgHGadapters))
			cout << device.GetAdapters().size() << endl;
		
		
		if (argParser.Exist(kke::ArgHGbusLanes))
			if (device.PollActivity().Valid)
				cout << device.PollActivity().Data.iCurrentBusLanes << endl;
		
		if (argParser.Exist(kke::ArgHGbusLanesMax))
			if (device.PollActivity().Valid)
				cout << device.PollActivity().Data.iMaximumBusLanes << endl;
		
		if (argParser.Exist(kke::ArgHGbusSpeed))
			if (device.PollActivity().Valid)
				cout << device.PollActivity().Data.iCurrentBusSpeed << endl;
		
		if (argParser.Exist(kke::ArgOGperfLevel))
			if (device.PollActivity().Valid)
				cout << device.PollActivity().Data.iCurrentPerformanceLevel << endl;
		
		if (argParser.Exist(kke::ArgOGtemperature) && device.PollTemperature().Valid)
			cout << device.PollTemperature().Data / 1000 << endl;
		
		if (argParser.Exist(kke::ArgOGactivity) && device.PollActivity().Valid)
			cout << device.PollActivity().Data.iActivityPercent << endl;
		
		if (argParser.Exist(kke::ArgOGclocksGpu) && device.PollActivity().Valid)
			cout << device.PollActivity().Data.iEngineClock / 100 << endl;
		
		if (argParser.Exist(kke::ArgOGclocksMem) && device.PollActivity().Valid)
			cout << device.PollActivity().Data.iMemoryClock / 100 << endl;
		
		if (argParser.Exist(kke::ArgOGclocksVddc) && device.PollActivity().Valid)
			cout << device.PollActivity().Data.iVddc << endl;
		
		// Fan
		if (argParser.Exist(kke::ArgOGfanType))
		{
			if (device.PollFanSpeed().Valid && device.PollFanInfo().Valid)
			{
				if (device.PollFanSpeed().Data.iSpeedType == ADL_DL_FANCTRL_SPEED_TYPE_RPM)
					cout << "RPM" << endl;
				else
					cout << "Percent" << endl;
			}
		}
		
		if (argParser.Exist(kke::ArgOGfan))
		{
			if (device.PollFanSpeed().Valid && device.PollFanInfo().Valid)
			{
				if (argParser.GetString(kke::ArgOCfanType, "Percent") == "RPM")
					cout << device.PollFanSpeed().Data.iFanSpeed << endl;
				else // Calculate percentage
					cout << (int)((float)device.PollFanSpeed().Data.iFanSpeed / device.PollFanInfo().Data.iMaxRPM * 100.0f) << endl;
			}
		}
		
		if (argParser.Exist(kke::ArgOGfanMin))
		{
			if (device.PollFanSpeed().Valid && device.PollFanInfo().Valid)
			{
				if (argParser.GetString(kke::ArgOCfanType, "Percent") == "RPM")
					cout << device.PollFanInfo().Data.iMinRPM << endl;
				else
					cout << device.PollFanInfo().Data.iMinPercent << endl;
			}
		}
		
		if (argParser.Exist(kke::ArgOGfanMax))
		{
			if (device.PollFanSpeed().Valid && device.PollFanInfo().Valid)
			{
				if (argParser.GetString(kke::ArgOCfanType, "Percent") == "RPM")
					cout << device.PollFanInfo().Data.iMaxRPM << endl;
				else
					cout << device.PollFanInfo().Data.iMaxPercent << endl;
			}
		}

		
		// --- Overclocking
		if (argParser.Exist(kke::ArgOSclocksGpu) || argParser.Exist(kke::ArgOSclocksMem) || argParser.Exist(kke::ArgOSclocksVddc) 
		|| argParser.Exist(kke::ArgOSfan) || argParser.Exist(kke::ArgOSfanReset)
		|| argParser.Exist(kke::ArgOSclocksGpuReset) || argParser.Exist(kke::ArgOSclocksMemReset) || argParser.Exist(kke::ArgOSclocksVddcReset)
		|| argParser.Exist(kke::ArgOSclocksReset))
		{
			if (startDevice != endDevice - 1)
			{
				cout << "You should probably not set clocks/fan on ALL devices, see -h, --help." << endl;
				continue;
			}
			
			{
				int tmpGpu = argParser.GetInt(kke::ArgOSclocksGpu, 0) * 100, 
					tmpMem = argParser.GetInt(kke::ArgOSclocksMem, 0) * 100, 
					tmpVddc = argParser.GetInt(kke::ArgOSclocksVddc, 0);
					
				if (device.PollODParams().Valid)
				{
					if (tmpGpu != 0 && (tmpGpu < device.PollODParams().Data.sEngineClock.iMin || tmpGpu > device.PollODParams().Data.sEngineClock.iMax))
					{
						cout << "GPU speed not within range." << endl;
						tmpGpu = 0;
					}
					
					if (tmpMem != 0 &&(tmpMem < device.PollODParams().Data.sMemoryClock.iMin || tmpMem > device.PollODParams().Data.sMemoryClock.iMax))
					{
						cout << "Memory speed not within range." << endl;
						tmpMem = 0;
					}
					
					if (tmpVddc != 0 && (tmpVddc < device.PollODParams().Data.sVddc.iMin || tmpVddc > device.PollODParams().Data.sVddc.iMax))
					{
						cout << "VDDC not within range." << endl;
						tmpVddc = 0;
					}
					
					if (tmpGpu != 0 || tmpMem != 0 || tmpVddc != 0)
						if (!device.ODSetAllLevels(tmpGpu, tmpMem, tmpVddc))
							cout << "Error when setting clocks/vddc." << endl;
				}

				if (argParser.Exist(kke::ArgOSclocksReset)  || argParser.Exist(kke::ArgOSallReset))
				{
					if (device.PollPerfLvls(true).Valid)
					{
						if (!device.ODSetLevels(device.PollPerfLvls(true).Data))
							cout << "Error when setting performance levels." << endl;
					}
				}
			}
		}

		// --- Fan
		if (argParser.Exist(kke::ArgOSfan))
		{
			if (device.PollFanInfo().Valid && device.PollFanSpeed().Valid)
			{
				if (argParser.GetString(kke::ArgOCfanType, "Percent") == "Percent")
				{
					if (argParser.GetInt(kke::ArgOSfan) < device.PollFanInfo().Data.iMinPercent || argParser.GetInt(kke::ArgOSfan) > device.PollFanInfo().Data.iMaxPercent)
						cout << "Fan value is incorrect. Valid are (PERCENT): " << device.PollFanInfo().Data.iMinPercent << "-" << device.PollFanInfo().Data.iMaxPercent << endl;
					else
					{
						if (!device.ODSetFan(argParser.GetInt(kke::ArgOSfan), ADL_DL_FANCTRL_SPEED_TYPE_PERCENT))
							cout << "Setting fan speed failed (PERCENT)." << endl;
					}
				}
				else // RPM
				{
					if (argParser.GetInt(kke::ArgOSfan) < device.PollFanInfo().Data.iMinRPM || argParser.GetInt(kke::ArgOSfan) > device.PollFanInfo().Data.iMaxRPM)
						cout << "Fan value is invalid. Valid are (RPM): " << device.PollFanInfo().Data.iMinRPM << "-" << device.PollFanInfo().Data.iMaxRPM << endl;
					else
					{
						if (!device.ODSetFan(argParser.GetInt(kke::ArgOSfan), ADL_DL_FANCTRL_SPEED_TYPE_RPM))
							cout << "Setting fan speed failed (RPM)." << endl;
					}
				}
			}
		}
	
		if (argParser.Exist(kke::ArgOSfanReset) || argParser.Exist(kke::ArgOSallReset))
		{
			// Set default fan
			if (!device.ODSetFanDefault())
				cout << "Error when setting default fan speed." << endl;
		}
		
		// Display
	
		if (argParser.Exist(kke::ArgDGConDisplays) || argParser.Exist(kke::ArgDGMapDisplays) || argParser.Exist(kke::ArgDGAllDisplays))
		{
			for (kke::DisplayVector::iterator disp = device.GetDisplays().begin(); disp != device.GetDisplays().end(); disp++)
			{			
				if (argParser.Exist(kke::ArgDGAllDisplays) || (disp->IsConnected() && argParser.Exist(kke::ArgDGConDisplays)) ||
					(disp->IsMapped() && argParser.Exist(kke::ArgDGMapDisplays)))
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
	
	if (argParser.Exist(kke::ArgHelp))
	{
		for (int i = 0; argParser.Registered(i); i++)
		{
			const int col1 = 1, col2 = 7, col3 = 25;

			alignArg("-" + argParser.GetsName(i), "--" + argParser.GetlName(i), argParser.GetInfo(i), col1, col2, col3);
			
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
	
	if (argParser.Exist(kke::ArgVersion))
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
	good = good && argParser.Register (kke::ArgHelp, kke::ArgumentExist, "help", "h", "Help dialog.");
	good = good && argParser.Register (kke::ArgVersion, kke::ArgumentExist, "version", "v", "Version.");

	// Device options
// 	good = good && argParser.Register (kke::ArgCdevice, kke::ArgumentInt, "device", "Cd", "Choose device by number.");
	good = good && argParser.Register (kke::ArgCdeviceName, kke::ArgumentString, "device-name", "Cdn", "Choose device by name. (Not for Crossfire)");
	good = good && argParser.Register (kke::ArgCdeviceUdid, kke::ArgumentString, "device-udid", "Cdu", "Choose device by UDID.");
	good = good && argParser.Register (kke::ArgCdeviceIndex, kke::ArgumentInt, "device-index", "Cdi", "Choose device by index.");
	good = good && argParser.Register (kke::ArgCperfLevel, kke::ArgumentInt, "perf-level", "Cpl", "Choose performance level.");

	// Hardware info
	good = good && argParser.Register (kke::ArgHGinfo, kke::ArgumentExist, "get-info", "HGi", "Device information.");
	good = good && argParser.Register (kke::ArgHGinfoLevels, kke::ArgumentExist, "get-info-levels", "HGil", "Extended device information.");
	good = good && argParser.Register (kke::ArgHGname, kke::ArgumentExist, "get-name", "HGn", "Device name.");
	good = good && argParser.Register (kke::ArgHGudid, kke::ArgumentExist, "get-udid", "HGu", "Device UDID.");
	good = good && argParser.Register (kke::ArgHGindex, kke::ArgumentExist, "get-index", "HGin", "Device index.");
	good = good && argParser.Register (kke::ArgHGadapters, kke::ArgumentExist, "get-adapters", "HGa", "Amount of device adapters.");
	good = good && argParser.Register (kke::ArgHGbusLanes, kke::ArgumentExist, "get-bus-lanes", "HGbl", "PCI-E bus lanes.");
	good = good && argParser.Register (kke::ArgHGbusLanesMax, kke::ArgumentExist, "get-bus-lanes-max", "HGblm", "Max PCI-E bus lanes.");
	good = good && argParser.Register (kke::ArgHGbusSpeed, kke::ArgumentExist, "get-bus-speed", "HGbs", "PCI-E bus speed.");

	// Device varying info
	good = good && argParser.Register (kke::ArgOGperfLevel, kke::ArgumentExist, "get-perf-level", "OGpl", "Performance level.");
	good = good && argParser.Register (kke::ArgOGclocksGpu, kke::ArgumentExist, "get-clocks-gpu", "OGcg", "GPU speed (MHz).");
	good = good && argParser.Register (kke::ArgOGclocksMem, kke::ArgumentExist, "get-clocks-mem", "OGcm", "Memory speed (MHz).");
	good = good && argParser.Register (kke::ArgOGclocksVddc, kke::ArgumentExist, "get-clocks-vddc", "OGcv", "VDDC voltage (mV).");
	good = good && argParser.Register (kke::ArgOGactivity, kke::ArgumentExist, "get-gpu-activity", "OGga", "GPU activity (%).");
	good = good && argParser.Register (kke::ArgOGtemperature, kke::ArgumentExist, "get-gpu-temp", "OGgt", "GPU temperature (C).");

	// Device fan
	good = good && argParser.Register (kke::ArgOCfanType, kke::ArgumentString, "fan-type", "OCft", "Choose fan type (RPM, Percent (default)).");
	good = good && argParser.Register (kke::ArgOGfanType, kke::ArgumentExist, "get-fan-type", "OGft", "Fan type (RPM, percent).");
	good = good && argParser.Register (kke::ArgOGfan, kke::ArgumentExist, "get-fan", "OGf", "Fan speed.");
	good = good && argParser.Register (kke::ArgOGfanMin, kke::ArgumentExist, "get-fan-min", "OGfmi", "Minimum fan speed.");
	good = good && argParser.Register (kke::ArgOGfanMax, kke::ArgumentExist, "get-fan-max", "OGfma", "Maximum fan speed.");
	good = good && argParser.Register (kke::ArgOSfan, kke::ArgumentInt, "set-fan", "OSf", "Set fan speed, recommend Percent (default) type.");

	// Set device values. (capital S to avoid typo).
	good = good && argParser.Register (kke::ArgOSclocksGpu, kke::ArgumentInt, "set-clocks-gpu", "OScg", "Set GPU speed (MHz).");
	good = good && argParser.Register (kke::ArgOSclocksMem, kke::ArgumentInt, "set-clocks-mem", "OScm", "Set Memory speed (MHz).");
	good = good && argParser.Register (kke::ArgOSclocksVddc, kke::ArgumentInt, "set-clocks-vddc", "OScv", "Set VDDC voltage (mV).");
	// Set reset
	good = good && argParser.Register (kke::ArgOSfanReset, kke::ArgumentExist, "set-fan-reset", "OSfr", "Reset fan speed.");
	good = good && argParser.Register (kke::ArgOSclocksGpuReset, kke::ArgumentExist, "set-clocks-gpu-reset", "OScgr", "Reset GPU clocks.");
	good = good && argParser.Register (kke::ArgOSclocksMemReset, kke::ArgumentExist, "set-clocks-mem-reset", "OScmr", "Reset Memory clocks.");
	good = good && argParser.Register (kke::ArgOSclocksVddcReset, kke::ArgumentExist, "set-clocks-vddc-reset", "OScvr", "Reset VDDC voltage.");
	good = good && argParser.Register (kke::ArgOSclocksReset, kke::ArgumentExist, "set-clocks-reset", "OScr", "Reset all clocks.");
	good = good && argParser.Register (kke::ArgOSallReset, kke::ArgumentExist, "set-all-reset", "OSar", "Reset clocks and fan.");
	
	// Display	
// 	good = good && argParser.Register(kke::ArgDGPixelFormat, kke::ArgumentExist, "get-pixel-format", "DGpf", "Pixel format (HDMI).");
// 	good = good && argParser.Register(kke::ArgDSPixelFormat, kke::ArgumentString, "set-pixel-format", "DSpf", "Pixel format (HDMI).");
	good = good && argParser.Register(kke::ArgDGConDisplays, kke::ArgumentExist, "get-con-displays", "DGcd", "Show connected displays.");
	good = good && argParser.Register(kke::ArgDGMapDisplays, kke::ArgumentExist, "get-map-displays", "DGmd", "Show used/mapped displays.");
	good = good && argParser.Register(kke::ArgDGAllDisplays, kke::ArgumentExist, "get-all-displays", "DGad", "Show all displays.");

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

