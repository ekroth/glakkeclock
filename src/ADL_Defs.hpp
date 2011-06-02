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

#ifndef KKE_ADL_DEFS_HPP
#define KKE_ADL_DEFS_HPP

namespace kke
{
	// ADL
	typedef int (*ADL_MAIN_CONTROL_CREATE)(ADL_MAIN_MALLOC_CALLBACK, int);
	typedef int (*ADL_MAIN_CONTROL_DESTROY)();

	// Adapter/Crossfire
	typedef int (*ADL_ADAPTER_NUMBEROFADAPTERS_GET)(int*);
	typedef int (*ADL_ADAPTER_ADAPTERINFO_GET)( LPAdapterInfo, int );
	typedef int (*ADL_ADAPTER_ID_GET)(int, int*);
	typedef int (*ADL_ADAPTER_CROSSFIRE_CAPS)(int, int*, int*, ADLCrossfireComb**);
	typedef int (*ADL_ADAPTER_CROSSFIRE_GET)(int, ADLCrossfireComb*, ADLCrossfireInfo*);
	typedef int (*ADL_ADAPTER_CROSSFIRE_SET)(int, ADLCrossfireComb*);
	typedef int (*ADL_ADAPTER_VIDEOBIOSINFO_GET)(int iAdapterIndex, ADLBiosInfo *lpBiosInfo);
	typedef int (*ADL_ADAPTER_ACCESSIBILITY_GET)(int iAdapterIndex, int *lpAccessibility);

	// Overdrive
	typedef int (*ADL_OVERDRIVE5_CURRENTACTIVITY_GET) (int, ADLPMActivity*);
	typedef int (*ADL_OVERDRIVE5_TEMPERATURE_GET)(int, int, ADLTemperature*);
	typedef int (*ADL_OVERDRIVE5_FANSPEEDINFO_GET)(int, int, ADLFanSpeedInfo*);
	typedef int (*ADL_OVERDRIVE5_FANSPEED_GET)(int, int, ADLFanSpeedValue*);
	typedef int (*ADL_OVERDRIVE5_FANSPEED_SET)(int, int, ADLFanSpeedValue*);
	typedef int (*ADL_OVERDRIVE5_FANSPEEDTODEFAULT_SET)(int, int);
	typedef int (*ADL_OVERDRIVE5_ODPARAMETERS_GET)(int, ADLODParameters*);
	typedef int (*ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET)(int, int, ADLODPerformanceLevels*);
	typedef int (*ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET)(int, ADLODPerformanceLevels*);
	
	// Various
	typedef int (*ADL_DISPLAY_WRITEANDREADI2CREV_GET)(int iAdapterIndex, int *lpMajor, int *lpMinor);

	// Display
	typedef int (*ADL_DISPLAY_DISPLAYINFO_GET)(int, int*, ADLDisplayInfo**, int);
	typedef int (*ADL_DISPLAY_PIXELFORMAT_SET)(int, int, int);
	typedef int (*ADL_DISPLAY_PIXELFORMAT_GET)(int, int, int*);
	typedef int (*ADL_DISPLAY_NUMBEROFDISPLAYS_GET)(int iAdapterIndex, int *lpNumDisplays);

}

#endif // KKE_ADL_DEFS_HPP