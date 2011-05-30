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

// TODO: Make it a singleton, not total static.

#ifndef KKEADL_HPP
#define KKEADL_HPP

#include "ADL/adl_sdk.h"
#include "Interface/Noncopyable.hpp"
#include "ADL_Defs.hpp"

#include <cstdlib>
#include <string>

#ifdef WINDOWS
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif

// Memory allocation function
void* __stdcall ADL_Main_Memory_Alloc(int iSize);
// Optional Memory de-allocation function
void __stdcall ADL_Main_Memory_Free(void** lpBuffer);

namespace kke
{
	class ADLManager : public Noncopyable
	{
	public:
		/**
		* @brief Initialize ADL and Pointers.
		*
		* @return bool
		**/
		static bool Init();

		/**
		* @brief Terminate ADL.
		*
		* @return void
		**/
		static void Terminate();
		
		
		
		// Wrapper functions

		// ADL
		static bool ADL_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK, int);
		static bool ADL_Main_Control_Destroy();
		
		// Adapter
		static bool ADL_Adapter_NumberOfAdapters_Get(int *lpNumAdapters);
		static bool ADL_Adapter_AdapterInfo_Get(LPAdapterInfo lpInfo, int iInputSize);
		static bool ADL_Adapter_ID_Get(int iAdapterIndex, int *lpAdapterID);
		static bool ADL_Adapter_Crossfire_Caps(int iAdapterIndex, int *lpPreferred, int *lpNumComb, ADLCrossfireComb **ppCrossfireComb);
		static bool ADL_Adapter_Crossfire_Get(int iAdapterIndex, ADLCrossfireComb *lpCrossfireComb, ADLCrossfireInfo *lpCrossfireInfo);
		static bool ADL_Adapter_Crossfire_Set(int iAdapterIndex, ADLCrossfireComb *lpCrossfireComb);
		static bool ADL_Adapter_VideoBiosInfo_Get (int iAdapterIndex, ADLBiosInfo *lpBiosInfo);
		static bool ADL_Adapter_Accessibility_Get (int iAdapterIndex, int *lpAccessibility);

		// Overdrive
		static bool ADL_Overdrive5_CurrentActivity_Get(int iAdapterIndex, ADLPMActivity *lpActivity);
		static bool ADL_Overdrive5_Temperature_Get(int iAdapterIndex, int iThermalControllerIndex, ADLTemperature *lpTemperature);
		static bool ADL_Overdrive5_FanSpeedInfo_Get(int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedInfo *lpFanSpeedInfo);
		static bool ADL_Overdrive5_FanSpeed_Get(int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedValue *lpFanSpeedValue);
		static bool ADL_Overdrive5_FanSpeed_Set(int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedValue *lpFanSpeedValu);
		static bool ADL_Overdrive5_FanSpeedToDefault_Set(int iAdapterIndex, int iThermalControllerIndex);
		static bool ADL_Overdrive5_ODParameters_Get(int iAdapterIndex, ADLODParameters *lpOdParameters);
		static bool ADL_Overdrive5_ODPerformanceLevels_Get(int iAdapterIndex, int iDefault, ADLODPerformanceLevels *lpOdPerformanceLevels);
		static bool ADL_Overdrive5_ODPerformanceLevels_Set(int iAdapterIndex, ADLODPerformanceLevels *lpOdPerformanceLevels);
		
		// Display
		static bool ADL_Display_DisplayInfo_Get(int iAdapterIndex, int *lpNumDisplays, ADLDisplayInfo **lppInfo, int iForceDetect);
		static bool ADL_Display_PixelFormat_Set(int iAdapterIndex, int iDisplayIndex, int iPixelFormat);
		static bool ADL_Display_PixelFormat_Get(int iAdapterIndex, int iDisplayIndex, int *lpPixelFormat);
		static bool ADL_Display_NumberOfDisplays_Get(int iAdapterIndex, int *lpNumDisplays);
		
		static void SetOutputErrors(bool value);

	private:
	/**
		* @brief Analyze ADL error and output error/warnings to cerr.
		*
		* @param error Result of ADL function.
		* @return bool true if error was without warnings or errors.
		**/
		static bool GetAdlErr(int error);
		static bool outputErrors;
		
		// ADL
		static ADL_MAIN_CONTROL_CREATE			_ADL_Main_Control_Create;
		static ADL_MAIN_CONTROL_DESTROY			_ADL_Main_Control_Destroy;
		
		// Adapter/Crossfire
		static ADL_ADAPTER_NUMBEROFADAPTERS_GET	_ADL_Adapter_NumberOfAdapters_Get;
		static ADL_ADAPTER_ADAPTERINFO_GET		_ADL_Adapter_AdapterInfo_Get;
		static ADL_ADAPTER_ID_GET				_ADL_Adapter_ID_Get;
		static ADL_ADAPTER_CROSSFIRE_CAPS		_ADL_Adapter_Crossfire_Caps;
		static ADL_ADAPTER_CROSSFIRE_GET		_ADL_Adapter_Crossfire_Get;
		static ADL_ADAPTER_CROSSFIRE_SET		_ADL_Adapter_Crossfire_Set;
		static ADL_ADAPTER_VIDEOBIOSINFO_GET	_ADL_Adapter_VideoBiosInfo_Get;
		static ADL_ADAPTER_ACCESSIBILITY_GET	_ADL_Adapter_Accessibility_Get;
		
		// Overdrive
		static ADL_OVERDRIVE5_CURRENTACTIVITY_GET		_ADL_Overdrive5_CurrentActivity_Get;
		static ADL_OVERDRIVE5_TEMPERATURE_GET			_ADL_Overdrive5_Temperature_Get;
		static ADL_OVERDRIVE5_FANSPEEDINFO_GET			_ADL_Overdrive5_FanSpeedInfo_Get;
		static ADL_OVERDRIVE5_FANSPEED_GET				_ADL_Overdrive5_FanSpeed_Get;
		static ADL_OVERDRIVE5_FANSPEED_SET				_ADL_Overdrive5_FanSpeed_Set;
		static ADL_OVERDRIVE5_FANSPEEDTODEFAULT_SET		_ADL_Overdrive5_FanSpeedToDefault_Set;
		static ADL_OVERDRIVE5_ODPARAMETERS_GET			_ADL_Overdrive5_ODParameters_Get;
		static ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET	_ADL_Overdrive5_ODPerformanceLevels_Get;
		static ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET	_ADL_Overdrive5_ODPerformanceLevels_Set;
		
		// Display
		static ADL_DISPLAY_DISPLAYINFO_GET		_ADL_Display_DisplayInfo_Get;
		static ADL_DISPLAY_PIXELFORMAT_SET		_ADL_Display_PixelFormat_Set;
		static ADL_DISPLAY_PIXELFORMAT_GET		_ADL_Display_PixelFormat_Get;
		static ADL_DISPLAY_NUMBEROFDISPLAYS_GET _ADL_Display_NumberOfDisplays_Get;

		static void LogError(const std::string &msg);

	#if defined (LINUX)
		static void* adlLib;
		static void* getProcAddress(void *lib, const char *name);
	#else
		static HINSTANCE adlLib;
		static void* getProcAddress(HINSTANCE lib, const char *name);
	#endif
	};
}

#endif // KKEADL_HPP

