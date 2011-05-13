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

#include "kkeADL.hpp"
#include "Debug/Logger.hpp"

#include <cstdlib>
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <string>

using namespace kke;

using std::cout;
//using std::cerr;
using std::endl;
using std::string;

void *ADLManager::adlLib;

ADL_MAIN_CONTROL_CREATE					ADLManager::_ADL_Main_Control_Create;
ADL_MAIN_CONTROL_DESTROY				ADLManager::_ADL_Main_Control_Destroy;

ADL_ADAPTER_NUMBEROFADAPTERS_GET		ADLManager::_ADL_Adapter_NumberOfAdapters_Get;
ADL_ADAPTER_ADAPTERINFO_GET				ADLManager::_ADL_Adapter_AdapterInfo_Get;
ADL_ADAPTER_ID_GET						ADLManager::_ADL_Adapter_ID_Get;
ADL_ADAPTER_CROSSFIRE_CAPS				ADLManager::_ADL_Adapter_Crossfire_Caps;
ADL_ADAPTER_CROSSFIRE_GET				ADLManager::_ADL_Adapter_Crossfire_Get;
ADL_ADAPTER_CROSSFIRE_SET				ADLManager::_ADL_Adapter_Crossfire_Set;
ADL_ADAPTER_VIDEOBIOSINFO_GET			ADLManager::_ADL_Adapter_VideoBiosInfo_Get;
ADL_ADAPTER_ACCESSIBILITY_GET			ADLManager::_ADL_Adapter_Accessibility_Get;

ADL_OVERDRIVE5_CURRENTACTIVITY_GET		ADLManager::_ADL_Overdrive5_CurrentActivity_Get;
ADL_OVERDRIVE5_TEMPERATURE_GET			ADLManager::_ADL_Overdrive5_Temperature_Get;
ADL_OVERDRIVE5_FANSPEEDINFO_GET			ADLManager::_ADL_Overdrive5_FanSpeedInfo_Get;
ADL_OVERDRIVE5_FANSPEED_GET				ADLManager::_ADL_Overdrive5_FanSpeed_Get;
ADL_OVERDRIVE5_FANSPEED_SET				ADLManager::_ADL_Overdrive5_FanSpeed_Set;
ADL_OVERDRIVE5_FANSPEEDTODEFAULT_SET	ADLManager::_ADL_Overdrive5_FanSpeedToDefault_Set;
ADL_OVERDRIVE5_ODPARAMETERS_GET			ADLManager::_ADL_Overdrive5_ODParameters_Get;
ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET	ADLManager::_ADL_Overdrive5_ODPerformanceLevels_Get;
ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET	ADLManager::_ADL_Overdrive5_ODPerformanceLevels_Set;

ADL_DISPLAY_DISPLAYINFO_GET				ADLManager::_ADL_Display_DisplayInfo_Get;
ADL_DISPLAY_PIXELFORMAT_GET				ADLManager::_ADL_Display_PixelFormat_Get;
ADL_DISPLAY_PIXELFORMAT_SET				ADLManager::_ADL_Display_PixelFormat_Set;
ADL_DISPLAY_NUMBEROFDISPLAYS_GET        ADLManager::_ADL_Display_NumberOfDisplays_Get;

bool ADLManager::outputErrors = false;

// Memory allocation function
void* __stdcall ADL_Main_Memory_Alloc(int iSize)
{
	void* lpBuffer = malloc(iSize);
	return lpBuffer;
}

// Optional Memory de-allocation function
void __stdcall ADL_Main_Memory_Free(void **lpBuffer)
{
	if(*lpBuffer != NULL)
	{
		free(*lpBuffer);
		*lpBuffer = NULL;
	}
}

bool ADLManager::Init()
{
	LOGGROUP(Log_Debug, "ADLManager") << "Initializing ADL..";
	// Load the library.

#if defined (LINUX)
	adlLib = dlopen(ADLLOC, RTLD_LAZY | RTLD_GLOBAL);
#else
	adlLib = LoadLibrary("atiadlxx.dll");
	if(adlLib == NULL)
		// A 32 bit calling application on 64 bit OS will fail to LoadLIbrary.
		// Try to load the 32 bit library (atiadlxy.dll) instead
		adlLib = LoadLibrary("atiadlxy.dll");
#endif
	// Get addresses of all needed functions.

	_ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE)getProcAddress(adlLib, "ADL_Main_Control_Create");
	if(_ADL_Main_Control_Create == 0)
		return false;

	_ADL_Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY)getProcAddress(adlLib, "ADL_Main_Control_Destroy");
	if(_ADL_Main_Control_Destroy == 0)
		return false;


	_ADL_Adapter_NumberOfAdapters_Get = (ADL_ADAPTER_NUMBEROFADAPTERS_GET)getProcAddress(adlLib, "ADL_Adapter_NumberOfAdapters_Get");
	if(_ADL_Adapter_NumberOfAdapters_Get == 0)
		return false;

	_ADL_Adapter_AdapterInfo_Get = (ADL_ADAPTER_ADAPTERINFO_GET)getProcAddress(adlLib, "ADL_Adapter_AdapterInfo_Get");
	if(_ADL_Adapter_AdapterInfo_Get == 0)
		return false;

	_ADL_Adapter_ID_Get = (ADL_ADAPTER_ID_GET)getProcAddress(adlLib, "ADL_Adapter_ID_Get");
	_ADL_Adapter_Crossfire_Caps = (ADL_ADAPTER_CROSSFIRE_CAPS)getProcAddress(adlLib, "ADL_Adapter_Crossfire_Caps");
	_ADL_Adapter_Crossfire_Get = (ADL_ADAPTER_CROSSFIRE_GET)getProcAddress(adlLib, "ADL_Adapter_Crossfire_Get");
	_ADL_Adapter_Crossfire_Set = (ADL_ADAPTER_CROSSFIRE_SET)getProcAddress(adlLib, "ADL_Adapter_Crossfire_Set");
	_ADL_Adapter_VideoBiosInfo_Get = (ADL_ADAPTER_VIDEOBIOSINFO_GET)getProcAddress(adlLib, "ADL_Adapter_VideoBiosInfo_Get");
	_ADL_Adapter_Accessibility_Get = (ADL_ADAPTER_ACCESSIBILITY_GET)getProcAddress(adlLib, "ADL_Adapter_Accessibility_Get");

	_ADL_Overdrive5_CurrentActivity_Get = (ADL_OVERDRIVE5_CURRENTACTIVITY_GET)getProcAddress(adlLib, "ADL_Overdrive5_CurrentActivity_Get");
	_ADL_Overdrive5_Temperature_Get = (ADL_OVERDRIVE5_TEMPERATURE_GET)getProcAddress(adlLib, "ADL_Overdrive5_Temperature_Get");
	_ADL_Overdrive5_FanSpeedInfo_Get = (ADL_OVERDRIVE5_FANSPEEDINFO_GET)getProcAddress(adlLib, "ADL_Overdrive5_FanSpeedInfo_Get");
	_ADL_Overdrive5_FanSpeed_Get = (ADL_OVERDRIVE5_FANSPEED_GET)getProcAddress(adlLib, "ADL_Overdrive5_FanSpeed_Get");
	_ADL_Overdrive5_FanSpeed_Set = (ADL_OVERDRIVE5_FANSPEED_SET)getProcAddress(adlLib, "ADL_Overdrive5_FanSpeed_Set");
	_ADL_Overdrive5_FanSpeedToDefault_Set = (ADL_OVERDRIVE5_FANSPEEDTODEFAULT_SET)getProcAddress(adlLib, "ADL_Overdrive5_FanSpeedToDefault_Set");
	_ADL_Overdrive5_ODParameters_Get = (ADL_OVERDRIVE5_ODPARAMETERS_GET)getProcAddress(adlLib, "ADL_Overdrive5_ODParameters_Get");
	_ADL_Overdrive5_ODPerformanceLevels_Get = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET)getProcAddress(adlLib, "ADL_Overdrive5_ODPerformanceLevels_Get");
	_ADL_Overdrive5_ODPerformanceLevels_Set = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET)getProcAddress(adlLib, "ADL_Overdrive5_ODPerformanceLevels_Set");
	
	_ADL_Display_DisplayInfo_Get = (ADL_DISPLAY_DISPLAYINFO_GET)getProcAddress(adlLib, "ADL_Display_DisplayInfo_Get");
	_ADL_Display_PixelFormat_Get = (ADL_DISPLAY_PIXELFORMAT_GET)getProcAddress(adlLib, "ADL_Display_PixelFormat_Get");	
	_ADL_Display_PixelFormat_Set = (ADL_DISPLAY_PIXELFORMAT_SET)getProcAddress(adlLib, "ADL_Display_PixelFormat_Set");
	_ADL_Display_NumberOfDisplays_Get = (ADL_DISPLAY_NUMBEROFDISPLAYS_GET)getProcAddress(adlLib, "ADL_Display_NumberOfDisplays_Get");
	
	if(!GetAdlErr(_ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1)))
	{
		LOGGROUP(Log_Error, "ADLManager") << "ADL_Main_Control_Create was not successful.";
		return false;
	}

#if defined (LINUX)
	dlclose(adlLib);
#else
	FreeLibrary(adlLib);
#endif
	adlLib = NULL;
	
	LOGGROUP(Log_Debug, "ADLManager") << "..successfully initialized ADL!";

	return true;
}

void ADLManager::Terminate()
{
	if(!GetAdlErr(_ADL_Main_Control_Destroy()))
		LogError("Destroy ADL failed.");
}

void *ADLManager::getProcAddress(void *lib, const char *name)
{
#if defined (LINUX)
#define GetProc dlsym
#else
#define GetProc GetProcAddress
#endif

	void *address = GetProc(lib, name);

	if(address == 0)
		LogError("Failed to get address: " + std::string(name));

	return address;
}

bool ADLManager::GetAdlErr(int error)
{
	switch(error)
	{
		case ADL_OK_WAIT:
			LogError("All OK, but need to wait.");
			break;

		case ADL_OK_RESTART:
			LogError("All OK, but need restart.");
			break;

		case ADL_OK_MODE_CHANGE:
			LogError("All OK but need mode change.");
			break;

		case ADL_OK_WARNING:
			LogError("All OK, but with warning.");
			break;

		case ADL_OK:
			//cout << "ADL function completed successfully." << endl;
			return true;
			break;

		case ADL_ERR:
			LogError("Generic Error. Most likely one or more of the Escape calls to the driver failed!");
			break;

		case ADL_ERR_NOT_INIT:
			LogError("ADL not initialized.");
			break;

		case ADL_ERR_INVALID_PARAM:
			LogError("One of the parameter passed is invalid.");
			break;

		case ADL_ERR_INVALID_PARAM_SIZE:
			LogError("One of the parameter size is invalid.");
			break;

		case ADL_ERR_INVALID_ADL_IDX:
			LogError("Invalid ADL index passed.");
			break;

		case ADL_ERR_INVALID_CONTROLLER_IDX:
			LogError("Invalid controller index passed.");
			break;

		case ADL_ERR_INVALID_DIPLAY_IDX:
			LogError("Invalid display index passed.");
			break;

		case ADL_ERR_NOT_SUPPORTED:
			LogError("Function not supported by the driver.");
			break;

		case ADL_ERR_NULL_POINTER:
			LogError("Null Pointer error.");
			break;

		case ADL_ERR_DISABLED_ADAPTER:
			LogError("Call can't be made due to disabled adapter.");
			break;

		case ADL_ERR_INVALID_CALLBACK:
			LogError("Invalid Callback.");
			break;

		case ADL_ERR_RESOURCE_CONFLICT:
			LogError("Display Resource conflict.");
			break;

		default:
			LogError("Unknown Error (which shouldn't exist) occured!");
			break;
	}

	return false;
}

void ADLManager::LogError(const string &msg)
{
	if(outputErrors)
	{
		LOGGROUP(Log_Error, "ADLManager") << msg;
	}
}

// Wrapper functions

bool ADLManager::ADL_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK p1, int p2)
{
	if(!GetAdlErr(_ADL_Main_Control_Create(p1, p2)))
	{
		LogError("ADL_Main_Control_Create");
		return false;
	}

	return true;
}
bool ADLManager::ADL_Main_Control_Destroy()
{
	if(!GetAdlErr(_ADL_Main_Control_Destroy()))
	{
		LogError("ADL_Main_Control_Destroy");
		return false;
	}

	return true;
}

bool ADLManager::ADL_Adapter_NumberOfAdapters_Get(int *p1)
{
	if(!GetAdlErr(_ADL_Adapter_NumberOfAdapters_Get(p1)))
	{
		LogError("ADL_Adapter_NumberOfAdapters_Get");
		return false;
	}

	return true;
}
bool ADLManager::ADL_Adapter_AdapterInfo_Get(LPAdapterInfo p1, int p2)
{
	if(!GetAdlErr(_ADL_Adapter_AdapterInfo_Get(p1, p2)))
	{
		LogError("ADL_Adapter_AdapterInfo_Get");
		return false;
	}

	return true;
}
bool ADLManager::ADL_Adapter_ID_Get(int p1, int *p2)
{
	if (_ADL_Adapter_ID_Get == 0)
	{
		LogError("ADL_Adapter_ID_Get not set.");
		return false;
	}
	
	if(!GetAdlErr(_ADL_Adapter_ID_Get(p1, p2)))
	{
		LogError("ADL_Adapter_ID_Get");
		return false;
	}

	return true;
}
bool ADLManager::ADL_Adapter_Crossfire_Caps(int p1, int *p2, int *p3, ADLCrossfireComb **p4)
{
	if (_ADL_Adapter_Crossfire_Caps == 0)
	{
		LogError("ADL_Adapter_Crossfire_Caps not set.");
		return false;
	}
	
	if(!GetAdlErr(_ADL_Adapter_Crossfire_Caps(p1, p2, p3, p4)))
	{
		LogError("ADL_Adapter_Crossfire_Caps");
		return false;
	}

	return true;
}
bool ADLManager::ADL_Adapter_Crossfire_Get(int p1, ADLCrossfireComb *p2, ADLCrossfireInfo *p3)
{
	if (_ADL_Adapter_Crossfire_Get == 0)
	{
		LogError("ADL_Adapter_Crossfire_Get not set.");
		return false;
	}
	
	if(!GetAdlErr(_ADL_Adapter_Crossfire_Get(p1, p2, p3)))
	{
		LogError("ADL_Adapter_Crossfire_Get");
		return false;
	}

	return true;
}
bool ADLManager::ADL_Adapter_Crossfire_Set(int p1, ADLCrossfireComb *p2)
{
	if (_ADL_Adapter_Crossfire_Set == 0)
	{
		LogError("ADL_Adapter_Crossfire_Set not set.");
		return false;
	}
	
	if(!GetAdlErr(_ADL_Adapter_Crossfire_Set(p1, p2)))
	{
		LogError("ADL_Adapter_Crossfire_Set");
		return false;
	}

	return true;
}

bool ADLManager::ADL_Adapter_VideoBiosInfo_Get (int iAdapterIndex, ADLBiosInfo *lpBiosInfo)
{
	if (_ADL_Adapter_VideoBiosInfo_Get == 0)
	{
		LogError("ADL_Adapter_VideoBiosInfo_Get not set.");
		return false;
	}
	
	if (!GetAdlErr(_ADL_Adapter_VideoBiosInfo_Get(iAdapterIndex, lpBiosInfo)))
	{
		LogError("ADL_Adapter_VideoBiosInfo_Get");
		return false;
	}
	
	return true;
}

bool ADLManager::ADL_Adapter_Accessibility_Get (int iAdapterIndex, int *lpAccessibility)
{
	if (_ADL_Adapter_Accessibility_Get == 0)
	{
		LogError("ADL_Adapter_Accessibility_Get not set.");
		return false;
	}
	
	if (!GetAdlErr(_ADL_Adapter_Accessibility_Get(iAdapterIndex, lpAccessibility)))
	{
		LogError("ADL_Adapter_Accessibility_Get");
		return false;
	}
	
	return true;
}



bool ADLManager::ADL_Overdrive5_CurrentActivity_Get(int p1, ADLPMActivity *p2)
{
	if (_ADL_Overdrive5_CurrentActivity_Get == 0)
	{
		LogError("ADL_Overdrive5_CurrentActivity_Get not set.");
		return false;
	}
	
	if(!GetAdlErr(_ADL_Overdrive5_CurrentActivity_Get(p1, p2)))
	{
		LogError("ADL_Overdrive5_CurrentActivity_Get");
		return false;
	}

	return true;
}
bool ADLManager::ADL_Overdrive5_Temperature_Get(int p1, int p2, ADLTemperature *p3)
{
	if (_ADL_Overdrive5_Temperature_Get == 0)
	{
		LogError("ADL_Overdrive5_Temperature_Get not set.");
		return false;
	}
	
	if(!GetAdlErr(_ADL_Overdrive5_Temperature_Get(p1, p2, p3)))
	{
		LogError("ADL_Overdrive5_Temperature_Get");
		return false;
	}

	return true;
}
bool ADLManager::ADL_Overdrive5_FanSpeedInfo_Get(int p1, int p2, ADLFanSpeedInfo *p3)
{
	if (_ADL_Overdrive5_FanSpeedInfo_Get == 0)
	{
		LogError("ADL_Overdrive5_FanSpeedInfo_Get not set.");
		return false;
	}
	
	if(!GetAdlErr(_ADL_Overdrive5_FanSpeedInfo_Get(p1, p2, p3)))
	{
		LogError("ADL_Overdrive5_FanSpeedInfo_Get");
		return false;
	}

	return true;
}
bool ADLManager::ADL_Overdrive5_FanSpeed_Get(int p1, int p2, ADLFanSpeedValue *p3)
{
	if (_ADL_Overdrive5_FanSpeed_Get == 0)
	{
		LogError("ADL_Overdrive5_FanSpeed_Get not set.");
		return false;
	}
	
	int speed = p3->iSpeedType;
	if(!GetAdlErr(_ADL_Overdrive5_FanSpeed_Get(p1, p2, p3)))
	{
		LogError("ADL_Overdrive5_FanSpeed_Get");

		if (speed == ADL_DL_FANCTRL_SPEED_TYPE_PERCENT)
			LogError("iSpeedType Input: ADL_DL_FANCTRL_SPEED_TYPE_PERCENT");
		else
			LogError("iSpeedType Input: ADL_DL_FANCTRL_SPEED_TYPE_RPM");

		if (p3->iSpeedType == ADL_DL_FANCTRL_SPEED_TYPE_PERCENT)
			LogError("iSpeedType Output: ADL_DL_FANCTRL_SPEED_TYPE_PERCENT");
		else
			LogError("iSpeedType Output: ADL_DL_FANCTRL_SPEED_TYPE_RPM");

		return false;
	}

	return true;
}
bool ADLManager::ADL_Overdrive5_FanSpeed_Set(int p1, int p2, ADLFanSpeedValue *p3)
{
	if (_ADL_Overdrive5_FanSpeed_Set == 0)
	{
		LogError("ADL_Overdrive5_FanSpeed_Set not set.");
		return false;
	}
	
	int speed = p3->iSpeedType;
	if(!GetAdlErr(_ADL_Overdrive5_FanSpeed_Set(p1, p2, p3)))
	{
		LogError("ADL_Overdrive5_FanSpeed_Set");

		if(speed == ADL_DL_FANCTRL_SPEED_TYPE_PERCENT)
			LogError("iSpeedType Input: ADL_DL_FANCTRL_SPEED_TYPE_PERCENT");
		else
			LogError("iSpeedType Input: ADL_DL_FANCTRL_SPEED_TYPE_RPM");

		return false;
	}

	return true;
}
bool ADLManager::ADL_Overdrive5_FanSpeedToDefault_Set(int p1, int p2)
{
	if (_ADL_Overdrive5_FanSpeedToDefault_Set == 0)
	{
		LogError("ADL_Overdrive5_FanSpeedToDefault_Set not set.");
		return false;
	}
	
	if(!GetAdlErr(_ADL_Overdrive5_FanSpeedToDefault_Set(p1, p2)))
	{
		LogError("ADL_Overdrive5_FanSpeedToDefault_Set");
		return false;
	}

	return true;
}
bool ADLManager::ADL_Overdrive5_ODParameters_Get(int p1, ADLODParameters *p2)
{
	if (_ADL_Overdrive5_ODParameters_Get == 0)
	{
		LogError("ADL_Overdrive5_ODParameters_Get not set.");
		return false;
	}
	
	if(!GetAdlErr(_ADL_Overdrive5_ODParameters_Get(p1, p2)))
	{
		LogError("ADL_Overdrive5_ODParameters_Get");
		return false;
	}

	return true;
}
bool ADLManager::ADL_Overdrive5_ODPerformanceLevels_Get(int p1, int p2, ADLODPerformanceLevels *p3)
{
	if (_ADL_Overdrive5_ODPerformanceLevels_Get == 0)
	{
		LogError("ADL_Overdrive5_ODPerformanceLevels_Get not set.");
		return false;
	}
	
	if(!GetAdlErr(_ADL_Overdrive5_ODPerformanceLevels_Get(p1, p2, p3)))
	{
		LogError("ADL_Overdrive5_ODPerformanceLevels_Get");
		return false;
	}

	return true;
}
bool ADLManager::ADL_Overdrive5_ODPerformanceLevels_Set(int p1, ADLODPerformanceLevels *p2)
{
	if (_ADL_Overdrive5_ODPerformanceLevels_Set == 0)
	{
		LogError("ADL_Overdrive5_ODPerformanceLevels_Set not set.");
		return false;
	}
	
	if(!GetAdlErr(_ADL_Overdrive5_ODPerformanceLevels_Set(p1, p2)))
	{
		LogError("ADL_Overdrive5_ODPerformanceLevels_Set");
		return false;
	}

	return true;
}

bool ADLManager::ADL_Display_DisplayInfo_Get(int iAdapterIndex, int *lpNumDisplays, ADLDisplayInfo **lppInfo, int iForceDetect)
{
	if (_ADL_Display_DisplayInfo_Get == 0)
	{
		LogError("ADL_Display_DisplayInfo_Get not set.");
		return false;
	}
	
	if(!GetAdlErr(_ADL_Display_DisplayInfo_Get(iAdapterIndex, lpNumDisplays, lppInfo , iForceDetect)))
	{
		LogError("ADL_Display_DisplayInfo_Get");
		return false;
	}

	return true;
}
bool ADLManager::ADL_Display_PixelFormat_Set(int iAdapterIndex, int iDisplayIndex, int iPixelFormat)
{
	if (_ADL_Display_PixelFormat_Set == 0)
	{
		LogError("ADL_Display_PixelFormat_Set not set.");
		return false;
	}
	
	if (!GetAdlErr(_ADL_Display_PixelFormat_Set(iAdapterIndex, iDisplayIndex, iPixelFormat)))
	{
		LogError("ADL_Display_PixelFormat_Set");
		return false;
	}
	return true;
}

bool ADLManager::ADL_Display_PixelFormat_Get(int iAdapterIndex, int iDisplayIndex, int *lpPixelFormat)
{
	if (_ADL_Display_PixelFormat_Get == 0)
	{
		LogError("ADL_Display_PixelFormat_Get not set.");
		return false;
	}
	
	if (!GetAdlErr(_ADL_Display_PixelFormat_Get(iAdapterIndex, iDisplayIndex, lpPixelFormat)))
	{
		LogError("ADL_Display_PixelFormat_Get");
		return false;
	}
	return true;
}

bool ADLManager::ADL_Display_NumberOfDisplays_Get(int iAdapterIndex, int* lpNumDisplays)
{
	if (_ADL_Display_NumberOfDisplays_Get == 0)
	{
		LogError("ADL_Display_NumberOfDisplays_Get not set.");
		return false;
	}
	
	if (!GetAdlErr(_ADL_Display_NumberOfDisplays_Get(iAdapterIndex, lpNumDisplays)))
	{
		LogError("ADL_Display_NumberOfDisplays_Get");
		return false;
	}
	return true;
}

void ADLManager::SetOutputErrors(bool value)
{
	outputErrors = value;
}