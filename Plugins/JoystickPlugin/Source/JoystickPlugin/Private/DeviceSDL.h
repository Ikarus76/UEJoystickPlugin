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

#define MAX_CONTROLLED_DEVICES 16

extern JoystickHotPlugInterface* g_HotPlugDelegate;

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
	int deviceNumber = -1;

	bool isConnected = false;

	FString strName;
	
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
	
	int32 getNumberOfAxes(int32 InputDeviceIndex = 0);
	int32 getNumberOfButtons(int32 InputDeviceIndex = 0);
	int32 getNumberOfHats(int32 InputDeviceIndex = 0);
	int32 getNumberOfBalls(int32 InputDeviceIndex = 0);

	bool getDeviceSDL(int32 iDevice, sDeviceInfoSDL &deviceInfo);
	bool getDeviceState(FJoystickState &InputData, FJoystickInfo &JoystickInfo, int32 InputDeviceIndex = 0);

	FString getDeviceName(int32 InputDeviceIndex = 0);

	Sint32 getDeviceInstanceId(int32 InputDeviceIndex = 0);
	FString getDeviceGUIDtoString(int32 InputDeviceIndex = 0);
	FGuid getDeviceGUIDtoGUID(int32 InputDeviceIndex = 0);


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
	void resetDevice(int iDevice);

	bool initDevices();
	bool initDevice(int deviceNumber, sDeviceInfoSDL &deviceInfo);
	sDeviceInfoSDL * getDevice(int iDevice = 0);
	bool doneDevice(sDeviceInfoSDL &deviceInfo);

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

	sDeviceInfoSDL m_Devices[MAX_CONTROLLED_DEVICES];	
	
	int m_NumberOfDevices;

	SDL_Event m_Event_SDL;
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
