/*
*
* Copyright (C) <2014> samiljan <Sam Persson>, tsky <thomas.kollakowksy@w-hs.de>
* All rights reserved.
*
* This software may be modified and distributed under the terms
* of the BSD license.  See the LICENSE file for details.
*/

#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_joystick.h"
#include "SDL2/SDL_gamecontroller.h"

#include "IJoystickPlugin.h"
#include "FJoystickPlugin.h"

DECLARE_LOG_CATEGORY_EXTERN(JoystickPluginLog, Log, All);

struct sDeviceInfoSDL
{
	DeviceIndex deviceIndex {0};
	DeviceId deviceId {0};
	InstanceId instanceId {0};

	bool isConnected = false;

	FString strName = "unknown";
	
	bool isGameController = false;
	bool isJoystick = false;
	bool hasHaptic = false;

	SDL_Haptic *haptic = nullptr;
	SDL_Joystick *joystick = nullptr;
	SDL_GameController *gameController = nullptr;
};

class DeviceSDL
{

public:
	int32 getNumberOfDevices();
	
	int32 getNumberOfAxes(DeviceId device = DeviceId(0));
	int32 getNumberOfButtons(DeviceId device = DeviceId(0));
	int32 getNumberOfHats(DeviceId device = DeviceId(0));
	int32 getNumberOfBalls(DeviceId device = DeviceId(0));

	bool getDeviceSDL(DeviceId device, sDeviceInfoSDL &deviceInfo);
	bool getDeviceState(FJoystickState &InputData, FJoystickInfo &JoystickInfo, DeviceId device);

	FString getDeviceName(DeviceId device = DeviceId(0));

	Sint32 getDeviceInstanceId(DeviceId device = DeviceId(0));
	FString getDeviceGUIDtoString(DeviceId device = DeviceId(0));
	FGuid getDeviceGUIDtoGUID(DeviceId device = DeviceId(0));

	void resetDevices();
	void resetDevice(DeviceId device = DeviceId(0));

	bool initDevice(DeviceIndex deviceNumber, sDeviceInfoSDL &deviceInfo);
	sDeviceInfoSDL * getDevice(DeviceId iDevice = DeviceId(0));
	bool doneDevice(DeviceId deviceInfo);

	void update();

	DeviceSDL(JoystickEventInterface * eventInterface);
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

	JoystickEventInterface* eventInterface;
};
