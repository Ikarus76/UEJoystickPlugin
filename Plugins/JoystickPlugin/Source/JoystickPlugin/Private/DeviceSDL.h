/*
*
* Copyright (C) <2014> <w-hs - cglab projects>
* All rights reserved.
*
* This software may be modified and distributed under the terms
* of the BSD license.  See the LICENSE file for details.
*/

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

#include "SDL2/SDL.h"
#include "SDL2/SDL_joystick.h"
#include "SDL2/SDL_gamecontroller.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

#include <SlateBasics.h>

#include "IJoystickPlugin.h"
#include "JoystickDelegate.h"
#include "FJoystickPlugin.h"

DECLARE_LOG_CATEGORY_EXTERN(JoystickPluginLog, Log, All);

#include "JoystickPluginPrivatePCH.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

extern JoystickEventInterface* g_HotPlugDelegate;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

/**
 * Type definition for shared pointers to instances of
 */
typedef TSharedPtr<class CG221InputDeviceSDL> CG221InputDevicePtr;

/**
 * Type definition for shared references to instances of
 */
typedef TSharedRef<class CG221InputDeviceSDL> CG221InputDeviceRef;

//typedef TArray<UCG221InputObject *> tCG221InputObjectArray;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

struct sDeviceInfoSDL
{
	DeviceIndex deviceIndex = DeviceIndex::_invalid;
	DeviceId deviceId = DeviceId::_invalid;
	InstanceId instanceId = InstanceId::_invalid;

	bool isConnected = false;

	FString strName = "unknown";
	
	bool isGameController = false;
	bool isJoystick = false;
	bool hasHaptic = false;

	SDL_Haptic *haptic = NULL;
	SDL_Joystick *joystick = NULL;
	SDL_GameController *gameController = NULL;
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

class DeviceSDL
{

public:
	int32 getNumberOfDevices();
	
	int32 getNumberOfAxes(DeviceId device = DeviceId::_min);
	int32 getNumberOfButtons(DeviceId device = DeviceId::_min);
	int32 getNumberOfHats(DeviceId device = DeviceId::_min);
	int32 getNumberOfBalls(DeviceId device = DeviceId::_min);

	bool getDeviceSDL(DeviceId device, sDeviceInfoSDL &deviceInfo);
	bool getDeviceState(FJoystickState &InputData, FJoystickInfo &JoystickInfo, DeviceId device);

	FString getDeviceName(DeviceId device = DeviceId::_min);

	Sint32 getDeviceInstanceId(DeviceId device = DeviceId::_min);
	FString getDeviceGUIDtoString(DeviceId device = DeviceId::_min);
	FGuid getDeviceGUIDtoGUID(DeviceId device = DeviceId::_min);


	//inline uint32 getTypeHash(const GUID guid)
	//{
	//	auto data = reinterpret_cast<const uint64*>(&guid);
	//	return HashCombine(GetTypeHash(data[0]), GetTypeHash(data[1]));
	//}

	//inline FGuid ToFGuid(GUID g)
	//{
	//	FGuid guid;
	//	memcpy(&guid, &g, sizeof(FGuid));
	//	return guid;
	//}

	//inline GUID ToGUID(FGuid g)
	//{
	//	GUID guid;
	//	memcpy(&guid, &g, sizeof(FGuid));
	//	return guid;
	//}	
	
	void resetDevices();
	void resetDevice(DeviceId device = DeviceId::_min);

	//bool initDevices();
	bool initDevice(DeviceIndex deviceNumber, sDeviceInfoSDL &deviceInfo);
	sDeviceInfoSDL * getDevice(DeviceId iDevice = DeviceId::_min);
	bool doneDevice(DeviceId deviceInfo);

	void update(float DeltaTime);

	DeviceSDL();
	virtual ~DeviceSDL();

protected:
	void initSDL();
	void doneSDL();

private:
	bool hasJoysticks;
	bool hasGameController;
	bool hasHaptic;

	TMap<DeviceId, sDeviceInfoSDL> m_Devices;	
	TMap<InstanceId, DeviceId> m_DeviceMapping;
	
	SDL_Event m_Event_SDL;
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
