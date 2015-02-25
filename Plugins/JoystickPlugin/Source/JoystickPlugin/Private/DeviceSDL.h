/*
*
* Copyright (C) <2014> samiljan <Sam Persson>, tsky <thomas.kollakowksy@w-hs.de>
* All rights reserved.
*
* This software may be modified and distributed under the terms
* of the BSD license.  See the LICENSE file for details.
*/

#pragma once

// @third party code - BEGIN SDL
#include "SDL2/SDL.h"
#include "SDL2/SDL_joystick.h"
#include "SDL2/SDL_gamecontroller.h"
// @third party code - END SDL

#include "JoystickInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(JoystickPluginLog, Log, All);

class IJoystickEventInterface;

struct FDeviceInfoSDL
{
	FDeviceIndex DeviceIndex {0};
	FDeviceId DeviceId {0};
	FInstanceId InstanceId {0};

	bool bIsConnected = false;

	FString Name = "unknown";

	SDL_Haptic* Haptic = nullptr;
	SDL_Joystick* Joystick = nullptr;
	SDL_GameController* GameController = nullptr;
};

class FDeviceSDL
{
public:
	bool GetDeviceState(FJoystickState &InputData, const FJoystickInfo &JoystickInfo, FDeviceId device);

	static FString DeviceGUIDtoString(FDeviceIndex device);
	static FGuid DeviceGUIDtoGUID(FDeviceIndex device);

	bool InitDevice(FDeviceIndex deviceNumber, FDeviceInfoSDL &deviceInfo);
	FDeviceInfoSDL * GetDevice(FDeviceId iDevice);
	void DoneDevice(FDeviceId deviceInfo);

	void Update();

	FDeviceSDL(IJoystickEventInterface * eventInterface);
	virtual ~FDeviceSDL();

private:
	TMap<FDeviceId, FDeviceInfoSDL> Devices;	
	TMap<FInstanceId, FDeviceId> DeviceMapping;
	
	SDL_Event Event;

	IJoystickEventInterface* EventInterface;
};
