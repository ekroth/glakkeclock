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

#include "Device.hpp"
#include "kkeADL.hpp"
#include "Debug/Logger.hpp"

#include <iostream>
#include <cstring>

using namespace kke;

using std::vector;
using std::string;
using std::cout;
using std::endl;

// Display

Display::Display(const Adapter *adapter, const ADLDisplayInfo &info) : 
	adapter(adapter),
	info(info)
{

}

Display::~Display()
{
}

const kke::DiPixelFormat& Display::PollPixelFormat(bool refresh)
{
	if (!pixelFormat.Valid || refresh)
	{
		int pixel;
		pixelFormat.Valid = ADLManager::ADL_Display_PixelFormat_Get(adapter->GetInfo().iAdapterIndex, info.displayID.iDisplayLogicalIndex, &pixel);
	}
	
	return pixelFormat;
}

bool Display::SetPixelFormat(int format)
{
	return ADLManager::ADL_Display_PixelFormat_Set(adapter->GetInfo().iAdapterIndex, info.displayID.iDisplayLogicalIndex, format);
}

bool Display::IsConnected() const
{
	return (info.iDisplayInfoValue & ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED) == ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED;
}

bool Display::IsMapped() const
{
	return (info.iDisplayInfoValue & ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED) == ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED;
}

const ADLDisplayInfo &Display::GetInfo()
{	
	return info;
}

void Display::ResetPolled()
{
	pixelFormat.Valid = false;
}


// Adapter

Adapter::Adapter(const Device *device, const AdapterInfo &info) : 
	device(device),
	info(info)
{
}

Adapter::~Adapter()
{
}

// -- Polling
const kke::AId& Adapter::PollID(bool refresh)
{
	if (!id.Valid || refresh)
		id.Valid = ADLManager::ADL_Adapter_ID_Get(info.iAdapterIndex, &id.Data);
	
	return id;
}

const AdapterInfo& Adapter::GetInfo() const
{
	return info;
}

void Adapter::ResetPolled()
{
	id.Valid = false;
}

// Device

// Static
void Device::CreateDevices(DeviceVector &devices)
{	
	LOGGROUP(Log_Debug, "Device") << "Generating device(s)..";
	int numAdapters = 0;
	ADLManager::ADL_Adapter_NumberOfAdapters_Get(&numAdapters);
	AdapterInfo globalAdapters[numAdapters];
	// Populate
	ADLManager::ADL_Adapter_AdapterInfo_Get(globalAdapters, numAdapters * sizeof(AdapterInfo));
	
	for (int i = 0; i < numAdapters; i++)
	{
		LOGGROUP(Log_Debug, "Device") << "-- New adapter: ";
		LOGGROUP(Log_Debug, "Device") << "iAdapterIndex: " << globalAdapters[i].iAdapterIndex;
		LOGGROUP(Log_Debug, "Device") << "iDeviceNumber: " << globalAdapters[i].iDeviceNumber;
		LOGGROUP(Log_Debug, "Device") << "strUDID: " << globalAdapters[i].strUDID;
		LOGGROUP(Log_Debug, "Device") << "iDrvIndex: " << globalAdapters[i].iDrvIndex;
		
		bool alreadyExists = false;
		for (DeviceVector::iterator dev = devices.begin(); dev != devices.end(); dev++)
			if ((*dev)->GetUDID() == globalAdapters[i].strUDID)
			{
				alreadyExists = true;
				(*dev)->adapters.push_back(new Adapter(*dev, globalAdapters[i]));
				LOGGROUP(Log_Debug, "Device") << "Added adapter to existing device.";
			}
			
		if (alreadyExists)
			continue;
		
		// No previous device, create new
		Device *device = new Device();
		device->adapters.push_back(new Adapter(device, globalAdapters[i]));
		devices.push_back(device);
		
		LOGGROUP(Log_Debug, "Device") << "Created new device for adapter.";
	}
	
	LOGGROUP(Log_Debug, "Device") << "..successfully created " << devices.size() << " device(s), with " << numAdapters << " adapter(s).";
	LOGGROUP(Log_Debug, "Device") << "Bubble sorting device(s)..";
	
	// Sort devices by UDID
	// Bubble sort
	bool bubbleChange = false;
	do
	{
		bubbleChange = false;
		for (uint i = 0; i < devices.size(); i++)
		{
			if (i + 1 < devices.size())
			{
				if (devices[i]->GetUDID() > devices[i + 1]->GetUDID())
				{
					bubbleChange = true;
					Device *tmp = devices[i];
					devices[i] = devices[i + 1];
					devices[i + 1] = tmp;
				}
			}
		}
	} while (bubbleChange);
	
	LOGGROUP(Log_Debug, "Device") << "..done sorting.";
	
	// Force sort adapters on devices
	for (uint i = 0; i < devices.size(); i++)
		devices[i]->sortAdapters();
}

Device::Device():
	pollAdapter(0),
	pollThermal(0)
{
}

Device::~Device()
{
	for (AdapterVector::iterator it = adapters.begin(); it != adapters.end(); it++)
		delete *it;
}

const kke::DAccess& Device::PollAccess(bool refresh)
{
	if (!access.Valid || refresh)
	{
		int value;
		access.Valid = ADLManager::ADL_Adapter_Accessibility_Get(adapters[pollAdapter]->GetInfo().iAdapterIndex, &value);
		access.Data = value == ADL_TRUE ? true : false;
	}
	
	return access;
}

const kke::DBiosInfo& Device::PollBios(bool refresh)
{
	if (!bios.Valid || refresh)
		bios.Valid = ADLManager::ADL_Adapter_VideoBiosInfo_Get(adapters[pollAdapter]->GetInfo().iAdapterIndex, &bios.Data);
	
	return bios;
}

const kke::DActivity& Device::PollActivity(bool refresh)
{
	if (!activity.Valid || refresh)
		activity.Valid = ADLManager::ADL_Overdrive5_CurrentActivity_Get(adapters[pollAdapter]->GetInfo().iAdapterIndex, &activity.Data);
	
	return activity;
}

const kke::DTemperature& Device::PollTemperature(bool refresh)
{
	if (!temperature.Valid || refresh)
	{
		ADLTemperature temp;
		temp.iSize = sizeof(ADLTemperature);
		temperature.Valid = ADLManager::ADL_Overdrive5_Temperature_Get(adapters[pollAdapter]->GetInfo().iAdapterIndex, pollThermal, &temp);
		temperature.Data = temp.iTemperature;
	}
	
	return temperature;
}

const kke::DFanInfo& Device::PollFanInfo(bool refresh)
{
	if (!fanInfo.Valid || refresh)
		fanInfo.Valid = ADLManager::ADL_Overdrive5_FanSpeedInfo_Get(adapters[pollAdapter]->GetInfo().iAdapterIndex, pollThermal, &fanInfo.Data);
	
	return fanInfo;
}

const kke::DFanSpeed& Device::PollFanSpeed(bool refresh)
{
	if (!fanSpeed.Valid || refresh)
		fanSpeed.Valid = ADLManager::ADL_Overdrive5_FanSpeed_Get(adapters[pollAdapter]->GetInfo().iAdapterIndex, pollThermal, &fanSpeed.Data);
	
	return fanSpeed;
}

const kke::DOdParams& Device::PollODParams(bool refresh)
{
	if (!odParams.Valid || refresh)
		odParams.Valid = ADLManager::ADL_Overdrive5_ODParameters_Get(adapters[pollAdapter]->GetInfo().iAdapterIndex, &odParams.Data);
	
	return odParams;
}

const kke::DPerfLvls& Device::PollPerfLvls(bool defaultVals, bool refresh)
{
	if (!perfLevels.Valid || refresh)
	{
		const kke::DOdParams &params = PollODParams(true);
		if (!params.Valid)
		{
			perfLevels.Valid = false;
			return perfLevels;
		}
		
		const size_t size = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (params.Data.iNumberOfPerformanceLevels - 1);
		ADLODPerformanceLevels *perfLevelsNew = (ADLODPerformanceLevels*)malloc(size);
		memset(perfLevelsNew,'\0', size);
		perfLevelsNew->iSize = size;
		
		perfLevels.Valid = ADLManager::ADL_Overdrive5_ODPerformanceLevels_Get(adapters[pollAdapter]->GetInfo().iAdapterIndex, defaultVals ? ADL_TRUE : ADL_FALSE, perfLevelsNew);
		
		if (perfLevels.Valid)
			for (int i = 0; i < params.Data.iNumberOfPerformanceLevels; i++)
				perfLevels.Data.push_back(perfLevelsNew->aLevels[i]);
		
		free(perfLevelsNew);
	}
	
	return perfLevels;
}

const kke::DDisplayCount& Device::PollDisplayCount (bool refresh)
{
	if (!displayCount.Valid || refresh)
	{
		displayCount.Valid = ADLManager::ADL_Display_NumberOfDisplays_Get(adapters[pollAdapter]->GetInfo().iAdapterIndex, &displayCount.Data);
	}
	
	return displayCount;
}

bool Device::ODSetFan(int value, int type)
{
	if (type != ADL_DL_FANCTRL_SPEED_TYPE_PERCENT && type != ADL_DL_FANCTRL_SPEED_TYPE_RPM)
	{
		cout << "Invalid Fan Speed Type." << endl;
		return false;
	}
	
	ADLFanSpeedValue speedValue;
	speedValue.iSize = sizeof(ADLFanSpeedValue);
	speedValue.iFanSpeed = value;
	speedValue.iSpeedType = type;
	speedValue.iFlags = ADL_DL_FANCTRL_FLAG_USER_DEFINED_SPEED;
	return ADLManager::ADL_Overdrive5_FanSpeed_Set(adapters[pollAdapter]->GetInfo().iAdapterIndex, pollThermal, &speedValue);
}

bool Device::ODSetFanDefault()
{
	return ADLManager::ADL_Overdrive5_FanSpeedToDefault_Set(adapters[pollAdapter]->GetInfo().iAdapterIndex, pollThermal);
}

bool Device::ODSetLevels(const std::vector< ADLODPerformanceLevel > &levels)
{
	ADLODPerformanceLevels *perfLevelsNew = (ADLODPerformanceLevels*)malloc(sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (levels.size() - 1));
	perfLevelsNew->iSize = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (levels.size() - 1);
	for (uint i = 0; i < levels.size(); i++)
		perfLevelsNew->aLevels[i] = levels[i];
	
	bool result = ADLManager::ADL_Overdrive5_ODPerformanceLevels_Set(adapters[pollAdapter]->GetInfo().iAdapterIndex, perfLevelsNew);
	
	free(perfLevelsNew);
	
	return result;
}

bool Device::ODSetOneLevel(int index, int engine, int memory, int vddc)
{
	kke::DPerfLvls lvls = PollPerfLvls(false, true);
	if (!lvls.Valid)
		return false;
	
	ADLODPerformanceLevel perfValue;
	perfValue.iEngineClock =	engine != 0 ? engine : lvls.Data[index].iEngineClock;
	perfValue.iMemoryClock = 	memory != 0 ? memory : lvls.Data[index].iMemoryClock;
	perfValue.iVddc = 			vddc != 0 ? vddc : lvls.Data[index].iVddc;
	
	ADLODPerformanceLevels *perfLevelsNew = (ADLODPerformanceLevels*)malloc(sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (lvls.Data.size() - 1));
	perfLevelsNew->iSize = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (lvls.Data.size() - 1);
	perfLevelsNew->aLevels[0] = perfValue;
	
	for (uint i = 1; i < lvls.Data.size(); i++)
		perfLevelsNew->aLevels[i] = lvls.Data[i];
	
	bool result = ADLManager::ADL_Overdrive5_ODPerformanceLevels_Set(adapters[pollAdapter]->GetInfo().iAdapterIndex, perfLevelsNew);
	
	free(perfLevelsNew);
	
	return result;
}

bool Device::ODSetAllLevels(int engine, int memory, int vddc)
{
	kke::DPerfLvls lvls = PollPerfLvls(true, true);
	if (!lvls.Valid)
		return false;
	
	ADLODPerformanceLevels *perfLevelsNew = (ADLODPerformanceLevels*)malloc(sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (lvls.Data.size() - 1));
	perfLevelsNew->iSize = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (lvls.Data.size() - 1);
	
	for (uint i = 0; i < lvls.Data.size(); i++)
	{
		perfLevelsNew->aLevels[i].iEngineClock = 	engine != 0 ? engine : lvls.Data[i].iEngineClock;
		perfLevelsNew->aLevels[i].iMemoryClock = 	memory != 0 ? memory : lvls.Data[i].iMemoryClock;
		perfLevelsNew->aLevels[i].iVddc = 			vddc != 0 ? vddc : lvls.Data[i].iVddc;
	}
	
	bool result = ADLManager::ADL_Overdrive5_ODPerformanceLevels_Set(adapters[pollAdapter]->GetInfo().iAdapterIndex, perfLevelsNew);
	
	free(perfLevelsNew);
	
	return result;
}

bool Device::ODResetAllLevels()
{
	kke::DPerfLvls lvls = PollPerfLvls(true, true);
	if (!lvls.Valid)
		return false;
	
	ADLODPerformanceLevels *perfLevelsNew = (ADLODPerformanceLevels*)malloc(sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (lvls.Data.size() - 1));
	perfLevelsNew->iSize = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (lvls.Data.size() - 1);
	
	for (uint i = 0; i < lvls.Data.size(); i++)
		perfLevelsNew->aLevels[i] = lvls.Data[i];
	
	bool result = ADLManager::ADL_Overdrive5_ODPerformanceLevels_Set(adapters[pollAdapter]->GetInfo().iAdapterIndex, perfLevelsNew);
	
	free(perfLevelsNew);
	
	return result;
}

const std::string Device::GetUDID()
{
	return string(adapters[pollAdapter]->GetInfo().strUDID);
}

const std::string Device::GetName()
{
	return string(adapters[pollAdapter]->GetInfo().strAdapterName);
}

AdapterVector& Device::GetAdapters()
{
	return adapters;
}

DisplayVector& Device::GetDisplays()
{
	return displays;
}

void Device::SetPollAdapter(int index)
{
	pollAdapter = index;
}

int Device::GetPollAdapter() const
{
	return pollAdapter;
}

void Device::DetectAdapters()
{
// 	LOGGROUP(Log_Debug, "Device") << "Detecting adapters for specific device..";
// 	const string oldUdid = GetUDID();
// 	int numAdapters = 0;
// 	ADLManager::ADL_Adapter_NumberOfAdapters_Get(&numAdapters);
// 	AdapterInfo globalAdapters[numAdapters];
// 	ADLManager::ADL_Adapter_AdapterInfo_Get(globalAdapters, numAdapters * sizeof(AdapterInfo));
// 	
// 	displays.clear();
// 	adapters.clear();
// 	
// 	bool firstFound = true;
// 	
// 	for (int i = 0; i < numAdapters; i++)
// 		if (string(globalAdapters[i].strUDID) == oldUdid)
// 		{
// 			if (firstFound)
// 			{
// 				// Set this to polling adapter index
// 				firstFound = false;
// 				adapterIndexDefault = globalAdapters[i].iAdapterIndex;
// 			}
// 			
// 			// Belongs to this device
// 			adapters.push_back(Adapter(this, globalAdapters[i]));
// 		}
// 		
// 	LOGGROUP(Log_Debug, "Device") << "..done, found " << numAdapters << " adapters.";
	LOGGROUP(Log_Error, "Device") << "DetectAdapter() not implemented.";
}

void Device::DetectDisplays()
{
	// Now detect displays
	displays.clear();
	
	ADLDisplayInfo *displayInfo = 0;
	int displayNum = 0;
	if (ADLManager::ADL_Display_DisplayInfo_Get(adapters[pollAdapter]->GetInfo().iAdapterIndex, &displayNum, &displayInfo, 0))
	{
		displayCount.Valid = true;
		displayCount.Data = displayNum;
		
		for (int i = 0; i < displayNum; i++)
		{
			displays.push_back(Display(adapters[0], displayInfo[i]));
		}
	}
	
	if (displayInfo != 0)
		free(displayInfo);
}

void Device::ResetPolled()
{
	LOGGROUP(Log_Debug, "Device") << "Reseting all polled values.";
	// Set all values to invalid, force refresh
	bios.Valid = false;
	access.Valid = false;
	activity.Valid = false;
	temperature.Valid = false;
	fanInfo.Valid = false;
	fanSpeed.Valid = false;
	odParams.Valid = false;
	perfLevels.Valid = false;
	defPerfLevels.Valid = false;
	displayCount.Valid = false;
}

void Device::sortAdapters()
{
	// Bubble sort adapters
	LOGGROUP(Log_Debug, "Device") << "Begin bubble sorting adapters..";
	
	bool bubbleChange = false;
	do
	{
		bubbleChange = false;
		for (uint i = 0; i < adapters.size(); i++)
		{
			if (i + 1 < adapters.size())
			{
				if (adapters[i]->GetInfo().iAdapterIndex > adapters[i]->GetInfo().iAdapterIndex)
				{
					bubbleChange = true;
					Adapter *tmp = adapters[i];
					adapters[i] = adapters[i + 1];
					adapters[i + 1] = tmp;
				}
			}
		}
	} while (bubbleChange);
	
	LOGGROUP(Log_Debug, "Device") << "..bubblesorting adapters done.";
}