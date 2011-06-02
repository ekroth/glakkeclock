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

#ifndef KKE_DEVICE_HPP
#define KKE_DEVICE_HPP

#include <ADL/adl_sdk.h>

#include <vector>
#include <string>

namespace kke
{
	class Adapter;
	class Device;
	
	template<typename T>
	struct PollData
	{
		PollData() : Valid(false) { }
		
		T Data;
		bool Valid;
	};
	
	typedef PollData<int> DiPixelFormat;
	
	class Display
	{
	public:
		Display(const Adapter *adapter, const ADLDisplayInfo &info);
		~Display();
		
		const DiPixelFormat& PollPixelFormat(bool refresh = false);
		bool SetPixelFormat(int format);
		
		bool IsConnected() const;
		bool IsMapped() const;
		
		const ADLDisplayInfo& GetInfo();
		void ResetPolled();
	private:
		const Adapter *adapter;
		ADLDisplayInfo info;
		DiPixelFormat pixelFormat;
	};
	
	typedef PollData<int> AId;
	typedef PollData<bool> AActive;
	
	class Adapter
	{
	public:
		Adapter(const Device *device, const AdapterInfo &info);
		~Adapter();
		
		/**
		 * @brief ID.
		 **/
		const AId& PollID(bool refresh = false);
		
		/**
		 * @brief If active.
		 **/
		const AActive& PollActive(bool refresh = false);
		
		/**
		 * @brief Information.
		 *
		 * @return const AdapterInfo&
		 **/
		const AdapterInfo &GetInfo() const;
		
		// Detect/Refresh displays.
		/**
		 * @brief Forces all data to be repolled.
		 **/
		void ResetPolled();
		
	private:
		const Device *device;
		AdapterInfo info;
		
		AId id;
		AActive active;
	};
	
	typedef std::vector<Display> DisplayVector;
	typedef std::vector<Adapter*> AdapterVector;
	typedef std::vector<Device*> DeviceVector;
	typedef std::vector<AdapterInfo> GlobalAdaptVec;
	
	typedef PollData<ADLBiosInfo> DBiosInfo;
	typedef PollData<bool> DAccess;
	typedef PollData<ADLPMActivity> DActivity;
	typedef PollData<int> DTemperature;
	typedef PollData<ADLFanSpeedInfo> DFanInfo;
	typedef PollData<ADLFanSpeedValue> DFanSpeed;
	typedef PollData<ADLODParameters> DOdParams;
	typedef PollData< ADLODPerformanceLevels* > DPerfLvls;
	typedef PollData<int> DDisplayCount;
  
	/**
	 * @brief Device manages Adapters and Displays, has one unique UDID.
	 **/
	class Device
	{
	public:
		// Creates a vector of (dynamic) devices, with adapters.
		static void CreateDevices(DeviceVector &devices);
		
	public:
		
		Device();
		~Device();
		
		const DBiosInfo& PollBios(bool refresh = false);
		const DAccess& PollAccess(bool refresh = false);
		const DActivity& PollActivity(bool refresh = false);
		const DTemperature& PollTemperature(bool refresh = false);
		const DFanInfo& PollFanInfo(bool refresh = false);
		const DFanSpeed& PollFanSpeed(int speedType = ADL_DL_FANCTRL_SPEED_TYPE_PERCENT, bool refresh = false);
		const DOdParams& PollODParams(bool refresh = false);
		const DPerfLvls& PollPerfLvls(bool defaultVals, bool refresh = false);
		const DDisplayCount& PollDisplayCount(bool refresh = false);
		
		bool ODSetFan(int value, int type = ADL_DL_FANCTRL_SPEED_TYPE_PERCENT);
		bool ODSetFanDefault();
		
		// Params set to 0 (except index) will be ignored.
		bool ODSetOneLevel(int index, int engine, int memory, int vddc);
		// Params set to 0 will be ignored.
		bool ODSetAllLevels(int engine, int memory, int vddc);
		bool ODResetAllLevels();
		
		const std::string GetUDID();
		const std::string GetName();
		AdapterVector& GetAdapters();
		DisplayVector& GetDisplays();
		
		void SetPollAdapter(int index);
		int GetPollAdapter() const;
		void SetPollThermal(int index);
		int GetPollThermal() const;
		
		// Detect/Refresh Adapters.
		void DetectAdapters();
		void DetectDisplays();
		void ResetPolled();
	private:
		void sortAdapters();
		
		DBiosInfo bios;
		DAccess access;
		DActivity activity;
		DTemperature temperature;
		DFanInfo fanInfo;
		DFanSpeed fanSpeed;
		DOdParams odParams;
		DPerfLvls perfLevels;
		DPerfLvls defPerfLevels;
		DDisplayCount displayCount;
		
		int pollAdapter;
		int pollThermal;
		
		AdapterVector adapters;
		DisplayVector displays;
	};
}

#endif // KKE_DEVICE_HPP